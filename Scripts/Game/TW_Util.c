/*!
	The reason for separate helper class is because for whatever reason when I tried using strings
	with the TW_IterableHelper, it would say 

	"auto-pointer 'string' must be class-type"

	Despite the same code for SCR_ArrayHelper working with strings. So I made the separate class specifically for strings
*/
class TW_IterableStringHelper
{
	static int GetIntersection(notnull array<string> one, notnull array<string> two, inout array<string> common)
	{
		int count = 0;
		
		foreach(string item : one)
		{
			if(!two.Contains(item))
				continue;
			
			common.Insert(item);
			count++;
		}
		
		return count;
	}
	
	static int GetIntersection(notnull set<string> one, notnull set<string> two, inout set<string> common)
	{
		int count = 0;
		
		foreach(string item : one)
		{
			if(!two.Contains(item))
				continue;
			common.Insert(item);
			count++;
		}
		
		return count;
	}
	
	static int GetDifference(notnull array<string> one, notnull array<string> two, inout array<string> diff)
	{
		int count = 0;
		foreach(string item : one)
		{
			if(two.Contains(item))
				continue;
			diff.Insert(item);
			count++;
		}
		
		return count;
	}
	
	static int GetDifference(notnull set<string> one, notnull set<string> two, inout set<string> diff)
	{
		int count = 0;
		foreach(string item : one)
		{
			if(two.Contains(item))
				continue;
			diff.Insert(item);
			count++;
		}
		
		return count;
	}
	
	static array<string> GetCopy(notnull array<string> items)
	{
		ref array<string> copy = {};
		copy.Copy(items);
		return copy;
	}
	
	static set<string> GetCopy(notnull set<string> items)
	{
		ref set<string> copy = new set<string>();
		copy.Copy(items);
		return copy;
	}
}

class TW_IterableHelper<Class T>
{
	//! Get common items between both iterable arrays
	static int GetIntersection(notnull array<T> one, notnull array<T> two, inout array<T> common)
	{
		int count = 0;
		
		foreach(ref T item : one)
		{
			if(!two.Contains(item))
				continue;
			
			common.Insert(item);
			count++;
		}
		
		return count;
	}
	
	//! Get common items between both iterable sets
	static int GetIntersection(notnull set<T> one, notnull set<T> two, inout set<T> common)
	{
		int count = 0;
		foreach(ref T item : one)
		{
			if(!two.contains(item))
				continue;
			
			common.Insert(item);
			count++;
		}
		
		return count;
	}
	
	static int GetDifference(notnull array<T> one, notnull array<T> two, inout array<T> diff)
	{
		int count = 0;
		foreach(ref T item : one)
		{
			if(two.Contains(item))
				continue;
			
			diff.Insert(item);
			count++;
		}
		
		return count;
	}
	
	static int GetDifference(notnull set<T> one, notnull set<T> two, inout set<T> diff)
	{
		int count = 0;
		
		foreach(ref T item : one)
		{
			if(two.Contains(item))
				continue;
			
			diff.Insert(item);
			count++;
		}
		
		return count;
	}
	
	static array<T> GetCopy(notnull array<T> one)
	{
		ref array<T> copy = {};
		copy.Copy(one);
		return copy;
	}
	
	static set<T> GetCopy(notnull set<T> one)
	{
		ref set<T> copy = new set<T>();
		copy.Copy(one);
		return one;
	}
};

class TW_Util 
{
	static ref RandomGenerator s_Generator = new RandomGenerator();
	
	static vector DEFAULT_TRACE_OFFSET = Vector(0, 2, 0);
	
	/*!
		Checks to see if position is visible from source entity
	*/
	static bool TraceEntityPointToPointLineOfSight(IEntity sourceEntity, vector from, vector to, float threshold = 1.0, BaseWorld world = null)
	{
		if(!world)
			world = GetGame().GetWorld();
		
		TraceParam traceParams = new TraceParam();
		traceParams.Flags = TraceFlags.ENTS | TraceFlags.WORLD | TraceFlags.VISIBILITY;
		traceParams.LayerMask = EPhysicsLayerDefs.Projectile;
		traceParams.Start = from;
		traceParams.End = to;
		traceParams.Exclude = sourceEntity;
		
		float percent = world.TraceMove(traceParams, null);
		bool result = (percent >= threshold);
		
		return result;
	}
	
