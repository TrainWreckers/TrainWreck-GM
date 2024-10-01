// Not TrainWreck, brought in to ensure if mod ever breaks we can fix it ourselves 
// Credit goes to ceo of bacon

modded class SCR_BaseGameMode 
{
	[RplProp(onRplName: "DisableGMBudget_OnBroadcastValueUpdated")]
	bool m_DisableGMBudgets_BudgetsEnabled = true;
	
	protected ref TW_MonitorPositions positionMonitor;
	protected float m_PositionMonitorUpdateInterval = 10.0;
	
	/*!
		This update doesn't trigger every interval. This only changes
		when player chunks from previous check are different from current
	
		This update will pass along the various chunk information
		- Player chunk positions
		- Anti spawn chunk positions
		- Unloaded chunks
	*/
	ScriptInvoker<GridUpdateEvent> GetOnPlayerPositionsUpdated() 
	{ 
		if(!positionMonitor) 
			return null;
		
		return positionMonitor.GetGridUpdate(); 
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
		
		InitializePositionMonitor();
		
		// 10 seconds before things kick off
		GetGame().GetCallqueue().CallLater(DelayEvents, 10000, false);	
	};
	
	private void DelayEvents()
	{
		if(positionMonitor)
			GetGame().GetCallqueue().CallLater(MonitorUpdate, m_PositionMonitorUpdateInterval * 1000, true);
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