class TW_Util 
{
	static ref RandomGenerator s_Generator = new RandomGenerator();
	
	//! Save Json File: Credit to Bacon
	static bool SaveJsonFile(string path, Managed data, bool useTypeDiscriminator = false) 
	{
		ContainerSerializationSaveContext saveContext = new ContainerSerializationSaveContext(false);
		if(useTypeDiscriminator)
			saveContext.EnableTypeDiscriminator();
		
		PrettyJsonSaveContainer container = new PrettyJsonSaveContainer();
		saveContext.SetContainer(container);
		
		if(!saveContext.WriteValue("", data)) 
		{
			PrintFormat("TrainWreck: Serialization of '%1' failed for file '%2'", data, path, LogLevel.ERROR);
			return false;
		}
		
		return container.SaveToFile(path);
	}
	
	//! Load Json File: Credit to Bacon
	static SCR_JsonLoadContext LoadJsonFile(string path, bool useTypeDiscriminator = false)
	{
		SCR_JsonLoadContext context = new SCR_JsonLoadContext(false);
		
		if(useTypeDiscriminator)
			context.EnableTypeDiscriminator();
		
		if(!context.LoadFromFile(path))
		{
			PrintFormat("TrainWreck: Loading JSON file '%1' failed", path, LogLevel.ERROR);
			return null;
		}
		
		return context;
	}
	
	static vector RandomPositionAround(IEntity point, int radius, int minimumDistance = 0)
	{
		return s_Generator.GenerateRandomPointInRadius(Math.Min(minimumDistance, radius), Math.Max(minimumDistance, radius), point.GetOrigin());
	}
	
	static vector RandomPositionAround(vector point, int radius, int minimumDistance = 0)
	{
		return s_Generator.GenerateRandomPointInRadius(Math.Min(minimumDistance, radius), Math.Max(minimumDistance, radius), point);
	}
	
	static vector RandomPositionAround_ButNotNear(IEntity point, notnull array<IEntity> entities, int radius, int minimumDistance = 0)
	{
		while(true)
		{
			vector position = RandomPositionAround(point, radius, minimumDistance);
			bool skip = false;
			
			// We cannot be within minimum spawn distance of provided entities
			foreach(IEntity entity : entities)
			{
				if(vector.Distance(entity.GetOrigin(), position) < minimumDistance)
				{
					skip = true;
					break;
				}
			}
			
			if(skip)
				continue;
			
			return position;
		}
		
		return vector.One;
	}
	
