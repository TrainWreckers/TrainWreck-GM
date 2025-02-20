class TW_CompositionPlacementEvent
{
	vector position;
	ResourceName prefab;	
};

class TW_CompositionSpawnHandler
{
	private int m_Padding = 10;
	private int m_SpawnDelay = 1000;
	private int m_Radius;
	private int m_PlacementAttempts;
	
	// Constants referring to the ~size of the 3 different composition types
	const int SMALL_COMPOSITION_SIZE = 8;
	const int MEDIUM_COMPOSITION_SIZE = 15;
	const int LARGE_COMPOSITION_SIZE = 23;
	
	private SCR_BaseGameMode m_GameMode;	
	private ref TW_MapManager m_MapManager;
	
	//! Entities this manager has created
	private ref array<IEntity> m_Entities = {};
	
	private static ref array<TW_CompositionSpawnType> cachedSpawnTypes = {};
	
	private ref ScriptInvoker<TW_CompositionPlacementEvent> m_OnPlacementFailed = new ScriptInvoker<TW_CompositionPlacementEvent>();
	private ref ScriptInvoker<TW_CompositionPlacementEvent> m_OnPlacementSuccess = new ScriptInvoker<TW_CompositionPlacementEvent>();
	private ref ScriptInvoker m_OnCompleted = new ScriptInvoker();
	
	private ref TW_CompositionSpawnSettings m_SpawnAmount;
	private ref TW_CompositionSpawnSettings m_Config;
	
	private vector m_PlacementCenter;
	private vector m_CurrentPosition;
	private ref array<vector> m_SpawnedPositions = {};
	
	private FactionKey m_FactionKey = FactionKey.Empty;
	private bool m_PlacementSucceeded = false;
	
	ScriptInvoker<TW_CompositionPlacementEvent> GetOnPlacementFailed() { return m_OnPlacementFailed; }
	ScriptInvoker<TW_CompositionPlacementEvent> GetOnPlacementSucceeded() { return m_OnPlacementSuccess; }
	ScriptInvoker GetOnCompleted() { return m_OnCompleted; }
	
	void TW_CompositionSpawnHandler()
	{
		m_GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		m_MapManager = m_GameMode.GetMapManager();
		
		if(cachedSpawnTypes.Count() == 0)
		{
			SCR_Enum.GetEnumValues(TW_CompositionSpawnType, cachedSpawnTypes);
		}
	}
	
	void ~TW_CompositionSpawnHandler()
	{
		GetOnPlacementFailed().Clear();
		GetOnPlacementSucceeded().Clear();
		GetOnCompleted().Clear();
	}
	
	bool IsTooClose(vector position)
	{
		if(m_SpawnedPositions.IsEmpty()) return false;
		
		foreach(vector pos : m_SpawnedPositions)
		{
			float distance = vector.DistanceXZ(position, pos);
			
			if(distance < SMALL_COMPOSITION_SIZE)
				return true;
		}
		
		return false;
	}
	
	private void RetryPlacement()
	{
		PrintFormat("TrainWreck-GM: Composition Spawn Handler -> Placement Failed (%1)", m_PlacementAttempts, LogLevel.WARNING);
		m_PlacementAttempts--;
		
		foreach(IEntity entity : m_Entities)
			SCR_EntityHelper.DeleteEntityAndChildren(entity);
		
		m_Entities.Clear();
		m_PlacementCenter = TW_Util.RandomPositionAround(m_PlacementCenter, m_Radius, m_Radius * 0.25);
		
		if(m_PlacementAttempts > 0)
			SpawnStart();
		else
			GetOnPlacementFailed().Remove(RetryPlacement);
	}
	
	void Init(vector centerLocation, float radius, TW_CompositionSpawnSettings settings, int placementAttempts = 5)
	{
		m_PlacementCenter = centerLocation;
		m_Radius = radius;
		m_PlacementAttempts = placementAttempts;
		
		m_Config = settings;		
	}
	
	private bool FindOpenAreaForComposition(vector center, int radius, int size, out vector position)
	{
		bool result = SCR_WorldTools.FindEmptyTerrainPosition(position, center, radius, size, size, TraceFlags.ENTS | TraceFlags.WORLD | TraceFlags.OCEAN);
		
		int attempts = 25;
		while(attempts > 25)
		{			
			if(TW_Util.IsWater(position) || IsTooClose(position))
			{
				result = SCR_WorldTools.FindEmptyTerrainPosition(position, center, radius, size, size, TraceFlags.ENTS | TraceFlags.WORLD | TraceFlags.OCEAN);
				attempts--;
			}
			else 
				break;		
		}
		
		PrintFormat("TrainWreck: Found %1", position);
		return result;
	}
	
