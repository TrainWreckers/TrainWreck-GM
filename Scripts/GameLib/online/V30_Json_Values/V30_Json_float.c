class V30_Json_float : V30_Json_Value {
	protected float m_fValue;
	
	void V30_Json_float(float value = 0.0) {
		m_fValue = value;
	};
	
	// override string AsString() {
	// 	return m_fValue.ToString();
	// };
	
	// V30_Json_int ToInt() {
	// 	return V30_Json_int(m_fValue);
	// };
	
	override V30_Json_float AsFloat() {
		return this;
	};
	
	float Get() {
		return m_fValue;
	};
	
	override V30_Json_EValueType GetType() {
		return V30_Json_EValueType.FLOAT;
	};
};