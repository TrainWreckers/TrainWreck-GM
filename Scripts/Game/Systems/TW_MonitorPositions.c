class TW_Grid
{
	private int m_GridSize;
	private int m_GridRadius;
	private int m_AntiGridRadius;
	
	private ref set<string> m_PlayerChunks;
	private ref set<string> m_AntiChunks;
	
	void TW_Grid(int gridSize, int gridRadius, int antiRadius=-1)
	{
		m_GridSize = gridSize;
		m_GridRadius = gridRadius;
		
		if(antiRadius > 0)
			m_AntiGridRadius = antiRadius;
		
		m_PlayerChunks = new set<string>();
		m_AntiChunks = new set<string>();
	}
	
	int GetGridSize() { return m_GridSize; }
	int GetGridRadius() { return m_GridRadius; }
	int GetAntiGridRadius() { return m_AntiGridRadius; }
	
	void SetAntiRadius(int radius) { m_AntiGridRadius = radius; }
	void SetGridRadius(int radius) { m_GridRadius = radius; }
	
	set<string> GetPlayerChunks() { return m_PlayerChunks; }
	set<string> GetAntiChunks() { return m_AntiChunks; }
	
	bool HasChunk(string chunk) { return m_PlayerChunks.Contains(chunk); }
	bool HasAntiChunk(string chunk) { return m_AntiChunks.Contains(chunk); }
	
	void AddChunk(string chunk) { m_PlayerChunks.Insert(chunk); }
	void AddAntiChunk(string chunk) { m_AntiChunks.Insert(chunk); }
	
	void SetPlayerChunks(set<string> chunks)
	{
		m_PlayerChunks.Clear();
		m_PlayerChunks.Copy(chunks);
	}
	
	void SetAntiChunks(notnull set<string> chunks)
	{
		m_AntiChunks.Clear();
		m_AntiChunks.Copy(chunks);
	}
	
	string GetPosition(IEntity player)
	{
		return TW_Util.ToGridText(player.GetOrigin(), GetGridSize());
	}
	
	string GetPosition(vector position)
	{
		return TW_Util.ToGridText(position, GetGridSize());
	}
};

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

void TW_OnPlayerPositionsChanged(GridUpdateEvent gridInfo);
typedef func TW_OnPlayerPositionsChangedDelegate;
typedef ScriptInvoker<ref TW_OnPlayerPositionsChangedDelegate> TW_OnPlayerPositionsChangedInvoker;

void TW_OnGridSystemChanged(int oldGridSize, int newGridSize, TW_Grid newGrid);
typedef func TW_OnGridSystemChangedDelegate;
typedef ScriptInvoker<ref TW_OnGridSystemChangedDelegate> TW_OnGridSystemChangedInvoker;

class TW_MonitorPositions
{
	private static TW_MonitorPositions s_Instance;
	
	static TW_MonitorPositions GetInstance()
	{
		return s_Instance;
	}
	
	private ref map<int, ref TW_Grid> m_GridSystems = new map<int, ref TW_Grid>();
		
	private ref array<IEntity> m_Players = {};
	
	private ref map<int, ref TW_OnPlayerPositionsChangedInvoker> m_OnGridUpdate = new map<int, ref TW_OnPlayerPositionsChangedInvoker>();
	
	private ref TW_OnGridSystemChangedInvoker m_OnGridSystemChanged = new TW_OnGridSystemChangedInvoker();
	
	TW_OnGridSystemChangedInvoker GetOnGridSystemChanged() { return m_OnGridSystemChanged; }
	
	TW_OnPlayerPositionsChangedInvoker GetGridUpdate(int gridSize) 
	{ 
		if(!HasGridSystem(gridSize)) 
			return null;
		
		if(!m_OnGridUpdate.Contains(gridSize))
			return null;
		
		return m_OnGridUpdate.Get(gridSize);
	}
	
	//! Does the monitor have a system for specific grid size
	bool HasGridSystem(int size) { return m_GridSystems.Contains(size); }
	
	//! Retrieve grid system by size (if it exists)
	TW_Grid GetGridSystem(int size)
	{
		if(!HasGridSystem(size)) return null;
		return m_GridSystems.Get(size);
	}
	
	//! Swap an existing grid with new grid, keyed by size
	void ReplaceGridSystem(int oldSize, int newSize, notnull TW_Grid grid)
	{
		if(HasGridSystem(oldSize))
		{
			m_GridSystems.Remove(oldSize);
		}
		
		RemoveGridInvoker(oldSize);
		
		m_GridSystems.Insert(newSize, grid);
		if(GetOnGridSystemChanged())
			GetOnGridSystemChanged().Invoke(oldSize, newSize, grid);
	}
	
	//! Remove subscriptions to grid size
	private void RemoveGridInvoker(int oldSize)
	{
		if(m_OnGridUpdate.Contains(oldSize))
		{
			ref TW_OnPlayerPositionsChangedInvoker invoker = m_OnGridUpdate.Get(oldSize);
			invoker.Clear();
			delete invoker;
			
			m_OnGridUpdate.Remove(oldSize);
		}	
	}
	
	//! Remove Grid System, and any listeners
	void RemoveGridSystem(int gridSize)
	{
		if(HasGridSystem(gridSize))
		{
			m_GridSystems.Remove(gridSize);
		}
		
		RemoveGridInvoker(gridSize);
	}
	
	void TW_MonitorPositions()
	{
		s_Instance = this;
	}
	
