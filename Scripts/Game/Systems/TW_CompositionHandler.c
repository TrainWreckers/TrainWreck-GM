class TW_CompositionHandler
{
	private int m_Padding = 10;
	private int m_SpawnDelay = 1000;
	private int m_Radius;
	
	const int SMALL_COMPOSITION_SIZE = 8;
	const int MEDIUM_COMPOSITION_SIZE = 15;
	const int LARGE_COMPOSITION_SIZE = 23;
	
	private SCR_BaseGameMode m_GameMode;
	private ref TW_MapManager m_MapManager;
	private ref array<IEntity> m_Entities = {};
	
	private ref ScriptInvoker m_OnCompositionPlacementFailed = new ScriptInvoker();
	private ref ScriptInvoker m_OnCompositionPlacementSuccess = new ScriptInvoker();
	
	private ref TW_CompositionSpawnSettings m_SpawnAmount;
	private ref TW_CompositionSpawnSettings m_Config;
	
	private vector m_PlacementCenter;
	private vector m_CurrentPosition;
	
	private FactionKey m_FactionKey = FactionKey.Empty;
	
	ScriptInvoker GetOnCompositionPlacementFailed() { return m_OnCompositionPlacementFailed; }
	ScriptInvoker GetOnCompositionPlacementSuccess() { return m_OnCompositionPlacementSuccess; }
	
	void TW_CompositionHandler(SCR_BaseGameMode gameMode, TW_MapManager mapManager)
	{
		m_GameMode = gameMode;
		m_MapManager = mapManager;
	}
	
	void SpawnAt(vector centerLocation, float radius, TW_CompositionSpawnSettings settings)
	{
		m_PlacementCenter = centerLocation;
		m_CurrentPosition = m_PlacementCenter;
		
		m_Config = settings;
		m_SpawnAmount = TW_CompositionSpawnSettings();
		m_Entities.Clear();		
	}
	
	private bool TrySpawnComposition(out IEntity entity, TW_CompositionSize size, ResourceName prefab)
	{
		if(FindOpenAreaForComposition(m_CurrentPosition, m_Radius, size, m_CurrentPosition))
		{
			EntitySpawnParams params = EntitySpawnParams();
			vector mat[4];
			mat[3] = m_CurrentPosition;
			
			if(!SCR_TerrainHelper.SnapToTerrain(mat, GetGame().GetWorld(), true))
				return false;
			
			params.Transform = mat;
			params.TransformMode = ETransformMode.WORLD;
			vector angles = Math3D.MatrixToAngles(params.Transform);
			angles[0] = Math.RandomFloat(0, 360);
			Math3D.AnglesToMatrix(angles, params.Transform);
			
			entity = GetGame().SpawnEntityPrefab(prefab, false, GetGame().GetWorld(), params);
			
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
			
			return true;
		} 
		
		return false;
	}
	
	bool FindOpenAreaForComposition(vector center, int radius, TW_CompositionSize size, out vector position)
	{
		int compositionSize = SMALL_COMPOSITION_SIZE;
		
		switch(size)
		{
			case TW_CompositionSize.MEDIUM:
			{
				compositionSize = MEDIUM_COMPOSITION_SIZE;
				break;
			}
			
			case TW_CompositionSize.LARGE:
			{
				compositionSize = LARGE_COMPOSITION_SIZE;
				break;
			}
		}
		
		return SCR_WorldTools.FindEmptyTerrainPosition(position, center, radius, compositionSize, compositionSize, TraceFlags.ENTS | TraceFlags.WORLD | TraceFlags.OCEAN);
	}	
	
	private void SpawnSmallComposition()
	{
		if(!m_Config.Compositions.HasSmallCompositions())
			m_SpawnAmount.SmallCompositions = -1;
		
		if(m_SpawnAmount.SmallCompositions >= m_Config.SmallCompositions || m_SpawnAmount.SmallCompositions < 0)
		{
			GetGame().GetCallqueue().CallLater(SpawnMediumCompositions, m_SpawnDelay, false);
			return;
		}
		
		m_PlacementCenter = TW_Util.RandomPositionAround(m_PlacementCenter, m_Radius, SMALL_COMPOSITION_SIZE);
		
		IEntity current;
		int attempts = 5;
		bool success = false;
		while(attempts > 0)
		{
			attempts--;
			
			success = TrySpawnComposition(current, TW_CompositionSize.SMALL, m_Config.Compositions.GetRandomSmallComposition());
			
			if(success)
			{
				m_Entities.Insert(current);
				break;
			}
		}
		
		if(!success)
		{
			m_OnCompositionPlacementFailed.Invoke();
			return;
		}
		
		m_SpawnAmount.SmallCompositions++;
		GetGame().GetCallqueue().CallLater(SpawnSmallComposition, m_SpawnDelay, false);
	}
	
	private void SpawnMediumCompositions()
	{
		if(!m_Config.Compositions.HasMediumCompositions())
			m_SpawnAmount.MediumCompositions = -1;
		
		if(m_SpawnAmount.MediumCompositions >= m_Config.MediumCompositions || m_SpawnAmount.MediumCompositions < 0)
		{
			GetGame().GetCallqueue().CallLater(SpawnLargeCompositions, m_SpawnDelay, false);
			return;
		}
		
		m_PlacementCenter = TW_Util.RandomPositionAround(m_PlacementCenter, m_Radius, MEDIUM_COMPOSITION_SIZE);
		
		IEntity current;
		int attempts = 5;
		bool success = false;
		while(attempts > 0)
		{
			attempts--;
			
			success = TrySpawnComposition(current, TW_CompositionSize.MEDIUM, m_Config.Compositions.GetRandomMediumComposition());
			if(success)
			{
				m_Entities.Insert(current);
				break;
			}
		}
		
		if(!success)
		{
			m_OnCompositionPlacementFailed.Invoke();
			return;
		}
		
		m_SpawnAmount.SmallCompositions++;
		GetGame().GetCallqueue().CallLater(SpawnLargeCompositions, m_SpawnDelay, false);
	}
	
	private void SpawnLargeCompositions()
	{
		if(!m_Config.Compositions.HasLargeCompositions())
			m_SpawnAmount.LargeCompositions = -1;
		
		if(m_SpawnAmount.LargeCompositions >= m_Config.LargeCompositions || m_SpawnAmount.LargeCompositions < 0)
		{
			GetGame().GetCallqueue().CallLater(SpawnDefensiveBunkers, m_SpawnDelay, false);
			return;
		}
		
		m_PlacementCenter = TW_Util.RandomPositionAround(m_PlacementCenter, m_Radius, LARGE_COMPOSITION_SIZE);
		
		IEntity current;
		int attempts = 5;
		bool success = false;
		while(attempts > 0)
		{
			attempts--;
			
			success = TrySpawnComposition(current, TW_CompositionSize.LARGE, m_Config.Compositions.GetRandomLargeComposition());
			if(success)
			{
				m_Entities.Insert(current);
				break;
			}
		}
		
		if(!success)
		{
			m_OnCompositionPlacementFailed.Invoke();
			return;
		}
		
		
		m_SpawnAmount.LargeCompositions++;
		GetGame().GetCallqueue().CallLater(SpawnDefensiveBunkers, m_SpawnDelay, false);
	}
	
	private void SpawnDefensiveBunkers()
	{
		if(!m_Config.Compositions.HasDefensiveBunkers())
			m_SpawnAmount.DefensiveBunkers = -1;
		
		if(m_SpawnAmount.DefensiveBunkers >= m_Config.DefensiveBunkers || m_SpawnAmount.DefensiveBunkers)
		{
			GetGame().GetCallqueue().CallLater(SpawnDefensiveWalls, m_SpawnDelay, SMALL_COMPOSITION_SIZE);
			return;
		}
		
		m_PlacementCenter = TW_Util.RandomPositionAround(m_PlacementCenter, m_Radius, SMALL_COMPOSITION_SIZE);
		
		IEntity current;
		int attempts = 5;
		bool success = false;
		while(attempts > 0)
		{
			attempts--;
			success = TrySpawnComposition(current, TW_CompositionSize.SMALL, m_Config.Compositions.GetRandomDefensiveBunkder());
			
			if(success)
			{
				m_Entities.Insert(current);
				break;
			}
		}
		
		if(!success)
		{
			m_OnCompositionPlacementFailed.Invoke();
			return;
		}
			
		m_SpawnAmount.DefensiveBunkers++;		
		GetGame().GetCallqueue().CallLater(SpawnDefensiveBunkers, m_SpawnDelay, false);
	}
	
	private void SpawnDefensiveWalls()
	{
		if(m_Config.Compositions.HasDefensiveWalls())
			m_SpawnAmount.DefensiveBunkers = -1;
		
		if(m_SpawnAmount.DefensiveWalls >= m_Config.DefensiveWalls || m_SpawnAmount.DefensiveWalls < 0)
		{
			m_OnCompositionPlacementSuccess.Invoke();
			return;
		}
		
		m_PlacementCenter = TW_Util.RandomPositionAround(m_PlacementCenter, m_Radius, SMALL_COMPOSITION_SIZE);
		
		IEntity current;
		int attempts = 5;
		bool success = false;
		while(attempts > 0)
		{
			attempts--;
			success = TrySpawnComposition(current, TW_CompositionSize.SMALL, m_Config.Compositions.GetRandomDefensiveWall());
			
			if(success)
			{
				m_Entities.Insert(current);
				break;
			}
		}
		
		if(!success)
		{
			m_OnCompositionPlacementFailed.Invoke();
			return;
		}
		
		m_SpawnAmount.DefensiveWalls++;
		GetGame().GetCallqueue().CallLater(SpawnDefensiveWalls, m_SpawnDelay, false);
	}
};