	static bool TraceEntityPointsLineOfSight(IEntity sourceEntity, vector from, IEntity targetEntity, vector to, float threshold = 1.0, BaseWorld world = null)
	{
		if(!world)
			world = GetGame().GetWorld();
		
		TraceParam traceParams = new TraceParam();
		traceParams.Flags = TraceFlags.ENTS | TraceFlags.WORLD | TraceFlags.VISIBILITY;
		traceParams.LayerMask = EPhysicsLayerDefs.Projectile;
		
		traceParams.Start = from;
		traceParams.End = to;
		traceParams.Exclude = sourceEntity;
		
		float percent = world.TraceMove(traceParams, null);
		bool result = false;
		
		GenericEntity ent = GenericEntity.Cast(traceParams.TraceEnt);
		if(ent) {
			if(ent == targetEntity || ent.GetParent() == targetEntity || ent.GetRootParent() == targetEntity) {
				result = true;
				percent = 1;
			}
		} else if (percent >= threshold) 
			result = true;
		
		return result;
	}
	
	static bool TraceEntitiesLineOfSight(IEntity from, IEntity to, vector commonOffset = vector.Zero)
	{
		return TraceEntityPointsLineOfSight(from, from.GetOrigin() + commonOffset, to, to.GetOrigin() + commonOffset);
	}
	
	static bool CheckCharactersLineOfSight(array<IEntity> characters, IEntity entity)
	{
		foreach(IEntity character : characters)
			if(TraceEntitiesLineOfSight(character, entity, DEFAULT_TRACE_OFFSET))
				return true;
		return false;
	}
	
	static bool FindEmptyTerrainPosition(
		out vector outPosition,
		vector areaCenter,
		float areaRadius,
		float traceRadius = 0.5,
		float traceHeight = 2,
		TraceFlags flags = TraceFlags.ENTS | TraceFlags.OCEAN,
		BaseWorld world = null
	) {
		if(areaRadius <= 0 || traceRadius <= 0 || traceHeight <= 0)
		{
			outPosition = areaCenter;
			return false;
		}
		
		if(!world)
			world = GetGame().GetWorld();
		
		const float cellW = traceRadius * Math.Sqrt(3);
		const float cellH = traceRadius * 2;
		const vector traceVectorOffset = Vector(0, traceHeight * 0.5, 0);
		const int rMax = Math.Ceil(areaRadius / traceRadius / Math.Sqrt(3));
		const float maxDistanceSq = (areaRadius - traceRadius) * (areaRadius - traceRadius);
		
		TraceParam trace = new TraceParam();
		trace.Flags = flags | TraceFlags.WORLD;
		const vector traceOffset = Vector(0, 10, 0);
		
		float posX, posY;
		int yMin, yMax, yStep;
		float traceCoef;
		
		for(int r; r < rMax; r++)
		{
			for(int x = -r; x <= r; x++)
			{
				posX = cellW * x;
				posY = cellH * (x - SCR_Math.fmod(x, 1)) * 0.5;
				
				yMin = Math.Max(-r - x, -r);
				yMax = Math.Min(r - x, r);
				
				if(Math.AbsInt(x) == r)
					yStep = 1;
				else
					yStep = yMax - yMin;
				
				for(int y = yMin; y <= yMax; y += yStep)
				{
					outPosition = areaCenter + Vector(posX, 0, posY + cellH * y);
					if(vector.DistanceSqXZ(outPosition, areaCenter) > maxDistanceSq)
						continue;
					
					const float surfaceY = world.GetSurfaceY(outPosition[0], outPosition[2]);
					if(surfaceY < world.GetOceanHeight(outPosition[0], outPosition[2]))
						continue;
					
					trace.Start = outPosition;
					trace.End = outPosition - traceOffset;
					traceCoef = world.TraceMove(trace, null);
					
					outPosition[1] = Math.Max(trace.Start[1] - traceCoef * traceOffset[1] + 0.01, surfaceY);
					
					if(SCR_WorldTools.TraceCylinder(outPosition + traceVectorOffset, traceRadius, traceHeight, flags, world))
						return true;
				}
			}
		}
		
		outPosition = areaCenter;
		return false;
	}
	
	static vector GetTerrainSnappedVector(vector worldPos, BaseWorld world = null)
	{
		if(!world) world = GetGame().GetWorld();
		worldPos[1] = worldPos[1] + Math.Max(world.GetSurfaceY(worldPos[0], worldPos[2]), world.GetOceanBaseHeight());
		return worldPos;
	}
	
