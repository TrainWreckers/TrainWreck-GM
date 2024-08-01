[EntityEditorProps(category: "GameScripted/TrainWreck/Entites", description: "This entity will spawn units in buildings")]
class TW_SpawnInBuildingsClass : GenericEntityClass {};

class TW_SpawnInBuildings : GenericEntity
{	
	[Attribute("", UIWidgets.ResourceNamePicker, params: "et")]
	// Prefabs to spawn at positions nearby
	protected ref array<ResourceName> m_GroupPrefabs;
	
	[Attribute("1", UIWidgets.ComboBox, "Projection type", "", ParamEnumArray.FromEnum(TW_EFactionType))]
	protected TW_EFactionType m_FactionType;
	
	//! Can this system spawn more units?
	protected bool m_RespawnEnabled = false;
	
	//! If respawn enabled, time until system spawns more units
	protected int m_RespawnTimerInMinutes = 500;
	
	//! If respawn enabled, distance players must be in order to spawn
	protected float m_MinimumPlayerDistance = 300;
	
	//! Chance a group/unit prefab will patrol immediately upon spawning.	
	protected float m_ChanceToPatrolOnSpawn = 0.3;
	
	//! Cached spawn points within radius
	protected ref array<TW_AISpawnPoint> m_NearbySpawnpoints = {};
	
	[Attribute("10", UIWidgets.Slider, params: "1 50 1")]
	//! Amount of units to manage/spawn
	protected int m_AmountToSpawn;
	
	[Attribute("50", UIWidgets.Slider, params: "10 1000 10")]
	protected float m_SpawnRadius;
	
	protected bool m_IsActive = false;
	
	protected RplComponent m_RplComp;
	protected TW_RuntimeAreaMesh m_AreaMesh;
	private bool HasFired = false;
	private IEntity m_Owner;
	
	void Initialize(float radius, notnull array<ResourceName> groupPrefabs, int amount)
	{		
		// Ensure we're empty in case this is called again in the future
		m_GroupPrefabs.Clear(); 				
		m_GroupPrefabs.InsertAll(groupPrefabs);
		
		m_AmountToSpawn = amount;
		m_SpawnRadius = radius;				
		
		GetNearbySpawnPoints();
	}
	
	override void EOnInit(IEntity owner)
	{
		if(!GetGame().InPlayMode())
			return;
		
		m_Owner = owner;
		m_AreaMesh = TW_RuntimeAreaMesh.Cast(owner.FindComponent(TW_RuntimeAreaMesh));
		
		m_RplComp = RplComponent.Cast(owner.FindComponent(RplComponent));
		
		if(m_AreaMesh)
			m_AreaMesh.SetRadius(GetSpawnRadius());
		
		if(!m_RplComp || !m_RplComp.IsProxy())
			SetEventMask(EntityEvent.FRAME);
	}
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if(!m_IsActive)
			return;
		
		if(!HasFired)
		{
			HasFired = true;
			GetNearbySpawnPoints();
			
			if(m_NearbySpawnpoints.Count() <= 0)
			{				
				Delete();
				return;
			}
			
			TrickleSpawn((int)m_AmountToSpawn);		
		}
	}
	
	private void Delete()
	{
		RplComponent.DeleteRplEntity(m_Owner, false);
	}
	
	private void TrickleSpawn(int amountLeft)
	{
		if(amountLeft <= 0)
		{
			Delete();
			return;
		}
		
		TW_AISpawnPoint spawnPoint = m_NearbySpawnpoints.GetRandomElement();
		
		if(!spawnPoint)
			return;
		
		spawnPoint.Spawn(m_GroupPrefabs.GetRandomElement());
		amountLeft--;
		
		// Trickle spawn to avoid slowdown
		GetGame().GetCallqueue().CallLater(TrickleSpawn, 250, false, amountLeft);
	}
	
	private void GetNearbySpawnPoints()
	{
		m_NearbySpawnpoints.Clear();		
		ref array<TW_AISpawnPoint> nearbySpawnPoints = {};
		TW_AISpawnPoint.GetNearbySpawnPoints(GetOrigin(), nearbySpawnPoints);
				
		foreach(TW_AISpawnPoint spawnPoint : nearbySpawnPoints)
		{
			if(!spawnPoint)
			{
				Print("Why is this null", LogLevel.WARNING);
				continue;
			}
			
			if(vector.Distance(GetOrigin(), spawnPoint.GetOrigin()) <= m_SpawnRadius)
				m_NearbySpawnpoints.Insert(spawnPoint);
		}
	}
	
	float GetSpawnRadius() { return m_SpawnRadius; }
	void SetSpawnRadius(float radius) 
	{ 
		m_SpawnRadius = radius; 
		
		Rpc(RpcUpdateAreaMesh, m_RplComp.Id(), m_SpawnRadius);
		m_AreaMesh.SetRadius(radius);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RpcUpdateAreaMesh(RplId id, float newRadius)
	{
		m_SpawnRadius = newRadius;
		m_AreaMesh.SetRadius(newRadius);
	}
	
	int GetSpawnAmount() { return m_AmountToSpawn; }
	void SetSpawnAmount(int amount) { m_AmountToSpawn = amount; }
	
	float GetPatrolChance() { return m_ChanceToPatrolOnSpawn; }
	void SetPatrolChance(float chance) { m_ChanceToPatrolOnSpawn = chance; }
	
	bool IsActive() { return m_IsActive; }
	void SetIsActive(bool value) { m_IsActive = value; }
	
	
	TW_EFactionType GetFactionType() { return m_FactionType; }
	void SetFactionType(TW_EFactionType factionType)
	{
		m_FactionType = factionType;
	}
	
	void SetPrefabs(notnull array<ResourceName> prefabs)
	{
		m_GroupPrefabs.Clear();
		m_GroupPrefabs.InsertAll(prefabs);
	}
	
	array<ResourceName> GetPrefabs()
	{
		return m_GroupPrefabs;
	}
};