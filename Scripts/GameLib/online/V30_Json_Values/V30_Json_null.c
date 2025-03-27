class V30_Json_null : V30_Json_Value {
	void V30_Json_null() {
		return;
	};
	
	// override string AsString() {
	// 	return "null";
	// };
	
	override V30_Json_null AsNull() {
		return this;
	};
	
	override V30_Json_EValueType GetType() {
		return V30_Json_EValueType.NULL;
	};
};