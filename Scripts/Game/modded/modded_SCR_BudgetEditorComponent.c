modded class SCR_BudgetEditorComponent 
{
	private static bool TW_IsBudgetEnabled = true;
	static bool GetIsBudgetEnabled() { return TW_IsBudgetEnabled; }
	static void SetIsBudgetEnabled(bool value) { TW_IsBudgetEnabled = value; }
	
	override bool IsBudgetCapEnabled() 
	{ 
		return TW_IsBudgetEnabled; 
	}
};