class TW_MapManager
{
	private static TW_MapManager _instance;
	
	static TW_MapManager GetInstance() { return _instance; }
	
	protected ref map<string, ref TW_MapLocation> locations;
	ref LocationSettings settings;
	ref set<string> locationNames = new set<string>();
	
	ref TW_GridCoordItemManager<string> gridManager;
	private SCR_BaseGameMode m_GameMode;
	
	private ref map<EMapDescriptorType, ref array<vector>> _mapTypeToLocations = new map<EMapDescriptorType, ref array<vector>>();
	
	void InitializeMap(SCR_BaseGameMode gameMode, MapEntity mapManager)
	{
		_instance = this;
		
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
			
			if(!_mapTypeToLocations.Contains(type))
				_mapTypeToLocations.Insert(type, {});
			
			_mapTypeToLocations.Get(type).Insert(item.GetPos());
					
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
	
	bool GetRandomPositionByMapType(EMapDescriptorType type, out vector position)
	{
		if(!_mapTypeToLocations.Contains(type))
			return false;
		
		position = _mapTypeToLocations.Get(type).GetRandomElement();
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