class TW_GridCoordArray<Class T>
{
	int x;
	int y;
	
	private ref array<T> data = {};
	
	void TW_GridCoordArray(int x, int y)
	{
		this.x = x;
		this.y = y;
	}
	
	void Add(T item) { data.Insert(item); }
	void RemoveItem(T item) { data.RemoveItem(item); }
	
	T GetRandomElement() { return data.GetRandomElement(); }
	array<T> GetAll() { return data; }
	
	int GetData(notnull out array<T> items)
	{
		int count = data.Count();
		
		for(int i = 0; i < count; i++)
			items.Insert(data.Get(i));
		
		return count;	
	}
};

class TW_GridCoordItem<Class T>
{
	int x;
	int y;
	
	private T item;
	
	void TW_GridCoordItem(int x, int y, T item)
	{
		this.x = x;
		this.y = y;
		this.item = item;
	}
	
	T GetItem() { return item; }
	void SetItem(T item) { this.item = item; }
}