	void SpawnStart()
	{
		m_SpawnAmount = new TW_CompositionSpawnSettings();
		
		foreach(IEntity entity : m_Entities)
			SCR_EntityHelper.DeleteEntityAndChildren(entity);
		
		m_Entities.Clear();
		m_SpawnedPositions.Clear();
		
		vector center = m_PlacementCenter;
		m_PlacementCenter = TW_Util.RandomPositionAround(center, m_Radius);		
		
		int attempts = 50;
		while(attempts > 50)
		{
			if(TW_Util.IsWater(m_PlacementCenter))
			{
				m_PlacementCenter = TW_Util.RandomPositionAround(center, m_Radius);
				attempts--;
			}
			else break;
		}
		
		if(TW_Util.IsWater(m_PlacementCenter))
		{
			Print("TrainWreck: Was unable to find a land position for composition", LogLevel.ERROR);
			return;
		}
		
		m_CurrentPosition = m_PlacementCenter;
				
		Spawn();
	}
	
	private int GetRemainingSpawn(TW_CompositionSpawnType type)
	{
		switch(type)
		{
			case TW_CompositionSpawnType.WALLS:
			{
				return m_Config.DefensiveWalls - m_SpawnAmount.DefensiveWalls;
			}
			
			case TW_CompositionSpawnType.BUNKERS:
			{
				return m_Config.DefensiveBunkers - m_SpawnAmount.DefensiveBunkers;
			}
			
			case TW_CompositionSpawnType.LARGE:
			{
				return m_Config.LargeCompositions - m_SpawnAmount.LargeCompositions;
			}
			
			case TW_CompositionSpawnType.MEDIUM:
			{
				return m_Config.MediumCompositions - m_SpawnAmount.MediumCompositions;
			}
			
			case TW_CompositionSpawnType.SMALL:
			{
				return m_Config.SmallCompositions - m_SpawnAmount.SmallCompositions;
			}			
		}
		
		return 0;
	}
	
	private void IncrementSpawnCount(TW_CompositionSpawnType type)
	{
		switch(type)
		{
			case TW_CompositionSpawnType.WALLS:
			{
				m_SpawnAmount.DefensiveWalls = m_SpawnAmount.DefensiveWalls + 1;
				break;
			}
			
			case TW_CompositionSpawnType.BUNKERS:
			{
				m_SpawnAmount.DefensiveBunkers = m_SpawnAmount.DefensiveBunkers + 1;
				break;
			}
			
			case TW_CompositionSpawnType.LARGE:
			{
				m_SpawnAmount.LargeCompositions = m_SpawnAmount.LargeCompositions + 1;
				break;
			}
			
			case TW_CompositionSpawnType.MEDIUM:
			{
				m_SpawnAmount.MediumCompositions = m_SpawnAmount.MediumCompositions + 1;
				break;
			}
			
			case TW_CompositionSpawnType.SMALL:
			{
				m_SpawnAmount.SmallCompositions = m_SpawnAmount.SmallCompositions + 1;
				break;
			}			
		}
	}
	
	private ResourceName GetRandomPrefabForType(TW_CompositionSpawnType type)
	{
		switch(type)
		{
			case TW_CompositionSpawnType.WALLS:
			{
				return m_Config.Compositions.GetRandomDefensiveWall();
				break;
			}
			
			case TW_CompositionSpawnType.BUNKERS:
			{
				return m_Config.Compositions.GetRandomDefensiveBunkder();
				break;
			}
			
			case TW_CompositionSpawnType.LARGE:
			{
				return m_Config.Compositions.GetRandomLargeComposition();
				break;
			}
			
			case TW_CompositionSpawnType.MEDIUM:
			{
				return m_Config.Compositions.GetRandomMediumComposition();
				break;
			}
			
			case TW_CompositionSpawnType.SMALL:
			{
				return m_Config.Compositions.GetRandomSmallComposition();
				break;
			}
		}
		
		return ResourceName.Empty;
	}
	
