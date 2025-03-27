class V30_Json_object : V30_Json_Value {
	protected ref map<string, ref V30_Json_Value> m_mValue;
	
	void V30_Json_object(map<string, ref V30_Json_Value> value = null) {
		if (!value)
			m_mValue = new map<string, ref V30_Json_Value>();
		else
			m_mValue = value;
	};
	
	map<string, ref V30_Json_Value> Get() {
		return m_mValue;
	};
	
	int Count() {
		return m_mValue.Count();
	};
	
	V30_Json_Value GetAt(string key) {
		V30_Json_Value value;
		if (!m_mValue.Find(key, value)) return new V30_Json_null();
		return value;
	};
	
	bool ContainsAt(string key) {
		return m_mValue.Contains(key);
	};
	
	override V30_Json_object AsObject() {
		return this;
	};
	
	// override string AsString() {
	// 	string str = "{";
	// 	if (!m_mValue.IsEmpty()) {
	// 		bool first = true;
	// 		foreach (string key, ref V30_Json_Value value : m_mValue) {
	// 			if (!first) {str += ",";};
	// 			first = false;
	// 			str += "\"";
	// 			str += key;
	// 			str += "\":";
	// 			str += value.AsString();
	// 		};
	// 		str = str.Substring(0, str.Length() - 1);
	// 	};
	// 	str += "}";
	// 	return str;
	//};
	
	override V30_Json_EValueType GetType() {
		return V30_Json_EValueType.OBJECT;
	};
};