//! Utility class for aggregating a type of data and providing weights for them
class WeightedType<Class T>
{
	private ref array<T> items = {};
	private ref array<float> weights = {};
	
	//! Get a random item, using weight system
	T GetRandomItem()
	{
		if(items.IsEmpty() || weights.IsEmpty())
			return null;
		
		int index = SCR_ArrayHelper.GetWeightedIndex(weights, Math.RandomFloat01());
		return items.Get(index);
	}
	
	//! Add item with specific weight
	void Add(T item, float weight)
	{
		items.Insert(item);
		weights.Insert(weight);
	}
};