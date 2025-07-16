class TW_GridFilterBase
{
	bool ShouldIncludeCoord(int x, int y) { return true; }
	void ProcessGrid(TW_GridCoordArray coordArray);
};

class TW_GridTimeFilter : TW_GridFilterBase
{
	private ref map<string, float> _timeMap = new map<string, float>();
	
};

class TW_GridDensityFilter : TW_GridFilterBase
{
	private int m_MaxDensity;
	int GetMaxDensity() { return m_MaxDensity; }
};