	void ~TW_MonitorPositions()
	{
		m_GridSystems.Clear();
		
		foreach(int size, TW_OnPlayerPositionsChangedInvoker invoker : m_OnGridUpdate)
			invoker.Clear();
		
		m_OnGridUpdate.Clear();
	}
	
	void AddGridSystem(int gridSize, int gridRadius, int antiRadius)
	{
		if(HasGridSystem(gridSize))
		{
			ref TW_Grid grid = GetGridSystem(gridSize);
			
			if(grid.GetGridRadius() != gridRadius)
			{
				PrintFormat("TrainWreck: Grid Size '%1' already existed. Existing radius of '%2' is being updated to '%3'", gridSize, grid.GetGridRadius(), gridRadius, LogLevel.WARNING);
			}
			
			if(grid.GetAntiGridRadius() != antiRadius)
			{
				PrintFormat("TrainWreck: Grid Size '%1' already existed. Existing anti radius of '%2' is being updated to '%3'", gridSize, grid.GetAntiGridRadius(), antiRadius, LogLevel.WARNING);
			}
			
			grid.SetGridRadius(gridRadius);
			grid.SetAntiRadius(antiRadius);
			return;
		}
		
		ref TW_Grid grid = new TW_Grid(gridSize, gridRadius, antiRadius);
		m_GridSystems.Insert(gridSize, grid);
		m_OnGridUpdate.Insert(gridSize, new TW_OnPlayerPositionsChangedInvoker());
	}
	
	void AddSpawnGridSystem(int gridSize, int gridRadius)
	{
		if(HasGridSystem(gridSize))
		{
			ref TW_Grid grid = GetGridSystem(gridSize);
			
			if(grid.GetGridRadius() != gridRadius)
			{
				PrintFormat("TrainWreck: Grid Size '%1' already existed. Existing radius of '%2' is being updated to '%3'", gridSize, grid.GetGridRadius(), gridRadius, LogLevel.WARNING);
			}
			
			grid.SetGridRadius(gridRadius);
			
			return;
		}
		
		ref TW_Grid grid = new TW_Grid(gridSize, gridRadius);
		m_GridSystems.Insert(gridSize, grid);
		m_OnGridUpdate.Insert(gridSize, new TW_OnPlayerPositionsChangedInvoker());
	}
	
	void AddAntiGridSystem(int gridSize, int gridRadius)
	{
		if(HasGridSystem(gridSize))
		{
			GetGridSystem(gridSize).SetAntiRadius(gridRadius);
			return;
		}
		
		ref TW_Grid grid = new TW_Grid(gridSize, -1, gridRadius);
		m_GridSystems.Insert(gridSize, grid);
		m_OnGridUpdate.Insert(gridSize, new TW_OnPlayerPositionsChangedInvoker());
	}
	
	void MonitorPlayers()
	{
		ref array<int> playerIds = {};
		ref array<IEntity> players = {};
		
		GetGame().GetPlayerManager().GetPlayers(playerIds);
		
		ref set<string> currentPositions = new set<string>();
		ref set<string> unloaded = new set<string>();
		
		bool positionsHaveChanged = false;
		
		ref set<string> chunksAroundPlayer = new set<string>();
		ref set<string> antiSpawnChunks = new set<string>();
		
		foreach(int playerId : playerIds)
		{
			auto player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
			if(!player) continue;
			players.Insert(player);
		}
		
		m_Players.Clear();
		m_Players.Copy(players);
		
		foreach(int gridSize, ref TW_Grid gridSystem : m_GridSystems)
		{
			positionsHaveChanged = false;
			antiSpawnChunks.Clear();
			chunksAroundPlayer.Clear();
			unloaded.Clear();
			currentPositions.Clear();
			
			foreach(IEntity player : players)
			{
				chunksAroundPlayer.Clear();
				
				if(gridSystem.GetAntiGridRadius() > 0)
				{
					TW_Util.AddSurroundingGridSquares(antiSpawnChunks, player.GetOrigin(), gridSystem.GetAntiGridRadius(), gridSystem.GetGridSize());
					gridSystem.SetAntiChunks(antiSpawnChunks);
				}
				
				if(gridSystem.GetGridRadius() > 0)
				{
					TW_Util.AddSurroundingGridSquares(chunksAroundPlayer, player.GetOrigin(), gridSystem.GetGridRadius(), gridSystem.GetGridSize());
					
					foreach(string chunk : chunksAroundPlayer)
					{
						if(!gridSystem.HasChunk(chunk))
						{
							positionsHaveChanged = true;
							gridSystem.AddChunk(chunk);
						}
						
						if(!currentPositions.Contains(chunk))
							currentPositions.Insert(chunk);
					}
				}
			}
			
			if(gridSystem.GetGridRadius() > 0)
			{
				int chunkCount = gridSystem.GetPlayerChunks().Count();
				
				for(int i = 0; i < chunkCount; i++)
				{
					string playerCoord = gridSystem.GetPlayerChunks().Get(i);
					
					if(!currentPositions.Contains(playerCoord))
					{
						unloaded.Insert(playerCoord);
						gridSystem.GetPlayerChunks().Remove(i);
						i -= 1;
						chunkCount -= 1;
						positionsHaveChanged = true;
					}
				}
			}
			
			if(positionsHaveChanged)
			{
				if(!m_OnGridUpdate.Contains(gridSystem.GetGridSize()))
					continue;
				
				ref GridUpdateEvent updateEvent = new GridUpdateEvent(gridSystem.GetPlayerChunks(), gridSystem.GetAntiChunks(), unloaded);
				m_OnGridUpdate.Get(gridSystem.GetGridSize()).Invoke(updateEvent);
			}
		}
	}
};