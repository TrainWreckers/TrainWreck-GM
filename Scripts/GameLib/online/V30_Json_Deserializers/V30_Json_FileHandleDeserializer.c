class V30_Json_FileHandleDeserializer : V30_Json_Deserializer {
	protected ref FileHandle m_FileHandle;
	
	void V30_Json_FileHandleDeserializer(notnull FileHandle fileHandle) {
		m_FileHandle = fileHandle;
	};
	
	FileHandle GetFileHandle() {
		return m_FileHandle;
	};
	
	override void Next() {
		m_Token = null;
		
		if (m_FileHandle.IsEOF()) {
			m_Token = new V30_Json_Deserializer_Token_EOF("");
			return;
		}
		else if (!m_FileHandle.IsOpen()) {
			PrintFormat("%1: File handle %2 is not open.", this, m_FileHandle, level: LogLevel.ERROR);
			return;
		};
		
		auto begin = m_FileHandle.GetPos();
		
		string data;
		m_FileHandle.Read(data, 1);
		
		if (data == "n") {
			m_FileHandle.Read(data, "ull".Length());
			if (data == "ull") {
				m_Token = new V30_Json_Deserializer_Token_Null("null");
			};
		}
		else if (data == "t") {
			m_FileHandle.Read(data, "rue".Length());
			if (data == "rue") {
				m_Token = new V30_Json_Deserializer_Token_True("true");
			};
		}
		else if (data == "f") {
			m_FileHandle.Read(data, "alse".Length());
			if (data == "alse") {
				m_Token = new V30_Json_Deserializer_Token_False("false");
			};
		}
		else if (data == "[") {
			m_Token = new V30_Json_Deserializer_Token_OpenBracket("[");
		}
		else if (data == "]") {
			m_Token = new V30_Json_Deserializer_Token_CloseBracket("]");
		}
		else if (data == "{") {
			m_Token = new V30_Json_Deserializer_Token_OpenBrace("{");
		}
		else if (data == "}") {
			m_Token = new V30_Json_Deserializer_Token_CloseBrace("}");
		}
		else if (data == ",") {
			m_Token = new V30_Json_Deserializer_Token_Coma(",");
		}
		else if (data == ":") {
			m_Token = new V30_Json_Deserializer_Token_Colon(":");
		}
		else if (data.IsSpaceAt(0)) {
			if (data == "\n") {
				m_Token = new V30_Json_Deserializer_Token_NewLine(data);
			}
			else {
				m_Token = new V30_Json_Deserializer_Token_WhiteSpace(data);
			};
		}
		else if (data == "\"") {
			auto slashes = 0;
			string prev = data;
			while (!m_FileHandle.IsEOF()) {
				string curr;
				m_FileHandle.Read(curr, 1);
				if (curr == "\"" && (prev != "\\" || slashes % 2 == 0)) {
					m_Token = new V30_Json_Deserializer_Token_String(Substring(begin));
					break;
				}
				else if (curr == "\\") {
					slashes++;
				};
				prev = curr;
			};
			if (!m_Token) {
				m_Token = new V30_Json_Deserializer_Token_Error_UnfinishedString(Substring(begin));
			};
		}
		else {
			m_FileHandle.Seek(begin);
			m_FileHandle.Read(data, 32);
			
			int iParsed;
			data.ToInt(parsed: iParsed);
			
			int fParsed;
			data.ToFloat(parsed: fParsed);
			
			if (fParsed > 0 && fParsed > iParsed) {
				m_Token = new V30_Json_Deserializer_Token_Float(data.Substring(0, fParsed));
				m_FileHandle.Seek(begin + fParsed);
			}
			else if (iParsed > 0) {
				m_Token = new V30_Json_Deserializer_Token_Int(data.Substring(0, iParsed));
				m_FileHandle.Seek(begin + iParsed);
			};
		};
		
		if (!m_Token) {
			m_FileHandle.Seek(begin);
			m_Token = new V30_Json_Deserializer_Token_Error("");
		};
	};
	
	protected string Substring(int from) {
		auto to = m_FileHandle.GetPos();
		string data;
		m_FileHandle.Seek(from);
		m_FileHandle.Read(data, to - from);
		return data;
	};
};