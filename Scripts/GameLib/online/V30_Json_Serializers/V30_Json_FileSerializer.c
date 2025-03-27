class V30_Json_FileSerializer : V30_Json_Serializer {
	protected ref V30_Json_FileHandleSerializer m_FileHandleSerializer;
	
	void V30_Json_FileSerializer(string path = string.Empty) {
		if (path == string.Empty) return;
		Open(path);
	};
	
	override void Serialize(V30_Json_Value value = null) {
		m_FileHandleSerializer.Serialize(value);
	};
	
	void Open(string path) {
		if (IsOpen()) Close();
		
		auto fileHandle = FileIO.OpenFile(path, FileMode.WRITE);
		if (!fileHandle || !fileHandle.IsOpen()) {
			PrintFormat("%1: Failed to open file '%2'.", this, path, level: LogLevel.ERROR);
			return;
		};
		
		m_FileHandleSerializer = new V30_Json_FileHandleSerializer(fileHandle);
	};
	
	bool IsOpen() {
		return m_FileHandleSerializer;
	};
	
	void Close() {
		m_FileHandleSerializer.GetFileHandle().Close();
		m_FileHandleSerializer = null;
	};
	
	FileHandle GetFileHandle() {
		return m_FileHandleSerializer.GetFileHandle();
	};
	
	void ~V30_Json_FileSerializer() {
		if (!IsOpen()) return;
		Close();
	};
};
