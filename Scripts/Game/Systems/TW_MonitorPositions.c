void TW_OnPlayerPositionsChanged(GridUpdateEvent gridInfo);
typedef func TW_OnPlayerPositionsChangedDelegate;
typedef ScriptInvoker<ref TW_OnPlayerPositionsChangedDelegate> TW_OnPlayerPositionsChangedInvoker;

void TW_OnGridSystemChanged(int oldGridSize, int newGridSize, TW_GridRadiusSubscription newGrid);
typedef func TW_OnGridSystemChangedDelegate;
typedef ScriptInvoker<ref TW_OnGridSystemChangedDelegate> TW_OnGridSystemChangedInvoker;

/*!
	Each grid will have a subscription object which manages the different systems
	Interested in the various radius values on that grid
*/
class TW_GridRadiusSubscription
{
	/*!
		The design requirement for having system names to radius is to track who is interested in what.st
	
		The spawn system, for instance, has two radius values (Spawn and Anti Spawn radiuses). If these two values
		are the same, but the GM changes it at runtime - we need to know whether we need to cleanup the 
		ScriptInvoker or not. 
	
		So, for the case of the Spawn System, it will have to register two different systems against the same radius - 
		assuming they're both using the same grid size.
	*/
	private ref map<string, int> m_IdToRadius = new map<string, int>();
	
	/*!
		Should avoid manually modifying this. Should only update via Add/Remove subscription methods.
		
		The issue is we aren't able to determine which functions have already subscribed to a ScriptInvoker.
		Therefore we have two systems to help "track" who subscribed to what
	
		Multiple systems could be interested in the same radius and grid size. So we enable that by having these two maps 
		be in-sync.
	
		Most of the problem lies with the ability for GMs to tweak the grid size and radius values AT RUNTIME.
		Tracking who subscribed to what allows us to properly clean things up as values change. 
	*/
	private ref map<int, ref TW_OnPlayerPositionsChangedInvoker> m_Subscriptions = new map<int, ref TW_OnPlayerPositionsChangedInvoker>();
	
	//! Stores the previous radius's checks so we can determine which chunks were unloaded since last check
	private ref map<int, set<string>> m_Cache = new map<int, set<string>>();
	
	private int m_GridSize;
	
	void TW_GridRadiusSubscription(int gridSize)
	{
		m_GridSize = gridSize;
	}
	
	void ~TW_GridRadiusSubscription()
	{
		foreach(int radius, ref TW_OnPlayerPositionsChangedInvoker invoker : m_Subscriptions)
		{
			invoker.Clear();
			m_Subscriptions.Remove(radius);
		}
		
		m_IdToRadius.Clear();
		m_Cache.Clear();
	}
	
	bool HasSubscribers()
	{
		return !m_IdToRadius.IsEmpty();
	}
	
	/*!
		Iterate over player positions and calculate the positions around the players based on subscriptions.
		Track previous positions.
		Send updates if positions have changed.
	*/
	void Update(notnull array<IEntity> players)
	{
		ref set<string> playerChunks = new set<string>();
		ref set<string> previousCheck = new set<string>();
		ref set<string> unloaded = new set<string>();
		
		foreach(IEntity player : players)
		{
			foreach(int radius, ref TW_OnPlayerPositionsChangedInvoker invoker : m_Subscriptions)
			{
				bool hasChanged = false;
				
				previousCheck.Clear();
				playerChunks.Clear();
				unloaded.Clear();
				
				if(m_Cache.Contains(radius))
				{
					ref set<string> existing = m_Cache.Get(radius);
					
					// Appears to crash Workbench when empty...
					// So we're doing a check to prevent that
					if(existing && !existing.IsEmpty())
					{
						previousCheck.Clear();
						previousCheck.Copy(existing);
					}
				}
				
				TW_Util.AddSurroundingGridSquares(playerChunks, player.GetOrigin(), radius, m_GridSize);
				
				int count = playerChunks.Count();
				
				ref set<string> difference = new set<string>();
				int diff = TW_IterableStringHelper.GetDifference(playerChunks, previousCheck, difference);
				
				m_Cache.Set(radius, TW_IterableStringHelper.GetCopy(playerChunks));
				
				if(diff > 0)
				{
					ref GridUpdateEvent updateEvent = new GridUpdateEvent(playerChunks, difference);
					invoker.Invoke(updateEvent);
				}
			}
		}
	}
	
