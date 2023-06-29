class EDF_ReflectionUtils
{
	static typename GetAttributeParent()
	{
		string callStack;
		Debug.DumpStack(callStack);
		int startRead = callStack.LastIndexOf("#CreateAttributes") + 17;
		int stopRead = callStack.IndexOfFrom(startRead, "(");
		return callStack.Substring(startRead, stopRead - startRead).ToType();
	}
};

class EDF_ReflectionUtilsT<Class T>
{
	//------------------------------------------------------------------------------------------------
	static bool Get(notnull Class instance, string variableName, out T value)
	{
		EDF_ReflectionVariableInfo info = EDF_ReflectionVariableInfo.Get(instance, variableName);
		if (!info)
			return false;
		return info.m_tHolderType.GetVariableValue(instance, info.m_iVariableIndex, value);
	}

	//------------------------------------------------------------------------------------------------
	//TODO: Add Set() via Json serializer
};

enum EDF_ReflectionVariableCollectionType
{
	NONE,
	ARRAY,
	SET,
	MAP
};

class EDF_ReflectionVariableInfo
{
	protected static ref map<string, ref EDF_ReflectionVariableInfo> s_mTypeCache;

	int m_iVariableIndex;
	typename m_tVaribleType;
	typename m_tHolderType;
	typename m_tCollectionKeyType;
	typename m_tCollectionValueType;
	EDF_ReflectionVariableCollectionType m_eCollectionType;

	//------------------------------------------------------------------------------------------------
	//! Empty fieldname indicates nested collection
	static EDF_ReflectionVariableInfo Get(notnull Class instance, string fieldName)
	{
		if (!s_mTypeCache)
			s_mTypeCache = new map<string, ref EDF_ReflectionVariableInfo>();

		typename type = instance.Type();
		string typeCacheKey = string.Format("%1::%2", type.ToString(), fieldName);
		EDF_ReflectionVariableInfo info = s_mTypeCache.Get(typeCacheKey);
		if (!info)
		{
			int variableIndex = -1;
			typename varibleType;
			typename holderType;

			if (fieldName)
			{
				holderType = type;
				for (int vIdx = 0; vIdx < type.GetVariableCount(); vIdx++)
				{
					if (type.GetVariableName(vIdx) == fieldName)
					{
						variableIndex = vIdx;
						varibleType = type.GetVariableType(vIdx);
						break;
					}
				}
			}
			else
			{
				varibleType = type;
			}

			info = new EDF_ReflectionVariableInfo(variableIndex, varibleType, holderType);
			s_mTypeCache.Set(typeCacheKey, info);
		}

		return info;
	}

	//------------------------------------------------------------------------------------------------
	protected void EDF_ReflectionVariableInfo(int variableIndex, typename variableType, typename holderType)
	{
		m_iVariableIndex = variableIndex;
		m_tVaribleType = variableType;
		m_tHolderType = holderType;

		if (variableType.IsInherited(array) || variableType.IsInherited(set))
		{
			if (variableType.IsInherited(array))
			{
				m_eCollectionType = EDF_ReflectionVariableCollectionType.ARRAY;
			}
			else
			{
				m_eCollectionType = EDF_ReflectionVariableCollectionType.SET;
			}

			string typeString = variableType.ToString();
			typeString.Replace("@", "");

			int templateStart = typeString.IndexOf("<") + 1;
			string collectionValueTypeString = typeString.Substring(templateStart, typeString.Length() - templateStart - 1);
			m_tCollectionValueType = collectionValueTypeString.ToType();
		}
		else if (variableType.IsInherited(map))
		{
			m_eCollectionType = EDF_ReflectionVariableCollectionType.MAP;

			string typeString = variableType.ToString();
			typeString.Replace("@", "");

			int keyTypeStart = typeString.IndexOf("<") + 1;
			int valueTypeStart = typeString.IndexOfFrom(keyTypeStart, ",") + 1;

			string collectionKeyTypeString = typeString.Substring(keyTypeStart, valueTypeStart - keyTypeStart - 1);
			m_tCollectionKeyType = collectionKeyTypeString.ToType();

			string collectionValueTypeString = typeString.Substring(valueTypeStart, typeString.Length() - valueTypeStart - 1);
			m_tCollectionValueType = collectionValueTypeString.ToType();
		}
	}
};
