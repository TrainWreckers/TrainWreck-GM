class PercentageFieldSetting
{
	int Min;
	int Max;
	
	int GetRandomPercentage() { return Math.RandomIntInclusive(Min, Max); }
}