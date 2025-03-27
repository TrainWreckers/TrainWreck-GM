//! Composition Size Type, where the integer value corresponds to its diameter
enum TW_CompositionSize
{
	SMALL = 8,
	MEDIUM = 15,
	LARGE = 23
};

enum TW_CompositionType
{
	SMALL = 0,
	MEDIUM = 1,
	LARGE = 2,
	WALLS = 3,
	BUNKERS = 4,
	NONE = 5
};


[BaseContainerProps(configRoot: true)]
class TW_MustSpawnPrefabConfig
{
	[Attribute("", UIWidgets.ComboBox, "Size of prefab", "", ParamEnumArray.FromEnum(TW_CompositionSize))]
	private TW_CompositionSize _compositionSize;
	
	[Attribute("", UIWidgets.ResourceNamePicker, params: "et")]
	private ResourceName _prefab;
	
	ResourceName GetPrefab() { return _prefab; }
	TW_CompositionSize GetSize() { return _compositionSize; }
};