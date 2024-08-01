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

	private static ref TW_GridCoordArrayManager<TW_AISpawnPoint> s_GridManager = new TW_GridCoordArrayManager<TW_AISpawnPoint>(500);
	
	static void GetNearbySpawnPoints(vector center, notnull array<TW_AISpawnPoint> spawnPoints)
	{
		int x, y;
		TW_Util.ToGrid(center, x, y, 500);
		
		if(!s_GridManager.HasCoord(x, y))
			return;
		
		TW_GridCoordArray<TW_AISpawnPoint> coord = s_GridManager.GetCoord(x, y);
		spawnPoints.InsertAll(coord.GetAll());
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