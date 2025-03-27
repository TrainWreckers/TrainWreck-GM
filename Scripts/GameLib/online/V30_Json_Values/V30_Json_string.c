class V30_Json_string : V30_Json_Value {
	protected string m_sValue;
	
	void V30_Json_string(string value) {
		m_sValue = value;
	};
	
	// override string AsString() {
	// 	if (m_sValue.Contains("\"")) {
	// 		string value = m_sValue;
	// 		value.Replace("\"", "\\" + "\"");
	// 		return value;
	// 	};
	// 	
	// 	return "\"" + m_sValue + "\"";
	// };
	
	// override V30_Json_string ToString() {
	// 	return V30_Json_string(m_sValue);
	// };
	
	override V30_Json_string AsString() {
		return this;
	};
	
	string Get() {
		return m_sValue;
	};
	
	int Length() {
		return m_sValue.Length();
	};
	
	override V30_Json_EValueType GetType() {
		return V30_Json_EValueType.STRING;
	};
};