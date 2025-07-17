class TW_GridAIInfo
{
	private int _max;
	private int _lastCount = 0;
	private ref array<SCR_AIGroup> _ai = {};
	private ref array<SCR_AIGroup> _pending = {};
	
	void TW_GridAIInfo(int max)
	{
		_max = max;
	}
	
	int GetMax() { return _max; }
	
	void SetMax(int max) { _max = max; }
	
	void AddAI(SCR_AIGroup group)
	{
		if(!group)
			return;
		
		_pending.Insert(group);
		GetGame().GetCallqueue().CallLater(HandlePending, 250, false, group);
		
		if(TW_GeneralSettings.GetInstance().IsDebug)
			PrintFormat("TrainWreck: %1/%2", _lastCount + group.GetAgentsCount(), GetMax());
	}
	
	private void HandlePending(SCR_AIGroup group)
	{
		_pending.RemoveItem(group);
		_ai.Insert(group);
	}
	
	bool IsMaxed()
	{
		if(!_pending.IsEmpty())
			return true;
		
		int currentCount = GetAICount();
		return currentCount >= _max;
	}
	
	int GetAICount()
	{
		int count = 0;
		int length = 0;
		for(int i = 0; i < length; i++)
		{
			SCR_AIGroup group = _ai.Get(i);
			
			if(!group || group.GetAgentsCount() <= 0)
			{
				_ai.Remove(i);
				length -= 1;
				i -= 1;
				continue;
			}
			
			count += group.GetAgentsCount();
		}
		
		_lastCount = count;
		return count;
	}
	
	int GetAI(inout array<SCR_AIGroup> groups)
	{
		int count = 0;
		int length = _ai.Count();
		for(int i = 0; i < length; i++)
		{
			SCR_AIGroup group = groups.Get(i);
			if(!group)
			{
				groups.Remove(i);
				length -= 1;
				i -= 1;
				continue;
			}
			
			count++;
			groups.Insert(group);
		}
		_lastCount = count;
		return count;
	}
}

class TW_AISpawnPointGrid
{
	private static ref TW_AISpawnPointGrid s_Grid = new TW_AISpawnPointGrid();
	static TW_AISpawnPointGrid GetInstance() { return s_Grid; }
	
	private int m_GridSize = 500;
	private ref TW_GridCoordArrayManager<TW_AISpawnPoint> m_Grid = new TW_GridCoordArrayManager<TW_AISpawnPoint>(m_GridSize);
	private ref map<string, ref TW_GridAIInfo> _densityMap = new map<string, ref TW_GridAIInfo>();
	
	//! Calculate the density of the grid based on its number of spawn points
	private int GetDensity(int spawnPointCount)
	{
		float ratio = spawnPointCount * TW_GeneralSettings.GetInstance().GridDensitySettings.AIBudgetRatio;
		return Math.Max(0, Math.Min(ratio, TW_GeneralSettings.GetInstance().GridDensitySettings.MaxAIPerGrid));
	}
	
	//! Retrieve the grid manager that manages each bucket of items
	TW_GridCoordArrayManager<TW_AISpawnPoint> GetGridManager() { return m_Grid; }
	
	//! Size of grid square in meters
	int GetGridSize() { return m_GridSize; }
	
	/*!
		This is how the grid will be notified that a group has spawned.
		We have to convert their position into grid coordinates that our system understands.
		
		Then associate them with a particular grid square for tracking purposes.
	*/
	void OnGroupSpawn(SCR_AIGroup group)
	{
		if(!group) 
			return;
		
		string chunk = TW_Util.ToGridText(group.GetOrigin(), m_GridSize);
		if(_densityMap.Contains(chunk))
		{
			ref TW_GridAIInfo info = _densityMap.Get(chunk);
			info.AddAI(group);
		}
	}
	
	//! Calculate the density of each grid square
	void CalculateDensity()
	{
		_densityMap.Clear();
		
		if(!TW_GeneralSettings.GetInstance() || !TW_GeneralSettings.GetInstance().GridDensitySettings)
			return;
		
		ref map<string, ref TW_GridCoordArray<TW_AISpawnPoint>> gridResults = m_Grid.GetGrid();
		
		foreach(string chunk, ref TW_GridCoordArray<TW_AISpawnPoint> grid : gridResults)
		{
			int count = grid.Count();
			int max = GetDensity(count);
			_densityMap.Set(chunk, new TW_GridAIInfo(max));
			
			if(TW_GeneralSettings.GetInstance().IsDebug)
				PrintFormat("TrainWreck: %1 - Density: %2", chunk, max);
		}
	}
	
	//! Calculate which areas should be excluded based on its current AI count
	private void GetExclusion(notnull set<string> exclude)
	{
		exclude.Clear();
		foreach(string chunk, ref TW_GridAIInfo info : _densityMap)
		{
			if(info.IsMaxed())
				exclude.Insert(chunk);
		}
	}
	
	/*!
		Translate the current grid system from one grid size to another.
	*/
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
		CalculateDensity();
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
		ref set<string> exclude = new set<string>();
		GetExclusion(exclude);
		
		foreach(string chunk : chunks)
		{
			m_Grid.GetNeighborsAround(spawnPoints, chunks, radius, exclude);
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
		
		ref set<string> exclude = new set<string>();
		GetExclusion(exclude);
		
		ref array<ref TW_GridCoordArray<TW_AISpawnPoint>> items = {};
		m_Grid.GetNeighbors(items, x, y, radius, exclude: exclude);
		
		foreach(ref TW_GridCoordArray<TW_AISpawnPoint> item : items)
		{
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
	
	bool CanSpawnFrom(TW_AISpawnPoint spawnPoint)
	{
		string chunk = TW_Util.ToGridText(spawnPoint.GetOrigin(), GetGridSize());
		
		if(_densityMap.Contains(chunk))
		{
			ref TW_GridAIInfo info = _densityMap.Get(chunk);
			if(info.IsMaxed())
				return false;
		}
		
		return true;
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
		foreach(string chunk : chunks)
		{
			m_Grid.GetNeighborsAround(spawnPoints, chunks, radius);
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
		
		ref array<ref TW_GridCoordArray<TW_VehicleSpawnPoint>> items = {};
		m_Grid.GetNeighbors(items, x, y, radius);
		
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