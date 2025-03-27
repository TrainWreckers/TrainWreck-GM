class TW_AISafeAreaAttributes
{
	static TW_AISafeArea IsValidEntity(Managed item)
	{
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		
		if(!editableEntity)
			return null;
		
		IEntity owner = editableEntity.GetOwner();
		
		if(!owner)
			return null;
		
		return TW_AISafeArea.Cast(owner);
	}
};

[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class TW_AISafeAreaRadiusAttribute : SCR_BaseValueListEditorAttribute
{
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		TW_AISafeArea sib = TW_AISafeAreaAttributes.IsValidEntity(item);
		
		if(!sib) return null;
		
		return SCR_BaseEditorAttributeVar.CreateFloat(sib.GetRadius());				
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if(!var)
			return;
		
		TW_AISafeArea sib = TW_AISafeAreaAttributes.IsValidEntity(item);
		
		if(!sib)
			return;
		
		sib.SetRadius(Math.AbsFloat(var.GetFloat()));
	}
};