class V30_Json_Deserializer_Token {
	protected string m_sPlain;

	void V30_Json_Deserializer_Token(string plain) {
		m_sPlain = plain;
	};

	string GetPlain() {
		return m_sPlain;
	};
};

class V30_Json_Deserializer_Token_WhiteSpace : V30_Json_Deserializer_Token {};

class V30_Json_Deserializer_Token_NewLine : V30_Json_Deserializer_Token_WhiteSpace {};

class V30_Json_Deserializer_Token_Literal : V30_Json_Deserializer_Token {};

class V30_Json_Deserializer_Token_Null : V30_Json_Deserializer_Token_Literal {};

class V30_Json_Deserializer_Token_True : V30_Json_Deserializer_Token_Literal {
	bool Get() {
		return true;
	};
};

class V30_Json_Deserializer_Token_False : V30_Json_Deserializer_Token_Literal {
	bool Get() {
		return false;
	};
};

class V30_Json_Deserializer_Token_Int : V30_Json_Deserializer_Token_Literal {
	int Get() {
		return GetPlain().ToInt();
	};
};

class V30_Json_Deserializer_Token_Float : V30_Json_Deserializer_Token_Literal {
	float Get() {
		return GetPlain().ToFloat();
	};
};

class V30_Json_Deserializer_Token_String : V30_Json_Deserializer_Token_Literal {
	string Get() {
		return GetPlain().Substring(1, GetPlain().Length() - 2);
	};
};

class V30_Json_Deserializer_Token_Punctuator : V30_Json_Deserializer_Token {};

class V30_Json_Deserializer_Token_OpenBracket : V30_Json_Deserializer_Token_Punctuator {};

class V30_Json_Deserializer_Token_CloseBracket : V30_Json_Deserializer_Token_Punctuator {};

class V30_Json_Deserializer_Token_OpenBrace : V30_Json_Deserializer_Token_Punctuator {};

class V30_Json_Deserializer_Token_CloseBrace : V30_Json_Deserializer_Token_Punctuator {};

class V30_Json_Deserializer_Token_Coma : V30_Json_Deserializer_Token_Punctuator {};

class V30_Json_Deserializer_Token_Colon : V30_Json_Deserializer_Token_Punctuator {};

class V30_Json_Deserializer_Token_Error : V30_Json_Deserializer_Token {};

class V30_Json_Deserializer_Token_Error_IllegalCharacter : V30_Json_Deserializer_Token_Error {};

class V30_Json_Deserializer_Token_Error_UnfinishedString : V30_Json_Deserializer_Token_Error {};

class V30_Json_Deserializer_Token_EOF : V30_Json_Deserializer_Token_Error {};



class V30_Json_Deserializer_Error {
	string What();
};

class V30_Json_Deserializer_Error_Ok : V30_Json_Deserializer_Error {
	override string What() {
		return "OK";
	};
};

class V30_Json_Deserializer_Error_UnexpectedToken : V30_Json_Deserializer_Error {
	protected ref V30_Json_Deserializer_Token m_Token;

	void V30_Json_Deserializer_Error_UnexpectedToken(V30_Json_Deserializer_Token token) {
		m_Token = token;
	};

	V30_Json_Deserializer_Token Get() {
		return m_Token;
	};

	override string What() {
		return string.Format("Unexpected token \"%1\"", m_Token.GetPlain());
	};
};

class V30_Json_Deserializer_Error_MissingValue : V30_Json_Deserializer_Error_UnexpectedToken {
	override string What() {
		return string.Format("Expected any json value, received \"%1\"", Get().GetPlain());
	};
};

class V30_Json_Deserializer_Error_MissingEOF : V30_Json_Deserializer_Error_UnexpectedToken {
	override string What() {
		return string.Format("Expected EOF, received \"%1\"", Get().GetPlain());
	};
};

class V30_Json_Deserializer_Error_MissingArrayValue : V30_Json_Deserializer_Error_UnexpectedToken {
	override string What() {
		return string.Format("Expected array value, received \"%1\"", Get().GetPlain());
	};
};

class V30_Json_Deserializer_Error_MissingArrayValueOrCloseBraket : V30_Json_Deserializer_Error_UnexpectedToken {
	override string What() {
		return string.Format("Expected array value or \"]\", received \"%1\"", Get().GetPlain());
	};
};

