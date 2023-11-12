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
}

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
}

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
}

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
		saveContext.WriteValue("_type", "DbFindAnd");
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
}

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
		saveContext.WriteValue("_type", "DbFindOr");
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
}

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
}

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
		saveContext.WriteValue("_type", "DbFindCheckFieldNullOrDefault");
		SerializationWritePath(saveContext);
		saveContext.WriteValue("shouldBeNullOrDefault", m_ShouldBeNullOrDefault);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static EDF_DbFindCheckFieldNullOrDefault Create(string fieldPath, bool shouldBeNullOrDefault)
	{
		EDF_DbFindCheckFieldNullOrDefault inst();
		inst.m_sFieldPath = fieldPath;
		inst.m_ShouldBeNullOrDefault = shouldBeNullOrDefault;

		if (!ALLOC_BUFFER) ALLOC_BUFFER = {null};
		ALLOC_BUFFER.Set(0, inst);
		return inst;
	}
}

enum EDF_EDbFindOperator
{
	EQUAL,
	NOT_EQUAL,
	LESS_THAN,
	LESS_THAN_OR_EQUAL,
	GREATER_THAN,
	GREATER_THAN_OR_EQUAL,
	CONTAINS,
	NOT_CONTAINS,
	CONTAINS_ALL,
	NOT_CONTAINS_ALL
}

class EDF_DbFindCompareFieldValues<Class ValueType> : EDF_DbFindFieldCondition
{
	private static ref array<ref EDF_DbFindCompareFieldValues<ValueType>> ALLOC_BUFFER_TVALUES;

	EDF_EDbFindOperator m_eComparisonOperator;
	ref array<ValueType> m_aComparisonValues;
	ref array<ref ValueType> m_aComparisonValuesReferences;
	bool m_bStringsInvariant;
	bool m_bStringsPartialMatches;

	//------------------------------------------------------------------------------------------------
	override protected string GetDebugString()
	{
		string resultString = "{";
		foreach (int idx, ValueType value : m_aComparisonValues)
		{
			if (idx != 0)
				resultString += ",";

			if (idx > 10)
			{
				resultString += "...";
				break;
			}

			resultString += ValueToString(value);
		}
		resultString += "}";

		resultString = string.Format("Compare(fieldPath:'%1', operator:%2, values:%3", m_sFieldPath, typename.EnumToString(EDF_EDbFindOperator, m_eComparisonOperator), resultString);

		if (m_bStringsInvariant)
			resultString += ", invariant: true";

		if (m_bStringsPartialMatches)
			resultString += ", partialMatches: true";

		return resultString + ")";
	}

	//------------------------------------------------------------------------------------------------
	protected string ValueToString(int value)
	{
		return value.ToString();
	}

	//------------------------------------------------------------------------------------------------
	protected string ValueToString(float value)
	{
		return value.ToString();
	}

	//------------------------------------------------------------------------------------------------
	protected string ValueToString(bool value)
	{
		return value.ToString();
	}

	//------------------------------------------------------------------------------------------------
	protected string ValueToString(string value)
	{
		return string.ToString(value, false, false, true);
	}

	//------------------------------------------------------------------------------------------------
	protected string ValueToString(vector value)
	{
		return value.ToString();
	}

	//------------------------------------------------------------------------------------------------
	protected string ValueToString(typename value)
	{
		return value.ToString();
	}

	//------------------------------------------------------------------------------------------------
	protected string ValueToString(array<int> values)
	{
		string resultString = "{";
		foreach (int idx, int value : values)
		{
			if (idx != 0)
				resultString += ",";

			if (idx > 10)
			{
				resultString += "...";
				break;
			}

			resultString += ValueToString(value);
		}
		resultString += "}";
		return resultString;
	}

	//------------------------------------------------------------------------------------------------
	protected string ValueToString(array<float> values)
	{
		string resultString = "{";
		foreach (int idx, float value : values)
		{
			if (idx != 0)
				resultString += ",";

			if (idx > 10)
			{
				resultString += "...";
				break;
			}

			resultString += ValueToString(value);
		}
		resultString += "}";
		return resultString;
	}

