class V30_Json_int : V30_Json_Value {
	protected int m_iValue;
	
	void V30_Json_int(int value = 0) {
		m_iValue = value;
	};
	
	// override string AsString() {
	// 	return m_iValue.ToString();
	// };
	
	// V30_Json_float ToFloat() {
	// 	return V30_Json_float(m_iValue);
	// };
	
	// V30_Json_bool ToBool() {
	// 	return V30_Json_bool(m_iValue != 0);
	// };
	
	override V30_Json_int AsInt() {
		return this;
	};
	
	int Get() {
		return m_iValue;
	};
	
	override V30_Json_EValueType GetType() {
		return V30_Json_EValueType.INT;
	};
};