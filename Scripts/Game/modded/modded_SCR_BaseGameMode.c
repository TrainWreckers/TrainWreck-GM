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

[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class DisableGMBudget_BudgetsEnabledAttribute : SCR_BaseEditorAttribute
{	
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		//If opened in global attributes
		
		SCR_BaseGameMode gamemode = SCR_BaseGameMode.Cast(item);
		if (!gamemode)
			return null;
		
		return SCR_BaseEditorAttributeVar.CreateBool(gamemode.DisableGMBudget_AreBudgetsEnabled());
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (!var) 
			return;
		
		SCR_BaseGameMode gamemode = SCR_BaseGameMode.Cast(item);
		if (!gamemode)
			return;
		
		gamemode.DisableGMBudget_SetBudgetsEnabled(var.GetBool());
	}
};

modded class SCR_BudgetEditorComponent 
{
	protected ref map<EEditableEntityBudget, int> m_DisableGMBudget_OriginalMaxBudgets = new map<EEditableEntityBudget, int>();
	
	override void EOnEditorInit() 
	{
		super.EOnEditorInit();
		
		foreach	(SCR_EntityBudgetValue maxBudget : m_MaxBudgets)
		{
			m_DisableGMBudget_OriginalMaxBudgets.Set(maxBudget.GetBudgetType(), maxBudget.GetBudgetValue());
		};
		
		SCR_BaseGameMode game = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!game || game.DisableGMBudget_AreBudgetsEnabled())
			return;
			
		DisableGMBudget_BudgetsUpdated(false);
	};
	
	void DisableGMBudget_BudgetsUpdated(bool enabled) 
	{
		foreach	(SCR_EntityBudgetValue maxBudget : m_MaxBudgets)
		{
			if (enabled)
				maxBudget.SetBudgetValue(m_DisableGMBudget_OriginalMaxBudgets.Get(maxBudget.GetBudgetType()));
			else
				maxBudget.SetBudgetValue(m_DisableGMBudget_OriginalMaxBudgets.Get(maxBudget.GetBudgetType()) * 500);
		};
	};
};
