// Not TrainWreck, brought in to ensure if mod ever breaks we can fix it ourselves 
// Credit goes to ceo of bacon

modded class SCR_BaseGameMode 
{
	[RplProp(onRplName: "DisableGMBudget_OnBroadcastValueUpdated")]
	bool m_DisableGMBudgets_BudgetsEnabled = true;
	
	override void EOnInit(IEntity owner) {
		super.EOnInit(owner);
	};
	
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