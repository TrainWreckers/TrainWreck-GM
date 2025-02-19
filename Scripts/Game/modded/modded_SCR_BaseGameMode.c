// Not TrainWreck, brought in to ensure if mod ever breaks we can fix it ourselves 
// Credit goes to ceo of bacon

modded class SCR_BaseGameMode 
{
	[RplProp(onRplName: "DisableGMBudget_OnBroadcastValueUpdated")]
	bool m_DisableGMBudgets_BudgetsEnabled = true;
	
	protected ref TW_MonitorPositions positionMonitor;
	protected float m_PositionMonitorUpdateInterval = 10.0;
	
	protected ref TW_MapManager m_MapManager;
	protected ref TW_AISpawnManager m_AISpawnManager = new TW_AISpawnManager();
	
	protected ref FactionCompositions m_USCompositions;	
	protected ref FactionCompositions m_USSRCompositions;	
	protected ref FactionCompositions m_FIACompositions;	
	
	FactionCompositions GetFIACompositions() { return m_FIACompositions; }
	FactionCompositions GetUSSRCompositions() { return m_USSRCompositions; }
	FactionCompositions GetUSCompositions() { return m_USCompositions; }
	
	protected ref ScriptInvoker Event_OnGameInitializePlugins = new ScriptInvoker();
	protected ref ScriptInvoker Event_OnGamePluginsInitialized = new ScriptInvoker();
	protected ref ScriptInvoker<TW_MonitorPositions> Event_OnPositionMonitorChanged = new ScriptInvoker<TW_MonitorPositions>();
	
	private void InitializeCompositions()
	{
		m_USCompositions = Load("{0C96E3FCC84E8CD4}Configs/Compositions/TW_US_Compositions.conf");
		m_USSRCompositions = Load("{B482220F45A754E6}Configs/Compositions/TW_USSR_Compositions.conf");
		m_FIACompositions = Load("{E37B578DE2724443}Configs/Compositions/TW_FIA_Compositions.conf");
	}
	
	protected void InitializePlugins();	
	
	override void StartGameMode()
	{
		if (!IsMaster())
			return;

		if (IsRunning())
		{
			Print("Trying to start a gamemode that is already running.", LogLevel.WARNING);
			return;
		}
		
		if(Event_OnGameInitializePlugins)
		{
			Print("TrainWreck: Initializing Plugins");
			Event_OnGameInitializePlugins.Invoke();
		}
		
		InitializePlugins();
		
		if(Event_OnGamePluginsInitialized)
		{
			Print("TrainWreck: Initialized Plugins");
			Event_OnGamePluginsInitialized.Invoke();
		}
		
		InitializePositionMonitor();
		
		GetOnPositionMonitorChanged().Invoke(positionMonitor);
		
		m_MapManager = TW_MapManager();
		m_MapManager.InitializeMap(this, GetGame().GetMapManager());
				
		Print("TrainWreck: GameMode Starting...");

		m_fTimeElapsed = 0.0;
		m_eGameState = SCR_EGameModeState.GAME;
		Replication.BumpMe();

		// Raise event for authority
		OnGameStateChanged();
	}
	
	private FactionCompositions Load(ResourceName prefab)
	{
		Resource configContainer = BaseContainerTools.LoadContainer(prefab);
		if (!configContainer || !configContainer.IsValid())
			return null;

		ref FactionCompositions registry = FactionCompositions.Cast(BaseContainerTools.CreateInstanceFromContainer(configContainer.GetResource().ToBaseContainer()));
		return registry;
	}
	
	/*!
		This update doesn't trigger every interval. This only changes
		when player chunks from previous check are different from current
	
		This update will pass along the various chunk information
		- Player chunk positions
		- Anti spawn chunk positions
		- Unloaded chunks
	*/
	ref ScriptInvoker<GridUpdateEvent> GetOnPlayerPositionsUpdated() 
	{ 
		if(!positionMonitor) 
			return null;
		
		return positionMonitor.GetGridUpdate(); 
	}
	
	ref ScriptInvoker<TW_MonitorPositions> GetOnPositionMonitorChanged()
	{
		return Event_OnPositionMonitorChanged;
	}
	
	ref ScriptInvoker GetOnPluginsInitialized()
	{
		return Event_OnGameInitializePlugins;
	}
	
	ref ScriptInvoker GetOnInitializePlugins()
	{
		return Event_OnGamePluginsInitialized;
	}
	
	//! Initialize the player update monitor
	protected void InitializePositionMonitor()
	{
		positionMonitor = new TW_MonitorPositions(250, 5, 150, 2);		
	}
	
	private void MonitorUpdate()
	{
		if(positionMonitor)
			positionMonitor.MonitorPlayers();
	}
	
	override void EOnInit(IEntity owner) 
	{
		super.EOnInit(owner);
		
		if(!TW_Global.IsServer(this) || !TW_Global.IsInRuntime())
			return;
		
		Event_OnPositionMonitorChanged.Insert(RegisterMonitorEvent);
	};
	
	private void RegisterMonitorEvent(TW_MonitorPositions monitor)
	{
		if(monitor)
			GetGame().GetCallqueue().CallLater(MonitorUpdate, m_PositionMonitorUpdateInterval * 1000, true);		
	}
	
	
	private int m_CompositionSpawnAttempts = 50;
	void TryToSpawnSite()
	{
		m_CompositionSpawnAttempts--;
		
		ref TW_MapLocation location = m_MapManager.GetRandomLocation();
		ref LocationConfig config = m_MapManager.settings.GetConfigType(location.LocationType());
		
		m_CompositionSpawnAttempts--;
		bool result = m_MapManager.TrySpawnCompositionCollection("USSR", location.GetPosition(), config.radius * m_MapManager.settings.gridSize, Math.RandomIntInclusive(3,10), Math.RandomIntInclusive(2,4), Math.RandomIntInclusive(2,6), Math.RandomIntInclusive(1,3), Math.RandomIntInclusive(0, 1));	
		PrintFormat("TrainWreck-GM: Spawned composition(%1)", result);
		
		if(m_CompositionSpawnAttempts < 0)
		{
			PrintFormat("TrainWreck-GM: Failed to find suitable location for base", LogLevel.ERROR);
			m_MapManager.GetOnCompositionBasePlacementFailed().Remove(TryToSpawnSite);
		}
	}
		
	
	void DisableGMBudget_SetBudgetsEnabled(bool enabled) 
	{
		m_DisableGMBudgets_BudgetsEnabled = enabled;
		Replication.BumpMe();
		
		DisableGMBudget_OnBroadcastValueUpdated();
	};
	
	bool DisableGMBudget_AreBudgetsEnabled() 
	{
		return m_DisableGMBudgets_BudgetsEnabled;
	};
	
	void DisableGMBudget_OnBroadcastValueUpdated() 
	{
		SCR_BudgetEditorComponent budgetManager = SCR_BudgetEditorComponent.Cast(SCR_BudgetEditorComponent.GetInstance(SCR_BudgetEditorComponent, false, true));
		if (!budgetManager)
			return;
		
		budgetManager.DisableGMBudget_BudgetsUpdated(m_DisableGMBudgets_BudgetsEnabled);
	};
}