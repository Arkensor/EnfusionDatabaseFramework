class EDF_DbFind
{
	//------------------------------------------------------------------------------------------------
	static EDF_DbFindAnd And(notnull array<ref EDF_DbFindCondition> conditions)
	{
		return EDF_DbFindAnd.Create(conditions);
	}

	//------------------------------------------------------------------------------------------------
	static EDF_DbFindOr Or(notnull array<ref EDF_DbFindCondition> conditions)
	{
		return EDF_DbFindOr.Create(conditions);
	}

	//------------------------------------------------------------------------------------------------
	static EDF_DbFindFieldCollectionHandlingBuilder Field(string fieldPath)
	{
		return EDF_DbFindFieldFinalBuilder.Create().Field(fieldPath);
	}

	//------------------------------------------------------------------------------------------------
	static EDF_DbFindFieldInvertableConditionBuilder Id()
	{
		// Return with only primitive field builder options (implicit cast)
		return EDF_DbFindFieldFinalBuilder.Create().Field(EDF_DbEntity.FIELD_ID);
	}
};

class EDF_DbFindCondition
{
	protected static ref array<ref EDF_DbFindCondition> ALLOC_BUFFER;

	//------------------------------------------------------------------------------------------------
	sealed void Debug()
	{
		PrintFormat("%1:\n%2", this, GetDebugString());
	}

	//------------------------------------------------------------------------------------------------
	string GetDebugString();
};

class EDF_DbFindConditionWithChildren : EDF_DbFindCondition
{
	ref array<ref EDF_DbFindCondition> m_aConditions;

	//------------------------------------------------------------------------------------------------
	override protected string GetDebugString()
	{
		if (!m_aConditions)
			return "null";

		string dbg = "(";

		foreach (int nCondtion, EDF_DbFindCondition condition : m_aConditions)
		{
			dbg += "\n";

			array<string> conditionLines();
			condition.GetDebugString().Split("\n", conditionLines, true);

			foreach (int nLine, string line : conditionLines)
			{
				dbg += string.Format(" %1", line);

				if (nLine != conditionLines.Count() -1)
					dbg += "\n";
			}

			if (nCondtion == m_aConditions.Count() - 1)
			{
				dbg += "\n";
			}
			else
			{
				dbg += ",";
			}
		}

		return dbg + ")";
	}
};

class EDF_DbFindAnd : EDF_DbFindConditionWithChildren
{
	//------------------------------------------------------------------------------------------------
	override protected string GetDebugString()
	{
		return "And" + super.GetDebugString();
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		saveContext.WriteValue("$type", "DbFindAnd");
		saveContext.WriteValue("conditions", m_aConditions);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static EDF_DbFindAnd Create(notnull array<ref EDF_DbFindCondition> conditions)
	{
		EDF_DbFindAnd inst();
		inst.m_aConditions = conditions;

		if (!ALLOC_BUFFER) ALLOC_BUFFER = {null};
		ALLOC_BUFFER.Set(0, inst);
		return inst;
	}
};

class EDF_DbFindOr : EDF_DbFindConditionWithChildren
{
	//------------------------------------------------------------------------------------------------
	override protected string GetDebugString()
	{
		return "Or" + super.GetDebugString();
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		saveContext.WriteValue("$type", "DbFindOr");
		saveContext.WriteValue("conditions", m_aConditions);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static EDF_DbFindOr Create(notnull array<ref EDF_DbFindCondition> conditions)
	{
		EDF_DbFindOr inst();
		inst.m_aConditions = conditions;

		if (!ALLOC_BUFFER) ALLOC_BUFFER = {null};
		ALLOC_BUFFER.Set(0, inst);
		return inst;
	}
};

class EDF_DbFindFieldCondition : EDF_DbFindCondition
{
	string m_sFieldPath;
	bool m_bUsesTypename;

	//------------------------------------------------------------------------------------------------
	protected void SerializationWritePath(BaseSerializationSaveContext saveContext)
	{
		// Replace any typenames in path with their db name aliases
		string fieldPath = m_sFieldPath;
		if (m_bUsesTypename)
		{
			fieldPath = string.Empty;
			array<string> pathSegments();
			m_sFieldPath.Split(EDF_DbFindFieldAnnotations.SEPERATOR, pathSegments, true);
			int count = pathSegments.Count();
			foreach (int idx, string pathSegment : pathSegments)
			{
				string typeString;
				int modifierIdx = pathSegment.IndexOf(":");
				if (modifierIdx != -1)
				{
					typeString = pathSegment.Substring(0, modifierIdx);
				}
				else
				{
					typeString = pathSegment;
				}

				typename type = typeString.ToType();
				if (type)
				{
					fieldPath += EDF_DbName.Get(type);

					if (modifierIdx != -1)
						fieldPath += pathSegment.Substring(modifierIdx, pathSegment.Length() - modifierIdx);
				}
				else
				{
					fieldPath += pathSegment;
				}

				if (idx < count - 1)
					fieldPath += EDF_DbFindFieldAnnotations.SEPERATOR;
			}
		}

		saveContext.WriteValue("fieldPath", fieldPath);
	}
};

class EDF_DbFindCheckFieldNullOrDefault : EDF_DbFindFieldCondition
{
	bool m_ShouldBeNullOrDefault;

