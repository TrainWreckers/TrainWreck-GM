enum TW_VehicleType
{
	Small = 1 << 1,
	Medium = 1 << 2,
	Large = 1 << 3,
	Air = 1 << 4
};

[EntityEditorProps(category: "GameScripted/TrainWreck", description: "Vehicle Spawn Point")]
class TW_VehicleSpawnPointClass : GenericEntityClass{};
class TW_VehicleSpawnPoint : GenericEntity 
{	
	[Attribute("0", UIWidgets.Flags, "", enums: ParamEnumArray.FromEnum(TW_VehicleType))]
	protected TW_VehicleType m_AllowedVehicleTypes;
	
	protected bool m_CanSpawn = true;
	
	protected RplComponent m_RplComponent;
	
	override void EOnInit(IEntity owner)
	{
		m_RplComponent = TW<RplComponent>.Find(owner);		
		
		if(!TW_AISpawnManager.GetInstance())
			return;
		
		TW_AISpawnManager.GetInstance().GetVehicleSpawnGrid().InsertByWorld(GetOrigin(), this);		
	}
	
	bool CanSpawnVehicleType(TW_VehicleType type)
	{
		return SCR_Enum.HasFlag(m_AllowedVehicleTypes, type);
	}
	
	bool SpawnVehicle(ResourceName vehiclePrefab, out IEntity vehicle)
	{
		if(!m_CanSpawn)
			return false;
		
		if(!m_RplComponent.IsMaster() || m_RplComponent.Role() != RplRole.Authority)
			return false;
		
		Resource resource = Resource.Load(vehiclePrefab);
		
		if(!resource.IsValid())
			return false;
		
		EntitySpawnParams params = EntitySpawnParams();
		GetTransform(params.Transform);
		params.TransformMode = ETransformMode.WORLD;
		
		vehicle = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
		
		if(!vehicle)
			return false;
		
		return true;
	}
	
	vector GetForwardVec()
	{
		return GetTransformAxis(0).Normalized();
	}
	
	#ifdef WORKBENCH
	
	[Attribute(defvalue: "1", desc: "Show the debug shapes in workbench", category: "Debug")]
	protected bool m_ShowDebugShapesInWorkbench;
	
	protected WorldEditorAPI m_API;
	protected IEntity m_PreviewEntity;
	protected ref Shape m_DebugShape;
	protected int m_DebugShapeColor = Color.CYAN;
	
	override void _WB_AfterWorldUpdate(float timeSlice)
	{
		DrawDebugShape();
	}
	
	override bool _WB_OnKeyChanged(BaseContainer src, string key, BaseContainerList ownerContainers, IEntity parent)
	{
		if(key == "m_ShowDebugShapesInWorkbench")
			DrawDebugShape();
		return true;
	}
	
	protected void DrawDebugShape()
	{
		if(!m_ShowDebugShapesInWorkbench)
			return;
		
		int shapeFlags = ShapeFlags.DOUBLESIDE;
		
		vector origin = GetOrigin();
		
		vector start = Vector(0, 1, -5);
		vector end = Vector(0, 1, 5);
		
		m_DebugShape = Shape.CreateArrow(start, end, 3, m_DebugShapeColor, shapeFlags);
		
		vector globalTransform[4];
		GetTransform(globalTransform);
		m_DebugShape.SetMatrix(globalTransform);			
	}
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if(m_ShowDebugShapesInWorkbench && m_DebugShape)
		{
			vector transform[4];
			GetTransform(transform);
			m_DebugShape.SetMatrix(transform);
		}
	}
	
	#endif
};