class TW_GridCoordArrayManager<Class T>
{
	private ref map<int, ref map<int, ref TW_GridCoordArray<T>>> grid = new map<int, ref map<int, ref TW_GridCoordArray<T>>>();
	private int GridSize;
	
	// Coordinates that are currently active
	private ref set<string> activeCoords = new set<string>();
	
	private int pointerIndex = -1;
	private int activeCoordsCount = 0;
	
	int GetGridSize() { return GridSize; }
	int GetTrackedItemsCount()
	{
		ref array<T> allItems = {};
		return GetAllItems(allItems);
	}
	
	void TW_GridCoordArrayManager(int gridSize = 1000)
	{
		this.GridSize = gridSize;
	}
	
	TW_GridCoordArray<T> GetCoord(int x, int y)
	{
		if(!HasCoord(x, y))
			return null;
		
		map<int, ref TW_GridCoordArray<T>> sub = grid.Get(x);
		return sub.Get(y);
	}
	
	//! Try to insert item via world position
	void InsertByWorld(vector position, T item)
	{
		int x, y;
		TW_Util.ToGrid(position, x, y, GridSize);
		
		ref TW_GridCoordArray<T> coord;
		
		if(!HasCoord(x, y))
		{
			coord = new TW_GridCoordArray<T>(x, y);
			InsertCoord(coord);
		}
		else
			coord = GetCoord(x, y);
		
		coord.Add(item);
	}
	
	void RemoveByWorld(vector position, T item)
	{
		int x, y;
		TW_Util.ToGrid(position, x, y, GridSize);
		
		
		if(!HasCoord(x, y))
			return;
		
		ref TW_GridCoordArray<T> coord = GetCoord(x, y);
		
		coord.RemoveItem(item);
	}
	
	//! Remove multiple chunks from active coordinates
	void RemoveCoords(notnull set<string> coords)
	{
		int x, y;
		foreach(string coord : coords)
		{
			TW_Util.FromGridString(coord, x, y);
			
			if(!HasCoord(x, y))
				continue;
			
			ref map<int, ref TW_GridCoordArray<T>> sub = grid.Get(x);
			sub.Remove(y);
			
			if(sub.Count() <= 0)
				sub.Remove(x);
		}
	}
	
	//! Remove sector from grid
	void RemoveCoord(vector position)
	{
		int x, y;
		TW_Util.ToGrid(position, x, y, GridSize);
		
		if(!HasCoord(x, y))
			return;
		
		ref map<int, ref TW_GridCoordArray<T>> sub = grid.Get(x);
		sub.Remove(y);
		
		if(sub.Count() <= 0)
			sub.Remove(x);
	}
	
	//! Does the grid have coordinate
	bool HasCoord(int x, int y)
	{
		if(!grid.Contains(x))
			return false;
		
		map<int, ref TW_GridCoordArray<T>> sub = grid.Get(x);

		return sub.Contains(y);
	}
	
	bool HasPosition(vector worldPosition)
	{
		int x, y;
		TW_Util.ToGrid(worldPosition, x, y, GridSize);
		
		return HasCoord(x,y);
	}
	
	//! Retrieve coords around center
	int GetNeighbors(notnull out array<ref TW_GridCoordArray<T>> items, int x, int y, int radius = 1, bool includeCenter = true)
	{
		int leftBounds = x - radius;
		int rightBounds = x + radius;
		int upperBounds = y + radius;
		int lowerBounds = y - radius;
		int count = 0;
		
		for(int gridX = leftBounds; gridX <= rightBounds; gridX++)
		{
			for(int gridY = lowerBounds; gridY <= upperBounds; gridY++)
			{
				if(gridX == x && gridY == y && !includeCenter)
					continue;
				
				if(HasCoord(gridX, gridY))
				{
					items.Insert(GetCoord(gridX, gridY));
					count++;
				}
			}
		}
		
		return count;
	}
	
