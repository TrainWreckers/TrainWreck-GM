enum V30_Json_Serializer_EState {
	DEFAULT,
	ARRAY,
	OBJECT
};

class V30_Json_Serializer {
	protected V30_Json_Serializer_EState m_eState;

	void Serialize(V30_Json_Value value = null) {
		if (!value) value = V30_Json_null();
		OnBegin(value);
		m_eState = V30_Json_Serializer_EState.DEFAULT;
		switch (value.GetType()) {
			case V30_Json_EValueType.NULL:
				OnNull();
				break;
			case V30_Json_EValueType.BOOL:
				OnBool(V30_Json_bool.Cast(value).Get());
				break;
			case V30_Json_EValueType.INT:
				OnInt(V30_Json_int.Cast(value).Get());
				break;
			case V30_Json_EValueType.FLOAT:
				OnFloat(V30_Json_float.Cast(value).Get());
				break;
			case V30_Json_EValueType.STRING:
				OnString(V30_Json_string.Cast(value).Get());
				break;
			case V30_Json_EValueType.ARRAY:
				SerializeArray(V30_Json_array.Cast(value));
				break;
			case V30_Json_EValueType.OBJECT:
				SerializeObject(V30_Json_object.Cast(value));
				break;
		};
		OnEnd();
	};

	protected void SerializeArray(V30_Json_array arr) {
		auto a = arr.Get();
		OnArrayBegin(a);
		auto state = m_eState;
		m_eState = V30_Json_Serializer_EState.ARRAY;
		foreach (int i, V30_Json_Value v : a) {
			OnArrayValue(i, v);
			switch (v.GetType()) {
				case V30_Json_EValueType.NULL:
					OnNull();
					break;
				case V30_Json_EValueType.BOOL:
					OnBool(V30_Json_bool.Cast(v).Get());
					break;
				case V30_Json_EValueType.INT:
					OnInt(V30_Json_int.Cast(v).Get());
					break;
				case V30_Json_EValueType.FLOAT:
					OnFloat(V30_Json_float.Cast(v).Get());
					break;
				case V30_Json_EValueType.STRING:
					OnString(V30_Json_string.Cast(v).Get());
					break;
				case V30_Json_EValueType.ARRAY:
					SerializeArray(V30_Json_array.Cast(v));
					break;
				case V30_Json_EValueType.OBJECT:
					SerializeObject(V30_Json_object.Cast(v));
					break;
			};
		};
		m_eState = state;
		OnArrayEnd(a);
	};

	protected void SerializeObject(V30_Json_object obj) {
		auto o = obj.Get();
		OnObjectBegin(o);
		auto state = m_eState;
		m_eState = V30_Json_Serializer_EState.OBJECT;
		foreach (string k, V30_Json_Value v : o) {
			OnObjectValue(k, v);
			switch (v.GetType()) {
				case V30_Json_EValueType.NULL:
					OnNull();
					break;
				case V30_Json_EValueType.BOOL:
					OnBool(V30_Json_bool.Cast(v).Get());
					break;
				case V30_Json_EValueType.INT:
					OnInt(V30_Json_int.Cast(v).Get());
					break;
				case V30_Json_EValueType.FLOAT:
					OnFloat(V30_Json_float.Cast(v).Get());
					break;
				case V30_Json_EValueType.STRING:
					OnString(V30_Json_string.Cast(v).Get());
					break;
				case V30_Json_EValueType.ARRAY:
					SerializeArray(V30_Json_array.Cast(v));
					break;
				case V30_Json_EValueType.OBJECT:
					SerializeObject(V30_Json_object.Cast(v));
					break;
			};
		};
		m_eState = m_eState;
		OnObjectEnd(o);
	};

	protected void OnBegin(V30_Json_Value value);

	protected void OnNull();

	protected void OnBool(bool value);

	protected void OnInt(int value);

	protected void OnFloat(float value);

	protected void OnString(string value);

	protected void OnArrayBegin(array<ref V30_Json_Value> value);

	protected void OnArrayValue(int index, V30_Json_Value value);

	protected void OnArrayEnd(array<ref V30_Json_Value> value);

	protected void OnObjectBegin(map<string, ref V30_Json_Value> value);

	protected void OnObjectValue(string key, V30_Json_Value value);

	protected void OnObjectEnd(map<string, ref V30_Json_Value> value);

	protected void OnEnd();

	protected bool IsPlain() {
		return m_eState == V30_Json_Serializer_EState.DEFAULT;
	};

	protected bool IsArray() {
		return m_eState == V30_Json_Serializer_EState.ARRAY;
	};

	protected bool IsObject() {
		return m_eState == V30_Json_Serializer_EState.OBJECT;
	};
};
