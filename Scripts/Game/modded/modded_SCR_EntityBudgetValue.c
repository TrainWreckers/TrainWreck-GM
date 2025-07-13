[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(EEditableEntityBudget, "m_BudgetType")]
modded class SCR_EntityBudgetValue
{
	override int GetBudgetValue() 
	{
		if(SCR_BudgetEditorComponent.GetIsBudgetEnabled()) 
			return m_Value;
		
		return int.MAX; 
	}
};
