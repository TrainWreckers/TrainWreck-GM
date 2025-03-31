
[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class TW_FactionSelectorAttribute : SCR_BasePresetsEditorAttribute
{			
	
	[Attribute("MISSING NAME", desc: "Text shown if Faction has no name assigned")]
	protected LocalizedString m_UnknownName;
	
	[Attribute("{4B4B51FACB828BF9}UI/Textures/Tasks/TaskIcons/96/Icon_Task_Unknown.edds", desc: "Icon used Faction is unknown")]
	protected ResourceName m_UnknownIcon;
	
	/*!
		This is static to help with performance. We shouldn't have to recollect things
		once we have processed things.		
	*/
	static ref map<int, ref SCR_Faction> s_CachedFactions = new map<int, ref SCR_Faction>();
	
	//! Save index so we can reload where player left off
	static int m_SelectedFactionIndex = -1;	
	
	override void InitializeVariable()
	{
		PrintFormat("WHY: Faction Index: %1", m_SelectedFactionIndex, LogLevel.ERROR);
		SetVariable(SCR_BaseEditorAttributeVar.CreateInt(m_SelectedFactionIndex));
	}
	
	override void PreviewVariable(bool setPreview, SCR_AttributesManagerEditorComponent manager)
	{
		if(!GetVariable()) return;
		
		m_SelectedFactionIndex = GetVariable().GetInt();
		if(s_CachedFactions && s_CachedFactions.Count() > 0)
		{				 
			TW_CharacterSelectorAttribute attribute = TW_CharacterSelectorAttribute.Cast(manager.GetAttributeRef(TW_CharacterSelectorAttribute));
			attribute.OnFactionChanged(s_CachedFactions.Get(m_SelectedFactionIndex));			
		}
	}
	
	private void InitializeFactions()
	{
		s_CachedFactions.Clear();
		
		SCR_DelegateFactionManagerComponent delegateFactionManager = SCR_DelegateFactionManagerComponent.GetInstance();		
		if (!delegateFactionManager)
		{
			Print("Delegate Faction Manager not present", LogLevel.ERROR);
			return;
		}
		
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
		{
			Print("Faction Manager not present", LogLevel.ERROR);
			return;
		}			
		
		SCR_SortedArray<SCR_EditableFactionComponent> factionDelegates = new SCR_SortedArray<SCR_EditableFactionComponent>;
		int count = delegateFactionManager.GetSortedFactionDelegates(factionDelegates);
		SCR_Faction faction;
		
		ref array<SCR_Faction> validFactions = new array<SCR_Faction>();
		
		// First filter out all factions
		for(int i = 0; i < count; i++)
		{
			faction = SCR_Faction.Cast(factionDelegates.Get(i).GetFaction());
			
			if(!faction)
				continue;	
			
			validFactions.Insert(faction);
		}
		
		count = validFactions.Count();
		// Now assign them indices
		for(int i = 0; i < count; i++)
			s_CachedFactions.Insert(i, validFactions.Get(i));		
		
		PrintFormat("Discovered %1 factions", count);
	}
	
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		if(!s_CachedFactions || s_CachedFactions.IsEmpty())
			InitializeFactions();
		
		if(m_SelectedFactionIndex < 0)
			m_SelectedFactionIndex = 0;
		
		TW_SpawnInBuildings sib = TW_SpawnInBuildingsAttribute.IsValidEntity(item);
		
		if(!sib)
			return null;
		
		if(s_CachedFactions.IsEmpty())
		{
			PrintFormat("TrainWreck: Faction Selector requires a Faction Manager to be present on the map", LogLevel.ERROR);
			return null;
		}
		
		if(!sib.GetFaction())
		{
			sib.SetFaction(s_CachedFactions.Get(m_SelectedFactionIndex));
			return SCR_BaseEditorAttributeVar.CreateInt(m_SelectedFactionIndex);
		}
		
		int count = s_CachedFactions.Count();
		for(int i = 0; i < count; i++)
		{
			if(sib.GetFaction().GetFactionKey() == s_CachedFactions.Get(i).GetFactionKey())
				return SCR_BaseEditorAttributeVar.CreateInt(i);	
		}
		
		return SCR_BaseEditorAttributeVar.CreateInt(m_SelectedFactionIndex);
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{		
		if(!s_CachedFactions || s_CachedFactions.IsEmpty())
			InitializeFactions();
		
		if(!var)
			return;		
		
		TW_SpawnInBuildings sib = TW_SpawnInBuildingsAttribute.IsValidEntity(item);
		
		if(!sib)
			return;
		
		int index = var.GetInt();
		if(index >= 0 && index < s_CachedFactions.Count())
		{
			sib.SetFaction(s_CachedFactions.Get(index));
			m_SelectedFactionIndex = index;
			PrintFormat("Selected Faction Index: %1", index, LogLevel.WARNING);
		}
		else
		{
			PrintFormat("Faction Index %1 is out of bounds. Must be a number between 0 and %2", index, s_CachedFactions.Count(), LogLevel.ERROR);
		}
	}
	
	override protected void CreatePresets()
	{
		m_aValues.Clear();
		
		SCR_EditorAttributeFloatStringValueHolder value;
		
		if(!s_CachedFactions || s_CachedFactions.IsEmpty())
			InitializeFactions();
		
		int count = s_CachedFactions.Count();
		string factionName;
		
		for(int i = 0; i < count; i++)
		{
			value = new SCR_EditorAttributeFloatStringValueHolder();
			
			factionName = s_CachedFactions.Get(i).GetFactionName();
			value.SetWithUIInfo(s_CachedFactions.Get(i).GetUIInfo(), i);
			m_aValues.Insert(value);
		}
	}		
};
