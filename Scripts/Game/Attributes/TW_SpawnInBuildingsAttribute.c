/*
	Spawn in buildings attribute 

	For getting and setting variables in Editor Attribute Window
*/
class TW_SpawnInBuildingsAttribute 
{
	static TW_SpawnInBuildings IsValidEntity(Managed item)
	{
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		if(!editableEntity)
			return null;
		
		IEntity owner = editableEntity.GetOwner();
		
		if(!owner)
			return null;
		
		return TW_SpawnInBuildings.Cast(owner);
	}
};

[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class TW_SpawnInBuildingsRadiusAttribute : SCR_BaseValueListEditorAttribute
{
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		TW_SpawnInBuildings sib = TW_SpawnInBuildingsAttribute.IsValidEntity(item);
		
		if(!sib)
			return null;
		
		// If this entity is already active, then we'll ignore it
		if(sib.IsActive())
			return null;
		
		return SCR_BaseEditorAttributeVar.CreateFloat(sib.GetSpawnRadius());
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if(!var)
			return;
		
		TW_SpawnInBuildings sib = TW_SpawnInBuildingsAttribute.IsValidEntity(item);
		
		if(!sib)
			return;
		
		if(sib.IsActive())
			return;
		
		sib.SetSpawnRadius(Math.AbsFloat(var.GetFloat()));
	}
}

[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class TW_SpawnInBuildingsAmountAttribute : SCR_BaseValueListEditorAttribute
{
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		TW_SpawnInBuildings sib = TW_SpawnInBuildingsAttribute.IsValidEntity(item);
		
		if(!sib)
			return null;
		
		if(sib.IsActive())
			return null;
		
		return SCR_BaseEditorAttributeVar.CreateInt(sib.GetSpawnAmount());
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if(!var) 
			return;
		
		TW_SpawnInBuildings sib = TW_SpawnInBuildingsAttribute.IsValidEntity(item);
		
		if(!sib)
			return;
		
		if(sib.IsActive())
			return;
		
		sib.SetSpawnAmount(Math.AbsInt(var.GetInt()));
	}
}

[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class TW_SpawnInBuildingsActiveAttribute : SCR_BaseValueListEditorAttribute
{
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		TW_SpawnInBuildings sib = TW_SpawnInBuildingsAttribute.IsValidEntity(item);
		
		if(!sib)
			return null;
		
		return SCR_BaseEditorAttributeVar.CreateBool(sib.IsActive());
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if(!var)
			return;
		
		TW_SpawnInBuildings sib = TW_SpawnInBuildingsAttribute.IsValidEntity(item);
		
		if(!sib)
			return;
		
		sib.SetIsActive(var.GetBool());
	}
}

[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class TW_SpawnInBuildingsFactionType : SCR_BaseValueListEditorAttribute
{
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		TW_SpawnInBuildings sib = TW_SpawnInBuildingsAttribute.IsValidEntity(item);
		
		if(!sib)
			return null;
		
		return SCR_BaseEditorAttributeVar.CreateInt(sib.GetFactionType());
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if(!var)
			return;
		
		TW_SpawnInBuildings sib = TW_SpawnInBuildingsAttribute.IsValidEntity(item);
		
		if(!sib)
			return;
		
		sib.SetFactionType(var.GetInt());
	}
}