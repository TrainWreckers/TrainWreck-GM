class LocationConfig : JsonApiStruct
{
	// Number of grid chunks in radius format
	int radius;
	
	void SetData(int _radius)
	{
		this.radius = _radius;
	}
};