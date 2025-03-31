[EntityEditorProps(category: "GameScripted/TrainWreck/Systems", description: "Spawn point for units, and behaves as waypoints when using TrainWreck Systems")]
class TW_AISpawnPointClass : GenericEntityClass{};

class TW_AISpawnPoint : GenericEntity
{
	[Attribute("{2FCBE5C76E285A7B}Prefabs/AI/Waypoints/AIWaypoint_DefendSmall.et", UIWidgets.ResourceNamePicker, category: "Waypoints")]
	protected ResourceName m_DefaultWaypointPrefab;
	
	[Attribute("{22A875E30470BD4F}Prefabs/AI/Waypoints/AIWaypoint_Patrol.et", UIWidgets.ResourceNamePicker, category: "Waypoints")]
	protected ResourceName m_PatrolWaypointPrefab;
	
	[Attribute("{35BD6541CBB8AC08}Prefabs/AI/Waypoints/AIWaypoint_Cycle.et", UIWidgets.ResourceNamePicker, category: "Waypoints")]
	protected ResourceName m_CycleWaypointPrefab;
	
	[Attribute("{0EC5F76A0DDF05EF}Prefabs/Systems/Compositions/PatrolPoint/LoiterPost.et", UIWidgets.ResourceNamePicker, category: "Loiter")]
	protected ResourceName m_LoiterPoint;
	
	//! Size of grid that is to be used by spawn points
	private static int s_SpawnGridSize = 500;
	
	private bool _isActive = true;
	
	void SetIsActive(bool value) { _isActive = value; }
	bool IsActive() { return _isActive; }
	
	//! Get size of spawn grid
	static int GetSpawnGridSize() { return s_SpawnGridSize; }
	
	//! Change spawn grid. Will re-register already registered points to new grid manager
	static void ChangeSpawnGridSize(int newSize)
	{
		s_SpawnGridSize = newSize;
		ref TW_GridCoordArrayManager<TW_AISpawnPoint> manager = new TW_GridCoordArrayManager<TW_AISpawnPoint>(newSize);
		ref array<TW_AISpawnPoint> items = {};
		int count = s_GridManager.GetAllItems(items);
		
		foreach(TW_AISpawnPoint spawnPoint : items)
			if(spawnPoint)
				manager.InsertByWorld(spawnPoint.GetOrigin(), spawnPoint);
		
		delete s_GridManager;
		s_GridManager = manager;
	}	
	
	static TW_GridCoordArrayManager<TW_AISpawnPoint> GetGridManager() { return s_GridManager; }
	
	//! Manager which will handle grabbing spawn points by grid square
	private static ref TW_GridCoordArrayManager<TW_AISpawnPoint> s_GridManager = new TW_GridCoordArrayManager<TW_AISpawnPoint>(s_SpawnGridSize);
	
	//! Get spawn points in each of the provided chunks
	static void GetSpawnPointsInChunks(notnull set<string> chunks, notnull array<TW_AISpawnPoint> spawnPoints)
	{
		int x, y;
		foreach(string chunk : chunks)
		{
			TW_Util.FromGridString(chunk, x, y);
			if(s_GridManager.HasCoord(x, y))
			{
				TW_GridCoordArray<TW_AISpawnPoint> coord = s_GridManager.GetCoord(x,y);
				
				ref array<TW_AISpawnPoint> points = {};
				coord.GetData(points);
				
				foreach(TW_AISpawnPoint sp : points)
					if(sp.IsActive())
						spawnPoints.Insert(sp);				
			}
		}
	}
	
	static void GetSpawnPointsInChunks(notnull set<string> playerChunks, notnull set<string> antiSpawnChunks, notnull array<TW_AISpawnPoint> spawnPoints)
	{
		int antiRadius = TW_MonitorPositions.GetInstance().GetAntiSpawnGridSizeInMeters();
		
		int x, y;
		ref array<TW_AISpawnPoint> points = {};
		
		// We iterate over our player chunks to see if those chunks exist within our grid manager
		foreach(string chunk : playerChunks)
		{
			TW_Util.FromGridString(chunk, x, y);
			
			if(!s_GridManager.HasCoord(x, y))
				continue;
			
			ref TW_GridCoordArray<TW_AISpawnPoint> coord = s_GridManager.GetCoord(x, y);
			points.Clear();
			coord.GetData(points);
			
			// We then iterate over our spawn points to verify they don't
			// correspond to an antispawn chunk
			foreach(TW_AISpawnPoint spawnPoint : points)
			{
				if(!spawnPoint.IsActive()) 
					continue;
				
				string antiChunk = TW_Util.ToGridText(spawnPoint.GetOrigin(), antiRadius);
				if(antiSpawnChunks.Contains(antiChunk))
					continue;
				spawnPoints.Insert(spawnPoint);
			}
		}
	}
	
