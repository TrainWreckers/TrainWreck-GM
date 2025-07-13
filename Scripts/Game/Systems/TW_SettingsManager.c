class TW_SettingsInterface<Class T>
{
	protected static bool isInitialized;
	static bool IsInitialized() { return isInitialized; }
	
	void Initialize(T settings);
	void SaveSettings();
	void ResetToDefault();
};

class TW_SettingsManager<TW_SettingsInterface TInterface>
{
	protected ref TInterface _interface;
	
	TInterface GetInterface() { return _interface; }
	
	void TW_SettingsManager()
	{
		_interface = new TInterface();
		PrintFormat("TrainWreck: Settings manager for %1 initialized", GetInterface().Type());
	}
	
	private static ref TW_SettingsManager<TInterface> _instance;
	static TW_SettingsManager<TInterface> GetInstance()
	{
		if(_instance) return _instance;
		_instance = new TW_SettingsManager<TInterface>();
		return _instance;
	}
};

//! Dummy settings to make things compile
class DummySettings {};

//! Dummy interface to make things compile
class TW_SettingsDummyInterface : TW_SettingsInterface<DummySettings>{};

//! Dummy manager, simply to make things compile
typedef TW_SettingsManager<ref TW_SettingsDummyInterface<DummySettings>> TW_DummySettingsManager;