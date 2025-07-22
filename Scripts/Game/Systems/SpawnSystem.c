class TW_AISpawnPointGrid
{
	private static ref TW_AISpawnPointGrid s_Grid = new TW_AISpawnPointGrid();
	static TW_AISpawnPointGrid GetInstance() { return s_Grid; }
	
	private int m_GridSize = 500;
	private ref TW_GridCoordArrayManager<TW_AISpawnPoint> m_Grid = new TW_GridCoordArrayManager<TW_AISpawnPoint>(m_GridSize);
	
	TW_GridCoordArrayManager<TW_AISpawnPoint> GetGridManager() { return m_Grid; }
	int GetGridSize() { return m_GridSize; }
	
	void ChangeGridSize(int newSize)
	{
		m_GridSize = newSize;
		ref TW_GridCoordArrayManager<TW_AISpawnPoint> manager = new TW_GridCoordArrayManager<TW_AISpawnPoint>(m_GridSize);
		ref array<TW_AISpawnPoint> items = {};
		int count = m_Grid.GetAllItems(items);
		
		foreach(TW_AISpawnPoint spawnPoint : items)
			if(spawnPoint)
				manager.InsertByWorld(spawnPoint.GetOrigin(), spawnPoint);
		
		delete m_Grid;
		m_Grid = manager;
	}
	
	void RegisterSpawnPoint(TW_AISpawnPoint point)
	{
		m_Grid.InsertByWorld(point.GetOrigin(), point);
	}
	
	void UnregisterSpawnPoint(TW_AISpawnPoint point)
	{
		m_Grid.RemoveByWorld(point.GetOrigin(), point);
	}
	
	/*!
		Get spawn points within specified chunks. Optionally, include chunks in radius around each chunk
	*/
	void GetSpawnPointsInChunks(notnull set<string> chunks, notnull array<TW_AISpawnPoint> spawnPoints, int radius = -1)
	{
		ref set<string> processed = new set<string>();
		foreach(string chunk : chunks)
		{
			m_Grid.GetNeighborsAround(spawnPoints, chunks, processed, radius);
		}
	}
	
	/*!
		Get nearby spawn points around a given point. 
	
		- This is used by GameMaster, with the SpawnInBuildings system.
	*/
	void GetNearbySpawnPoints(vector center, notnull array<TW_AISpawnPoint> spawnPoints, int radius = -1)
	{
		int x, y;
		TW_Util.ToGrid(center, x, y, m_GridSize);
		
		if(!m_Grid.HasCoord(x, y))
			return;
		
		ref set<string> processed = new set<string>();
		
		processed.Insert(TW_Util.ToGridText(center, m_GridSize));
		
		ref array<ref TW_GridCoordArray<TW_AISpawnPoint>> items = {};
		m_Grid.GetNeighbors(items, x, y, processed, radius);
		
		foreach(ref TW_GridCoordArray<TW_AISpawnPoint> item : items)
		{
			string currentCoord = TW_Util.ToGridText(item.x, item.y);
			
			if(processed.Contains(currentCoord))
				continue;
			
			processed.Insert(currentCoord);
			
			ref array<TW_AISpawnPoint> points = item.GetAll();
			foreach(TW_AISpawnPoint spawnPoint : points)
			{
				if(!spawnPoint)
					continue;
				
				if(spawnPoint.IsActive())
					spawnPoints.Insert(spawnPoint);
			}
		}
	}
};

class TW_VehicleSpawnPointGrid
{
	private static ref TW_VehicleSpawnPointGrid s_Grid = new TW_VehicleSpawnPointGrid();
	static TW_VehicleSpawnPointGrid GetInstance() { return s_Grid; }
	
	private int m_GridSize = 500;
	private ref TW_GridCoordArrayManager<TW_VehicleSpawnPoint> m_Grid = new TW_GridCoordArrayManager<TW_VehicleSpawnPoint>(m_GridSize);
	
	TW_GridCoordArrayManager<TW_VehicleSpawnPoint> GetGridManager() { return m_Grid; }
	int GetGridSize() { return m_GridSize; }
	
	void ChangeGridSize(int newSize)
	{
		m_GridSize = newSize;
		ref TW_GridCoordArrayManager<TW_VehicleSpawnPoint> manager = new TW_GridCoordArrayManager<TW_VehicleSpawnPoint>(m_GridSize);
		ref array<TW_VehicleSpawnPoint> items = {};
		int count = m_Grid.GetAllItems(items);
		
		foreach(TW_VehicleSpawnPoint spawnPoint : items)
			if(spawnPoint)
				manager.InsertByWorld(spawnPoint.GetOrigin(), spawnPoint);
		
		delete m_Grid;
		m_Grid = manager;
	}
	
	void RegisterSpawnPoint(TW_VehicleSpawnPoint point)
	{
		m_Grid.InsertByWorld(point.GetOrigin(), point);
	}
	
	void UnregisterSpawnPoint(TW_VehicleSpawnPoint point)
	{
		m_Grid.RemoveByWorld(point.GetOrigin(), point);
	}
	
	/*!
		Get spawn points within specified chunks. Optionally, include chunks in radius around each chunk
	*/
	void GetSpawnPointsInChunks(notnull set<string> chunks, notnull array<TW_VehicleSpawnPoint> spawnPoints, int radius = -1)
	{
		ref set<string> processed = new set<string>();
		foreach(string chunk : chunks)
		{
			m_Grid.GetNeighborsAround(spawnPoints, chunks, processed, radius);
		}
	}
	
	/*!
		Get nearby spawn points around a given point. 
	
		- This is used by GameMaster, with the SpawnInBuildings system.
	*/
	void GetNearbySpawnPoints(vector center, notnull array<TW_VehicleSpawnPoint> spawnPoints, int radius = -1)
	{
		int x, y;
		TW_Util.ToGrid(center, x, y, m_GridSize);
		
		if(!m_Grid.HasCoord(x, y))
			return;
		
		ref set<string> processed = new set<string>();
		
		ref array<ref TW_GridCoordArray<TW_VehicleSpawnPoint>> items = {};
		m_Grid.GetNeighbors(items, x, y, processed, radius);
		
		foreach(ref TW_GridCoordArray<TW_VehicleSpawnPoint> item : items)
		{
			ref array<TW_VehicleSpawnPoint> points = item.GetAll();
			foreach(TW_VehicleSpawnPoint spawnPoint : points)
			{
				if(!spawnPoint)
					continue;
				
				if(spawnPoint.IsActive())
					spawnPoints.Insert(spawnPoint);
			}
		}
	}
};