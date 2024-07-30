[WorkbenchPluginAttribute(name: "TrainWreck Hierarchy Plugin", category: "TrainWreck Plugins", shortcut: "Ctrl+T", wbModules: {"WorldEditor"})]
class TrainWreckWorldEditorPlugin : WorldEditorPlugin
{
	override void Run()
	{
		// Get World Editor module
		WorldEditor worldEditor = Workbench.GetModule(WorldEditor);
		// Get World Editor API
		WorldEditorAPI api = worldEditor.GetApi();
		
		int selectedEntitiesCount;
		selectedEntitiesCount = api.GetSelectedEntitiesCount();
		
		int count = 0;
		
		for(int i = 0; i < selectedEntitiesCount; i++)
		{
			IEntitySource selected = api.GetSelectedEntity(i);
			
			SCR_PrefabHelper.CreateEntitySourceComponentIfNeeded(selected, "Hierarchy");
			count++;			
		}
		
		Print(string.Format("TrainWreck: Added %1 Hierarchy Components", count));
	}
}