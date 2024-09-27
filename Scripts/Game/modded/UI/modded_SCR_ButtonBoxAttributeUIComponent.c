modded class SCR_ButtonBoxAttributeUIComponent
{
	private Widget myWidget;
	private SCR_BaseEditorAttribute myAttribute;
	
	private void Refresh()
	{
		if(!myWidget || !myAttribute) return;
		
		Widget toolBox = myWidget.FindAnyWidget(m_sUiComponentName);
		if (!toolBox) return;
		
		m_ToolBoxComponent = SCR_ToolboxComponent.Cast(toolBox.FindHandler(SCR_ToolboxComponent));
		if (!m_ToolBoxComponent) return;
		
		m_ToolBoxComponent.ClearAll();
		
		super.Init(myWidget, myAttribute);	
	}
	
	override void Init(Widget w, SCR_BaseEditorAttribute attribute)
	{		
		if(!w || !attribute)
			return;
		
		myWidget = w;
		myAttribute = attribute;
		
		SCR_BasePresetsEditorAttribute moddedPresets = SCR_BasePresetsEditorAttribute.Cast(attribute);
		if(moddedPresets)
		{
			moddedPresets.m_OnEntriesChanged.Insert(Refresh);
			moddedPresets.InitializeVariable();
		}
		
		super.Init(w, attribute);
	}
};