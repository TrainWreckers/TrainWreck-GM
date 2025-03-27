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
	
	static ref TW_GridCoordArrayManager<TW_VehicleSpawnPoint> s_VehicleGrid = new TW_GridCoordArrayManager<TW_VehicleSpawnPoint>();
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
		
		s_VehicleGrid.InsertByWorld(GetOrigin(), this);		
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
			TW_Util.ToGrid(vehicle.GetOrigin(), x, y, s_VehicleGrid.GetGridSize());		
			
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
	
	static void ChangeSpawnGridSize(int newSize)
	{
		ref TW_GridCoordArrayManager<TW_VehicleSpawnPoint> manager = new TW_GridCoordArrayManager<TW_VehicleSpawnPoint>(newSize);
		ref array<TW_VehicleSpawnPoint> items = {};
		int count = s_VehicleGrid.GetAllItems(items);
		
		foreach(TW_VehicleSpawnPoint spawnPoint : items)
			if(spawnPoint)
				manager.InsertByWorld(spawnPoint.GetOrigin(), spawnPoint);
		
		delete s_VehicleGrid;
		s_VehicleGrid = manager;
	}	
	
	static void GetSpawnPointsInChunks(notnull set<string> chunks, notnull array<TW_VehicleSpawnPoint> spawnPoints)
	{
		int x, y;
		foreach(string chunk : chunks)
		{			
			TW_Util.FromGridString(chunk, x, y);
			
			if(s_VehicleGrid.HasCoord(x, y))
			{
				TW_GridCoordArray<TW_VehicleSpawnPoint> coord = s_VehicleGrid.GetCoord(x,y);
				
				ref array<TW_VehicleSpawnPoint> points = {};
				coord.GetData(points);
				
				foreach(TW_VehicleSpawnPoint sp : points)
					if(sp.IsActive())
						spawnPoints.Insert(sp);				
			}
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
	
	bool SpawnVehicle(ResourceName vehiclePrefab, out IEntity vehicle, float chanceToSpawn)
	{
		if(!m_CanSpawn)
			return false;
		
		float roll = Math.RandomFloat01();
		
		if(roll <= chanceToSpawn)
		{
			m_CanSpawn = false;
			s_VehicleGrid.RemoveByWorld(GetOrigin(), this);
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
		s_VehicleGrid.RemoveByWorld(GetOrigin(), this);		
		
		return true;
	}
	
	vector GetForwardVec()
	{
		return GetTransformAxis(0).Normalized();
	}	
};