class V30_Json_PrettyFileHandleSerializer : V30_Json_FileHandleSerializer {
	protected int m_iLevel;
	
	protected int m_iSpaceCount;
	
	protected bool m_bUseTabs;
	
	void V30_Json_PrettyFileHandleSerializer(FileHandle fileHandle, int spaceCount = 2, bool useTabs = false) {
		m_iSpaceCount = spaceCount;
		m_bUseTabs = useTabs;
	};
	
	override protected void OnBegin(V30_Json_Value value) {
		super.OnBegin(value);
		m_iLevel = 0;
	};
	
	override protected void OnArrayBegin(array<ref V30_Json_Value> value) {
		super.OnArrayBegin(value);
		m_iLevel++;
	};
	
	override protected void OnArrayValue(int index, V30_Json_Value value) {
		super.OnArrayValue(index, value);
		Write("\n");
		WriteTabs();
	};
	
	override protected void OnArrayEnd(array<ref V30_Json_Value> value) {
		m_iLevel--;
		if (value.Count() != 0) {
			Write("\n");
			WriteTabs();
		};
		super.OnArrayEnd(value);
	};
	
	override protected void OnObjectBegin(map<string, ref V30_Json_Value> value) {
		super.OnObjectBegin(value);
		m_iLevel++;
		if (value.Count() == 0) return;
		Write("\n");
		WriteTabs();
	};
	
	override protected void OnObjectValue(string key, V30_Json_Value value) {
		if (!m_bIsFirstObjectValue) {
			Write(",\n");
			WriteTabs();
		};
		WriteString(key);
		Write(": ");
		m_bIsFirstObjectValue = false;
	};
	
	override protected void OnObjectEnd(map<string, ref V30_Json_Value> value) {
		m_iLevel--;
		if (value.Count() != 0) {
			Write("\n");
			WriteTabs();
		};
		super.OnObjectEnd(value);
	};
	
	protected void WriteTabs(int count = -1) {
		if (count == -1) {
			count = m_iLevel;
		};
		for (int i = 0; i < count; i++) {
			if (!m_bUseTabs) {
				for (int j = 0; j < m_iSpaceCount; j++) {
					Write(" ");
				};
			}
			else {
				Write("\t");
			};
		};
	};
};