	static void ApplyRandomDamageToEntity(IEntity entity)
	{
		if(!entity)
			return;
		
		DamageManagerComponent damageManager = DamageManagerComponent.Cast(entity.FindComponent(DamageManagerComponent));
		
		if(!damageManager)
			return;
		
		ref array<HitZone> zones = {};
		damageManager.GetAllHitZones(zones);
		
		int randomAmount = Math.RandomInt(0, zones.Count());
		
		for(int i = 0; i < randomAmount; i++)
		{
			if(zones.IsEmpty()) break;			
						
			HitZone zone = zones.GetRandomElement();
			zones.RemoveItem(zone);
			
			float max = zone.GetMaxHealth();
			float damagePercent = Math.RandomFloat01();
			float damage = max * damagePercent;
			
			zone.HandleDamage(damage, EDamageType.KINETIC, entity);
		}
		
		SCR_FuelManagerComponent fuelManager = SCR_FuelManagerComponent.Cast(entity.FindComponent(SCR_FuelManagerComponent));
		
		if(!fuelManager)
			return;
		
		float fuelLevel = Math.RandomFloat01();
		fuelManager.SetTotalFuelPercentage(fuelLevel);		
	}
	
	static float GetAngleTo(vector center, vector position)
	{
		float yDiff = position[0] - center[0];
		float xDiff = position[2] - center[2];
		
		float atan2 = (float)Math.Atan2(xDiff, yDiff);
		float radians = atan2 / 2;
		
		if(radians < 0.0)
			radians += (float)Math.PI;
		
		return radians * (180 * Math.PI);		
	}
	
	//! Save Json File: Credit to Bacon
	static bool SaveJsonFile(string path, Managed data, bool useTypeDiscriminator = false) 
	{
		ContainerSerializationSaveContext saveContext = new ContainerSerializationSaveContext(false);
		if(useTypeDiscriminator)
			saveContext.EnableTypeDiscriminator(true);
		
		PrettyJsonSaveContainer container = new PrettyJsonSaveContainer();
		saveContext.SetContainer(container);
		
		if(!saveContext.WriteValue("", data)) 
		{
			PrintFormat("TrainWreck: Serialization of '%1' failed for file '%2'", data, path, LogLevel.ERROR);
			return false;
		}
		
		return container.SaveToFile(path);
	}
	
	static string ToJson(Managed data, bool useTypeDiscriminator = false)
	{
		ContainerSerializationSaveContext saveContext = new ContainerSerializationSaveContext(false);
		if(useTypeDiscriminator)
			saveContext.EnableTypeDiscriminator(true);
		
		JsonSaveContainer container = new JsonSaveContainer();
		saveContext.SetContainer(container);
		
		if(!saveContext.WriteValue("", data))
		{
			PrintFormat("TrainWreck: Failed to serialize data.", LogLevel.ERROR);
			return string.Empty;
		}
		
		return container.ExportToString();
	}
	
	//! Load Json File: Credit to Bacon
	static SCR_JsonLoadContext LoadJsonFile(string path, bool useTypeDiscriminator = false)
	{
		SCR_JsonLoadContext context = new SCR_JsonLoadContext(false);
		
		if(useTypeDiscriminator)
			context.EnableTypeDiscriminator(true);
		
		if(!context.LoadFromFile(path))
		{
			PrintFormat("TrainWreck: Loading JSON file '%1' failed", path, LogLevel.ERROR);
			return null;
		}
		
		return context;
	}
	
	//! Attempt to retrieve the affiliated faction for this AI Agent
	static FactionKey GetFactionKeyFromAgent(SCR_ChimeraAIAgent agent)
	{
		SCR_ChimeraAIAgent ai = SCR_ChimeraAIAgent.Cast(agent);
		
		IEntity controlledEntity = ai.GetControlledEntity();
		
		if(!controlledEntity) 
			return FactionKey.Empty;
		
		FactionAffiliationComponent fac = FactionAffiliationComponent.Cast(controlledEntity.FindComponent(FactionAffiliationComponent));
		
		if(!fac)
			return FactionKey.Empty;
		
		return fac.GetAffiliatedFactionKey();
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
		
		if(!resource) 
		{			
			PrintFormat("TrainWreck: Waypoint was invalid %1", waypointPrefab, LogLevel.ERROR);
			return null;
		}
		
		AIWaypoint wp = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(resource));		
		
