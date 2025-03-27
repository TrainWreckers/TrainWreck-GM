class V30_Json_StringDeserializer : V30_Json_Deserializer {
	protected string m_sSrc;
	
	protected int m_iFrom;
	
	protected int m_iTo;
	
	protected int m_iCursor;
	
	void V30_Json_StringDeserializer(string src, int from = 0, int count = -1) {
		m_sSrc = src;
		m_iFrom = from;
		if (count == -1) count = m_sSrc.Length() - from;
		m_iTo = from + count;
		m_iCursor = m_iFrom;
	};
	
	override void Next() {
		m_Token = null;
		
		if (m_iCursor >= m_iTo) {
			m_Token = new V30_Json_Deserializer_Token_EOF("");
			return;
		};
		
		if (m_sSrc.ContainsAt("null", m_iCursor)) {
			m_Token = new V30_Json_Deserializer_Token_Null("null");
			m_iCursor += "null".Length();
		}
		else if (m_sSrc.ContainsAt("true", m_iCursor)) {
			m_Token = new V30_Json_Deserializer_Token_True("true");
			m_iCursor += "true".Length();
		}
		else if (m_sSrc.ContainsAt("false", m_iCursor)) {
			m_Token = new V30_Json_Deserializer_Token_False("false");
			m_iCursor += "false".Length();
		}
		else if (m_sSrc.ContainsAt("[", m_iCursor)) {
			m_Token = new V30_Json_Deserializer_Token_OpenBracket("[");
			m_iCursor += "[".Length();
		}
		else if (m_sSrc.ContainsAt("]", m_iCursor)) {
			m_Token = new V30_Json_Deserializer_Token_CloseBracket("]");
			m_iCursor += "]".Length();
		}
		else if (m_sSrc.ContainsAt("{", m_iCursor)) {
			m_Token = new V30_Json_Deserializer_Token_OpenBrace("{");
			m_iCursor += "{".Length();
		}
		else if (m_sSrc.ContainsAt("}", m_iCursor)) {
			m_Token = new V30_Json_Deserializer_Token_CloseBrace("}");
			m_iCursor += "}".Length();
		}
		else if (m_sSrc.ContainsAt(",", m_iCursor)) {
			m_Token = new V30_Json_Deserializer_Token_Coma(",");
			m_iCursor += ",".Length();
		}
		else if (m_sSrc.ContainsAt(":", m_iCursor)) {
			m_Token = new V30_Json_Deserializer_Token_Colon(":");
			m_iCursor += ":".Length();
		}
		else if (m_sSrc.ContainsAt(" ", m_iCursor)) {
			m_Token = new V30_Json_Deserializer_Token_WhiteSpace(" ");
			m_iCursor += " ".Length();
		}
		else if (m_sSrc.ContainsAt("\t", m_iCursor)) {
			m_Token = new V30_Json_Deserializer_Token_WhiteSpace("\t");
			m_iCursor += "\t".Length();
		}
		else if (m_sSrc.ContainsAt("\n", m_iCursor)) {
			m_Token = new V30_Json_Deserializer_Token_NewLine(m_sSrc.Substring(m_iCursor, "\n".Length()));
			m_iCursor += "\n".Length();
		}
		else if (m_sSrc.ContainsAt("\"", m_iCursor)) {
			auto i = m_iCursor;
			auto slashes = 0;
			m_iCursor++;
			while (m_iCursor < m_iTo) {
				if (m_sSrc.Substring(m_iCursor, 1) == "\"" && (m_sSrc.Substring(m_iCursor - 1, 1) != "\\" || slashes % 2 == 0)) {
					m_iCursor++;
					m_Token = new V30_Json_Deserializer_Token_String(Substring(i));
					break;
				}
				else if (m_sSrc.Substring(m_iCursor, 1) == "\\") {
					slashes++;
				};
				m_iCursor++;
			};
			if (!m_Token) {
				m_Token = new V30_Json_Deserializer_Token_Error_UnfinishedString(Substring(i));
			};
		}
		else {
			int iParsed;
			m_sSrc.ToInt(offset: m_iCursor, parsed: iParsed);
			
			int fParsed;
			m_sSrc.ToFloat(offset: m_iCursor, parsed: fParsed);
			
			if (fParsed > 0 && fParsed > iParsed) {
				m_Token = new V30_Json_Deserializer_Token_Float(m_sSrc.Substring(m_iCursor, fParsed));
				m_iCursor += fParsed;
			}
			else if (iParsed > 0) {
				m_Token = new V30_Json_Deserializer_Token_Int(m_sSrc.Substring(m_iCursor, iParsed));
				m_iCursor += iParsed;
			}
			else {
				m_Token = new V30_Json_Deserializer_Token_Error_IllegalCharacter(m_sSrc.Substring(m_iCursor, 1));
				m_iCursor += 1;
			};
		};
	};
	
	string Substring(int from) {
		return m_sSrc.Substring(from, m_iCursor - from);
	};
};