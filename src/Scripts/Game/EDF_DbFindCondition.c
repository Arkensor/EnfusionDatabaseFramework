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

	//------------------------------------------------------------------------------------------------
	//! Collects id field comparision values and returns true if there are no other fields that conditions need to be applied to
	static bool CollectConditionIds(EDF_DbFindCondition condition, out set<string> findIds, out set<string> skipIds)
	{
		EDF_DbFindFieldStringMultiple stringMultipleCondition = EDF_DbFindFieldStringMultiple.Cast(condition);
		if (stringMultipleCondition)
		{
			if (stringMultipleCondition.m_sFieldPath != EDF_DbEntity.FIELD_ID ||
				(stringMultipleCondition.m_eComparisonOperator != EDF_EDbFindOperator.EQUAL &&
				stringMultipleCondition.m_eComparisonOperator != EDF_EDbFindOperator.NOT_EQUAL))
			{
				return false;
			}

			foreach (string id : stringMultipleCondition.m_aComparisonValues)
			{
				if (stringMultipleCondition.m_eComparisonOperator == EDF_EDbFindOperator.EQUAL)
				{
					findIds.Insert(id);
				}
				else
				{
					skipIds.Insert(id);
				}
			}

			return true;
		}

		EDF_DbFindConditionWithChildren conditionWithChildren = EDF_DbFindConditionWithChildren.Cast(condition);
		if (conditionWithChildren)
		{
			bool isComplex = false;
			foreach (EDF_DbFindCondition childCondition : conditionWithChildren.m_aConditions)
			{
				if (!CollectConditionIds(childCondition, findIds, skipIds))
					isComplex = true;
			}

			return !isComplex;
		}

		// TODO: Can be optimized to check if id field is part of AND, and there are no other toplevel ORs, so we can know that only specific ids need to be loaded and then filters applied.
		return false;
	}
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

	//------------------------------------------------------------------------------------------------
	void EDF_DbFindConditionWithChildren(notnull array<ref EDF_DbFindCondition> conditions)
	{
		m_aConditions = conditions;
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
		EDF_DbFindAnd inst = new EDF_DbFindAnd(conditions);
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
		EDF_DbFindOr inst = new EDF_DbFindOr(conditions);
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

class EDF_DbFindCheckFieldNull : EDF_DbFindFieldCondition
{
	bool m_bShouldBeNull;

	//------------------------------------------------------------------------------------------------
	override protected string GetDebugString()
	{
		return string.Format("CheckNull(fieldPath:'%1', shouldBeNull:%2)", m_sFieldPath, m_bShouldBeNull.ToString());
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		saveContext.WriteValue("$type", "DbFindCheckFieldNull");
		SerializationWritePath(saveContext);
		saveContext.WriteValue("shouldBeNull", m_bShouldBeNull);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static EDF_DbFindCheckFieldNull Create(string fieldPath, bool shouldBeNull, bool usesTypename)
	{
		auto inst = new EDF_DbFindCheckFieldNull(fieldPath, shouldBeNull, usesTypename);
		if (!ALLOC_BUFFER) ALLOC_BUFFER = {null};
		ALLOC_BUFFER.Set(0, inst);
		return inst;
	}

	//------------------------------------------------------------------------------------------------
	void EDF_DbFindCheckFieldNull(string fieldPath, bool shouldBeNull, bool usesTypename)
	{
		m_sFieldPath = fieldPath;
		m_bShouldBeNull = shouldBeNull;
		m_bUsesTypename = usesTypename;
	}
};

class EDF_DbFindCheckFieldEmpty : EDF_DbFindFieldCondition
{
	bool m_ShouldBeEmpty;

	//------------------------------------------------------------------------------------------------
	override protected string GetDebugString()
	{
		return string.Format("CheckEmpty(fieldPath:'%1', shouldBeEmpty:%2)", m_sFieldPath, m_ShouldBeEmpty.ToString());
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		saveContext.WriteValue("$type", "DbFindCheckFieldEmpty");
		SerializationWritePath(saveContext);
		saveContext.WriteValue("shouldBeEmpty", m_ShouldBeEmpty);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static EDF_DbFindCheckFieldEmpty Create(string fieldPath, bool shouldBeEmpty, bool usesTypename)
	{
		auto inst = new EDF_DbFindCheckFieldEmpty(fieldPath, shouldBeEmpty, usesTypename);
		if (!ALLOC_BUFFER) ALLOC_BUFFER = {null};
		ALLOC_BUFFER.Set(0, inst);
		return inst;
	}

	//------------------------------------------------------------------------------------------------
	void EDF_DbFindCheckFieldEmpty(string fieldPath, bool shouldBeEmpty, bool usesTypename)
	{
		m_sFieldPath = fieldPath;
		m_ShouldBeEmpty = shouldBeEmpty;
		m_bUsesTypename = usesTypename;
	}
};

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
	ARR_EQUAL,
	ARR_NOT_EQUAL
};

class EDF_DbFindCompareFieldValues<Class ValueType> : EDF_DbFindFieldCondition
{
	private static ref array<ref EDF_DbFindCompareFieldValues<ValueType>> ALLOC_BUFFER_TVALUES;

	EDF_EDbFindOperator m_eComparisonOperator;
	ref array<ValueType> m_aComparisonValues;

	//------------------------------------------------------------------------------------------------
	override protected string GetDebugString()
	{
		string valuesString = "{";
		foreach (int idx, ValueType value : m_aComparisonValues)
		{
			if (idx != 0) valuesString += ",";

			if (idx > 10)
			{
				valuesString += "...";
				break;
			}

			typename valueType = ValueType;

			if (valueType.IsInherited(bool))
			{
				if (value)
				{
					valuesString += "true";
				}
				else
				{
					valuesString += "false";
				}
			}
			else
			{
				valuesString += string.Format("%1", value);
			}
		}
		valuesString += "}";

		return string.Format("Compare(fieldPath:'%1', operator:%2, values:%3)", m_sFieldPath, typename.EnumToString(EDF_EDbFindOperator, m_eComparisonOperator), valuesString);
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

		return true;
	}

	//------------------------------------------------------------------------------------------------
	static EDF_DbFindCompareFieldValues<ValueType> Create(
		string fieldPath,
		EDF_EDbFindOperator comparisonOperator,
		notnull array<ValueType> comparisonValues,
		bool usesTypename)
	{
		auto inst = new EDF_DbFindCompareFieldValues<ValueType>(fieldPath, comparisonOperator, comparisonValues, usesTypename);
		if (!ALLOC_BUFFER_TVALUES) ALLOC_BUFFER_TVALUES = {null};
		ALLOC_BUFFER_TVALUES.Set(0, inst);
		return inst;
	}

	//------------------------------------------------------------------------------------------------
	void EDF_DbFindCompareFieldValues(string fieldPath, EDF_EDbFindOperator comparisonOperator, notnull array<ValueType> comparisonValues, bool usesTypename)
	{
		m_sFieldPath = fieldPath;
		m_eComparisonOperator = comparisonOperator;
		m_aComparisonValues = comparisonValues;
		m_bUsesTypename = usesTypename;
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

typedef EDF_DbValues<int> EDF_DbValuesInt;
typedef EDF_DbValues<float> EDF_DbValuesFloat;
typedef EDF_DbValues<bool> EDF_DbValuesBool;
typedef EDF_DbValues<string> EDF_DbValuesString;
typedef EDF_DbValues<vector> EDF_DbValuesVector;

class EDF_DbFindFieldAnnotations
{
	const string SEPERATOR = ".";
	const string ANY = ":any";
	const string ALL = ":all";
	const string LENGTH = ":length"; // for string length
	const string COUNT = ":count"; // for arrays
	const string KEYS = ":keys";
	const string VALUES = ":values";
};

class EDF_DbFindFieldConditionBuilder
{
	string m_sFieldPath;
	bool m_bInverted;
	bool m_bUsesTypenames;

	//------------------------------------------------------------------------------------------------
	protected void _AppendModifier(string pathValue)
	{
		if (m_sFieldPath.EndsWith(pathValue))
			m_sFieldPath += EDF_DbFindFieldAnnotations.SEPERATOR;

		m_sFieldPath += pathValue;
	}
};

class EDF_DbFindFieldNumericValueConditonBuilder : EDF_DbFindFieldConditionBuilder
{
	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(int comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, {comparisonValue}, m_bUsesTypenames);

		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(float comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, {comparisonValue}, m_bUsesTypenames);

		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition EqualsAnyOf(notnull array<int> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, comparisonValues, m_bUsesTypenames);

		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition EqualsAnyOf(notnull array<float> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, comparisonValues, m_bUsesTypenames);

		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition LessThan(int comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL, {comparisonValue}, m_bUsesTypenames);

		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN, {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition LessThan(float comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL, {comparisonValue}, m_bUsesTypenames);

		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN, {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition LessThanOrEquals(int comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN, {comparisonValue}, m_bUsesTypenames);

		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN_OR_EQUAL, {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition LessThanOrEquals(float comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN, {comparisonValue}, m_bUsesTypenames);

		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN_OR_EQUAL, {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThan(int comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN_OR_EQUAL, {comparisonValue}, m_bUsesTypenames);

		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN, {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThan(float comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN_OR_EQUAL, {comparisonValue}, m_bUsesTypenames);

		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN, {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThanOrEquals(int comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN, {comparisonValue}, m_bUsesTypenames);

		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL, {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThanOrEquals(float comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN, {comparisonValue}, m_bUsesTypenames);

		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL, {comparisonValue}, m_bUsesTypenames);
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
		if (m_bInverted) return EDF_DbFindFieldBoolMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, {comparisonValue}, m_bUsesTypenames);

		return EDF_DbFindFieldBoolMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(string comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, {comparisonValue}, m_bUsesTypenames);

		return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(vector comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, {comparisonValue}, m_bUsesTypenames);

		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition EqualsAnyOf(notnull array<string> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, comparisonValues, m_bUsesTypenames);

		return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition EqualsAnyOf(notnull array<vector> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, comparisonValues, m_bUsesTypenames);

		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThan(vector comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN_OR_EQUAL, {comparisonValue}, m_bUsesTypenames);

		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN, {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThanOrEquals(vector comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN, {comparisonValue}, m_bUsesTypenames);

		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL, {comparisonValue}, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Contains(string comparisonValue)
	{
		return ContainsAnyOf(EDF_DbValues<string>.From({comparisonValue}));
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<string> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues, m_bUsesTypenames);

		return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldNumericValueConditonBuilder Length()
	{
		_AppendModifier(EDF_DbFindFieldAnnotations.LENGTH);
		return this;
	}
};

class EDF_DbFindFieldAllValueConditonBuilder : EDF_DbFindFieldPrimitiveValueConditonBuilder
{
	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Null()
	{
		return EDF_DbFindCheckFieldNull.Create(m_sFieldPath, !m_bInverted, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Empty()
	{
		return EDF_DbFindCheckFieldEmpty.Create(m_sFieldPath, !m_bInverted, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(typename comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldTypenameMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, {comparisonValue}, true);

		return EDF_DbFindFieldTypenameMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, {comparisonValue}, true);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition EqualsAnyOf(notnull array<typename> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldTypenameMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues, true);

		return EDF_DbFindFieldTypenameMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues, true);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<int> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.ARR_NOT_EQUAL, comparisonValues, m_bUsesTypenames);

		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.ARR_EQUAL, comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<float> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.ARR_NOT_EQUAL, comparisonValues, m_bUsesTypenames);

		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.ARR_EQUAL, comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<bool> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldBoolMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.ARR_NOT_EQUAL, comparisonValues, m_bUsesTypenames);

		return EDF_DbFindFieldBoolMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.ARR_EQUAL, comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<string> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.ARR_NOT_EQUAL, comparisonValues, m_bUsesTypenames);

		return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.ARR_EQUAL, comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<vector> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.ARR_NOT_EQUAL, comparisonValues, m_bUsesTypenames);

		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.ARR_EQUAL, comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<typename> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldTypenameMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.ARR_NOT_EQUAL, comparisonValues, true);

		return EDF_DbFindFieldTypenameMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.ARR_EQUAL, comparisonValues, true);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Contains(int comparisonValue)
	{
		return ContainsAnyOf(EDF_DbValues<int>.From({comparisonValue}));
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Contains(float comparisonValue)
	{
		return ContainsAnyOf(EDF_DbValues<float>.From({comparisonValue}));
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Contains(bool comparisonValue)
	{
		return ContainsAnyOf(EDF_DbValues<bool>.From({comparisonValue}));
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Contains(vector comparisonValue)
	{
		return ContainsAnyOf(EDF_DbValues<vector>.From({comparisonValue}));
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Contains(typename comparisonValue)
	{
		return ContainsAnyOf(EDF_DbValues<typename>.From({comparisonValue}));
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<int> comparisonValues)
	{
		_AppendModifier(EDF_DbFindFieldAnnotations.ANY);

		if (m_bInverted) return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues, m_bUsesTypenames);

		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<float> comparisonValues)
	{
		_AppendModifier(EDF_DbFindFieldAnnotations.ANY);

		if (m_bInverted) return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues, m_bUsesTypenames);

		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<bool> comparisonValues)
	{
		_AppendModifier(EDF_DbFindFieldAnnotations.ANY);

		if (m_bInverted) return EDF_DbFindFieldBoolMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues, m_bUsesTypenames);

		return EDF_DbFindFieldBoolMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<vector> comparisonValues)
	{
		_AppendModifier(EDF_DbFindFieldAnnotations.ANY);

		if (m_bInverted) return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues, m_bUsesTypenames);

		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<typename> comparisonValues)
	{
		_AppendModifier(EDF_DbFindFieldAnnotations.ANY);

		if (m_bInverted) return EDF_DbFindFieldTypenameMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues, true);

		return EDF_DbFindFieldTypenameMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues, true);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<int> comparisonValues)
	{
		_AppendModifier(EDF_DbFindFieldAnnotations.ALL);

		if (m_bInverted) return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues, m_bUsesTypenames);

		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<float> comparisonValues)
	{
		_AppendModifier(EDF_DbFindFieldAnnotations.ALL);

		if (m_bInverted) return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues, m_bUsesTypenames);

		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<bool> comparisonValues)
	{
		_AppendModifier(EDF_DbFindFieldAnnotations.ALL);

		if (m_bInverted) return EDF_DbFindFieldBoolMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues, m_bUsesTypenames);

		return EDF_DbFindFieldBoolMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<string> comparisonValues)
	{
		_AppendModifier(EDF_DbFindFieldAnnotations.ALL);

		if (m_bInverted) return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues, m_bUsesTypenames);

		return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<vector> comparisonValues)
	{
		_AppendModifier(EDF_DbFindFieldAnnotations.ALL);

		if (m_bInverted) return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues, m_bUsesTypenames);

		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues, m_bUsesTypenames);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<typename> comparisonValues)
	{
		_AppendModifier(EDF_DbFindFieldAnnotations.ALL);

		if (m_bInverted) return EDF_DbFindFieldTypenameMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues, true);

		return EDF_DbFindFieldTypenameMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues, true);
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
