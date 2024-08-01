class TW_Util 
{
	static ref RandomGenerator s_Generator = new RandomGenerator();
	
	static vector RandomPositionAround(IEntity point, int radius, int minimumDistance = 0)
	{
		vector position = s_Generator.GenerateRandomPointInRadius(minimumDistance, radius, point.GetOrigin());
		
		while(IsWater(position))
		{
			position = RandomPositionAround(point, Math.Max(radius * 0.9, minimumDistance), minimumDistance);
		}
		
		return position;
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
		
		int attempts = 0;
		while(IsWater(params.Transform[3]) && attempts < 30)
		{
			params.Transform[3] = s_Generator.GenerateRandomPointInRadius(minimumDistance, Math.ClampInt(radius-(attempts*25), minimumDistance, int.MAX), center);
			attempts++;
		}
		
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
		
		if(!resource) return null;
		
		AIWaypoint wp = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(resource));
		if(!wp) return null;
		
		wp.SetOrigin(waypointPosition);
		return wp;
	}
};