	//------------------------------------------------------------------------------------------------
	override protected string GetDebugString()
	{
		return string.Format("CheckNullOrDefault(fieldPath:'%1', shouldBeNullOrDefault:%2)", m_sFieldPath, m_ShouldBeNullOrDefault.ToString());
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		saveContext.WriteValue("$type", "DbFindCheckFieldNullOrDefault");
		SerializationWritePath(saveContext);
		saveContext.WriteValue("shouldBeNullOrDefault", m_ShouldBeNullOrDefault);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static EDF_DbFindCheckFieldNullOrDefault Create(string fieldPath, bool shouldBeNullOrDefault, bool usesTypename)
	{
		EDF_DbFindCheckFieldNullOrDefault inst();
		inst.m_sFieldPath = fieldPath;
		inst.m_ShouldBeNullOrDefault = shouldBeNullOrDefault;
		inst.m_bUsesTypename = usesTypename;

		if (!ALLOC_BUFFER) ALLOC_BUFFER = {null};
		ALLOC_BUFFER.Set(0, inst);
		return inst;
	}
};

enum EDF_EDbFindOperator
{
	EQUAL,
	NOT_EQUAL,
	ARRAY_EQUAL,
	ARRAY_NOT_EQUAL,
	LESS_THAN,
	LESS_THAN_OR_EQUAL,
	GREATER_THAN,
	GREATER_THAN_OR_EQUAL,
	CONTAINS,
	NOT_CONTAINS,
	CONTAINS_ALL,
	NOT_CONTAINS_ALL
};

class EDF_DbFindCompareFieldValues<Class ValueType> : EDF_DbFindFieldCondition
{
	private static ref array<ref EDF_DbFindCompareFieldValues<ValueType>> ALLOC_BUFFER_TVALUES;

	EDF_EDbFindOperator m_eComparisonOperator;
	ref array<ValueType> m_aComparisonValues;
	bool m_bStringsInvariant;

	//------------------------------------------------------------------------------------------------
	override protected string GetDebugString()
	{
		string resultString = "{";
		foreach (int idx, ValueType value : m_aComparisonValues)
		{
			if (idx != 0) resultString += ",";

			if (idx > 10)
			{
				resultString += "...";
				break;
			}

			typename valueType = ValueType;

			if (valueType.IsInherited(bool))
			{
				if (value)
				{
					resultString += "true";
				}
				else
				{
					resultString += "false";
				}
			}
			else
			{
				resultString += string.Format("%1", value);
			}
		}
		resultString += "}";

		resultString = string.Format("Compare(fieldPath:'%1', operator:%2, values:%3", m_sFieldPath, typename.EnumToString(EDF_EDbFindOperator, m_eComparisonOperator), resultString);

		if (m_bStringsInvariant)
			resultString += ", invariant: true";

		return resultString + ")";
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		typename valueType = ValueType;
		saveContext.WriteValue("$type", "DbFindCompareFieldValues<" + valueType + ">");
		SerializationWritePath(saveContext);
		saveContext.WriteValue("comparisonOperator", m_eComparisonOperator);

		if (valueType.IsInherited(typename))
		{
			array<string> comparisonValues();
			comparisonValues.Reserve(m_aComparisonValues.Count());
			foreach (ValueType value : m_aComparisonValues)
			{
				// Go through string to bypass template restrictions
				comparisonValues.Insert(EDF_DbName.Get(("" + value).ToType()));
			}
			saveContext.WriteValue("comparisonValues", comparisonValues);
		}
		else
		{
			saveContext.WriteValue("comparisonValues", m_aComparisonValues);
		}

		if (m_bStringsInvariant)
			saveContext.WriteValue("stringsInvariant", true);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	static EDF_DbFindCompareFieldValues<ValueType> Create(
		string fieldPath,
		EDF_EDbFindOperator comparisonOperator,
		notnull array<ValueType> comparisonValues,
		bool usesTypename,
		bool stringsInvariant = false)
	{
		if (comparisonValues.IsEmpty())
		{
			Debug.Error("Db find condition comparison values can not be empty!");
			return null;
		}

		EDF_DbFindCompareFieldValues<ValueType> inst();
		inst.m_sFieldPath = fieldPath;
		inst.m_eComparisonOperator = comparisonOperator;
		inst.m_aComparisonValues = comparisonValues;
		inst.m_bUsesTypename = usesTypename;
		inst.m_bStringsInvariant = stringsInvariant;

		if (!ALLOC_BUFFER_TVALUES) ALLOC_BUFFER_TVALUES = {null};
		ALLOC_BUFFER_TVALUES.Set(0, inst);
		return inst;
	}
};

typedef EDF_DbFindCompareFieldValues<int> EDF_DbFindFieldIntMultiple;
typedef EDF_DbFindCompareFieldValues<float> EDF_DbFindFieldFloatMultiple;
typedef EDF_DbFindCompareFieldValues<bool> EDF_DbFindFieldBoolMultiple;
typedef EDF_DbFindCompareFieldValues<string> EDF_DbFindFieldStringMultiple;
typedef EDF_DbFindCompareFieldValues<vector> EDF_DbFindFieldVectorMultiple;
typedef EDF_DbFindCompareFieldValues<typename> EDF_DbFindFieldTypenameMultiple;

class EDF_DbValues<Class T>
{
	protected static ref array<ref array<T>> ALLOC_BUFFER;

