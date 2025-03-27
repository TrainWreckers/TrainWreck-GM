class V30_Json_PrettyFileSerializer : V30_Json_FileSerializer {
	protected int m_SpaceCount;
	
	protected bool m_UseTabs;
	
	void V30_Json_PrettyFileSerializer(string path = string.Empty, int spaceCount = 2, bool useTabs = false) {
		m_SpaceCount = spaceCount;
		m_UseTabs = useTabs;
	};
	
	override void Open(string path) {
		super.Open(path);
		m_FileHandleSerializer = new V30_Json_PrettyFileHandleSerializer(m_FileHandleSerializer.GetFileHandle(), m_SpaceCount, m_UseTabs);
	};
	
	int GetSpaceCount() {
		return m_SpaceCount;
	};
	
	bool IsUseTabs() {
		return m_UseTabs;
	};
};
