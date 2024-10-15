class LocationSettings
{
	//! Is module enabled
	bool isEnabled;
	
	//! Size of grid to use for locations
	int gridSize;
	
	//! Type of locations that are of interest to the scenario creator
	ref map<EMapDescriptorType, ref LocationConfig> locationSettings;
	
	bool IsValidMapDescriptor(EMapDescriptorType type)
	{
		return locationSettings && locationSettings.Contains(type);
	}
	
	LocationConfig GetConfigType(EMapDescriptorType type)
	{
		if(IsValidMapDescriptor(type))
			return locationSettings.Get(type);
		
		return null;
	}
	
	static string GetTypeName(EMapDescriptorType type)
	{
		return SCR_Enum.GetEnumName(EMapDescriptorType, type);
	}
	
	private static ref map<string, EMapDescriptorType> cachedTypes = new map<string, EMapDescriptorType>();
	
	static const string FILENAME = "$profile:locationSettings.json";
	
	static bool SaveToFile(LocationSettings settings)
	{
		ContainerSerializationSaveContext saveContext = new ContainerSerializationSaveContext();
		PrettyJsonSaveContainer prettyContainer = new PrettyJsonSaveContainer();
		saveContext.SetContainer(prettyContainer);
		
		saveContext.WriteValue("isEnabled", settings.isEnabled);
		saveContext.WriteValue("gridSize", settings.gridSize);
		
		ref map<string, ref LocationConfig> areaSettings = new map<string, ref LocationConfig>();
		foreach(string name, EMapDescriptorType type : cachedTypes)
		{
			if(!settings.IsValidMapDescriptor(type)) continue;
			
			areaSettings.Set(name, settings.locationSettings.Get(type));
		}
		
		saveContext.WriteValue("locations", areaSettings);
		return prettyContainer.SaveToFile(FILENAME);
	}
	
	private static LocationSettings GetDefault()
	{
		ref LocationSettings settings = new LocationSettings();
		
		settings.isEnabled = true;
		settings.gridSize = 250;
		
		settings.locationSettings = new map<EMapDescriptorType, ref LocationConfig>();
		
		ref LocationConfig cityConfig = new LocationConfig();
		cityConfig.SetData(5);
		
		ref LocationConfig townConfig = new LocationConfig();
		townConfig.SetData(5);
		
		ref LocationConfig villageConfig = new LocationConfig();
		villageConfig.SetData(4);
		
		ref LocationConfig generic = new LocationConfig();
		generic.SetData(2);
		
		settings.locationSettings.Set(EMapDescriptorType.MDT_NAME_CITY, cityConfig);
		settings.locationSettings.Set(EMapDescriptorType.MDT_NAME_TOWN, townConfig);
		settings.locationSettings.Set(EMapDescriptorType.MDT_NAME_VILLAGE, villageConfig);
		settings.locationSettings.Set(EMapDescriptorType.MDT_NAME_SETTLEMENT, villageConfig);
		settings.locationSettings.Set(EMapDescriptorType.MDT_BASE, townConfig);
		settings.locationSettings.Set(EMapDescriptorType.MDT_AIRPORT, townConfig);
		settings.locationSettings.Set(EMapDescriptorType.MDT_PORT, townConfig);
		settings.locationSettings.Set(EMapDescriptorType.MDT_NAME_GENERIC, generic);
		
		return settings;
	}
	
	private static void InitCacheTypes()
	{
		if(cachedTypes.Count() > 0) return;
		
		ref array<int> values = {};
		SCR_Enum.GetEnumValues(EMapDescriptorType, values);
		
		foreach(int type : values)
		{
			string name = SCR_Enum.GetEnumName(EMapDescriptorType, type);
			cachedTypes.Set(name, type);
		}
	}
	
	static LocationSettings LoadFromFile()
	{
		InitCacheTypes();
		
		SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
		
		if(!loadContext.LoadFromFile(FILENAME))
		{
			LocationSettings defaultSettings = GetDefault();
			SaveToFile(defaultSettings);
			return defaultSettings;
		}
		
		ref map<string, ref LocationConfig> areaSettings = new map<string, ref LocationConfig>();
		LocationSettings settings = new LocationSettings();
		
		loadContext.ReadValue("isEnabled", settings.isEnabled);
		loadContext.ReadValue("gridSize", settings.gridSize);
		
		loadContext.ReadValue("locations", areaSettings);
		settings.locationSettings = new map<EMapDescriptorType, ref LocationConfig>();
		
		foreach(string name, LocationConfig config : areaSettings)
		{
			if(!cachedTypes.Contains(name))
				continue;
			
			settings.locationSettings.Set(cachedTypes.Get(name), config);
		}
		
		return settings;
	}
};