	//------------------------------------------------------------------------------------------------
	static array<T> From(notnull array<T> values)
	{
		auto data = new array<T>();
		data.Reserve(values.Count());

		foreach (T value : values)
		{
			data.Insert(value);
		}

		if (!ALLOC_BUFFER) ALLOC_BUFFER = {null};
		ALLOC_BUFFER.Set(0, data);
		return data;
	}
};

class EDF_DbFindFieldAnnotations
{
	const string SEPERATOR = ".";
	const string ANY = ":any";
	const string ALL = ":all";
	const string LENGTH = ":length"; 	// for string length
	const string COUNT = ":count"; 		// for arrays/maps
	const string KEYS = ":keys";		// for maps
	const string VALUES = ":values";	// for maps
};

class EDF_DbFindFieldConditionBuilder
{
	string m_sFieldPath;
	bool m_bInverted;
	bool m_bStringsInvariant;
	bool m_bUsesTypenames;

	//------------------------------------------------------------------------------------------------
	//! TODO: This can be in-lined if we continue to not need any additional processing here
	protected void _AppendModifier(string pathValue)
	{
		m_sFieldPath += pathValue;
	}

	//------------------------------------------------------------------------------------------------
	protected EDF_EDbFindOperator _InvertIfNeeded(EDF_EDbFindOperator operator)
	{
		if (!m_bInverted)
			return operator;

		switch (operator)
		{
			case EDF_EDbFindOperator.EQUAL:
				return EDF_EDbFindOperator.NOT_EQUAL;

			case EDF_EDbFindOperator.NOT_EQUAL:
				return EDF_EDbFindOperator.EQUAL;

			case EDF_EDbFindOperator.ARRAY_EQUAL:
				return EDF_EDbFindOperator.ARRAY_NOT_EQUAL;

			case EDF_EDbFindOperator.ARRAY_NOT_EQUAL:
				return EDF_EDbFindOperator.ARRAY_EQUAL;

			case EDF_EDbFindOperator.LESS_THAN:
				return EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL;

			case EDF_EDbFindOperator.LESS_THAN_OR_EQUAL:
				return EDF_EDbFindOperator.GREATER_THAN;

			case EDF_EDbFindOperator.GREATER_THAN:
				return EDF_EDbFindOperator.LESS_THAN_OR_EQUAL;

			case EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL:
				return EDF_EDbFindOperator.LESS_THAN;

			case EDF_EDbFindOperator.CONTAINS:
				return EDF_EDbFindOperator.NOT_CONTAINS;

			case EDF_EDbFindOperator.NOT_CONTAINS:
				return EDF_EDbFindOperator.CONTAINS;

			case EDF_EDbFindOperator.CONTAINS_ALL:
				return EDF_EDbFindOperator.NOT_CONTAINS_ALL;

			case EDF_EDbFindOperator.NOT_CONTAINS_ALL:
				return EDF_EDbFindOperator.CONTAINS_ALL;
		}

		return -1;
	}
};

class EDF_DbFindFieldNumericValueConditonBuilder : EDF_DbFindFieldConditionBuilder
{
	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(int comparisonValue)
	{
		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(float comparisonValue)
	{
		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition EqualsAnyOf(notnull array<int> comparisonValues)
	{
		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition EqualsAnyOf(notnull array<float> comparisonValues)
	{
		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition LessThan(int comparisonValue)
	{
		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.LESS_THAN), {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition LessThan(float comparisonValue)
	{
		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.LESS_THAN), {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition LessThanOrEquals(int comparisonValue)
	{
		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.LESS_THAN_OR_EQUAL), {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition LessThanOrEquals(float comparisonValue)
	{
		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.LESS_THAN_OR_EQUAL), {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThan(int comparisonValue)
	{
		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.GREATER_THAN), {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThan(float comparisonValue)
	{
		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.GREATER_THAN), {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThanOrEquals(int comparisonValue)
	{
		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL), {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThanOrEquals(float comparisonValue)
	{
		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL), {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Between(int lowerBound, int upperBound)
	{
		if (m_bInverted)
		{
			return EDF_DbFindOr.Create({
					EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN, {lowerBound}, m_bUsesTypenames),
					EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN, {upperBound}, m_bUsesTypenames)
			});
		}

		return EDF_DbFindAnd.Create({
				EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL, {lowerBound}, m_bUsesTypenames),
				EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN_OR_EQUAL, {upperBound}, m_bUsesTypenames)
		});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Between(float lowerBound, float upperBound)
	{
		if (m_bInverted)
		{
			return EDF_DbFindOr.Create({
					EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN, {lowerBound}, m_bUsesTypenames),
					EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN, {upperBound}, m_bUsesTypenames)
			});
		}

		return EDF_DbFindAnd.Create({
				EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL, {lowerBound}, m_bUsesTypenames),
				EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN_OR_EQUAL, {upperBound}, m_bUsesTypenames)
		});
	}
};

class EDF_DbFindFieldPrimitiveValueConditonBuilder : EDF_DbFindFieldNumericValueConditonBuilder
{
	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(bool comparisonValue)
	{
		return EDF_DbFindFieldBoolMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(string comparisonValue)
	{
		return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), {comparisonValue}, m_bUsesTypenames, m_bStringsInvariant);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(vector comparisonValue)
	{
		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition EqualsAnyOf(notnull array<string> comparisonValues)
	{
		return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), comparisonValues, m_bUsesTypenames, m_bStringsInvariant);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition EqualsAnyOf(notnull array<vector> comparisonValues)
	{
		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThan(vector comparisonValue)
	{
		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.GREATER_THAN), {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThanOrEquals(vector comparisonValue)
	{
		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL), {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Contains(string comparisonValue)
	{
		return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), {comparisonValue}, m_bUsesTypenames, m_bStringsInvariant);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<string> comparisonValues)
	{
		return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), comparisonValues, m_bUsesTypenames, m_bStringsInvariant);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<string> comparisonValues)
	{
		return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS_ALL), comparisonValues, m_bUsesTypenames, m_bStringsInvariant);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldNumericValueConditonBuilder Length()
	{
		_AppendModifier(EDF_DbFindFieldAnnotations.LENGTH);
		return this;
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldCollectionHandlingBuilder Invariant()
	{
		m_bStringsInvariant = true;
		return EDF_DbFindFieldCollectionHandlingBuilder.Cast(this);
	}
};

class EDF_DbFindFieldAllValueConditonBuilder : EDF_DbFindFieldPrimitiveValueConditonBuilder
{
	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition NullOrDefault()
	{
		return EDF_DbFindCheckFieldNullOrDefault.Create(m_sFieldPath, !m_bInverted, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(typename comparisonValue)
	{
		return EDF_DbFindFieldTypenameMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), {comparisonValue}, true);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition EqualsAnyOf(notnull array<typename> comparisonValues)
	{
		return EDF_DbFindFieldTypenameMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), comparisonValues, true);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<int> comparisonValues)
	{
		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.ARRAY_EQUAL), comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<float> comparisonValues)
	{
		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.ARRAY_EQUAL), comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<bool> comparisonValues)
	{
		return EDF_DbFindFieldBoolMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.ARRAY_EQUAL), comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<string> comparisonValues)
	{
		return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.ARRAY_EQUAL), comparisonValues, m_bUsesTypenames, m_bStringsInvariant);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<vector> comparisonValues)
	{
		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.ARRAY_EQUAL), comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<typename> comparisonValues)
	{
		return EDF_DbFindFieldTypenameMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.ARRAY_EQUAL), comparisonValues, true);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Contains(int comparisonValue)
	{
		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Contains(float comparisonValue)
	{
		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Contains(bool comparisonValue)
	{
		return EDF_DbFindFieldBoolMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Contains(vector comparisonValue)
	{
		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Contains(typename comparisonValue)
	{
		return EDF_DbFindFieldTypenameMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), {comparisonValue}, true);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<int> comparisonValues)
	{
		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<float> comparisonValues)
	{
		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<bool> comparisonValues)
	{
		return EDF_DbFindFieldBoolMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<vector> comparisonValues)
	{
		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<typename> comparisonValues)
	{
		return EDF_DbFindFieldTypenameMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), comparisonValues, true);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<int> comparisonValues)
	{
		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS_ALL), comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<float> comparisonValues)
	{
		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS_ALL), comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<bool> comparisonValues)
	{
		return EDF_DbFindFieldBoolMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS_ALL), comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<vector> comparisonValues)
	{
		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS_ALL), comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<typename> comparisonValues)
	{
		return EDF_DbFindFieldTypenameMultiple.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS_ALL), comparisonValues, true);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldNumericValueConditonBuilder Count()
	{
		_AppendModifier(EDF_DbFindFieldAnnotations.COUNT);
		return this;
	}
};

