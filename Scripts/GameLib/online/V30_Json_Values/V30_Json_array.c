class V30_Json_array : V30_Json_Value {
	protected ref array<ref V30_Json_Value> m_aValue;
	
	void V30_Json_array(array<ref V30_Json_Value> value = null) {
		if (!value)
			m_aValue = new array<ref V30_Json_Value>();
		else
			m_aValue = value;
	};
	
	array<ref V30_Json_Value> Get() {
		return m_aValue;
	};
	
	V30_Json_Value GetAt(int i) {
		if (!m_aValue.IsIndexValid(i)) return new V30_Json_null();
		return m_aValue.Get(i);
	};
	
	int Count() {
		return m_aValue.Count();
	};
	
	override V30_Json_array AsArray() {
		return this;
	};
	
	// override string AsString() {
	// 	string str = "[";
	// 	if (!m_aValue.IsEmpty()) {
	// 		foreach (int i, ref V30_Json_Value value : m_aValue) {
	// 			if (i != 0) str += ",";
	// 			str += value.AsString();
	// 		};
	// 	};
	// 	str += "]";
	// 	return str;
	// };
	
	override V30_Json_EValueType GetType() {
		return V30_Json_EValueType.ARRAY;
	};
};