class V30_Json_Deserializer_Error_MissingObjectKey : V30_Json_Deserializer_Error_UnexpectedToken {
	override string What() {
		return string.Format("Expected object key, received \"%1\"", Get().GetPlain());
	};
};

class V30_Json_Deserializer_Error_MissingObjectKeyOrCloseBrace : V30_Json_Deserializer_Error_UnexpectedToken {
	override string What() {
		return string.Format("Expected object key or \"}\", received \"%1\"", Get().GetPlain());
	};
};

class V30_Json_Deserializer_Error_MissingObjectValue : V30_Json_Deserializer_Error_UnexpectedToken {
	override string What() {
		return string.Format("Expected object value, received \"%1\"", Get().GetPlain());
	};
};

class V30_Json_Deserializer_Error_MissingComaOrCloseBraket : V30_Json_Deserializer_Error_UnexpectedToken {
	override string What() {
		return string.Format("Expected \",\" or \"]\", received \"%1\"", Get().GetPlain());
	};
};

class V30_Json_Deserializer_Error_MissingComaOrCloseBrace : V30_Json_Deserializer_Error_UnexpectedToken {
	override string What() {
		return string.Format("Expected \",\" or \"}\", received \"%1\"", Get().GetPlain());
	};
};

class V30_Json_Deserializer_Error_MissingColon : V30_Json_Deserializer_Error_UnexpectedToken {
	override string What() {
		return string.Format("Expected \":\", received \"%1\"", Get().GetPlain());
	};
};

enum V30_Json_Deserializer_EState {
	DEFAULT,
	ARRAY_BEGIN,
	ARRAY_NEXT,
	ARRAY_VALUE,
	OBJECT_BEGIN,
	OBJECT_COLON,
	OBJECT_VALUE,
	OBJECT_NEXT,
	OBJECT_KEY,
	END
};

enum V30_Json_Deserializer_EStateValue {
	ARRAY,
	OBJECT
};

class V30_Json_Deserializer_Stack {
	protected ref array<V30_Json_Deserializer_EStateValue> m_aData;

	void V30_Json_Deserializer_Stack() {
		m_aData = new array<V30_Json_Deserializer_EStateValue>();
	};

	void Push(V30_Json_Deserializer_EStateValue value) {
		m_aData.Insert(value);
	};

	V30_Json_Deserializer_EStateValue Peek() {
		auto n = m_aData.Count();
		if (n == 0) return null;
		auto i = n - 1;
		return m_aData[i];
	};

	V30_Json_Deserializer_EStateValue Pop() {
		auto n = m_aData.Count();
		if (n == 0) return null;
		auto i = n - 1;
		auto value = m_aData[i];
		m_aData.Remove(i);
		return value;
	};

	void Clear() {
		m_aData.Clear();
	};

	bool IsEmpty() {
		return m_aData.IsEmpty();
	};

	int Count() {
		return m_aData.Count();
	};

	bool IsPlain() {
		return IsEmpty();
	};

	bool IsArray() {
		return !IsEmpty() && Peek() == V30_Json_Deserializer_EStateValue.ARRAY;
	};

	bool IsObject() {
		return !IsEmpty() && Peek() == V30_Json_Deserializer_EStateValue.OBJECT;
	};
};

class V30_Json_Deserializer_KeyStack {
	protected ref array<string> m_aData;

	void V30_Json_Deserializer_KeyStack() {
		m_aData = new array<string>();
	};

	void Push(string value) {
		m_aData.Insert(value);
	};

	string Peek() {
		auto n = m_aData.Count();
		if (n == 0) return string.Empty;
		auto i = n - 1;
		return m_aData[i];
	};

	string Pop() {
		auto n = m_aData.Count();
		if (n == 0) return string.Empty;
		auto i = n - 1;
		auto value = m_aData[i];
		m_aData.Remove(i);
		return value;
	};

	void Clear() {
		m_aData.Clear();
	};

	bool IsEmpty() {
		return m_aData.IsEmpty();
	};

	int Count() {
		return m_aData.Count();
	};
};

class V30_Json_Deserializer_ValueStack {
	protected ref array<ref V30_Json_Value> m_aData;

	void V30_Json_Deserializer_ValueStack() {
		m_aData = new array<ref V30_Json_Value>();
	};

	void Push(V30_Json_Value value) {
		m_aData.Insert(value);
	};

