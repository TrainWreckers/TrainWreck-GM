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