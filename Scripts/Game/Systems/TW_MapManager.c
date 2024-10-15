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
}

class TW_MapManager
{
	protected ref map<string, ref TW_MapLocation> locations;
	ref LocationSettings settings;
	ref set<string> locationNames = new set<string>();
	
	ref TW_GridCoordItemManager<string> gridManager;
	
	const int SMALL_COMPOSITION_SIZE = 8;
	const int MEDIUM_COMPOSITION_SIZE = 15;
	const int LARGE_COMPOSITION_SIZE = 23;
	private SCR_BaseGameMode m_GameMode;
	
	//! Is the manager currently trickle spawning compositions
	private bool m_IsTrickleSpawning = false;
	private ref array<IEntity> m_BaseEntities = {};
	
	
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
	}
	
	private void SpawnSmallCompositions(TW_CompositionSpawnSettings spawnAmount, TW_CompositionSpawnSettings config, vector center, vector currentPosition)
	{
		if(!config.Compositions.HasSmallCompositions())
			spawnAmount.SmallCompositions = -1;
		
		if(spawnAmount.SmallCompositions >= config.SmallCompositions || spawnAmount.SmallCompositions < 0)
		{
			GetGame().GetCallqueue().CallLater(SpawnMediumCompositions, 250, false, spawnAmount, config, center, currentPosition);
			return;
		}
		
		IEntity current;
		
		int attempts = 5;
		bool success = false;
		while(attempts > 0)
		{
			attempts--;
			
			success = TrySpawnComposition(center, current, currentPosition, 50, TW_CompositionSize.SMALL, config.Compositions.GetRandomSmallComposition());
			
			if(success)
			{
				m_BaseEntities.Insert(current);	
				break;
			}
		}
		
		if(!success)
		{
			DeleteAll(m_BaseEntities);
			m_IsTrickleSpawning = false;
			return;
		}
		
		spawnAmount.SmallCompositions++;
	}
	
	private void SpawnMediumCompositions(TW_CompositionSpawnSettings spawnAmount, TW_CompositionSpawnSettings config, vector center, vector currentPosition)
	{
		if(!config.Compositions.HasMediumCompositions())
			spawnAmount.MediumCompositions = -1;
		
		if(spawnAmount.MediumCompositions >= config.MediumCompositions || spawnAmount.MediumCompositions < 0)
		{
			GetGame().GetCallqueue().CallLater(SpawnLargeCompositions, 250, false, spawnAmount, config, center, currentPosition);
			return;
		}
		
		IEntity current;
		
		int attempts = 5;
		bool success = false;
		while(attempts > 0)
		{
			attempts--;
			
			success = TrySpawnComposition(center, current, currentPosition, 50, TW_CompositionSize.MEDIUM, config.Compositions.GetRandomMediumComposition());
			
			if(success)
			{
				m_BaseEntities.Insert(current);	
				break;
			}
		}
		
		if(!success)
		{
			DeleteAll(m_BaseEntities);
			m_IsTrickleSpawning = false;
			return;
		}
		
		spawnAmount.MediumCompositions++;
	}
	
	private void SpawnLargeCompositions(TW_CompositionSpawnSettings spawnAmount, TW_CompositionSpawnSettings config, vector center, vector currentPosition)
	{
		if(!config.Compositions.HasLargeCompositions())
			spawnAmount.LargeCompositions = -1;
		
		if(spawnAmount.LargeCompositions >= config.LargeCompositions || spawnAmount.LargeCompositions < 0)
		{
			GetGame().GetCallqueue().CallLater(SpawnDefensiveBunkers, 250, false, spawnAmount, config, center, currentPosition);
			return;
		}
		
		IEntity current;
		
		int attempts = 5;
		bool success = false;
		while(attempts > 0)
		{
			attempts--;
			
			success = TrySpawnComposition(center, current, currentPosition, 50, TW_CompositionSize.LARGE, config.Compositions.GetRandomLargeComposition());
			
			if(success)
			{
				m_BaseEntities.Insert(current);	
				break;
			}
		}
		
		if(!success)
		{
			DeleteAll(m_BaseEntities);
			m_IsTrickleSpawning = false;
			return;
		}
		
		spawnAmount.LargeCompositions++;
	}
	
	private void SpawnDefensiveBunkers(TW_CompositionSpawnSettings spawnAmount, TW_CompositionSpawnSettings config, vector center, vector currentPosition)
	{
		if(!config.Compositions.HasDefensiveBunkers())
			spawnAmount.DefensiveBunkers = -1;
		
		if(spawnAmount.DefensiveBunkers >= config.DefensiveBunkers || spawnAmount.DefensiveBunkers < 0)
		{
			GetGame().GetCallqueue().CallLater(SpawnDefensiveWalls, 250, false, spawnAmount, config, center, currentPosition);
			return;
		}
		
		IEntity current;
		
		int attempts = 5;
		bool success = false;
		while(attempts > 0)
		{
			attempts--;
			
			success = TrySpawnComposition(center, current, currentPosition, 50, TW_CompositionSize.SMALL, config.Compositions.GetRandomDefensiveBunkder());
			
			if(success)
			{
				m_BaseEntities.Insert(current);	
				break;
			}
		}
		
		if(!success)
		{
			DeleteAll(m_BaseEntities);
			m_IsTrickleSpawning = false;
			return;
		}
		
		spawnAmount.DefensiveBunkers++;
	}
	
	private void SpawnDefensiveWalls(TW_CompositionSpawnSettings spawnAmount, TW_CompositionSpawnSettings config, vector center, vector currentPosition)
	{
		if(!config.Compositions.HasDefensiveWalls())
			spawnAmount.DefensiveWalls = -1;
		
		if(spawnAmount.DefensiveWalls >= config.DefensiveWalls || spawnAmount.DefensiveWalls < 0)
		{
			m_IsTrickleSpawning = false;
			m_BaseEntities.Clear();
			return;
		}
		
		IEntity current;
		
		int attempts = 5;
		bool success = false;
		while(attempts > 0)
		{
			attempts--;
			
			success = TrySpawnComposition(center, current, currentPosition, 50, TW_CompositionSize.SMALL, config.Compositions.GetRandomDefensiveWall());
			
			if(success)
			{
				m_BaseEntities.Insert(current);	
				break;
			}
		}
		
		if(!success)
		{
			DeleteAll(m_BaseEntities);
			m_IsTrickleSpawning = false;
			return;
		}
		
		spawnAmount.DefensiveWalls++;
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
		
		if(config.HasSmallCompositions())
		{
			attempts = 0;
			for(int i = 0; i < smallCompositions; i++)
			{
				if(TrySpawnComposition(basePosition, spawnedEntity, position, 50, TW_CompositionSize.SMALL, config.GetRandomSmallComposition()))
				{
					spawnedEntities.Insert(spawnedEntity);
					basePosition = TW_Util.GetLandPositionAround(position, 25);
				}
				else
				{
					attempts++;
					
					if(attempts > 10)
					{
						DeleteAll(spawnedEntities);
						return false;
					}
					else 
						i--;
				}
			}
		}
		
		if(config.HasMediumCompositions())
		{
			attempts = 0;
			for(int i = 0; i < mediumCompositions; i++)
			{
				if(TrySpawnComposition(basePosition, spawnedEntity, position, 50, TW_CompositionSize.MEDIUM, config.GetRandomMediumComposition()))
				{
					spawnedEntities.Insert(spawnedEntity);
					basePosition = TW_Util.GetLandPositionAround(position, 25);
				}
				else
				{
					attempts++;
					
					if(attempts > 10)
					{
						DeleteAll(spawnedEntities);
						return false;
					}
					else 
						i--;
				}
			}
		}
		
		if(config.HasLargeCompositions())
		{
			attempts = 0;
			for(int i = 0; i < largeCompositions; i++)
			{
				if(TrySpawnComposition(basePosition, spawnedEntity, position, 50, TW_CompositionSize.LARGE, config.GetRandomLargeComposition()))
				{
					spawnedEntities.Insert(spawnedEntity);
					basePosition = TW_Util.GetLandPositionAround(position, 25);
				}
				else
				{
					attempts++;
					
					if(attempts > 10)
					{
						DeleteAll(spawnedEntities);
						return false;
					}
					else 
						i--;
				}
			}
		}
		
		if(config.HasDefensiveWalls())
		{
			attempts = 0;
			for(int i = 0; i < defensiveWalls; i++)
			{
				if(TrySpawnComposition(basePosition, spawnedEntity, position, 50, TW_CompositionSize.SMALL, config.GetRandomDefensiveWall()))
				{
					spawnedEntities.Insert(spawnedEntity);
					basePosition = TW_Util.GetLandPositionAround(position, 25);
				}
				else
				{
					attempts++;
					
					if(attempts > 10)
					{
						DeleteAll(spawnedEntities);
						return false;
					}
					else 
						i--;
				}
			}
		}
		
		if(config.HasDefensiveBunkers())
		{
			attempts = 0;
			for(int i = 0; i < defensiveStructures; i++)
			{
				if(TrySpawnComposition(basePosition, spawnedEntity, position, 50, TW_CompositionSize.SMALL, config.GetRandomDefensiveBunkder()))
				{
					spawnedEntities.Insert(spawnedEntity);
					basePosition = TW_Util.GetLandPositionAround(position, 25);
				}
				else
				{
					attempts++;
					
					if(attempts > 10)
					{
						DeleteAll(spawnedEntities);
						return false;
					}
					else 
						i--;
				}
			}
		}
		
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
			
			entity = GetGame().SpawnEntityPrefab(prefab, false, GetGame().GetWorld(), params);
			
			if(!entity) 
				return false;
			
			
			SCR_EditableEntityComponent editable = SCR_EditableEntityComponent.GetEditableEntity(entity);
			
			if(editable)
			{
				SCR_EditorPreviewParams previewParams = SCR_EditorPreviewParams.CreateParams(params.Transform, EEditorTransformVertical.TERRAIN);
				SCR_RefPreviewEntity.SpawnAndApplyReference(editable, previewParams);
			}
			
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
		
		return SCR_WorldTools.FindEmptyTerrainPosition(position, centerPosition, radius, compositionSize, compositionSize);
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