class GridUpdateEvent
{
	private ref set<string> m_PlayerChunks;
	private ref set<string> m_AntiChunks;
	private ref set<string> m_UnloadedChunks;
	
	void GridUpdateEvent(set<string> chunks, set<string> anti, set<string> unloaded)
	{
		m_PlayerChunks = chunks;
		m_AntiChunks = anti;
		m_UnloadedChunks = unloaded;
	}
	
	set<string> GetAntiChunks() { return m_AntiChunks; }
	set<string> GetPlayerChunks() { return m_PlayerChunks; }
	set<string> GetUnloadedChunks() { return m_UnloadedChunks; }
};

class TW_MonitorPositions
{
	private static TW_MonitorPositions s_Instance;
	
	static TW_MonitorPositions GetInstance()
	{
		return s_Instance;
	}
	
	private ref set<string> m_PlayerChunks = new set<string>();
	private ref set<string> m_AntiSpawnChunks = new set<string>();
	
	private ref array<IEntity> m_Players = {};
	private int m_PlayerChunkCount = 0;
	
	private int m_GridSize;
	private int m_GridRadius;
	private int m_AntiSpawnGridSize;
	private int m_AntiSpawnDistanceInChunks;
	
	private ref ScriptInvoker<ref GridUpdateEvent> m_OnGridUpdate = new ScriptInvoker<ref GridUpdateResponse>();
	
	ScriptInvoker<ref GridUpdateEvent> GetGridUpdate() { return m_OnGridUpdate; }
	
	int GetGridSizeInMeters() { return m_GridSize; }
	int GetDistanceInChunks() { return m_GridRadius; }
	int GetAntiSpawnGridSizeInMeters() { return m_AntiSpawnGridSize; }
	int GetAntiSpawnDistanceInChunks() { return m_AntiSpawnDistanceInChunks; }
	
	void TW_MonitorPositions(int gridSize, int gridRadius, int antiGridSize, int antiRadius)
	{
		UpdateGrid(gridSize, gridRadius, antiGridSize, antiRadius);
		s_Instance = this;
	}
	
	void ~TW_MonitorPositions()
	{
		m_OnGridUpdate.Clear();
	}
	
	//! Update grid dimensions at runtime
	void UpdateGrid(int gridSize, int gridRadius, int antiGridSize, int antiRadius)
	{
		m_GridSize = gridSize;
		m_GridRadius = gridRadius;
		m_AntiSpawnGridSize = antiGridSize;
		m_AntiSpawnDistanceInChunks = antiRadius;
		
		m_PlayerChunks.Clear();
		m_PlayerChunkCount = 0;
		m_AntiSpawnChunks.Clear();
	}
	
	void MonitorPlayers()
	{
		ref array<int> playerIds = {};
		ref array<IEntity> players = {};
		
		GetGame().GetPlayerManager().GetPlayers(playerIds);
		
		ref set<string> currentPositions = new set<string>();
		ref set<string> unloaded = new set<string>();
		
		bool positionsHaveChanged = false;
		
		m_AntiSpawnChunks.Clear();
		
		ref set<string> chunksAroundPlayer = new set<string>();
		
		// Where are players currently located
		foreach(int playerId : playerIds)
		{
			auto player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
			
			if(!player)
				continue;
			
			chunksAroundPlayer.Clear();
			
			TW_Util.AddSurroundingGridSquares(m_AntiSpawnChunks, player.GetOrigin(), m_AntiSpawnDistanceInChunks, m_AntiSpawnGridSize);
			TW_Util.AddSurroundingGridSquares(chunksAroundPlayer, player.GetOrigin(), m_GridRadius, m_GridSize);
			
			foreach(string chunk : chunksAroundPlayer)
			{
				if(!m_PlayerChunks.Contains(chunk))
				{
					positionsHaveChanged = true;
					m_PlayerChunks.Insert(chunk);
				}
				
				if(!currentPositions.Contains(chunk))
					currentPositions.Insert(chunk);
			}
			
			players.Insert(player);
		}
		
		m_PlayerChunkCount = m_PlayerChunks.Count();
		
		// are previous chunks still valid?
		for(int i = 0; i < m_PlayerChunkCount; i++)
		{
			string playerCoord = m_PlayerChunks.Get(i);
			
			if(!currentPositions.Contains(playerCoord))
			{
				unloaded.Insert(playerCoord);
				m_PlayerChunks.Remove(i);
				
				// Must remember to decrement otherwise we skip an item
				i -= 1;
				m_PlayerChunkCount -= 1;
				
				positionsHaveChanged = true;
			}
		}
		
		if(positionsHaveChanged)
		{
			ref GridUpdateEvent updateEvent = new GridUpdateEvent(m_PlayerChunks, m_AntiSpawnChunks, unloaded);
			m_OnGridUpdate.Invoke(updateEvent);
		}
	}
};