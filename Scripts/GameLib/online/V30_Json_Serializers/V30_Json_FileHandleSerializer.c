class V30_Json_FileHandleSerializer : V30_Json_Serializer {
	protected ref FileHandle m_FileHandle;
	
	protected bool m_bIsFirstObjectValue;
	
	void V30_Json_FileHandleSerializer(FileHandle fileHandle) {
		m_FileHandle = fileHandle;
	};
	
	FileHandle GetFileHandle() {
		return m_FileHandle;
	};
	
	override protected void OnNull() {
		Write("null");
	};
	
	override protected void OnBool(bool value) {
		Write(value.ToString());
	};
	
	override protected void OnInt(int value) {
		Write(value.ToString());
	};
	
	override protected void OnFloat(float value) {
		Write(value.ToString());
	};
	
	override protected void OnString(string value) {
		WriteString(value);
	};
	
	override protected void OnArrayBegin(array<ref V30_Json_Value> value) {
		Write("[");
	};
	
	override protected void OnArrayValue(int index, V30_Json_Value value) {
		if (index == 0) return;
		Write(",");
	};
	
	override protected void OnArrayEnd(array<ref V30_Json_Value> value) {
		Write("]");
	};
	
	override protected void OnObjectBegin(map<string, ref V30_Json_Value> value) {
		Write("{");
		m_bIsFirstObjectValue = true;
	};
	
	override protected void OnObjectValue(string key, V30_Json_Value value) {
		if (!m_bIsFirstObjectValue) Write(",");
		WriteString(key);
		Write(":");
		m_bIsFirstObjectValue = false;
	};
	
	override protected void OnObjectEnd(map<string, ref V30_Json_Value> value) {
		Write("}");
		m_bIsFirstObjectValue = false;
	};
	
	protected void WriteString(string src) {
		Write("\"");
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
				WriteSubstring(src, a, b - a);
				a = i + 1;
				b = a;
				Write("\\");
				switch (c) {
					// "
					case 0x22:
						Write("\"");
						break;
					// \
					case 0x5C:
						Write("\\");
						break;
					// // /
					// case 0x2F:
					// 	Write("/");
					// 	break;
					// BS
					case 0x08:
						Write("b");
						break;
					// FF
					case 0x0C:
						Write("f");
						break;
					// LF
					case 0x0A:
						Write("n");
						break;
					// CR
					case 0x0D:
						Write("r");
						break;
					// HT
					case 0x09:
						Write("t");
						break;
					
					default:
						Write("u");
						Write("00");
						Write("0123456789ABCDEF".Substring((c & 0xF0) >> 4, 1));
						Write("0123456789ABCDEF".Substring((c & 0x0F) >> 0, 1));
						break;
				};
			};
		};
		WriteSubstring(src, a, b - a);
		Write("\"");
	};
	
	protected void Write(string src) {
		if (!m_FileHandle.IsOpen()) {
			PrintFormat("%1: File handle %2 is not open.", this, m_FileHandle, level: LogLevel.ERROR);
			return;
		};
		
		m_FileHandle.Write(src);
	};
	
	protected void WriteSubstring(string src, int from, int count) {
		// enfusion://ScriptEditor/scripts/Core/generated/Types/string.c;88 
		// (Maximum output string size is limited to 8191 characters)
		while (count > 0) {
			int n;
			if (count <= 8191) n = count;
			else count = 8191;
			Write(src.Substring(from, n));
			from += n;
			count -= n;
		};
	};
};
