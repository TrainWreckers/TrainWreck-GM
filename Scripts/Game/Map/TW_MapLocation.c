typedef ScriptInvoker<TW_MapLocation> LocationCapturedEvent;

class TW_MapLocation
{
	//! Type of location
	private EMapDescriptorType locationType;
	
	//! Display name of location
	private string locationName;
	
	//! Who owns this location
	private FactionKey ownedBy;
	
	//! World coordinates of location
	private vector worldPosition;
	
	//! Chunks beloning to location
	private ref set<string> ownershipChunks;
	
	//! Event that triggers when ownership changes for this location
	private ref LocationCapturedEvent OnLocationCaptured = new LocationCapturedEvent();
	
	ref set<string> GetCoordinates()
	{
		ref set<string> items = new set<string>();
		foreach(string coord : ownershipChunks)
			items.Insert(coord);
		return items;
	}
	
	void SetData(EMapDescriptorType _locationType, vector _worldPosition, string _locationName, FactionKey _ownedBy = FactionKey.Empty, set<string> _ownedChunks = null)
	{
		this.locationType = _locationType;
		this.worldPosition = _worldPosition;
		this.locationName = _locationName;
		this.ownedBy = _ownedBy;
		
		if(_ownedChunks)
			this.ownershipChunks = _ownedChunks;
		else
			this.ownershipChunks = new set<string>();
	}		
	
	bool HasCoord(string coord) { return ownershipChunks && ownershipChunks.Contains(coord); }
	FactionKey OwnedBy() { return ownedBy; }
	string LocationName() { return locationName; }
	EMapDescriptorType LocationType() { return locationType; }
	vector GetPosition() { return worldPosition; }		
	
	LocationCapturedEvent OnCaptured() { return OnLocationCaptured; }
	
	void SetFactionOwner(FactionKey faction) 
	{ 
		ownedBy = faction;
		OnLocationCaptured.Invoke(this);
	}
	
	void InitializeChunks(notnull set<string> chunks)
	{
		if(ownershipChunks)
			ownershipChunks.Clear();
		
		foreach(string coord : chunks)
			ownershipChunks.Insert(coord);
	}
};