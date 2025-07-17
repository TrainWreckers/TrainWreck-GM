class TW_GridDensitySettings
{
	//! Maximum AI that can spawn within a grid. This value behaves like a unit-cap per grid.
	int MaxAIPerGrid = 30;
	
	//! Percentage to apply on AI budget
	float AIBudgetRatio = 0.2;
};

class TW_GeneralSettings
{
	
	bool IsDebug = true;
	
	[NonSerialized()]
	private static ref TW_GeneralSettings _instance;
	
	static TW_GeneralSettings GetInstance()
	{
		if(_instance) 
			return _instance;
		
		_instance = LoadFromFile();
		return _instance;
	}
	
	ref TW_GridDensitySettings GridDensitySettings;
	
	static TW_GeneralSettings GetDefault()
	{
		ref TW_GeneralSettings settings = new TW_GeneralSettings();
		
		settings.GridDensitySettings = new TW_GridDensitySettings();
		
		return settings;
	}
	
	[NonSerialized()]
	static bool IsDirty = false;
	const string FILENAME = "$profile:trainWreckGeneralSettings.json";
	
	private void Save()
	{
		SaveToFile(this);
		IsDirty = false;
	}
	
	private void SetDirty()
	{
		if(IsDirty) return;
		IsDirty = true;
		GetGame().GetCallqueue().CallLater(Save, 1000);
	}
	
	static bool SaveToFile(TW_GeneralSettings settings)
	{
		if(!TW_Util.SaveJsonFile(FILENAME, settings, true))
		{
			PrintFormat("TrainWreck: TW_GeneralSettings -> Failed to save to file '%1'", FILENAME, LogLevel.ERROR);
			return false;
		}
		
		return true;
	}
	
	static TW_GeneralSettings LoadFromFile(string json = string.Empty)
	{
		SCR_JsonLoadContext loadContext;
		if(json != string.Empty)
		{
			loadContext = new SCR_JsonLoadContext();
			loadContext.ImportFromString(json);
		}
		else 
			loadContext = TW_Util.LoadJsonFile(FILENAME, true);
		
		if(!loadContext)
		{
			PrintFormat("TrainWreck: TW_GeneralSettings -> failed to load settings from: '%1'", FILENAME, LogLevel.ERROR);
			ref TW_GeneralSettings settings = GetDefault();
			TW_GeneralSettings.SaveToFile(settings);
			return settings;
		}
		
		ref TW_GeneralSettings settings = new TW_GeneralSettings();
		
		if(!loadContext.ReadValue("", settings))
		{
			PrintFormat("TrainWreck: TW_GeneralSettings -> Failed to laod from file. Retrieving default settings", LogLevel.ERROR);
			settings = GetDefault();
			TW_GeneralSettings.SaveToFile(settings);
			return settings;
		}
		
		PrintFormat("TrainWreck TW_GeneralSettings -> Successfully loaded '%1'", FILENAME);
		return settings;
	}
};