class V30_Json_StringSerializer : V30_Json_Serializer {
	protected string m_sData;
	
	protected bool m_bIsFirstObjectValue;
	
	string Get() {
		return m_sData;
	};
	
	override protected void OnBegin(V30_Json_Value value) {
		m_sData = "";
	};
	
	override protected void OnNull() {
		m_sData += "null";
	};
	
	override protected void OnBool(bool value) {
		m_sData += value.ToString();
	};
	
	override protected void OnInt(int value) {
		m_sData += value.ToString();
	};
	
	override protected void OnFloat(float value) {
		m_sData += value.ToString();
	};
	
	override protected void OnString(string value) {
		WriteString(value);
	};
	
	override protected void OnArrayBegin(array<ref V30_Json_Value> value) {
		m_sData += "[";
	};
	
	override protected void OnArrayValue(int index, V30_Json_Value value) {
		if (index == 0) return;
		m_sData += ",";
	};
	
	override protected void OnArrayEnd(array<ref V30_Json_Value> value) {
		m_sData += "]";
	};
	
	override protected void OnObjectBegin(map<string, ref V30_Json_Value> value) {
		m_sData += "{";
		m_bIsFirstObjectValue = true;
	};
	
	override protected void OnObjectValue(string key, V30_Json_Value value) {
		if (!m_bIsFirstObjectValue) m_sData += ",";
		WriteString(key);
		m_sData += ":";
		m_bIsFirstObjectValue = false;
	};
	
	override protected void OnObjectEnd(map<string, ref V30_Json_Value> value) {
		m_sData += "}";
		m_bIsFirstObjectValue = false;
	};
	
	protected void WriteString(string src) {
		m_sData += "\"";
		auto n = src.Length();
		auto a = 0;
		auto b = a;
		for (int i = 0; i < n; i++) {
			auto c = src.ToAscii(i);
			if (c < 0) c = 127 - c;
			if (c > 0x1F && c != 0x7F && c != 0x22 && c != 0x5C/* && c != 0x2F*/) {
				b++;
			}
			else {
				AppendSubstring(m_sData, src, a, b - a);
				a = i + 1;
				b = a;
				m_sData += "\\";
				switch (c) {
					// "
					case 0x22:
						m_sData += "\"";
						break;
					// \
					case 0x5C:
						m_sData += "\\";
						break;
					// // /
					// case 0x2F:
					// 	m_sData += "/";
					// 	break;
					// BS
					case 0x08:
						m_sData += "b";
						break;
					// FF
					case 0x0C:
						m_sData += "f";
						break;
					// LF
					case 0x0A:
						m_sData += "n";
						break;
					// CR
					case 0x0D:
						m_sData += "r";
						break;
					// HT
					case 0x09:
						m_sData += "t";
						break;
					
					default:
						m_sData += "u";
						m_sData += "00";
						m_sData += "0123456789ABCDEF".Substring((c & 0xF0) >> 4, 1);
						m_sData += "0123456789ABCDEF".Substring((c & 0x0F) >> 0, 1);
						break;
				};
			};
		};
		AppendSubstring(m_sData, src, a, b - a);
		m_sData += "\"";
	};
	
	protected static void AppendSubstring(out string dst, string src, int from, int count) {
		// enfusion://ScriptEditor/scripts/Core/generated/Types/string.c;88 
		// (Maximum output string size is limited to 8191 characters)
		while (count > 0) {
			int n;
			if (count <= 8191) n = count;
			else count = 8191;
			dst += src.Substring(from, n);
			from += n;
			count -= n;
		};
	};
};
