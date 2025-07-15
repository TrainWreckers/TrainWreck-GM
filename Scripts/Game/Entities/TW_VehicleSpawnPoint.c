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
	
	TW_VehicleType GetAllowedVehicleTypes() { return m_AllowedVehicleTypes; }
	
	private static ref array<IEntity> _vehicles = {};
	
	void TW_VehicleSpawnPoint(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
		SetFlags(EntityFlags.ACTIVE, true);
	}
	
	override void EOnInit(IEntity owner)
	{
		if(!GetGame().InPlayMode())
			return;
		
		m_RplComponent = TW<RplComponent>.Find(owner);
		TW_VehicleSpawnPointGrid.GetInstance().RegisterSpawnPoint(this);
	}
	
	static void ManageVehicles(notnull set<string> playerChunks, int maxDistance = 3)
	{
		int count = _vehicles.Count();
		
		for(int i = 0; i < count; i++)
		{
			auto vehicle = _vehicles.Get(i);
			
			if(vehicle == null)
			{
				_vehicles.Remove(i);
				i -= 1;
				count -= 1;
			}			
		}
		
		int x, y, px, py;
		
		foreach(IEntity vehicle : _vehicles)
		{			
			TW_Util.ToGrid(vehicle.GetOrigin(), x, y, TW_VehicleSpawnPointGrid.GetInstance().GetGridSize());		
			
			bool isClose = false;
							
			foreach(string chunk : playerChunks)
			{
				TW_Util.FromGridString(chunk, px, py);
				
				int xDiff = Math.AbsInt(px - x);
				int yDiff = Math.AbsInt(py - y);
				
				if(xDiff <= maxDistance || yDiff <= maxDistance)
				{
					isClose = true;
					break;
				}
			}
			
			if(!isClose)				
				SCR_EntityHelper.DeleteEntityAndChildren(vehicle);
		}
	}
	
	bool CanSpawnVehicleType(TW_VehicleType type)
	{
		return SCR_Enum.HasFlag(m_AllowedVehicleTypes, type);
	}
	
	bool IsActive()
	{
		return m_CanSpawn;
	}
	
	bool SpawnVehicle(ResourceName vehiclePrefab, out IEntity vehicle, float chanceToSpawn, bool shouldRemove=true)
	{
		if(!m_CanSpawn)
			return false;
		
		float roll = Math.RandomFloat01();
		
		if(roll <= chanceToSpawn && shouldRemove)
		{
			m_CanSpawn = false;
			TW_VehicleSpawnPointGrid.GetInstance().UnregisterSpawnPoint(this);
		}
		
		Resource resource = Resource.Load(vehiclePrefab);
		
		if(!resource.IsValid())
			return false;
		
		EntitySpawnParams params = EntitySpawnParams();
		GetTransform(params.Transform);
		params.TransformMode = ETransformMode.WORLD;
		
		vehicle = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
		
		if(!vehicle)
			return false;
		
		_vehicles.Insert(vehicle);
		
		m_CanSpawn = false;
		
		if(shouldRemove)
			TW_VehicleSpawnPointGrid.GetInstance().UnregisterSpawnPoint(this);
		
		return true;
	}
	
	vector GetForwardVec()
	{
		return GetTransformAxis(0).Normalized();
	}	
};