	V30_Json_Value Peek() {
		auto n = m_aData.Count();
		if (n == 0) return null;
		auto i = n - 1;
		return m_aData[i];
	};

	V30_Json_Value Pop() {
		auto n = m_aData.Count();
		if (n == 0) return null;
		auto i = n - 1;
		auto value = m_aData[i];
		m_aData.Remove(i);
		return value;
	};

	void Clear() {
		m_aData.Clear();
	};

	bool IsEmpty() {
		return m_aData.IsEmpty();
	};

	int Count() {
		return m_aData.Count();
	};

	bool IsPlain() {
		return IsEmpty();
	};

	bool IsArray() {
		return !!V30_Json_array.Cast(Peek());
	};

	bool IsObject() {
		return !!V30_Json_object.Cast(Peek());
	};

	V30_Json_array PeekArray() {
		return V30_Json_array.Cast(Peek());
	};

	V30_Json_object PeekObject() {
		return V30_Json_object.Cast(Peek());
	};
};

class V30_Json_Deserializer {
	protected V30_Json_Deserializer_EState m_eState;

	protected int m_iIndex;

	protected string m_sKey;

	protected ref V30_Json_Deserializer_KeyStack m_KeyStack;

	protected ref V30_Json_Deserializer_Stack m_Stack;

	protected ref V30_Json_Deserializer_ValueStack m_ValueStack;

	protected ref V30_Json_Value m_Value;

	protected bool m_bWorking;

	protected ref V30_Json_Deserializer_Error m_Error;

	protected ref V30_Json_Deserializer_Token m_Token;

	void V30_Json_Deserializer() {
		m_Stack = new V30_Json_Deserializer_Stack();
	};

	protected V30_Json_Deserializer_Token Peek() {
		return m_Token;
	};

	protected void Next();

	V30_Json_Value Deserialize() {
		Start();
		while (m_bWorking) ProcessToken(Peek());
		V30_Json_Value value = null;
		OnEnd(value, m_Error);
		return value;
	};