	static SCR_AIGroup SpawnGroup(ResourceName groupPrefab, vector center, int radius = 1, int minimumDistance = 0)
	{
		if(!groupPrefab || groupPrefab.IsEmpty())
			return null;
		
		if(radius < minimumDistance)
			radius = minimumDistance;
		
		Resource resource = Resource.Load(groupPrefab);
		
		if(!resource || !resource.IsValid())
		{
			Print(string.Format("TrainWreck: Invalid Group Prefab: %1", groupPrefab), LogLevel.ERROR);
			return null;
		}
		
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		
		if(radius <= 4)
			params.Transform[3] = center;
		else
			params.Transform[3] = s_Generator.GenerateRandomPointInRadius(minimumDistance, radius, center);
		
		vector pos = params.Transform[3];
		SCR_WorldTools.FindEmptyTerrainPosition(pos, params.Transform[3], radius, 1, 1, TraceFlags.ENTS | TraceFlags.WORLD);
		params.Transform[3] = pos;
		
		return SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params));
	}
	
	static float GetWaterSurfaceY(BaseWorld world, vector worldPos, out EWaterSurfaceType waterSurfaceType, out float lakeArea)
	{
		vector obbExtens;
		vector transformWS[4];
		vector waterSurfacePos;
		
		ChimeraWorldUtils.TryGetWaterSurface(world, worldPos, waterSurfacePos, waterSurfaceType, transformWS, obbExtens);
		
		lakeArea = obbExtens[0] * obbExtens[2];
		return waterSurfacePos[1];
	}
	
	static vector GetMidpoint(vector one, vector two, int div = 2)
	{
		float x = (one[0] + two[0]) / div;
		float y = (one[1] + two[1]) / div;
		float z = (one[2] + two[2]) / div;
		
		return Vector(x, y, z);
	}
	
	static bool IsWater(vector worldPosition)
	{		
		bool isWater = false;
		BaseWorld world = GetGame().GetWorld();
		
		float surfaceHeight =  world.GetSurfaceY(worldPosition[0], worldPosition[2]);
		
		worldPosition[1] = surfaceHeight;
		float lakeArea;
		EWaterSurfaceType waterSurfaceType;
		
		float waterSurfaceY = GetWaterSurfaceY(world, worldPosition, waterSurfaceType, lakeArea);
		
		if(waterSurfaceType == EWaterSurfaceType.WST_OCEAN || waterSurfaceType == EWaterSurfaceType.WST_POND)
			isWater = true;
		
		return isWater;
	}		
	
	static void CreatePatrolPathFor(SCR_AIGroup group, ResourceName waypointPrefab, ResourceName cyclePrefab, int waypointCount, float radius)
	{
		vector currentPoint = GetLandPositionAround(group.GetOrigin(), radius);
		
		for(int i = 0; i < waypointCount; i++)
		{
			AIWaypoint waypoint = CreateWaypointAt(waypointPrefab, currentPoint);
			
			if(!waypoint)
				continue;
			
			group.AddWaypoint(waypoint);
			currentPoint = GetLandPositionAround(currentPoint, radius);
		}
		
		AIWaypoint cycle = CreateWaypointAt(cyclePrefab, group.GetOrigin());
		
		if(!cycle)
			return;
		
		group.AddWaypoint(cycle);
	}
	
	static vector GetLandPositionAround(vector center, int radius)
	{
		vector mat[4];
		vector position = mat[3];
		int attempts = 25;
		
		position = s_Generator.GenerateRandomPointInRadius(0, radius, center);
		position[1] = GetGame().GetWorld().GetSurfaceY(position[0], position[2]);
		
		int currentRadius = radius * (attempts / 25);
		while(IsWater(position) && attempts > 0)
		{
			attempts -= 1;
			position = s_Generator.GenerateRandomPointInRadius(0, currentRadius, center);
			position[1] = GetGame().GetWorld().GetSurfaceY(position[0], position[1]);
		}
		
		return position;
	}
	
	static vector GetLandPositionAround(IEntity center, int radius)
	{
		return GetLandPositionAround(center.GetOrigin(), radius);
	}
	
	static AIWaypoint CreateWaypointAt(ResourceName waypointPrefab, vector waypointPosition)
	{
		Resource resource = Resource.Load(waypointPrefab);
		
		if(!resource) return null;
		
		AIWaypoint wp = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(resource));
		if(!wp) return null;
		
		wp.SetOrigin(waypointPosition);
		return wp;
	}
	
	//! Output x and y coordinates for center of a grid square
	static void GetCenterOfGridSquare(vector position, out int x, out int y, int gridSize = 1000)
	{
		ToGrid(position, x, y, gridSize);
		
		x = (x * gridSize) + (gridSize / 2);
		y = (y * gridSize) + (gridSize / 2);
	}
	
	//! Add chunks in a radius around position to chunk set
	static void AddSurroundingGridSquares(notnull inout set<string> chunks, vector position, int radius = 1, int gridSize = 1000)
	{
		int x, y;
		ToGrid(position, x, y, gridSize);
		
		int xStart = x - radius;
		int xEnd = x + radius;
		int yStart = y - radius;
		int yEnd = y + radius;
		
		string text = "";
		
		for(int gx = xStart; gx <= xEnd; gx++)
		{
			for(int gy = yStart; gy <= yEnd; gy++)
			{
				text = string.Format("%1 %2", gx, gy);
				if(!chunks.Contains(text))
					chunks.Insert(text);
			}
		}
	}
	
	//! Convert position to grid text
	static string ToGridText(vector position, int gridSize = 1000)
	{
		int x, y;
		ToGrid(position, x, y, gridSize);
		return string.Format("%1 %2", x, y);
	}
	
	//! Convert World Position to grid-based coordinates
	static void ToGrid(vector position, out int x, out int y, int gridSize = 1000)
	{
		if(gridSize < 10)
			gridSize = 1000;
		
		x = position[0] / gridSize;
		y = position[2] / gridSize;
	}
	
	//! Extract x and y from Grid Text
	static void FromGridString(string grid, out int x, out int y)
	{		
		ref array<int> nums = SCR_StringHelper.GetIntsFromString(grid, " ");
		if(nums.Count() >= 2)
		{
			x = nums[0];
			y = nums[1];
		}
	}
	
	protected static ref map<ResourceName, ref UIInfo> s_ItemUIInfo = new map<ResourceName, ref UIInfo>();
	protected static ref map<ResourceName, ref SCR_EditableVehicleUIInfo> s_VehicleUIInfo = new map<ResourceName, ref SCR_EditableVehicleUIInfo>();
	
	static string GetNameFromPath(ResourceName path)
	{		
		string search = "" + path;
		
		int length = path.Length() - 1;
		
		int index = -1;
		for(int i = length; i > 0; i--)
		{
			if(search.Get(i) == "/")
			{
				index = i + 1;
				break;
			}
		}
		
		if(index != -1)
			return path.Substring(index, path.Length() - index);
		return path;		
	}
	
	static string ArsenalTypeAsString(SCR_EArsenalItemType type)
	{		
		return SCR_Enum.GetEnumName(SCR_EArsenalItemType, type);		
	}
	
	static int GetPlayerId()
	{
		IEntity playerEntity = SCR_PlayerController.GetLocalControlledEntity();
		
		if(!playerEntity) return 0;
		return GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(playerEntity);
	}
	
	static string GetPlayerLootFileName(int playerId = -1)
	{
		if(playerId < 0)
			playerId = GetPlayerId();
		
		return GetGame().GetPlayerManager().GetPlayerName(playerId) + ".json";
	}
	
	static bool PlayerHasLootFile(int playerId = -1)
	{
		string filename = GetPlayerLootFileName(playerId);
		
		SCR_JsonLoadContext context = new SCR_JsonLoadContext();
		
		return context.LoadFromFile(filename);
	}
	
	static map<string, int> DeserializeLootJson(string jsonContents)
	{
		MenuManager menuManager = GetGame().GetMenuManager();
		SCR_InventoryMenuUI inventoryMenu = SCR_InventoryMenuUI.Cast(menuManager.OpenMenu(ChimeraMenuPreset.Inventory20Menu));
		
		SCR_JsonLoadContext context = new SCR_JsonLoadContext();
		
		if(!context.ImportFromString(jsonContents))
		{
			Print(string.Format("TrainWreck: Player failed to load jsonContents %1", jsonContents), LogLevel.ERROR);
			return null;
		}
		
		ref map<string, int> items;		
		if(!context.ReadValue("items", items))
		{
			Print(string.Format("TrainWreck: Player failed to load items from jsonContents: %1", jsonContents), LogLevel.ERROR);
			return null;
		}
		
		return items;
	}
	
	static void ResetInventoryMenu()
	{
		MenuManager menuManager = GetGame().GetMenuManager();
		menuManager.CloseAllMenus();
		SCR_InventoryMenuUI inventoryMenu = SCR_InventoryMenuUI.Cast(menuManager.OpenMenu(ChimeraMenuPreset.Inventory20Menu));
	}
	
	static UIInfo GetItemUIInfo(ResourceName prefab)
	{
		UIInfo resultInfo = s_ItemUIInfo.Get(prefab);
		
		if(!resultInfo)
		{
			IEntitySource entitySource = SCR_BaseContainerTools.FindEntitySource(Resource.Load(prefab));
			
			if(entitySource)
			{
				for(int nComponent, componentCount = entitySource.GetComponentCount(); nComponent < componentCount; nComponent++)
				{
					IEntityComponentSource componentSource = entitySource.GetComponent(nComponent);
					
					if(componentSource.GetClassName().ToType().IsInherited(InventoryItemComponent))
					{
						BaseContainer attributesContainer = componentSource.GetObject("Attributes");
						if(attributesContainer)
						{
							BaseContainer itemDisplayNameContainer = attributesContainer.GetObject("ItemDisplayName");
							if(itemDisplayNameContainer)
							{
								resultInfo = UIInfo.Cast(BaseContainerTools.CreateInstanceFromContainer(itemDisplayNameContainer));
								break;
							}
						}
					}
				}
			}
			
			s_ItemUIInfo.Set(prefab, resultInfo);
		}
		
		return resultInfo;
	}
	
	static SCR_EditableVehicleUIInfo GetVehicleUIInfo(ResourceName prefab)
	{
		SCR_EditableVehicleUIInfo resultInfo = s_VehicleUIInfo.Get(prefab);
		
		if(!resultInfo)
		{
			IEntitySource entitySource = SCR_BaseContainerTools.FindEntitySource(Resource.Load(prefab));
			
			if(entitySource)
			{
				for(int nComponent, componentCount = entitySource.GetComponentCount(); nComponent < componentCount; nComponent++)
				{
					IEntityComponentSource componentSource = entitySource.GetComponent(nComponent);
					
					if(componentSource.GetClassName().ToType().IsInherited(SCR_EditableVehicleComponent))
					{
						BaseContainer baseUIInfo = componentSource.GetObject("m_UIInfo");
						if(baseUIInfo)
						{
							resultInfo = SCR_EditableVehicleUIInfo.Cast(BaseContainerTools.CreateInstanceFromContainer(baseUIInfo));
							break;
						}
					}
				}
			}
			
			s_VehicleUIInfo.Set(prefab, resultInfo);
		}
		
		return resultInfo;
	}
	
	private static ref map<ResourceName, vector> s_VehicleSizes = new map<ResourceName, vector>();
	static vector GetEntitySize(ResourceName prefab, BaseWorld world = null)
	{
		if(s_VehicleSizes.Contains(prefab))
			return s_VehicleSizes.Get(prefab);
		
		if(!world)
			world = GetGame().GetWorld();
		
		Resource prefabResource = Resource.Load(prefab);
		if(!prefabResource.IsValid())
			return vector.Zero;
		
		IEntity spawnedEntity = GetGame().SpawnEntityPrefab(prefabResource, world, EntitySpawnParams());
		
		if(!spawnedEntity)
		{
			s_VehicleSizes.Set(prefab, vector.One);
			return vector.One;
		}
		
		vector size = SCR_EntityHelper.GetEntitySize(spawnedEntity);
		s_VehicleSizes.Set(prefab, size);
		
		GetGame().GetCallqueue().CallLater(DeleteLater, 500, false, spawnedEntity);
		
		return size;
	}
	
	private static void DeleteLater(IEntity entity)
	{
		SCR_EntityHelper.DeleteEntityAndChildren(entity);
	}
	
	static string GetPrefabDisplayName(ResourceName prefab)
	{
		SCR_EditableVehicleUIInfo uiInfo = GetVehicleUIInfo(prefab);
		
		if(uiInfo)
			return uiInfo.GetName();
		
		UIInfo itemUIInfo = GetItemUIInfo(prefab);
		if(itemUIInfo)
			return itemUIInfo.GetName();
		
		return prefab;
	}
	
	static string GetPrefabDescription(ResourceName prefab)
	{
		SCR_EditableVehicleUIInfo uiInfo = GetVehicleUIInfo(prefab);
		
		if(uiInfo)
			return uiInfo.GetDescription();
		
		UIInfo itemInfo = GetItemUIInfo(prefab);
		
		if(itemInfo)
			return itemInfo.GetDescription();
				
		return prefab;
	}
	
	static ResourceName GetPrefabDisplayIcon(ResourceName prefab)
	{
		SCR_EditableVehicleUIInfo uiInfo = GetVehicleUIInfo(prefab);
		
		if(uiInfo)
			return uiInfo.GetIconPath();
		
		UIInfo itemInfo = GetItemUIInfo(prefab);
		if(itemInfo)
			return itemInfo.GetIconPath();
		
		return string.Empty;
	}
	
	static bool InsertAutoEquipItem(SCR_InventoryStorageManagerComponent inventory, IEntity item)
	{
		EStoragePurpose purpose = EStoragePurpose.PURPOSE_ANY;
		if(item.FindComponent(WeaponComponent)) purpose = EStoragePurpose.PURPOSE_WEAPON_PROXY;
		if(item.FindComponent(BaseLoadoutClothComponent)) purpose = EStoragePurpose.PURPOSE_LOADOUT_PROXY;
		if(item.FindComponent(SCR_GadgetComponent)) purpose = EStoragePurpose.PURPOSE_GADGET_PROXY;
		
		bool insertedItem = inventory.TryInsertItem(item, purpose, null);
		
		if(!insertedItem)
			insertedItem = inventory.TryInsertItem(item, EStoragePurpose.PURPOSE_ANY, null);
		
		return insertedItem;
	}
	
	//! Retrieve translated display name for a resource
	static string GetTranslatedDisplayName(ResourceName resource)
	{
		return WidgetManager.Translate(GetPrefabDisplayName(resource));
	}
	
	private static ResourceName s_EmptyGroupPrefab = "{9AF0548E8758756E}Prefabs/Groups/Group_Empty.et";
	
	static SCR_AIGroup CreateNewGroup(TW_AISpawnPoint spawnPoint, string factionKey, ResourceName characterPrefab, int groupSize)
	{
		SCR_AIGroup group = TW_Util.SpawnGroup(s_EmptyGroupPrefab, spawnPoint.GetOrigin(), 1, 0);
		
		// Have to set the group faction to selected faction
		group.SetFaction(GetGame().GetFactionManager().GetFactionByKey(factionKey));
		
		// Need to tell the group which prefabs to spawn
		for(int i = 0; i < groupSize; i++)			
			group.m_aUnitPrefabSlots.Insert(characterPrefab);
		
		// Finally spawn those units
		group.SpawnUnits();
		
		// Add them to point
		spawnPoint.AddGroupToPoint(group);
		
		return group;
	}
	
	//! Set vehicle to have random fuel between min/max
	static void SetVehicleFuel(IEntity vehicle, float min, float max)
	{
		SCR_FuelManagerComponent fuelManager = TW<SCR_FuelManagerComponent>.Find(vehicle);
		
		if(!fuelManager)
			return;
		
		float fuel = Math.RandomFloatInclusive(min, max);
		fuelManager.SetTotalFuelPercentage(fuel);				
	}
	
	//! Apply damage between min/max to a percentage of hit zones on vehicle at random
	static void SetVehicleDamage(IEntity vehicle, float zonePercent, float min, float max)
	{
		DamageManagerComponent damageManager = TW<DamageManagerComponent>.Find(vehicle);
		
		if(!damageManager)
			return;
		
		ref array<HitZone> zones = {};
		damageManager.GetAllHitZones(zones);
		
		int count = zones.Count() * zonePercent;
		
		for(int i = 0; i < count; i++)
		{
			HitZone zone = zones.Get(i);
			float maxHealth = zone.GetMaxHealth();
			float damagePercent = Math.RandomFloatInclusive(min, max);
			float damage = maxHealth * damagePercent;
			zone.HandleDamage(damage, EDamageType.KINETIC, vehicle);			
		}				
	}
};