	void InsertCoord(TW_GridCoordArray<T> coord)
	{
		// IF X coordinate doesn't exist we have to insert both x and y
		if(!grid.Contains(coord.x))
		{
			ref map<int, ref TW_GridCoordArray<T>> sub = new map<int, ref TW_GridCoordArray<T>>();
			sub.Insert(coord.y, coord);
			grid.Insert(coord.x, sub);
		}
		
		// IF X but no Y -- insert y and coord
		if(!grid.Get(coord.x).Contains(coord.y))
		{
			ref map<int, ref TW_GridCoordArray<T>> sub = grid.Get(coord.x);
			sub.Insert(coord.y, coord);
		}
	}
	
	//! Get all chunks around player positions
	int GetChunksAround(notnull out array<ref TW_GridCoordArray<T>> chunks, notnull set<string> textCoords, int radius = 1)
	{
		ref set<string> completedCoords = new set<string>();
		
		int x;
		int y;
		int totalCount = 0;
		
		foreach(string textCoord : textCoords)
		{
			if(completedCoords.Contains(textCoord))
				continue;
			
			TW_Util.FromGridString(textCoord, x, y);
					
			// If this coordinate has already been checked -- continue
			if(completedCoords.Contains(textCoord))
				continue;
					
			completedCoords.Insert(textCoord);
				
			if(HasCoord(x, y))
			{
				ref TW_GridCoordArray<T> chunk = GetCoord(x, y);
				chunks.Insert(chunk);
				totalCount++;
			}			
		}
		
		return totalCount;
	}
	
	int GetNeighborsAround(notnull out array<T> data, notnull set<string> textCoords, int radius = 1)
	{
		ref set<string> completedCoords = new set<string>();
		
		int x;
		int y;
		int totalCount = 0;
		
		foreach(string textCoord : textCoords)
		{
			if(completedCoords.Contains(textCoord))
				continue;
			
			TW_Util.FromGridString(textCoord, x, y);
			
			// If this coordinate has already been checked -- continue
			if(completedCoords.Contains(textCoord))
				continue;
					
			completedCoords.Insert(textCoord);
					
			if(HasCoord(x, y))
			{
				ref TW_GridCoordArray<T> chunk = GetCoord(x, y);
				totalCount += chunk.GetData(data);
			}
		}
		
		return totalCount;
	}	
	
	int GetNeighboringItems(notnull out array<T> items, int x, int y, int radius = 1, bool includeCenter = true)
	{
		ref array<ref TW_GridCoordArray<T>> neighbors = {};
		int chunks = GetNeighbors(neighbors, x, y, radius, includeCenter);
		
		if(chunks < 0)
			return 0;
		
		int count = 0;
		foreach(ref TW_GridCoordArray<T> chunk : neighbors)
		{
			count += chunk.GetData(items);
		}
		
		return count;
	}
	
	int GetAllItems(notnull out array<T> items)
	{
		int count = 0;
		
		foreach(int x, ref map<int, ref TW_GridCoordArray<T>> values : grid)
			foreach(int y, ref TW_GridCoordArray<T> value : values)
				count += value.GetData(items);
		
		return count;
	}
	
	//! Round robin through grid, return random item.
	T GetNextItemFromPointer(notnull set<string> incomingCoords)
	{
		if(incomingCoords != activeCoords || incomingCoords.Count() != activeCoords.Count())
		{
			Print(string.Format("TrainWreck: Updating active coords %1", activeCoords));
			activeCoords = incomingCoords;
			activeCoordsCount = activeCoords.Count();	
			pointerIndex = -1; // Reset to beginning
		}
		
		// Increment, round robin style
		pointerIndex++;
		
		// Clamp
		if(pointerIndex >= activeCoordsCount)
			pointerIndex = 0;
		
		// Convert text coords into something we can grab		
		string textCoord = activeCoords.Get(pointerIndex);
		int x, y;
		TW_Util.FromGridString(textCoord, x, y);
		
		if(!HasCoord(x,y))
			return null;
		
		ref TW_GridCoordArray<T> coord = GetCoord(x,y);
		
		// Failsafe in the event the value is null
		if(!coord) 
			return null;
		
		// Grab random item from coord
		return coord.GetRandomElement();
	}	
};