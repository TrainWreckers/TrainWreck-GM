class V30_Json_FileDeserializer : V30_Json_Deserializer {
	protected ref V30_Json_FileHandleDeserializer m_FileHandleDeserializer;
	
	void V30_Json_FileDeserializer(string path = string.Empty) {
		if (path == string.Empty) return;
		Open(path);
	};
	
	V30_Json_FileHandleDeserializer GetFileHandleDeserializer() {
		return m_FileHandleDeserializer;
	};
	
	override void Next() {
		m_FileHandleDeserializer.Next();
		m_Token = m_FileHandleDeserializer.m_Token;
	};
	
	void Open(string path) {
		if (IsOpen()) Close();
		
		if (!FileIO.FileExists(path)) {
			PrintFormat("%1.Open('%2'): File doesn't exists.", this, path, level: LogLevel.ERROR);
			return;
		};
		
		auto fileHandle = FileIO.OpenFile(path, FileMode.READ);
		if (!fileHandle || !fileHandle.IsOpen()) {
			PrintFormat("%1: Failed to open file '%2'.", this, path, level: LogLevel.ERROR);
			return;
		};
		
		m_FileHandleDeserializer = V30_Json_FileHandleDeserializer(fileHandle);
	};
	
	bool IsOpen() {
		return m_FileHandleDeserializer;
	};
	
	void Close() {
		m_FileHandleDeserializer.GetFileHandle().Close();
		m_FileHandleDeserializer = null;
	};
	
	FileHandle GetFileHandle() {
		return m_FileHandleDeserializer.GetFileHandle();
	};
	
	void ~V30_Json_FileDeserializer() {
		if (!IsOpen()) return;
		Close();
	};
};