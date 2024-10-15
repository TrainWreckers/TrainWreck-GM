class LocationConfig : JsonApiStruct
{
	// Number of grid chunks in radius format
	int radius;
	
	void SetData(int radius)
	{
		this.radius = radius;
	}
};