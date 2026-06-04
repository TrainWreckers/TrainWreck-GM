class TW_GridCoordArray<Class T>
{
	int x;
	int y;
	
	private ref array<T> data = {};
	
	void TW_GridCoordArray(int _x, int _y)
	{
		this.x = _x;
		this.y = _y;
	}
	
	void Add(T _item) { data.Insert(_item); }
	void RemoveItem(T _item) { data.RemoveItem(_item); }
	
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
	
	void TW_GridCoordItem(int _x, int _y, T _item)
	{
		this.x = _x;
		this.y = _y;
		this.item = _item;
	}
	
	T GetItem() { return item; }
	void SetItem(T _item) { this.item = _item; }
}