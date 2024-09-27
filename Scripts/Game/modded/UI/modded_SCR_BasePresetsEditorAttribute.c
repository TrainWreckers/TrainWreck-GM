modded class SCR_BasePresetsEditorAttribute
{	
	//! When attribute needs to inform layout to re-render things due to new options being available.
	ref ScriptInvoker m_OnEntriesChanged = new ScriptInvoker();
	
	/*!
		This is called before entries are created in layout.
		
		The initialize function in base object does not get called each time
		a player tries modifying it			
	*/
	void InitializeVariable()
	{
		
	}
};