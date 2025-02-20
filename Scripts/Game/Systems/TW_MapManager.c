enum TW_CompositionSize
{
	SMALL,
	MEDIUM,
	LARGE
}

enum TW_CompositionSpawnType
{
	SMALL = 0, 
	MEDIUM = 1, 
	LARGE = 2, 
	WALLS = 3, 
	BUNKERS = 4
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
	
	ref TW_GridItemManager<string> gridManager;	
	
	const int PADDING = 10;
	const int SMALL_COMPOSITION_SIZE = 8 + PADDING; // 8
	const int MEDIUM_COMPOSITION_SIZE = 15 + PADDING; // 15
	const int LARGE_COMPOSITION_SIZE = 23 + PADDING; // 23
	const int SPAWN_DELAY = 1000;
	private SCR_BaseGameMode m_GameMode;
	private MapEntity m_Map;
	
	private ref ScriptInvoker m_OnCompositionBasePlacementFailed = new ScriptInvoker();
	private ref ScriptInvoker m_OnCompositionBasePlacementSuccess = new ScriptInvoker();
	
	ScriptInvoker GetOnCompositionBasePlacementFailed() { return m_OnCompositionBasePlacementFailed; }
	ScriptInvoker GetOnCompositionBasePlacementSuccess() { return m_OnCompositionBasePlacementSuccess; }
	
	void InitializeMap(SCR_BaseGameMode gameMode, MapEntity mapManager)
	{
		m_GameMode = gameMode; 
		m_Map = mapManager;
		
		//! We're going to query all the things on the map. This allows us to grab things by descriptor types
		ref array<MapItem> entities = {};
		int locationsCount = mapManager.GetInsideRect(entities, vector.Zero, mapManager.Size());
		
		PrintFormat("TrainWreck-GM: Map items count --> %1", locationsCount);
		
		settings = LocationSettings.LoadFromFile();

		ref GridSettings temp = new GridSettings();
		temp.SizeInMeters = settings.gridSize;
				
		gridManager = new TW_GridItemManager<string>(temp);
		
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
	
	private void OnCompSpawnedCompleted()
	{
		Print("TrainWreck -> Successfully spawned compositions");
	}
	
	private void OnCompFailed(TW_CompositionPlacementEvent e)
	{
		PrintFormat("TrainWreck -> Failed to place '%1' at '%2'", e.prefab, e.position, LogLevel.ERROR);
	}	
	
	private void OnCompSuccess(TW_CompositionPlacementEvent e)
	{
		PrintFormat("TrainWreck -> Placed'%1' at '%2'", e.prefab, e.position, LogLevel.WARNING);
		ref MapItem item = m_Map.CreateCustomMapItem();
		item.SetPos(e.position[0], e.position[2]);
		item.SetInfoText(e.prefab);
		item.SetDisplayName(e.prefab);		
	}
	
	private ref TW_CompositionSpawnHandler handler = new TW_CompositionSpawnHandler();
	private ref TW_CompositionSpawnSettings spawnConfig;
	
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
		spawnConfig = new TW_CompositionSpawnSettings(config);
		
		ref array<IEntity> spawnedEntities = {};
		
		IEntity spawnedEntity;
		vector position = vector.Zero;
		int attempts = 0;
		basePosition = TW_Util.GetLandPositionAround(basePosition, spacing);
		
		spawnConfig.LargeCompositions = Math.RandomIntInclusive(0, 2);
		spawnConfig.MediumCompositions = Math.RandomIntInclusive(0, 4);
		spawnConfig.SmallCompositions = Math.RandomIntInclusive(1, 5);
		spawnConfig.DefensiveWalls = Math.RandomIntInclusive(0, 5);
		spawnConfig.DefensiveBunkers = Math.RandomIntInclusive(0, 4);
		
		handler.Init(basePosition, spacing, spawnConfig);
		handler.GetOnCompleted().Insert(OnCompSpawnedCompleted);
		handler.GetOnPlacementFailed().Insert(OnCompFailed);
		handler.GetOnPlacementSucceeded().Insert(OnCompSuccess);
		handler.SpawnStart();
		
		return true;		
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