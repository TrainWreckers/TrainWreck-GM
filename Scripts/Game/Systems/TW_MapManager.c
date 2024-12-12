enum TW_CompositionSize
{
	SMALL,
	MEDIUM,
	LARGE
}

class TW_CompositionSpawnSettings
{
	int SmallCompositions;
	int MediumCompositions;
	int LargeCompositions;
	int DefensiveWalls;
	int DefensiveBunkers;
	ref FactionCompositions Compositions;
	bool ShouldFailAfterRetries = false;
	
	void TW_CompositionSpawnSettings(FactionCompositions settings = null)
	{
		SmallCompositions = 0;
		MediumCompositions = 0;
		LargeCompositions = 0;
		DefensiveWalls = 0;
		DefensiveBunkers = 0;
		Compositions = settings;
	}
}

class TW_MapManager
{
	protected ref map<string, ref TW_MapLocation> locations;
	ref LocationSettings settings;
	ref set<string> locationNames = new set<string>();
	
	ref TW_GridCoordItemManager<string> gridManager;
	
	const int PADDING = 10;
	const int SMALL_COMPOSITION_SIZE = 8 + PADDING; // 8
	const int MEDIUM_COMPOSITION_SIZE = 15 + PADDING; // 15
	const int LARGE_COMPOSITION_SIZE = 23 + PADDING; // 23
	const int SPAWN_DELAY = 1000;
	private SCR_BaseGameMode m_GameMode;
	
	//! Is the manager currently trickle spawning compositions
	private bool m_IsTrickleSpawning = false;
	private ref array<IEntity> m_BaseEntities = {};
	
	private ref ScriptInvoker m_OnCompositionBasePlacementFailed = new ScriptInvoker();
	private ref ScriptInvoker m_OnCompositionBasePlacementSuccess = new ScriptInvoker();
	
	ScriptInvoker GetOnCompositionBasePlacementFailed() { return m_OnCompositionBasePlacementFailed; }
	ScriptInvoker GetOnCompositionBasePlacementSuccess() { return m_OnCompositionBasePlacementSuccess; }
	
	void InitializeMap(SCR_BaseGameMode gameMode, MapEntity mapManager)
	{
		m_GameMode = gameMode; 
		
		//! We're going to query all the things on the map. This allows us to grab things by descriptor types
		ref array<MapItem> entities = {};
		int locationsCount = mapManager.GetInsideRect(entities, vector.Zero, mapManager.Size());
		
		PrintFormat("TrainWreck-GM: Map items count --> %1", locationsCount);
		
		settings = LocationSettings.LoadFromFile();
		gridManager = new TW_GridCoordItemManager<string>(settings.gridSize);
		
		ref set<string> chunks = new set<string>();
		string center;
		
		this.locations = new map<string, ref TW_MapLocation>();
		
		foreach(MapItem item : entities)
		{
			if(!item || !item.Descriptor()) continue;
			
			EMapDescriptorType type = item.Descriptor().GetBaseType();
			
			if(!settings.IsValidMapDescriptor(type)) 
				continue;
			
			chunks.Clear();	
			ref TW_MapLocation location = new TW_MapLocation();
			ref LocationConfig config = settings.GetConfigType(type);
			
			if(config)
				TW_Util.AddSurroundingGridSquares(chunks, item.GetPos(), config.radius, settings.gridSize);
			
			location.SetData(type, item.GetPos(), item.GetDisplayName(), FactionKey.Empty, chunks);
			this.locations.Set(item.GetDisplayName(), location);
			locationNames.Insert(item.GetDisplayName());
		}
		
		entities.Clear();
		delete entities;
	}
	
	private void DeleteAll(notnull array<IEntity> entities)
	{
		foreach(IEntity entity : entities)
			SCR_EntityHelper.DeleteEntityAndChildren(entity);
		m_OnCompositionBasePlacementFailed.Invoke();
	}
	
