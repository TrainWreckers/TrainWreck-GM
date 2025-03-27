modded class SCR_BudgetEditorComponent 
{
	const int DEFAULT_MAX_BUDGET = int.MAX;
	override bool IsBudgetCapEnabled() { return false; }	
};

modded class SCR_EntityBudgetValue
{
	override int GetBudgetValue() { return int.MAX; }
	override void SetBudgetValue(int newValue) { m_Value = int.MAX; }
};
