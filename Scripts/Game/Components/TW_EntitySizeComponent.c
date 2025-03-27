class TW_EntitySizeComponentClass : ScriptComponentClass {};

class TW_EntitySizeComponent : ScriptComponent 
{
	override void OnPostInit(IEntity owner)
	{
		ref TW_MustSpawnPrefabConfig settings = new TW_MustSpawnPrefabConfig();
		
		
		typename settingsType = settings.Type();
//		settingsType.ClassName();
		int varCount = settingsType.GetVariableCount();
		for(int i = 0; i < varCount; i++)
		{
			string varname = settingsType.GetVariableName(i);
			typename vartype = settingsType.GetVariableType(i);
			
			PrintFormat("Var '%1' is of type %2", varname, vartype);
		}
	}
};