	//------------------------------------------------------------------------------------------------
	protected string ValueToString(array<bool> values)
	{
		string resultString = "{";
		foreach (int idx, bool value : values)
		{
			if (idx != 0)
				resultString += ",";

			if (idx > 10)
			{
				resultString += "...";
				break;
			}

			resultString += ValueToString(value);
		}
		resultString += "}";
		return resultString;
	}

	//------------------------------------------------------------------------------------------------
	protected string ValueToString(array<string> values)
	{
		string resultString = "{";
		foreach (int idx, string value : values)
		{
			if (idx != 0)
				resultString += ",";

			if (idx > 10)
			{
				resultString += "...";
				break;
			}

			resultString += ValueToString(value);
		}
		resultString += "}";
		return resultString;
	}

	//------------------------------------------------------------------------------------------------
	protected string ValueToString(array<vector> values)
	{
		string resultString = "{";
		foreach (int idx, vector value : values)
		{
			if (idx != 0)
				resultString += ",";

			if (idx > 10)
			{
				resultString += "...";
				break;
			}

			resultString += ValueToString(value);
		}
		resultString += "}";
		return resultString;
	}

	//------------------------------------------------------------------------------------------------
	protected string ValueToString(array<typename> values)
	{
		string resultString = "{";
		foreach (int idx, typename value : values)
		{
			if (idx != 0)
				resultString += ",";

			if (idx > 10)
			{
				resultString += "...";
				break;
			}

			resultString += ValueToString(value);
		}
		resultString += "}";
		return resultString;
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		typename valueType = ValueType;
		saveContext.WriteValue("_type", "DbFindCompareFieldValues<" + valueType + ">");
		SerializationWritePath(saveContext);

		if (m_bStringsInvariant)
			saveContext.WriteValue("stringsInvariant", true);

		if (m_bStringsPartialMatches)
			saveContext.WriteValue("stringsPartialMatches", true);

		saveContext.WriteValue("comparisonOperator", m_eComparisonOperator);

		if (valueType.ToString() == "typename")
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
		else if (valueType.ToString() == "array<typename>")
		{
			array<ref array<string>> comparisonValues();
			GetGame().GetScriptModule().Call(this, "SerializeTypenameArray", false, null, m_aComparisonValues, comparisonValues);
			saveContext.WriteValue("comparisonValues", comparisonValues);
		}
		else if (m_aComparisonValuesReferences) // TODO: Remove once https://feedback.bistudio.com/T173900 is fixed.
		{
			saveContext.WriteValue("comparisonValues", m_aComparisonValuesReferences);
		}
		else
		{
			saveContext.WriteValue("comparisonValues", m_aComparisonValues);
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	/*protected --Hotfix for 1.0, do not call directly please.*/
	void SerializeTypenameArray(Managed values, out array<ref array<string>> comparisonValues)
	{
		array<array<typename>> typedArray = array<array<typename>>.Cast(values);
		comparisonValues.Reserve(typedArray.Count());
		foreach (array<typename> typenameArray : typedArray)
		{
			array<string> types();
			types.Reserve(typenameArray.Count());
			foreach (typename value : typenameArray)
			{
				types.Insert(EDF_DbName.Get(value));
			}
			comparisonValues.Insert(types);
		}
	}

	//------------------------------------------------------------------------------------------------
	/*protected --Hotfix for 1.0, do not call directly please.*/
	void AddReference(ValueType value)
	{
		m_aComparisonValuesReferences.Insert(value);
	}

	//------------------------------------------------------------------------------------------------
	static typename GetValueType()
	{
		return ValueType;
	}

	//------------------------------------------------------------------------------------------------
	static EDF_DbFindCompareFieldValues<ValueType> Create(
		string fieldPath,
		EDF_EDbFindOperator comparisonOperator,
		notnull array<ValueType> comparisonValues,
		bool stringsInvariant = false,
		bool stringsPartialMatches = false)
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
		inst.m_bStringsInvariant = stringsInvariant;
		inst.m_bStringsPartialMatches = stringsPartialMatches;

		// TODO: Remove once https://feedback.bistudio.com/T172530 is added and we can use ref in template param directly instead.
		typename valueType = ValueType;
		if (valueType.IsInherited(array))
		{
			inst.m_aComparisonValuesReferences = {};
			inst.m_aComparisonValuesReferences.Reserve(comparisonValues.Count());
			ScriptModule scriptModule = GetGame().GetScriptModule();
			foreach (ValueType value : comparisonValues)
			{
				scriptModule.Call(inst, "AddReference", false, null, value);
			}
		}

		if (!ALLOC_BUFFER_TVALUES) ALLOC_BUFFER_TVALUES = {null};
		ALLOC_BUFFER_TVALUES.Set(0, inst);
		return inst;
	}
}

typedef EDF_DbFindCompareFieldValues<int> EDF_DbFindFieldInt;
typedef EDF_DbFindCompareFieldValues<float> EDF_DbFindFieldFloat;
typedef EDF_DbFindCompareFieldValues<bool> EDF_DbFindFieldBool;
typedef EDF_DbFindCompareFieldValues<string> EDF_DbFindFieldString;
typedef EDF_DbFindCompareFieldValues<vector> EDF_DbFindFieldVector;
typedef EDF_DbFindCompareFieldValues<typename> EDF_DbFindFieldTypename;
typedef EDF_DbFindCompareFieldValues<array<int>> EDF_DbFindFieldIntArray;
typedef EDF_DbFindCompareFieldValues<array<float>> EDF_DbFindFieldFloatArray;
typedef EDF_DbFindCompareFieldValues<array<bool>> EDF_DbFindFieldBoolArray;
typedef EDF_DbFindCompareFieldValues<array<string>> EDF_DbFindFieldStringArray;
typedef EDF_DbFindCompareFieldValues<array<vector>> EDF_DbFindFieldVectorArray;
typedef EDF_DbFindCompareFieldValues<array<typename>> EDF_DbFindFieldTypenameArray;

class EDF_DbFindFieldAnnotations
{
	const string SEPERATOR = ".";
	const string ANY = ":any";
	const string ALL = ":all";
	const string LENGTH = ":length"; 	// for string length
	const string COUNT = ":count"; 		// for arrays/maps
	const string KEYS = ":keys";		// for maps
	const string VALUES = ":values";	// for maps
}

class EDF_DbFindFieldConditionBuilder
{
	string m_sFieldPath;
	bool m_bInverted;
	bool m_bStringsInvariant;
	bool m_bStringsPartialMatches;

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
}

class EDF_DbFindFieldNumericValueConditonBuilder : EDF_DbFindFieldConditionBuilder
{
	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(int comparisonValue)
	{
		return EDF_DbFindFieldInt.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(float comparisonValue)
	{
		return EDF_DbFindFieldFloat.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition EqualsAnyOf(notnull array<int> comparisonValues)
	{
		return EDF_DbFindFieldInt.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition EqualsAnyOf(notnull array<float> comparisonValues)
	{
		return EDF_DbFindFieldFloat.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition LessThan(int comparisonValue)
	{
		return EDF_DbFindFieldInt.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.LESS_THAN), {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition LessThan(float comparisonValue)
	{
		return EDF_DbFindFieldFloat.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.LESS_THAN), {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition LessThanOrEquals(int comparisonValue)
	{
		return EDF_DbFindFieldInt.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.LESS_THAN_OR_EQUAL), {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition LessThanOrEquals(float comparisonValue)
	{
		return EDF_DbFindFieldFloat.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.LESS_THAN_OR_EQUAL), {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThan(int comparisonValue)
	{
		return EDF_DbFindFieldInt.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.GREATER_THAN), {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThan(float comparisonValue)
	{
		return EDF_DbFindFieldFloat.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.GREATER_THAN), {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThanOrEquals(int comparisonValue)
	{
		return EDF_DbFindFieldInt.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL), {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThanOrEquals(float comparisonValue)
	{
		return EDF_DbFindFieldFloat.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL), {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Between(int lowerBound, int upperBound)
	{
		if (m_bInverted)
		{
			return EDF_DbFindOr.Create({
					EDF_DbFindFieldInt.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN, {lowerBound}),
					EDF_DbFindFieldInt.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN, {upperBound})
			});
		}

		return EDF_DbFindAnd.Create({
				EDF_DbFindFieldInt.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL, {lowerBound}),
				EDF_DbFindFieldInt.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN_OR_EQUAL, {upperBound})
		});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Between(float lowerBound, float upperBound)
	{
		if (m_bInverted)
		{
			return EDF_DbFindOr.Create({
					EDF_DbFindFieldFloat.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN, {lowerBound}),
					EDF_DbFindFieldFloat.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN, {upperBound})
			});
		}

		return EDF_DbFindAnd.Create({
				EDF_DbFindFieldFloat.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL, {lowerBound}),
				EDF_DbFindFieldFloat.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN_OR_EQUAL, {upperBound})
		});
	}
}

class EDF_DbFindFieldPrimitiveValueConditonBuilder : EDF_DbFindFieldNumericValueConditonBuilder
{
	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(bool comparisonValue)
	{
		return EDF_DbFindFieldBool.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(string comparisonValue)
	{
		return EDF_DbFindFieldString.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), {comparisonValue}, m_bStringsInvariant, m_bStringsPartialMatches);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(vector comparisonValue)
	{
		return EDF_DbFindFieldVector.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition EqualsAnyOf(notnull array<string> comparisonValues)
	{
		return EDF_DbFindFieldString.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), comparisonValues, m_bStringsInvariant, m_bStringsPartialMatches);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition EqualsAnyOf(notnull array<vector> comparisonValues)
	{
		return EDF_DbFindFieldVector.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThan(vector comparisonValue)
	{
		return EDF_DbFindFieldVector.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.GREATER_THAN), {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThanOrEquals(vector comparisonValue)
	{
		return EDF_DbFindFieldVector.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL), {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Contains(string comparisonValue)
	{
		return EDF_DbFindFieldString.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), {comparisonValue}, m_bStringsInvariant, m_bStringsPartialMatches);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<string> comparisonValues)
	{
		return EDF_DbFindFieldString.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), comparisonValues, m_bStringsInvariant, m_bStringsPartialMatches);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<string> comparisonValues)
	{
		return EDF_DbFindFieldString.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS_ALL), comparisonValues, m_bStringsInvariant, m_bStringsPartialMatches);
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

	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldCollectionHandlingBuilder Partial()
	{
		m_bStringsPartialMatches = true;
		return EDF_DbFindFieldCollectionHandlingBuilder.Cast(this);
	}
}

class EDF_DbFindFieldAllValueConditonBuilder : EDF_DbFindFieldPrimitiveValueConditonBuilder
{
	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition NullOrDefault()
	{
		return EDF_DbFindCheckFieldNullOrDefault.Create(m_sFieldPath, !m_bInverted);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(typename comparisonValue)
	{
		return EDF_DbFindFieldTypename.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition EqualsAnyOf(notnull array<typename> comparisonValues)
	{
		return EDF_DbFindFieldTypename.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<int> comparisonValues)
	{
		return EDF_DbFindFieldIntArray.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), {comparisonValues});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<float> comparisonValues)
	{
		return EDF_DbFindFieldFloatArray.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), {comparisonValues});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<bool> comparisonValues)
	{
		return EDF_DbFindFieldBoolArray.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), {comparisonValues});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<string> comparisonValues)
	{
		return EDF_DbFindFieldStringArray.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), {comparisonValues}, m_bStringsInvariant, m_bStringsPartialMatches);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<vector> comparisonValues)
	{
		return EDF_DbFindFieldVectorArray.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), {comparisonValues});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<typename> comparisonValues)
	{
		return EDF_DbFindFieldTypenameArray.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.EQUAL), {comparisonValues});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Contains(int comparisonValue)
	{
		return EDF_DbFindFieldInt.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Contains(float comparisonValue)
	{
		return EDF_DbFindFieldFloat.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Contains(bool comparisonValue)
	{
		return EDF_DbFindFieldBool.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Contains(vector comparisonValue)
	{
		return EDF_DbFindFieldVector.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Contains(typename comparisonValue)
	{
		return EDF_DbFindFieldTypename.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<int> comparisonValues)
	{
		return EDF_DbFindFieldInt.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<float> comparisonValues)
	{
		return EDF_DbFindFieldFloat.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<bool> comparisonValues)
	{
		return EDF_DbFindFieldBool.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<vector> comparisonValues)
	{
		return EDF_DbFindFieldVector.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<typename> comparisonValues)
	{
		return EDF_DbFindFieldTypename.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS), comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<int> comparisonValues)
	{
		return EDF_DbFindFieldInt.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS_ALL), comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<float> comparisonValues)
	{
		return EDF_DbFindFieldFloat.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS_ALL), comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<bool> comparisonValues)
	{
		return EDF_DbFindFieldBool.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS_ALL), comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<vector> comparisonValues)
	{
		return EDF_DbFindFieldVector.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS_ALL), comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<typename> comparisonValues)
	{
		return EDF_DbFindFieldTypename.Create(m_sFieldPath, _InvertIfNeeded(EDF_EDbFindOperator.CONTAINS_ALL), comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldNumericValueConditonBuilder Count()
	{
		_AppendModifier(EDF_DbFindFieldAnnotations.COUNT);
		return this;
	}
}

class EDF_DbFindFieldMainConditionBuilder : EDF_DbFindFieldAllValueConditonBuilder
{
	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldCollectionHandlingBuilder Field(string fieldPath)
	{
		array<string> pathSegments();
		fieldPath.Split(EDF_DbFindFieldAnnotations.SEPERATOR, pathSegments, true);
		foreach (int idx, string pathSegment : pathSegments)
		{
			if ((idx == 0 && !m_sFieldPath.IsEmpty()) || idx > 0)
				m_sFieldPath += EDF_DbFindFieldAnnotations.SEPERATOR;

			string pathString, modifierString;
			int modifierIdx = pathSegment.IndexOf(":");
			if (modifierIdx != -1)
			{
				pathString = pathSegment.Substring(0, modifierIdx);
				modifierString = pathSegment.Substring(modifierIdx, pathSegment.Length() - modifierIdx);
			}
			else
			{
				pathString = pathSegment;
			}

			if (pathString)
			{
				int asIntValue = pathString.ToInt();
				if (asIntValue != 0 || pathString == "0")
				{
					m_sFieldPath += string.Format("{%1}", asIntValue);
				}
				else
				{
					typename type = pathString.ToType();
					if (type)
					{
						m_sFieldPath += string.Format("{%1}", EDF_DbName.Get(type));
					}
					else
					{
						m_sFieldPath += pathString;
					}
				}
			}

			m_sFieldPath += modifierString;
		}

		return EDF_DbFindFieldCollectionHandlingBuilder.Cast(this);
	}
}

class EDF_DbFindFieldInvertableConditionBuilder : EDF_DbFindFieldMainConditionBuilder
{
	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldAllValueConditonBuilder Not()
	{
		m_bInverted = true;
		return this;
	}
}

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
		return Field(string.Format("{%1}", index));
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldCollectionHandlingBuilder At(array<int> indices)
	{
		string indicesString = "{";
		foreach (int idx, int index : indices)
		{
			if (idx > 0)
				indicesString += ",";

			indicesString += index.ToString();
		}
		return Field(indicesString + "}");
	}
}

class EDF_DbFindFieldCollectionHandlingBuilder : EDF_DbFindFieldBasicCollectionHandlingBuilder
{
	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldCollectionHandlingBuilder OfType(typename type)
	{
		if (type == Class || type == Managed)
		{
			Debug.Error("Ilegal OfType() argument Class/Managed. Change it to a more specific type!");
			return null;
		}

		return Field(string.Format("{%1}", EDF_DbName.Get(type)));
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldCollectionHandlingBuilder OfTypes(array<typename> types)
	{
		string typesString = "{";
		foreach (int idx, typename type : types)
		{
			if (idx > 0)
				typesString += ",";

			if (type == Class || type == Managed)
			{
				Debug.Error("Ilegal OfTypes() argument Class/Managed. Change it to a more specific type!");
				return null;
			}

			typesString += EDF_DbName.Get(type);
		}
		return Field(typesString + "}");
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldBasicCollectionHandlingBuilder Keys()
	{
		_AppendModifier(EDF_DbFindFieldAnnotations.KEYS);
		return this;
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldCollectionHandlingBuilder Values()
	{
		_AppendModifier(EDF_DbFindFieldAnnotations.VALUES);
		return this;
	}
}

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
}
