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

	//! Size of grid that is to be used by spawn points
	private static int s_SpawnGridSize = 500;
	
	//! Get size of spawn grid
	static int GetSpawnGridSize() { return s_SpawnGridSize; }
	
	//! Change spawn grid. Will re-register already registered points to new grid manager
	static void ChangeSpawnGridSize(int newSize)
	{
		s_SpawnGridSize = newSize;
		ref TW_GridCoordArrayManager<TW_AISpawnPoint> manager = new TW_GridCoordArrayManager<TW_AISpawnPoint>();
		ref array<TW_AISpawnPoint> items = {};
		int count = s_GridManager.GetAllItems(items);
		
		foreach(TW_AISpawnPoint spawnPoint : items)
			manager.InsertByWorld(spawnPoint.GetOrigin(), spawnPoint);
		
		delete s_GridManager;
		s_GridManager = manager
	}
	
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
				coord.GetData(spawnPoints);
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
			spawnPoints.InsertAll(item.GetAll());				
	}
		
	static void RegisterSpawnPoint(TW_AISpawnPoint spawnPoint)
	{
		s_GridManager.InsertByWorld(spawnPoint.GetOrigin(), spawnPoint);
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
		AIWaypoint waypoint = TW_Util.CreateWaypointAt(waypointPrefab, group.GetOrigin());
		
		if(!waypoint)
			Print("TrainWreck: Invalid Waypoint provided.", LogLevel.ERROR);
		
		group.AddWaypoint(waypoint);
	}
};