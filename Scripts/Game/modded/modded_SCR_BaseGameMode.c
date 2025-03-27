// Not TrainWreck, brought in to ensure if mod ever breaks we can fix it ourselves 
// Credit goes to ceo of bacon

modded class SCR_BaseGameMode 
{
	protected ref TW_MonitorPositions positionMonitor;
	protected float m_PositionMonitorUpdateInterval = 10.0;
	
	protected ref TW_MapManager m_MapManager;
	
	protected ref ScriptInvoker Event_OnGameInitializePlugins = new ScriptInvoker();
	protected ref ScriptInvoker Event_OnGamePluginsInitialized = new ScriptInvoker();
	protected ref ScriptInvoker<TW_MonitorPositions> Event_OnPositionMonitorChanged = new ScriptInvoker<TW_MonitorPositions>();
	
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
		
		m_MapManager = TW_MapManager();
		m_MapManager.InitializeMap(this, GetGame().GetMapManager());
		
		InitializePlugins();
		
		if(Event_OnGamePluginsInitialized)
		{
			Print("TrainWreck: Initialized Plugins");
			Event_OnGamePluginsInitialized.Invoke();
		}
		
		InitializePositionMonitor();
		
		GetOnPositionMonitorChanged().Invoke(positionMonitor);
				
		Print("TrainWreck: GameMode Starting...");

		m_fTimeElapsed = 0.0;
		m_eGameState = SCR_EGameModeState.GAME;
		Replication.BumpMe();

		// Raise event for authority
		OnGameStateChanged();
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
	}
	
}