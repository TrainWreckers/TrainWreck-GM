[EntityEditorProps(category: "GameScripted/TrainWreck/Systems", description: "Spawn point for units, and behaves as waypoints when using TrainWreck Systems")]
class TW_AISpawnPointClass : GenericEntityClass{};

class TW_AISpawnPoint : GenericEntity
{
	[Attribute("{FAD1D789EE291964}Prefabs/AI/Waypoints/AIWaypoint_Defend_Large.et", UIWidgets.ResourceNamePicker, category: "Waypoints")]
	protected ResourceName m_DefaultWaypointPrefab;
	
	[Attribute("{22A875E30470BD4F}Prefabs/AI/Waypoints/AIWaypoint_Patrol.et", UIWidgets.ResourceNamePicker, category: "Waypoints")]
	protected ResourceName m_PatrolWaypointPrefab;
	
	[Attribute("{35BD6541CBB8AC08}Prefabs/AI/Waypoints/AIWaypoint_Cycle.et", UIWidgets.ResourceNamePicker, category: "Waypoints")]
	protected ResourceName m_CycleWaypointPrefab;

	static ref array<TW_AISpawnPoint> s_GlobalSpawnPoints = {};
	
	void TW_AISpawnPoint(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
		SetFlags(EntityFlags.ACTIVE, true);
	}
	
	override void EOnInit(IEntity owner)
	{
		if(!GetGame().InPlayMode())
			return;
		
		s_GlobalSpawnPoints.Insert(this);
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
			Print("TrainWrecl: Was unable to successfully spawn group", LogLevel.ERROR);
			return null;
		}
		
		if(shouldPatrol)
		{
			TW_Util.CreatePatrolPathFor(group, m_PatrolWaypointPrefab, m_CycleWaypointPrefab, patrolWaypointCount, patrolRadius);
			return group;
		}
		
		ResourceName waypointPrefab = m_DefaultWaypointPrefab;
		
		if(!waypointOverride)
			waypointPrefab = waypointOverride;
		
		AIWaypoint waypoint = TW_Util.CreateWaypointAt(waypointPrefab, spawnPosition);
		
		if(!waypoint)
		{
			Print("TrainWreck: Invalid Waypoint provided.", LogLevel.ERROR);
			return group;
		}
		
		group.AddWaypoint(waypoint);
		return group;
	}
};