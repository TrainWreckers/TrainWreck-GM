[EntityEditorProps(category: "GameScripted/TrainWreck/Systems", description: "Designates an area in which AI Spawn Points will be deactivated/ineligible for spawning")]
class TW_AISafeAreaClass : GenericEntityClass{};

class TW_AISafeArea : GenericEntity
{
	[Attribute("1", UIWidgets.Slider, params: "10 1000 25", category: "Safe Area")]
	protected int SafeZoneRadius;
	
	[Attribute("1", UIWidgets.CheckBox, category: "Safe Area")]
	protected bool IsActivated;
	
	static ref array<TW_AISafeArea> SafeAreas = {};
	
	protected RplComponent rplComp;
	protected TW_RuntimeAreaMesh areaMesh;
	protected IEntity owner;
	
	protected ref array<TW_AISpawnPoint> disabledSpawnPoints = {};
	private vector previousPosition;
	
	override void EOnInit(IEntity owner)
	{
		if(TW_Global.IsInRuntime())
			SafeAreas.Insert(this);
		
		this.owner = owner;
		areaMesh = TW_RuntimeAreaMesh.Cast(owner.FindComponent(TW_RuntimeAreaMesh));
		rplComp = RplComponent.Cast(owner.FindComponent(RplComponent));
		
		if(areaMesh)
			areaMesh.SetRadius(GetRadius());			
		
		previousPosition = GetOrigin();
		
		if(TW_Global.IsServer(owner))
			SetEventMask(EntityEvent.FRAME);
	}
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if(previousPosition != GetOrigin())
		{
			previousPosition = GetOrigin();
			DisableNearbySpawnPoints();
		}
	}
	
	protected void DisableNearbySpawnPoints()
	{
		// Ensure previously removed spawn points from the game
		// are added back BEFORE we disable more
		ReenableSpawnPoints();		
		
		GetWorld().QueryEntitiesBySphere(GetOrigin(), GetRadius(), QueryCallback);
	}
	
	private bool QueryCallback(IEntity e)
	{
		TW_AISpawnPoint comp = TW_AISpawnPoint.Cast(e);
		
		if(comp)
		{
			disabledSpawnPoints.Insert(comp);
			comp.SetIsActive(false);			
		}		
		
		return true;
	}
	
	private void ReenableSpawnPoints()
	{
		if(disabledSpawnPoints.IsEmpty())
			return;
	
		foreach(TW_AISpawnPoint spawnPoint : disabledSpawnPoints)
			spawnPoint.SetIsActive(true);
			
		disabledSpawnPoints.Clear();		
	}
	
	void ~TW_AISafeArea()
	{
		SafeAreas.RemoveItem(this);
		ReenableSpawnPoints();
	}
	
	float GetRadius() { return SafeZoneRadius; }
	void SetRadius(float radius) 
	{ 
		SafeZoneRadius = radius;
		Rpc(RpcUpdateAreaMesh, rplComp.Id(), GetRadius());
		areaMesh.SetRadius(radius);
		
		DisableNearbySpawnPoints();
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RpcUpdateAreaMesh(RplId id, float newRadius)
	{
		SafeZoneRadius = newRadius;
		areaMesh.SetRadius(newRadius);
	}
};