	private int GetCompositionSize(TW_CompositionSpawnType type)
	{
		switch(type)
		{
			case TW_CompositionSpawnType.WALLS:
			{
				return SMALL_COMPOSITION_SIZE;
			}
			
			case TW_CompositionSpawnType.BUNKERS:
			{
				return SMALL_COMPOSITION_SIZE;
			}
			
			case TW_CompositionSpawnType.LARGE:
			{
				return LARGE_COMPOSITION_SIZE;
			}
			
			case TW_CompositionSpawnType.MEDIUM:
			{
				return MEDIUM_COMPOSITION_SIZE;
			}
			
			case TW_CompositionSpawnType.SMALL:
			{
				return SMALL_COMPOSITION_SIZE;
			}
		}
		
		return SMALL_COMPOSITION_SIZE;
	}
	
	private void Spawn(int index = 0)
	{
		if(index >= cachedSpawnTypes.Count() || index < 0)
		{
			GetOnCompleted().Invoke();			
			return;
		}
				
		TW_CompositionSpawnType spawnType = cachedSpawnTypes.Get(index);		
		int remaining = GetRemainingSpawn(spawnType);
		
		PrintFormat("TrainWreck: Spawn Composition Type: %1 -> Remaining: %2", spawnType, remaining);
		
		if(remaining <= 0)
		{			
			GetGame().GetCallqueue().CallLater(Spawn, m_SpawnDelay, false, index + 1);
			return;
		}
		
		IncrementSpawnCount(spawnType);
		
		ResourceName prefab = GetRandomPrefabForType(spawnType);
		int size = GetCompositionSize(spawnType);
		
		IEntity current;
		int attempts = 0;
		bool success = false;
		
		while(attempts < 5)
		{
			attempts++;
			
			PrintFormat("TrainWreck-GM: Composition Spawn Handler -> '%1'", prefab);
			
			success = TrySpawnComposition(current, size, prefab, attempts);
			
			ref TW_CompositionPlacementEvent e = new TW_CompositionPlacementEvent();
			e.position = m_CurrentPosition;
			e.prefab = prefab;
			
			if(success)
			{
				m_Entities.Insert(current);
				GetOnPlacementSucceeded().Invoke(e);
				break;		
			}
			else
			{
				GetOnPlacementFailed().Invoke(e);				
			}			
		}				
		
		GetGame().GetCallqueue().CallLater(Spawn, m_SpawnDelay, false, index);	
	}
	
	private bool TrySpawnComposition(out IEntity entity, int size, ResourceName prefab, int attempts=0)
	{
		if(!FindOpenAreaForComposition(m_CurrentPosition, size + (size * attempts), size, m_CurrentPosition))
			return false;
		
		EntitySpawnParams params = EntitySpawnParams();
		vector mat[4];
		mat[3] = m_CurrentPosition;
		
		params.Transform = mat;
		params.TransformMode = ETransformMode.WORLD;
		vector angles = Math3D.MatrixToAngles(params.Transform);
		angles[0] = Math.RandomFloat(0, 360);
		Math3D.AnglesToMatrix(angles, params.Transform);
		
		Resource prefabResource = Resource.Load(prefab);
		
		if(!prefabResource.IsValid())
		{
			PrintFormat("TrainWreck-GM: Composition Spawn Handler failed to spawn prefab -> '%1' -> Invalid", prefab, LogLevel.WARNING);
			return false;
		}
		
		entity = GetGame().SpawnEntityPrefab(prefabResource, GetGame().GetWorld(), params);
		
		if(!entity)
			return false;
		
		SCR_EditableEntityComponent editable = SCR_EditableEntityComponent.GetEditableEntity(entity);
		
		if(editable)
		{
			SCR_EditorPreviewParams previewParams = SCR_EditorPreviewParams.CreateParams(params.Transform, EEditorTransformVertical.TERRAIN);
			SCR_RefPreviewEntity.SpawnAndApplyReference(editable, previewParams);
		}
		
		SCR_SlotCompositionComponent slotComp = SCR_SlotCompositionComponent.Cast(entity.FindComponent(SCR_SlotCompositionComponent));
		
		if(slotComp)
			slotComp.OrientToTerrain();
		
		SCR_AIWorld aiWorld = SCR_AIWorld.Cast(GetGame().GetAIWorld());
		
		if(aiWorld)
			aiWorld.RequestNavmeshRebuildEntity(entity);
		
		m_SpawnedPositions.Insert(mat[3]);
		
		return true;
	}		
};