	/*! 
		Ensures the subscriber is tracked, and returns event handler for the subscription
	*/
	TW_OnPlayerPositionsChangedInvoker AddSubscription(string subscriberName, int radius)
	{
		if(m_IdToRadius.Contains(subscriberName))
			return m_Subscriptions.Get(radius);
		
		m_IdToRadius.Insert(subscriberName, radius);
		
		if(!m_Subscriptions.Contains(radius))
			m_Subscriptions.Insert(radius, new TW_OnPlayerPositionsChangedInvoker());
		
		if(!m_Cache.Contains(radius))
			m_Cache.Set(radius, new set<string>());
		
		return m_Subscriptions.Get(radius);
	}
	
	/*!
		Removes subscriber from radius
	
		If no more subscribers exist for a specific radius, it will cleanup the 
		event manager
	*/
	TW_OnPlayerPositionsChangedInvoker RemoveSubscription(string subscriberName, int radius)
	{
		if(!m_IdToRadius.Contains(subscriberName))
			return null;
		else 
			m_IdToRadius.Remove(subscriberName);
		
		if(!m_Subscriptions.Contains(radius))
			return null;
		
		// Are we able to clean things up - if no subscribers exist for a specific radius we need to delete
		bool removeRadius = true;
		
		foreach(string name, int r : m_IdToRadius)
			if(r == radius)
				removeRadius = false;
		
		if(removeRadius)
		{
			m_Subscriptions.Remove(radius);
			m_Cache.Remove(radius);
			return null;
		}
		
		return m_Subscriptions.Get(radius);
	}
};

class GridUpdateEvent
{
	private ref set<string> m_PlayerChunks;
	private ref set<string> m_UnloadedChunks;
	
	void GridUpdateEvent(set<string> chunks, set<string> unloaded)
	{
		m_PlayerChunks = chunks;
		m_UnloadedChunks = unloaded;
	}
	
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
	
	private ref map<int, ref TW_GridRadiusSubscription> m_GridSystems = new map<int, ref TW_GridRadiusSubscription>();
		
	private ref array<IEntity> m_Players = {};
	
	private ref TW_OnGridSystemChangedInvoker m_OnGridSystemChanged = new TW_OnGridSystemChangedInvoker();
	
	//! Does the monitor have a system for specific grid size
	bool HasGridSystem(int size) { return m_GridSystems.Contains(size); }
	
	//! Retrieve grid system by size (if it exists)
	TW_GridRadiusSubscription GetGridSystem(int size)
	{
		if(!HasGridSystem(size)) return null;
		return m_GridSystems.Get(size);
	}
	
	//! Remove Grid System, and any listeners
	void RemoveGridSystem(int gridSize)
	{
		if(HasGridSystem(gridSize))
		{
			m_GridSystems.Remove(gridSize);
		}
	}
	
	void TW_MonitorPositions()
	{
		s_Instance = this;
	}
	
	void ~TW_MonitorPositions()
	{
		m_GridSystems.Clear();
	}
	
	TW_OnPlayerPositionsChangedInvoker AddGridSubscription(string systemName, int gridSize, int radius)
	{
		if(!m_GridSystems.Contains(gridSize))
		{
			ref TW_GridRadiusSubscription sub = new TW_GridRadiusSubscription(gridSize);
			m_GridSystems.Set(gridSize, sub);
			
			return sub.AddSubscription(systemName, radius);
		}
		else
		{
			ref TW_GridRadiusSubscription sub = m_GridSystems.Get(gridSize);
			return sub.AddSubscription(systemName, radius);
		}
	}
	
	TW_OnPlayerPositionsChangedInvoker RemoveGridSubscription(string systemName, int gridSize, int radius)
	{
		if(!m_GridSystems.Contains(gridSize))
			return null;
		
		ref TW_GridRadiusSubscription sub = m_GridSystems.Get(gridSize);
		return sub.RemoveSubscription(systemName, radius);
	}
	
	void MonitorPlayers()
	{
		ref array<int> playerIds = {};
		ref array<IEntity> players = {};
		
		GetGame().GetPlayerManager().GetPlayers(playerIds);
		
		foreach(int playerId : playerIds)
		{
			auto player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
			if(!player) continue;
			players.Insert(player);
		}
		
		foreach(int gridSize, ref TW_GridRadiusSubscription system : m_GridSystems)
		{
			system.Update(players);
		}
	}
};