	protected void ProcessToken(V30_Json_Deserializer_Token token) {
		if (V30_Json_Deserializer_Token_WhiteSpace.Cast(token)) {
			Next();
			return;
		};
		switch (m_eState) {
			case V30_Json_Deserializer_EState.DEFAULT:
				if (V30_Json_Deserializer_Token_Literal.Cast(token)) {
					Value(GetValueFromLiteral(V30_Json_Deserializer_Token_Literal.Cast(token)));
				}
				else if (V30_Json_Deserializer_Token_OpenBracket.Cast(token)) {
					BeginArray();
				}
				else if (V30_Json_Deserializer_Token_OpenBrace.Cast(token)) {
					BeginObject();
				}
				else {
					Error(new V30_Json_Deserializer_Error_MissingValue(token));
				};
				break;
			case V30_Json_Deserializer_EState.ARRAY_BEGIN:
				if (V30_Json_Deserializer_Token_Literal.Cast(token)) {
					ArrayValue(GetValueFromLiteral(V30_Json_Deserializer_Token_Literal.Cast(token)));
				}
				else if (V30_Json_Deserializer_Token_OpenBracket.Cast(token)) {
					BeginArray();
				}
				else if (V30_Json_Deserializer_Token_OpenBrace.Cast(token)) {
					BeginObject();
				}
				else if (V30_Json_Deserializer_Token_CloseBracket.Cast(token)) {
					EndArray();
				}
				else {
					Error(new V30_Json_Deserializer_Error_MissingArrayValueOrCloseBraket(token));
				};
				break;
			case V30_Json_Deserializer_EState.ARRAY_VALUE:
				if (V30_Json_Deserializer_Token_Literal.Cast(token)) {
					ArrayValue(GetValueFromLiteral(V30_Json_Deserializer_Token_Literal.Cast(token)));
				}
				else if (V30_Json_Deserializer_Token_OpenBracket.Cast(token)) {
					BeginArray();
				}
				else if (V30_Json_Deserializer_Token_OpenBrace.Cast(token)) {
					BeginObject();
				}
				else {
					Error(new V30_Json_Deserializer_Error_MissingArrayValue(token));
				};
				break;
			case V30_Json_Deserializer_EState.ARRAY_NEXT:
				if (V30_Json_Deserializer_Token_Coma.Cast(token)) {
					ArrayComa();
				}
				else if (V30_Json_Deserializer_Token_CloseBracket.Cast(token)) {
					EndArray();
				}
				else {
					Error(new V30_Json_Deserializer_Error_MissingComaOrCloseBraket(token));
				};
				break;
			case V30_Json_Deserializer_EState.OBJECT_BEGIN:
				if (V30_Json_Deserializer_Token_String.Cast(token)) {
					ObjectKey(V30_Json_Deserializer_Token_String.Cast(token).Get());
				}
				else if (V30_Json_Deserializer_Token_CloseBrace.Cast(token)) {
					EndObject();
				}
				else {
					Error(new V30_Json_Deserializer_Error_MissingObjectKeyOrCloseBrace(token));
				};
				break;
			case V30_Json_Deserializer_EState.OBJECT_COLON:
				if (V30_Json_Deserializer_Token_Colon.Cast(token)) {
					ObjectColon();
				}
				else {
					Error(new V30_Json_Deserializer_Error_MissingColon(token));
				};
				break;
			case V30_Json_Deserializer_EState.OBJECT_VALUE:
				if (V30_Json_Deserializer_Token_Literal.Cast(token)) {
					ObjectValue(GetValueFromLiteral(V30_Json_Deserializer_Token_Literal.Cast(token)));
				}
				else if (V30_Json_Deserializer_Token_OpenBracket.Cast(token)) {
					BeginArray();
				}
				else if (V30_Json_Deserializer_Token_OpenBrace.Cast(token)) {
					BeginObject();
				}
				else {
					Error(new V30_Json_Deserializer_Error_MissingObjectValue(token));
				};
				break;
			case V30_Json_Deserializer_EState.OBJECT_NEXT:
				if (V30_Json_Deserializer_Token_Coma.Cast(token)) {
					ObjectComa();
				}
				else if (V30_Json_Deserializer_Token_CloseBrace.Cast(token)) {
					EndObject();
				}
				else {
					Error(new V30_Json_Deserializer_Error_MissingComaOrCloseBrace(token));
				};
				break;
			case V30_Json_Deserializer_EState.OBJECT_KEY:
				if (V30_Json_Deserializer_Token_String.Cast(token)) {
					ObjectKey(V30_Json_Deserializer_Token_String.Cast(token).Get());
				}
				else {
					Error(new V30_Json_Deserializer_Error_MissingObjectKey(token));
				};
				break;
			case V30_Json_Deserializer_EState.END:
				if (V30_Json_Deserializer_Token_EOF.Cast(token)) {
					End();
				}
				else {
					Error(new V30_Json_Deserializer_Error_MissingEOF(token));
				};
				break;
		};
	};

	protected void Start() {
		m_bWorking = true;
		OnBegin();
		Next();
	};

	protected void End(V30_Json_Deserializer_Error error = null) {
		if (!error) error = V30_Json_Deserializer_Error_Ok();
		m_bWorking = false;
		m_Error = error;
	};

	protected void Value(V30_Json_Value value) {
		OnValue(value);
		m_eState = V30_Json_Deserializer_EState.END;
		Next();
	};

	protected void BeginArray() {
		OnArrayBegin();
		m_Stack.Push(V30_Json_Deserializer_EStateValue.ARRAY);
		m_eState = V30_Json_Deserializer_EState.ARRAY_BEGIN;
		Next();
	};

	protected void ArrayValue(V30_Json_Value value) {
		OnValue(value);
		m_eState = V30_Json_Deserializer_EState.ARRAY_NEXT;
		Next();
	};

	protected void ArrayComa() {
		m_eState = V30_Json_Deserializer_EState.ARRAY_VALUE;
		Next();
	};

	protected void EndArray() {
		OnArrayEnd();
		m_Stack.Pop();
		if (m_Stack.IsArray()) {
			m_eState = V30_Json_Deserializer_EState.ARRAY_NEXT;
		}
		else if (m_Stack.IsObject()) {
			m_eState = V30_Json_Deserializer_EState.OBJECT_NEXT;
		}
		else {
			m_eState = V30_Json_Deserializer_EState.END;
		};
		Next();
	};

	protected void BeginObject() {
		OnObjectBegin();
		m_Stack.Push(V30_Json_Deserializer_EStateValue.OBJECT);
		m_eState = V30_Json_Deserializer_EState.OBJECT_BEGIN;
		Next();
	};