	bool TrySpawnCompositionCollection(FactionKey faction, vector basePosition, float spacing, int defensiveWalls, int defensiveStructures, int smallCompositions, int mediumCompositions, int largeCompositions)
	{
		ref FactionCompositions config = m_GameMode.GetUSSRCompositions();
		
		switch(faction)
		{			
			case "US":
			{
				config = m_GameMode.GetUSCompositions();
				break;
			}
			case "FIA":
			{
				config = m_GameMode.GetFIACompositions();
				break;
			}
		}
		
		ref array<IEntity> spawnedEntities = {};
		
		IEntity spawnedEntity;
		vector position = vector.Zero;
		int attempts = 0;
		basePosition = TW_Util.GetLandPositionAround(basePosition, spacing);
		
		m_IsTrickleSpawning = true;
		
		if(!m_BaseEntities) m_BaseEntities = {};
		m_BaseEntities.Clear();
		
		ref TW_CompositionSpawnSettings spawnConfig = new TW_CompositionSpawnSettings(config);
		ref TW_CompositionSpawnSettings amounts = new TW_CompositionSpawnSettings();
		
		spawnConfig.LargeCompositions = Math.RandomIntInclusive(0, 2);
		spawnConfig.MediumCompositions = Math.RandomIntInclusive(0, 4);
		spawnConfig.SmallCompositions = Math.RandomIntInclusive(1, 5);
		spawnConfig.DefensiveWalls = Math.RandomIntInclusive(0, 5);
		spawnConfig.DefensiveBunkers = Math.RandomIntInclusive(0, 4);
		
		return true;		
	}
	
	bool TrySpawnComposition(vector centerPosition, out IEntity entity, out vector position, int radius, TW_CompositionSize size, ResourceName prefab)
	{
		if(FindOpenAreaForComposition(centerPosition, radius, size, position))
		{
			EntitySpawnParams params = EntitySpawnParams();
			vector mat[4];
			mat[3] = position;			
			
			if(!SCR_TerrainHelper.SnapToTerrain(mat, GetGame().GetWorld(), true))
				return false;
			
			params.Transform = mat;
			params.TransformMode = ETransformMode.WORLD;
			vector angles = Math3D.MatrixToAngles(params.Transform);
			angles[0] = Math.RandomFloat(0, 360);
			Math3D.AnglesToMatrix(angles, params.Transform);
			
			Resource prefabResource = Resource.Load(prefab);
			if(!prefabResource.IsValid())
				return false;
						
			entity = GetGame().SpawnEntityPrefab(prefabResource, GetGame().GetWorld(), params);
			
			if(!entity) 
				return false;
			
			
			SCR_EditableEntityComponent editable = SCR_EditableEntityComponent.GetEditableEntity(entity);
			
			if(editable)
			{
				SCR_EditorPreviewParams previewParams = SCR_EditorPreviewParams.CreateParams(params.Transform, EEditorTransformVertical.TERRAIN);
				SCR_RefPreviewEntity.SpawnAndApplyReference(editable, previewParams);
				Print("TrainWreck-GM: Oriented Terrain for Entity");
			}
			
			SCR_SlotCompositionComponent slotComposition = SCR_SlotCompositionComponent.Cast(entity.FindComponent(SCR_SlotCompositionComponent));
			
			if(slotComposition)				
				slotComposition.OrientToTerrain();
			
			SCR_AIWorld aiWorld = SCR_AIWorld.Cast(GetGame().GetAIWorld());
			if(aiWorld)
				aiWorld.RequestNavmeshRebuildEntity(entity);
			
			return true;
		}
		
		return false;
	}
	
	bool FindOpenAreaForComposition(vector centerPosition, float radius, TW_CompositionSize size, out vector position, FactionKey faction = FactionKey.Empty)
	{
		int compositionSize = SMALL_COMPOSITION_SIZE;
		
		switch(size)
		{
			case TW_CompositionSize.MEDIUM:
			{
				compositionSize = MEDIUM_COMPOSITION_SIZE;
				break;
			}
			
			case TW_CompositionSize.LARGE:
			{
				compositionSize = LARGE_COMPOSITION_SIZE;
				break;
			}
		}
		
		return SCR_WorldTools.FindEmptyTerrainPosition(position, centerPosition, radius, compositionSize, compositionSize, TraceFlags.ENTS | TraceFlags.OCEAN | TraceFlags.WORLD);
	}
	
	TW_MapLocation GetRandomLocation()
	{
		int randomIndex = Math.RandomInt(0, locationNames.Count());
		string name = locationNames.Get(randomIndex);
		
		if(!locations.Contains(name))
		{
			locations.Remove(name);
			return null;
		}
		
		return locations.Get(name);
	}
};