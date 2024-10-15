[BaseContainerProps(configRoot: true)]
class FactionCompositions
{
	[Attribute("", UIWidgets.ResourceNamePicker, params: "et", desc: "List of composition prefabs to use for defensive positions", category: "Defense")]
	private ref array<ResourceName> m_DefensiveWalls;
	
	[Attribute("", UIWidgets.ResourceNamePicker, params: "et", desc: "List of composition prefabs to use for defensive positions", category: "Defense")]
	private ref array<ResourceName> m_DefensiveBunkers;
	
	[Attribute("", UIWidgets.ResourceNamePicker, params: "et", desc: "Small Compositions", category: "Small Prefabs")]
	private ref array<ResourceName> m_SmallCompositions;
	
	[Attribute("", UIWidgets.ResourceNamePicker, params: "et", desc: "Medium Compositions", category: "Medium Prefabs")]
	private ref array<ResourceName> m_MediumCompositions;
	
	[Attribute("", UIWidgets.ResourceNamePicker, params: "et", desc: "Large Compositions", category: "Large Prefabs")]
	private ref array<ResourceName> m_LargeCompositions;
	
	bool HasDefensiveWalls() { return !m_DefensiveWalls.IsEmpty(); }
	bool HasDefensiveBunkers() { return !m_DefensiveBunkers.IsEmpty(); }
	bool HasSmallCompositions() { return !m_SmallCompositions.IsEmpty(); }
	bool HasMediumCompositions() { return !m_MediumCompositions.IsEmpty(); }
	bool HasLargeCompositions() { return !m_LargeCompositions.IsEmpty(); }
	
	ResourceName GetRandomDefensiveWall() 
	{ 
		if(m_DefensiveWalls.IsEmpty())
			return ResourceName.Empty;
		
		return m_DefensiveWalls.GetRandomElement(); 
	}
	
	ResourceName GetRandomDefensiveBunkder() 
	{ 
		if(m_DefensiveBunkers.IsEmpty())
			return ResourceName.Empty;
		
		return m_DefensiveBunkers.GetRandomElement(); 
	}
	
	ResourceName GetRandomSmallComposition() 
	{ 
		if(m_SmallCompositions.IsEmpty())
			return ResourceName.Empty;
		
		return m_SmallCompositions.GetRandomElement(); 
	}
	
	ResourceName GetRandomMediumComposition() 
	{ 
		if(m_MediumCompositions.IsEmpty())
			return ResourceName.Empty;
		
		return m_MediumCompositions.GetRandomElement(); 
	}
	
	ResourceName GetRandomLargeComposition() 
	{ 
		if(m_LargeCompositions.IsEmpty())
			return ResourceName.Empty;
		
		return m_LargeCompositions.GetRandomElement(); 
	}
};

enum TW_CompositionType
{	
	LIVING_QUARTERS = 1 << 1,
	BARRACKS = 1 << 2,
	AMMO_DEPOT = 1 << 3,
	LIGHT_VEHICLE_DEPOT = 1 << 4,
	HEAVY_VEHICLE_DEPOT = 1 << 5,
	HELIPAD = 1 << 6,
	FUEL_DEPOT = 1 << 7		
};