		if(!wp)
		{
			PrintFormat("TrainWreck: Was unable to spawn waypoint prefab: %1", waypointPrefab, LogLevel.ERROR);
			return null;
		}
		
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
	
	static string ToGridText(int x, int y)
	{
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
	
	static SCR_EditableEntityUIInfo GetCharacterUIInfo(ResourceName prefab)
	{	
		Resource resource = Resource.Load(prefab);
		IEntitySource entitySource = SCR_BaseContainerTools.FindEntitySource(resource);
		
		if(!entitySource) return null;
		
		for(int nComponent, componentCount = entitySource.GetComponentCount(); nComponent < componentCount; nComponent++)
		{
			IEntityComponentSource componentSource = entitySource.GetComponent(nComponent);
			
			if(componentSource.GetClassName().ToType().IsInherited(SCR_EditableCharacterComponent))
			{
				BaseContainer attributeContainer = componentSource.GetObject("m_UIInfo");
				if(!attributeContainer) return null;
				
				return SCR_EditableEntityUIInfo.Cast(BaseContainerTools.CreateInstanceFromContainer(attributeContainer));
			}
		}
		
		return null;
	}
	
	static UIInfo GetItemUIInfo(ResourceName prefab)
	{
		UIInfo resultInfo = s_ItemUIInfo.Get(prefab);
		
		if(!resultInfo)
		{
			Resource resource = Resource.Load(prefab);
			IEntitySource entitySource = SCR_BaseContainerTools.FindEntitySource(resource);
			
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
			Resource resource = Resource.Load(prefab);
			IEntitySource entitySource = SCR_BaseContainerTools.FindEntitySource(resource);
			
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
		
		if(!GetGame().InPlayMode())
		{
			return vector.Zero;
		}
		
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
	
	static const int SPAWN_MIN_PLAYER_DIST_SQ = (100*100);
	static const float SPAWN_MAX_LOS_CHECK_DIST_SQ = (1000*1000);
	
	static bool IsSpawnPositionVisible(TW_SpawnVisibilityCheckContext context, vector position)
	{
		array<float> playerDistances = {};
		playerDistances.Resize(context.playerEntities.Count());
		
		foreach(int i, vector playerPos : context.playerPositions)
		{
			float distSq = vector.DistanceSqXZ(playerPos, position);
			if(distSq < SPAWN_MIN_PLAYER_DIST_SQ) return true;
			playerDistances[i] = distSq;
		}
		
		vector traceTarget = position + DEFAULT_TRACE_OFFSET;
		vector playerPoint;
		
		foreach(int i, float dist : playerDistances)
		{
			if(dist >= SPAWN_MAX_LOS_CHECK_DIST_SQ) continue;
			playerPoint = context.playerPositions[i] + DEFAULT_TRACE_OFFSET;
			
			if(TraceEntityPointToPointLineOfSight(context.playerEntities[i], playerPoint, traceTarget))
				return true;
		}
		
		return false;
	}
	
	static TW_SpawnVisibilityCheckContext CreateVisibilityCheckContext()
	{
		TW_SpawnVisibilityCheckContext context = new TW_SpawnVisibilityCheckContext();
		GetAllPlayers(context.playerPositions, context.playerEntities);
		return context;
	}
	
	static void GetAllPlayers(out array<vector> outPositions, out array<IEntity> outEntities)
	{
		auto manager = GetGame().GetPlayerManager();
		array<int> players = new array<int>;
		manager.GetPlayers(players);
		
		outPositions.Clear();
		outEntities.Clear();
		
		IEntity entity;
		foreach(int playerId : players)
		{
			entity = manager.GetPlayerControlledEntity(playerId);
			
			if(!entity) continue;
			
			outPositions.Insert(entity.GetOrigin());
			outEntities.Insert(entity);
		}
	}
	
	static bool IsPlayerAlive(int playerId)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId));
		if(!character) return false;
		
		SCR_CharacterControllerComponent controller = TW<SCR_CharacterControllerComponent>.Find(character);
		return (controller && controller.GetLifeState() != ECharacterLifeState.DEAD);
	}
};

class TW_SpawnVisibilityCheckContext
{
	ref array<vector> playerPositions = {};
	ref array<IEntity> playerEntities = {};
}