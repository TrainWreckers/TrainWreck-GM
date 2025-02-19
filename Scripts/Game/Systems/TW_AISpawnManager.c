class TW_AISpawnManager
{
	private static ref TW_AISpawnManager s_Instance;
	
	static TW_AISpawnManager GetInstance() 
	{
		if(!s_Instance)
			s_Instance = TW_AISpawnManager();	 
		return s_Instance; 	
	}
	
	static GridSettings DefaultGridSettings()
	{
		ref GridSettings settings = new GridSettings();
		settings.SizeInMeters = 250;
		settings.DistanceInChunks = 3;
		
		return settings;
	}
	
	void TW_AISpawnManager()
	{
		s_Instance = this;
	}
	
	private ref TW_GridCoordArrayManager<TW_AISpawnPoint> _gridManager = new TW_GridCoordArrayManager<TW_AISpawnPoint>(DefaultGridSettings());	
	private ref TW_GridCoordArrayManager<TW_VehicleSpawnPoint> _vehicleGridManager = new TW_GridCoordArrayManager<TW_VehicleSpawnPoint>(DefaultGridSettings());
	
	TW_GridCoordArrayManager<TW_AISpawnPoint> GetAISpawnGrid() { return _gridManager; }
	TW_GridCoordArrayManager<TW_VehicleSpawnPoint> GetVehicleSpawnGrid() { return _vehicleGridManager; }
};