class EDF_DbFindFieldMainConditionBuilder : EDF_DbFindFieldAllValueConditonBuilder
{
	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldCollectionHandlingBuilder Field(string fieldPath)
	{
		if (!m_sFieldPath.IsEmpty())
			m_sFieldPath += EDF_DbFindFieldAnnotations.SEPERATOR;

		m_sFieldPath += fieldPath;

		// Check if user has manually put any typenames in the path for filtering
		if (!m_bUsesTypenames)
		{
			array<string> pathSegments();
			fieldPath.Split(EDF_DbFindFieldAnnotations.SEPERATOR, pathSegments, true);
			int count = pathSegments.Count();
			foreach (int idx, string pathSegment : pathSegments)
			{
				string typeString;
				int modifierIdx = pathSegment.IndexOf(":");
				if (modifierIdx != -1)
				{
					typeString = pathSegment.Substring(0, modifierIdx);
				}
				else
				{
					typeString = pathSegment;
				}

				if (typeString.ToType())
				{
					m_bUsesTypenames = true;
					break;
				}
			}
		}

		return EDF_DbFindFieldCollectionHandlingBuilder.Cast(this);
	}
};

class EDF_DbFindFieldInvertableConditionBuilder : EDF_DbFindFieldMainConditionBuilder
{
	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldAllValueConditonBuilder Not()
	{
		m_bInverted = true;
		return this;
	}
};