	protected void ObjectKey(string key) {
		OnObjectKey(key);
		m_eState = V30_Json_Deserializer_EState.OBJECT_COLON;
		Next();
	};

	protected void ObjectColon() {
		m_eState = V30_Json_Deserializer_EState.OBJECT_VALUE;
		Next();
	};

	protected void ObjectValue(V30_Json_Value value) {
		OnValue(value);
		m_eState = V30_Json_Deserializer_EState.OBJECT_NEXT;
		Next();
	};

	protected void ObjectComa() {
		m_eState = V30_Json_Deserializer_EState.OBJECT_KEY;
		Next();
	};

	protected void EndObject() {
		OnObjectEnd();
		m_Stack.Pop();
		if (m_Stack.IsArray()) {
			m_eState = V30_Json_Deserializer_EState.ARRAY_NEXT;
		}
		else if (m_Stack.IsObject()) {
			m_eState = V30_Json_Deserializer_EState.OBJECT_NEXT;
		}
		else {
			m_eState = V30_Json_Deserializer_EState.END;
		};
		Next();
	};

	protected void Error(V30_Json_Deserializer_Error error) {
		OnError(error);
		End();
	};



	// --------- Collect data into value

	protected void OnBegin() {
		m_sKey = "";
		m_Value = null;

		m_KeyStack = new V30_Json_Deserializer_KeyStack();
		m_ValueStack = new V30_Json_Deserializer_ValueStack();
	};

	protected void OnValue(V30_Json_Value value) {
		InsertValue(value);
	};

	protected void OnArrayBegin() {
		BeginContainer();
		m_Value = new V30_Json_array();
	};

	protected void OnArrayEnd() {
		EndContainer();
	};

	protected void OnObjectBegin() {
		BeginContainer();
		m_Value = new V30_Json_object();
	};

	protected void OnObjectKey(string key) {
		m_sKey = key;
	};

	protected void OnObjectEnd() {
		EndContainer();
	};

	protected void OnError(V30_Json_Deserializer_Error error) {
		Print(error.What(), level: LogLevel.ERROR);
	};

	protected void OnEnd(out V30_Json_Value value, V30_Json_Deserializer_Error error) {
		value = m_Value;

		m_sKey = string.Empty;
		m_Value = null;

		m_KeyStack = null;
		m_ValueStack = null;
	};

	private void InsertValue(V30_Json_Value value) {
		if (V30_Json_array.Cast(m_Value)) {
			V30_Json_array.Cast(m_Value).Get().Insert(value);
		}
		else if (V30_Json_object.Cast(m_Value)) {
			V30_Json_object.Cast(m_Value).Get().Insert(m_sKey, value);
		}
		else {
			m_Value = value;
		};
	};

	private void BeginContainer() {
		if (m_Value) {
			m_KeyStack.Push(m_sKey);
			m_ValueStack.Push(m_Value);
		};
		m_sKey = string.Empty;
	};

	private void EndContainer() {
		auto value = m_Value;
		m_Value = m_ValueStack.Pop();
		m_sKey = m_KeyStack.Pop();
		InsertValue(value);
	};



	// --------- Utilities

	private static V30_Json_Value GetValueFromLiteral(V30_Json_Deserializer_Token_Literal literal) {
		if (V30_Json_Deserializer_Token_Null.Cast(literal)) {
			return new V30_Json_null();
		}
		else if (V30_Json_Deserializer_Token_True.Cast(literal)) {
			return new V30_Json_bool(true);
		}
		else if (V30_Json_Deserializer_Token_False.Cast(literal)) {
			return new V30_Json_bool(false);
		}
		else if (V30_Json_Deserializer_Token_Int.Cast(literal)) {
			return new V30_Json_int(V30_Json_Deserializer_Token_Int.Cast(literal).Get());
		}
		else if (V30_Json_Deserializer_Token_Float.Cast(literal)) {
			return new V30_Json_float(V30_Json_Deserializer_Token_Float.Cast(literal).Get());
		}
		else if (V30_Json_Deserializer_Token_String.Cast(literal)) {
			return new V30_Json_string(V30_Json_Deserializer_Token_String.Cast(literal).Get());
		}
		else {
			return null;
		};
	};
};
