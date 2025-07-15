[EntityEditorProps(category: "GameScripted/TrainWreck/Entites", description: "This entity will spawn units in buildings")]
class TW_SpawnInBuildingsClass : GenericEntityClass {};

class TW_SpawnInBuildings : GenericEntity
{	
	//! Prefab to spawn
	protected ResourceName m_Prefab;
	
	[Attribute("", UIWidgets.Auto, "Faction")]
	protected ref SCR_Faction m_FactionToSpawn;
	
	[Attribute("{9AF0548E8758756E}Prefabs/Groups/Group_Empty.et", UIWidgets.ResourceNamePicker, params: "et")]
	protected ResourceName m_EmptyGroupPrefab;
	
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
	
	void Initialize(float radius, ResourceName prefab, int amount)
	{		
		// Ensure we're empty in case this is called again in the future
		m_Prefab = prefab;					
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
			
			GetGame().GetCallqueue().CallLater(Handle, 1000 * 3, false);
		}
	}
	
	private void Handle()
	{
		if(!m_Prefab)
		{
			PrintFormat("System does not have prefabs to use. Selected Faction: %1", m_FactionToSpawn, LogLevel.ERROR);
			return;
		}
		
		TrickleSpawn((int)m_AmountToSpawn);		
	}
	
	private void Delete()
	{
		RplComponent.DeleteRplEntity(m_Owner, false);
	}
	
	private void TrickleSpawn(int amountLeft)
	{
		if(!m_Prefab)
		{
			Delete();
			return;
		}
		
		if(amountLeft <= 0)
		{
			Delete();
			return;
		}
		
		TW_AISpawnPoint spawnPoint = m_NearbySpawnpoints.GetRandomElement();
		
		if(!spawnPoint)
			return;
		
		CreateNewGroup(spawnPoint);
		amountLeft--;
		
		// Trickle spawn to avoid slowdown
		GetGame().GetCallqueue().CallLater(TrickleSpawn, 250, false, amountLeft);
	}
	
	private void CreateNewGroup(TW_AISpawnPoint spawnPoint)
	{
		SCR_AIGroup group = TW_Util.SpawnGroup(m_EmptyGroupPrefab, spawnPoint.GetOrigin(), 1, 0);
		
		// Have to set the group faction to selected faction
		group.SetFaction(m_FactionToSpawn);
		
		// Need to tell the group which prefabs to spawn
		group.m_aUnitPrefabSlots.Insert(m_Prefab);
		
		// Finally spawn those units
		group.SpawnUnits();
		
		// Add them to point
		spawnPoint.AddGroupToPoint(group);
	}
	
	private void GetNearbySpawnPoints()
	{
		m_NearbySpawnpoints.Clear();		
		ref array<TW_AISpawnPoint> nearbySpawnPoints = {};
		TW_AISpawnPointGrid.GetInstance().GetNearbySpawnPoints(GetOrigin(), nearbySpawnPoints);
				
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
	
	void SetPrefab(ResourceName prefab) { m_Prefab = prefab; }
	ResourceName GetPrefab() { return m_Prefab; }
	
	SCR_Faction GetFaction() { return m_FactionToSpawn; }
	
	void SetFaction(SCR_Faction faction) 
	{ 		
		m_FactionToSpawn = faction;
		Print("Faction changed", LogLevel.WARNING);
	}		
};