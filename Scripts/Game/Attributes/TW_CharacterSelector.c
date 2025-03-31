
[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class TW_CharacterSelectorAttribute : SCR_BasePresetsEditorAttribute
{			
	
	[Attribute("MISSING NAME", desc: "Text shown if Faction has no name assigned")]
	protected LocalizedString m_UnknownName;
	
	[Attribute("{4B4B51FACB828BF9}UI/Textures/Tasks/TaskIcons/96/Icon_Task_Unknown.edds", desc: "Icon used Faction is unknown")]
	protected ResourceName m_UnknownIcon;		
	
	private ref map<int, ResourceName> m_CharacterPrefabs = new map<int, ResourceName>();
	private SCR_Faction m_SelectedFaction;
	static int m_SelectedIndex = 0;
	
	override void InitializeVariable()
	{
		SetVariable(SCR_BaseEditorAttributeVar.CreateInt(m_SelectedIndex));
	}
	
	void OnFactionChanged(SCR_Faction faction)
	{
		m_SelectedFaction = faction;		
		PrintFormat("New Faction for characters: %1", m_SelectedFaction.GetFactionName(), LogLevel.WARNING);
		InitializeCharacters();	
	}
	
	private string Strip(ResourceName prefab)
	{
		string path = TW_Util.GetNameFromPath(prefab);		
		path.Replace("_", " ");
		path.Replace("Character", "");
		return path;
	}
	
	private void InitializeCharacters()
	{
		if(!m_SelectedFaction)
		{
			m_CharacterPrefabs.Clear();
			m_aValues.Clear();
			PrintFormat("Character selector requires a valid faction", LogLevel.ERROR);
			return;
		}
		
		m_CharacterPrefabs.Clear();
		
		SCR_EntityCatalog characterCatalog = m_SelectedFaction.GetFactionEntityCatalogOfType(EEntityCatalogType.CHARACTER);
		
		if(!characterCatalog)
		{
			PrintFormat("Was unable to locate character catalog for faction: %1", m_SelectedFaction.GetFactionName(), LogLevel.ERROR);
			return;
		}
		
		ref array<SCR_EntityCatalogEntry> entries = new array<SCR_EntityCatalogEntry>();		
		int count = characterCatalog.GetEntityList(entries);
		
		int index = 0;
		foreach(SCR_EntityCatalogEntry entry : entries)
		{			
			m_CharacterPrefabs.Insert(index, entry.GetPrefab());
			index++;
		}
		
		CreatePresets();
		m_OnEntriesChanged.Invoke();		
	}
	
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		if(!manager) 
			return null;
		
		SCR_BaseEditorAttributeVar factionVar;
		
		TW_SpawnInBuildings sib = TW_SpawnInBuildingsAttribute.IsValidEntity(item);
		
		if(!sib)
			return null;
		
		if(!m_SelectedFaction || (m_SelectedFaction.GetFactionKey() != sib.GetFaction().GetFactionKey()))
		{
			m_SelectedFaction = sib.GetFaction();
			InitializeCharacters();		
		}
				
		int count = m_CharacterPrefabs.Count();
		
		for(int i = 0; i < count; i++)
			if(m_CharacterPrefabs.Get(i) == sib.GetPrefab())
				return SCR_BaseEditorAttributeVar.CreateInt(i);
		
		if(count > 0)
			sib.SetPrefab(m_CharacterPrefabs.Get(0));
		
		return SCR_BaseEditorAttributeVar.CreateInt(0);
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{		
		if(!manager)
			return;
		
		SCR_BaseEditorAttributeVar factionVar;
		
		if(!m_SelectedFaction)
		{
			PrintFormat("TrainWreck: Character Selector does not have faction selected", LogLevel.ERROR);
			return;
		}
		
		if(!var)
			return;		
		
		TW_SpawnInBuildings sib = TW_SpawnInBuildingsAttribute.IsValidEntity(item);
		
		if(!sib)
			return;
		
		int index = var.GetInt();
		
		if(index < 0 || index >= m_CharacterPrefabs.Count())
		{
			PrintFormat("Character Index must be >= 0 || < %1: Got %2", m_CharacterPrefabs.Count(), index, LogLevel.ERROR);			
			return;
		}
		
		m_SelectedIndex = index;
		sib.SetPrefab(m_CharacterPrefabs.Get(index));
	}
	
	override protected void CreatePresets()
	{		
		m_aValues.Clear();
		
		SCR_EditorAttributeFloatStringValueHolder value;
		int count = m_CharacterPrefabs.Count();
		
		for(int i = 0; i < count; i++)
		{
			value = new SCR_EditorAttributeFloatStringValueHolder();
			value.SetName(Strip(m_CharacterPrefabs.Get(i)));
			value.SetFloatValue(i);
			m_aValues.Insert(value);
		}		
	}		
};
