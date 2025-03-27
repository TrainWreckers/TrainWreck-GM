class V30_Json_Value {
	void V30_Json_Value() {
		return;
	};

	V30_Json_EValueType GetType();

	bool IsNull() {
		return GetType() == V30_Json_EValueType.NULL;
	};

	V30_Json_null AsNull() {
		return null;
	};

	bool IsBool() {
		return GetType() == V30_Json_EValueType.BOOL;
	};

	V30_Json_bool AsBool() {
		return null;
	};

	bool IsNumber() {
		return IsFloat() || IsInt();
	};

	bool IsInt() {
		return GetType() == V30_Json_EValueType.INT;
	};

	V30_Json_int AsInt() {
		return null;
	};

	bool IsFloat() {
		return GetType() == V30_Json_EValueType.FLOAT;
	};

	V30_Json_float AsFloat() {
		return null;
	};

	bool IsString() {
		return GetType() == V30_Json_EValueType.STRING;
	};

	V30_Json_string AsString() {
		return null;
	};

	bool IsArray() {
		return GetType() == V30_Json_EValueType.ARRAY;
	};

	V30_Json_array AsArray() {
		return null;
	};

	bool IsObject() {
		return GetType() == V30_Json_EValueType.OBJECT;
	};

	V30_Json_object AsObject() {
		return null;
	};

	static V30_Json_Value FromClass(Class instance) {
		auto value = new V30_Json_object();
		if (!instance) return value;
		auto type = instance.Type();
		auto n = type.GetVariableCount();
		for (int i = 0; i < n; i++) {
			auto vname = type.GetVariableName(i);
			auto vtype = type.GetVariableType(i);
			V30_Json_Value vvalue;
			if (vtype.IsInherited(bool)) {
				bool v;
				type.GetVariableValue(instance, i, v);
				vvalue = new V30_Json_bool(v);
			}
			else if (vtype.IsInherited(int)) {
				int v;
				type.GetVariableValue(instance, i, v);
				vvalue = new V30_Json_int(v);
			}
			else if (vtype.IsInherited(float)) {
				float v;
				type.GetVariableValue(instance, i, v);
				vvalue = new V30_Json_float(v);
			}
			else if (vtype.IsInherited(map)) {
				PrintFormat("V30_Json_Value.FromClass() value of type \"%1\" is not supported", vtype.ToString(), LogLevel.ERROR);
			}
			else if (vtype.IsInherited(array)) {
				PrintFormat("V30_Json_Value.FromClass() value of type \"%1\" is not supported", vtype.ToString(), LogLevel.ERROR);
			}
			else if (vtype.IsInherited(string)) {
				string v;
				type.GetVariableValue(instance, i, v);
				vvalue = new V30_Json_string(v);
			}
			else if (vtype.IsInherited(Class)) {
				Class v;
				type.GetVariableValue(instance, i, v);
				if (v) vvalue = FromClass(v);
				else vvalue = V30_Json_null();
			}
			vvalue = new V30_Json_null();
			value.Get().Insert(vname, vvalue);
		};
		return value;
	};
};

enum V30_Json_EValueType {
	NULL,
	BOOL,
	INT,
	FLOAT,
	STRING,
	ARRAY,
	OBJECT
};