class EDF_DbFindFieldBasicCollectionHandlingBuilder : EDF_DbFindFieldInvertableConditionBuilder
{
	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldCollectionHandlingBuilder Any()
	{
		_AppendModifier(EDF_DbFindFieldAnnotations.ANY);
		return EDF_DbFindFieldCollectionHandlingBuilder.Cast(this);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldCollectionHandlingBuilder All()
	{
		_AppendModifier(EDF_DbFindFieldAnnotations.ALL);
		return EDF_DbFindFieldCollectionHandlingBuilder.Cast(this);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldCollectionHandlingBuilder At(int index)
	{
		return Field(index.ToString());
	}
};

class EDF_DbFindFieldCollectionHandlingBuilder : EDF_DbFindFieldBasicCollectionHandlingBuilder
{
	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldCollectionHandlingBuilder OfType(typename type)
	{
		m_bUsesTypenames = true;
		return Field(type.ToString());
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldBasicCollectionHandlingBuilder Keys()
	{
		_AppendModifier(EDF_DbFindFieldAnnotations.KEYS);
		return this;
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldBasicCollectionHandlingBuilder Values()
	{
		_AppendModifier(EDF_DbFindFieldAnnotations.VALUES);
		return this;
	}
};

class EDF_DbFindFieldFinalBuilder
{
	protected static ref array<ref EDF_DbFindFieldCollectionHandlingBuilder>> ALLOC_BUFFER;

	//------------------------------------------------------------------------------------------------
	static EDF_DbFindFieldCollectionHandlingBuilder Create()
	{
		auto inst = new EDF_DbFindFieldCollectionHandlingBuilder();
		if (!ALLOC_BUFFER) ALLOC_BUFFER = {null};
		ALLOC_BUFFER.Set(0, inst);
		return inst;
	}
};