	//! Get nearby spawn points around a given point (used by game master, hopefully eliminates weird bordering)
	static void GetNearbySpawnPoints(vector center, notnull array<TW_AISpawnPoint> spawnPoints, int chunkRadius = 1)
	{
		int x, y;
		TW_Util.ToGrid(center, x, y, s_SpawnGridSize);
		
		if(!s_GridManager.HasCoord(x, y))
			return;
		
		ref array<ref TW_GridCoordArray<TW_AISpawnPoint>> items = {};
		s_GridManager.GetNeighbors(items, x, y, chunkRadius);
				
		foreach(ref TW_GridCoordArray<TW_AISpawnPoint> item : items)
		{
			ref array<TW_AISpawnPoint> points = item.GetAll();
			
			foreach(TW_AISpawnPoint spawnPoint : points)
			{
				if(!spawnPoint) continue;
				if(spawnPoint.IsActive())
					spawnPoints.Insert(spawnPoint);
			}		
		}			
	}
		
	static void RegisterSpawnPoint(TW_AISpawnPoint spawnPoint)
	{
		s_GridManager.InsertByWorld(spawnPoint.GetOrigin(), spawnPoint);
	}
	
	static void UnregisterSpawnPoint(TW_AISpawnPoint spawnPoint)
	{
		s_GridManager.RemoveByWorld(spawnPoint.GetOrigin(), spawnPoint);
	}
	
	void TW_AISpawnPoint(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
		SetFlags(EntityFlags.ACTIVE, true);
	}
	
	override void EOnInit(IEntity owner)
	{
		if(!GetGame().InPlayMode())
			return;
		
		RegisterSpawnPoint(this);
		
		EntitySpawnParams params = EntitySpawnParams();
		GetTransform(params.Transform);
		
		Resource loiterResource = Resource.Load(m_LoiterPoint);		
		if(loiterResource.IsValid())
			GetGame().SpawnEntityPrefab(loiterResource, GetWorld(), params);
	}
	
	void AddGroupToPoint(SCR_AIGroup group, ResourceName waypointOverride = ResourceName.Empty, IEntity goToPositionOverride = null, bool shouldPatrol = false, float patrolRadius = 250, int patrolWaypointCount = 5)
	{
		if(!group)
			return;
		
		if(shouldPatrol)
		{
			TW_Util.CreatePatrolPathFor(group, m_PatrolWaypointPrefab, m_CycleWaypointPrefab, patrolWaypointCount, patrolRadius);
			return;
		}
		
		ResourceName waypointPrefab = m_DefaultWaypointPrefab;
		
		if(waypointOverride)
			waypointPrefab = waypointOverride;
		
		GetGame().GetCallqueue().CallLater(SpawnWaypoint, 250, false, group, waypointPrefab);
	}
	
	SCR_AIGroup Spawn(ResourceName groupPrefab, ResourceName waypointOverride = ResourceName.Empty, IEntity goToPositionOverride = null, bool shouldPatrol = false,	float patrolRadius = 250, int patrolWaypointCount = 5)
	{
		if(!IsActive()) 
			return null;
		
		vector spawnPosition;
		
		vector mat[4];
		GetWorldTransform(mat);
		spawnPosition = mat[3];
		
		SCR_AIGroup group = TW_Util.SpawnGroup(groupPrefab, spawnPosition);
		
		if(!group)
		{
			Print("TrainWreck: Was unable to successfully spawn group", LogLevel.ERROR);
			return null;
		}
		
		if(shouldPatrol)
		{
			TW_Util.CreatePatrolPathFor(group, m_PatrolWaypointPrefab, m_CycleWaypointPrefab, patrolWaypointCount, patrolRadius);
			return group;
		}
		
		ResourceName waypointPrefab = m_DefaultWaypointPrefab;
		
		if(waypointOverride)
			waypointPrefab = waypointOverride;
		
		GetGame().GetCallqueue().CallLater(SpawnWaypoint, 250, false, group, waypointPrefab);
		
		return group;
	}
	
	void SpawnWaypoint(SCR_AIGroup group, ResourceName waypointPrefab)
	{				
		if(!group)
			return;
		
		AIWaypoint waypoint = TW_Util.CreateWaypointAt(waypointPrefab, group.GetOrigin());
		
		if(!waypoint)
			Print("TrainWreck: Invalid Waypoint provided.", LogLevel.ERROR);
		
		group.AddWaypoint(waypoint);
	}
};