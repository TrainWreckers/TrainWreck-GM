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
	
	void SetData(EMapDescriptorType locationType, vector worldPosition, string locationName, FactionKey ownedBy = FactionKey.Empty, set<string> ownedChunks = null)
	{
		this.locationType = locationType;
		this.worldPosition = worldPosition;
		this.locationName = locationName;
		this.ownedBy = ownedBy;
		
		if(ownedChunks)
			this.ownershipChunks = ownedChunks;
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