class TW_GridCoordItemManager<Class T>
{
	private ref map<int, ref map<int, ref TW_GridCoordItem<T>>> grid = new map<int, ref map<int, ref TW_GridCoordItem<T>>>();
	private int GridSize;
	
	// Coordinates that are currently active
	private ref set<string> activeCoords = new set<string>();
	
	private int pointerIndex = -1;
	private int activeCoordsCount = 0;
	
	void TW_GridCoordItemManager(int gridSize = 1000)
	{
		this.GridSize = gridSize;
	}
	
	TW_GridCoordItem<T> GetCoord(int x, int y)
	{
		if(!HasCoord(x, y))
			return null;
		
		map<int, ref TW_GridCoordItem<T>> sub = grid.Get(x);
		return sub.Get(y);
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
			
			ref map<int, ref TW_GridCoordItem<T>> sub = grid.Get(x);
			sub.Remove(y);
			
			if(sub.Count() <= 0)
				sub.Remove(x);
		}
	}
	
	void RemoveByWorld(vector position, T item)
	{
		int x, y;
		TW_Util.ToGrid(position, x, y, GridSize);
		
		
		if(!HasCoord(x, y))
			return;
		
		ref TW_GridCoordItem<T> coord = GetCoord(x, y);
		
		coord.SetItem(item);
	}
	
	//! Try to insert item via world position
	void InsertByWorld(vector position, T item)
	{
		int x, y;
		TW_Util.ToGrid(position, x, y, GridSize);
		
		ref TW_GridCoordItem<T> coord;
		
		if(!HasCoord(x, y))
		{
			coord = new TW_GridCoordItem<T>(x, y, item);
			InsertCoord(coord);
		}
		else
		{
			coord = GetCoord(x, y);
			coord.SetItem(item);
		}
	}
	
	//! Remove sector from grid
	void RemoveCoord(vector position)
	{
		int x, y;
		TW_Util.ToGrid(position, x, y, GridSize);
		
		if(!HasCoord(x, y))
			return;
		
		ref map<int, ref TW_GridCoordItem<T>> sub = grid.Get(x);
		sub.Remove(y);
		
		if(sub.Count() <= 0)
			sub.Remove(x);
	}
	
	//! Does the grid have coordinate
	bool HasCoord(int x, int y)
	{
		if(!grid.Contains(x))
			return false;
		
		map<int, ref TW_GridCoordItem<T>> sub = grid.Get(x);

		return sub.Contains(y);
	}
	
	void InsertCoord(TW_GridCoordItem<T> coord)
	{
		// IF X coordinate doesn't exist we have to insert both x and y
		if(!grid.Contains(coord.x))
		{
			ref map<int, ref TW_GridCoordItem<T>> sub = new map<int, ref TW_GridCoordItem<T>>();
			sub.Insert(coord.y, coord);
			grid.Insert(coord.x, sub);
		}
		
		// IF X but no Y -- insert y and coord
		if(!grid.Get(coord.x).Contains(coord.y))
		{
			ref map<int, ref TW_GridCoordItem<T>> sub = grid.Get(coord.x);
			sub.Insert(coord.y, coord);
		}
	}
};