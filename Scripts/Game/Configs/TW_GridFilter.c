class TW_GridFilterBase
{
	/*
		Intended to be used within the TW_GridCoordArrayManager.
	
		As it iterates over chunks to include it will pass the chunk coordinates into this method.
		If this method returns true - it will include the chunk.
		Otherwise, it'll skip the chunk
	*/
	bool ShouldIncludeCoord(int x, int y) { return true; }
	
	/*!
		Intended to be subscribed to TW_MonitorPositions. 
		This should allow things like special tracking of chunks
	*/
	void OnPlayerPositionsChanged(GridUpdateEvent gridInfo);

	private SCR_BaseGameMode _gameMode;
	protected SCR_BaseGameMode GetGameMode()
	{
		if(_gameMode)
			return _gameMode;
		_gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		return _gameMode;
	}
};

class TW_GridTimeFilter : TW_GridFilterBase
{
	private ref map<string, float> _timeMap = new map<string, float>();
	
	private int _timeThreshold = 30;
	
	private bool HasNewChunks(float currentTime)
	{
		foreach(string chunk, float time : _timeMap)
			if(time <= _timeThreshold)
				return true;
		return false;
	}
	
	override void OnPlayerPositionsChanged(GridUpdateEvent gridInfo)
	{
		ref set<string> unloaded = gridInfo.GetUnloadedChunks();
		foreach(string chunk : unloaded)
		{
			if(_timeMap.Contains(chunk))
				_timeMap.Remove(chunk);
		}
		
		ref set<string> playerChunks = gridInfo.GetPlayerChunks();
		float time = GetGameMode().GetElapsedTime();
		foreach(string chunk : playerChunks)
		{
			if(_timeMap.Contains(chunk))
				continue;
			_timeMap.Insert(chunk, time);
		}
	}
	
	override bool ShouldIncludeCoord(int x, int y)
	{
		float currentTime = GetGameMode().GetElapsedTime();
		string chunk = string.Format("%1 %2", x, y);
		
		// We only want to restrict/filter chunks if we have chunks
		// meeting our time threshold. Otherwise we'll revert to default behavior
		if(HasNewChunks(currentTime))
		{
			if(_timeMap.Contains(chunk))
			{
				float time = _timeMap.Get(chunk);
				
				//TODO: Make this configurable
				if(Math.AbsInt(currentTime - time) > 30)
					return false;
			}
		}
		
		return true;
	}
};