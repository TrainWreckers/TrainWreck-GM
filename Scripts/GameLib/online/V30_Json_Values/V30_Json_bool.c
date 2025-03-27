class V30_Json_bool : V30_Json_Value {
	protected bool m_bValue;
	
	void V30_Json_bool(bool value = false) {
		m_bValue = value;
	};
	
	// override string AsString() {
	// 	if (m_bValue) return "true";
	// 	else return "false";
	// };
	
	// V30_Json_int ToInt() {
	// 	if (m_bValue) return V30_Json_int(1);
	// 	else return V30_Json_int(0);
	// };
	
	override V30_Json_bool AsBool() {
		return this;
	};
	
	bool Get() {
		return m_bValue;
	};
	
	override V30_Json_EValueType GetType() {
		return V30_Json_EValueType.BOOL;
	};
};