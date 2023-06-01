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
		return EDF_DbFindFieldCollectionHandlingBuilder.Create(fieldPath);
	}

	//------------------------------------------------------------------------------------------------
	static EDF_DbFindFieldInvertableConditionBuilder Id()
	{
		// Return with only primitive field builder options (implicit cast)
		return EDF_DbFindFieldCollectionHandlingBuilder.Create(EDF_DbEntity.FIELD_ID);
	}
};

class EDF_DbFindCondition
{
	protected static ref array<ref EDF_DbFindCondition> ALLOC_BUFFER;

	//------------------------------------------------------------------------------------------------
	/*sealed*/ void Debug()
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
			foreach (EDF_DbFindCondition childCondition : conditionWithChildren.m_Conditions)
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
	ref array<ref EDF_DbFindCondition> m_Conditions;

	//------------------------------------------------------------------------------------------------
	override protected string GetDebugString()
	{
		if (!m_Conditions) return "null";

		string dbg = "(";

		foreach (int nCondtion, EDF_DbFindCondition condition : m_Conditions)
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

			if (nCondtion == m_Conditions.Count() - 1)
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
		m_Conditions = conditions;
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
		saveContext.WriteValue("$type", "And");
		saveContext.WriteValue("conditions", m_Conditions);
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
		saveContext.WriteValue("$type", "Or");
		saveContext.WriteValue("conditions", m_Conditions);
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
};

class EDF_DbFindCheckFieldNull : EDF_DbFindFieldCondition
{
	bool m_bShouldBeNull;

	//------------------------------------------------------------------------------------------------
	override protected string GetDebugString()
	{
		if (m_bShouldBeNull) return string.Format("CheckNull(fieldPath:'%1', shouldBeNull:true)", m_sFieldPath);

		return string.Format("CheckNull(fieldPath:'%1', shouldBeNull:false)", m_sFieldPath);
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		saveContext.WriteValue("$type", "CheckFieldNull");
		saveContext.WriteValue("fieldPath", m_sFieldPath);
		saveContext.WriteValue("shouldBeNull", m_bShouldBeNull);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static EDF_DbFindCheckFieldNull Create(string fieldPath, bool shouldBeNull)
	{
		auto inst = new EDF_DbFindCheckFieldNull(fieldPath, shouldBeNull);
		if (!ALLOC_BUFFER) ALLOC_BUFFER = {null};
		ALLOC_BUFFER.Set(0, inst);
		return inst;
	}

	//------------------------------------------------------------------------------------------------
	void EDF_DbFindCheckFieldNull(string fieldPath, bool shouldBeNull)
	{
		m_sFieldPath = fieldPath;
		m_bShouldBeNull = shouldBeNull;
	}
};

class EDF_DbFindCheckFieldEmpty : EDF_DbFindFieldCondition
{
	bool m_ShouldBeEmpty;

	//------------------------------------------------------------------------------------------------
	override protected string GetDebugString()
	{
		if (m_ShouldBeEmpty) return string.Format("CheckEmpty(fieldPath:'%1', shouldBeEmpty:true)", m_sFieldPath);

		return string.Format("CheckEmpty(fieldPath:'%1', shouldBeEmpty:false)", m_sFieldPath);
	}

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		saveContext.WriteValue("$type", "CheckFieldEmpty");
		saveContext.WriteValue("fieldPath", m_sFieldPath);
		saveContext.WriteValue("shouldBeEmpty", m_ShouldBeEmpty);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static EDF_DbFindCheckFieldEmpty Create(string fieldPath, bool shouldBeEmpty)
	{
		auto inst = new EDF_DbFindCheckFieldEmpty(fieldPath, shouldBeEmpty);
		if (!ALLOC_BUFFER) ALLOC_BUFFER = {null};
		ALLOC_BUFFER.Set(0, inst);
		return inst;
	}

	//------------------------------------------------------------------------------------------------
	void EDF_DbFindCheckFieldEmpty(string fieldPath, bool shouldBeEmpty)
	{
		m_sFieldPath = fieldPath;
		m_ShouldBeEmpty = shouldBeEmpty;
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
	NOT_CONTAINS
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
		saveContext.WriteValue("$type", "CompareFieldValues<" + ValueType + ">");
		saveContext.WriteValue("fieldPath", m_sFieldPath);
		saveContext.WriteValue("operator", m_eComparisonOperator);
		saveContext.WriteValue("values", m_aComparisonValues);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static EDF_DbFindCompareFieldValues<ValueType> Create(string fieldPath, EDF_EDbFindOperator comparisonOperator, notnull array<ValueType> comparisonValues)
	{
		auto inst = new EDF_DbFindCompareFieldValues<ValueType>(fieldPath, comparisonOperator, comparisonValues);
		if (!ALLOC_BUFFER_TVALUES) ALLOC_BUFFER_TVALUES = {null};
		ALLOC_BUFFER_TVALUES.Set(0, inst);
		return inst;
	}

	//------------------------------------------------------------------------------------------------
	void EDF_DbFindCompareFieldValues(string fieldPath, EDF_EDbFindOperator comparisonOperator, notnull array<ValueType> comparisonValues)
	{
		m_sFieldPath = fieldPath;
		m_eComparisonOperator = comparisonOperator;
		m_aComparisonValues = comparisonValues;
	}
};

typedef EDF_DbFindCompareFieldValues<int> EDF_DbFindFieldIntMultiple;
typedef EDF_DbFindCompareFieldValues<float> EDF_DbFindFieldFloatMultiple;
typedef EDF_DbFindCompareFieldValues<bool> EDF_DbFindFieldBoolMultiple;
typedef EDF_DbFindCompareFieldValues<string> EDF_DbFindFieldStringMultiple;
typedef EDF_DbFindCompareFieldValues<vector> EDF_DbFindFieldVectorMultiple;

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
	const string COUNT = ":count";
	const string KEYS = ":keys";
	const string VALUES = ":values";
};

class EDF_DbFindFieldConditionBuilder
{
	string m_sFieldPath;
	bool m_bInverted;

	//------------------------------------------------------------------------------------------------
	protected void _AppendIfNotPresent(string pathValue)
	{
		if (m_sFieldPath.EndsWith(pathValue)) return;

		m_sFieldPath += pathValue;
	}

	//------------------------------------------------------------------------------------------------
	protected array<string> _ConvertTypenameArray(array<typename> values)
	{
		array<string> valuesString();
		valuesString.Reserve(values.Count());
		foreach (typename type : values)
		{
			valuesString.Insert(EDF_DbName.Get(type));
		}

		return valuesString;
	}

	//------------------------------------------------------------------------------------------------
	void EDF_DbFindFieldConditionBuilder(string fieldPath, bool inverted = false)
	{
		m_sFieldPath = fieldPath;
		m_bInverted = inverted;
	}
};

class EDF_DbFindFieldNumericValueConditonBuilder : EDF_DbFindFieldConditionBuilder
{
	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(int comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, {comparisonValue});

		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(float comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, {comparisonValue});

		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition EqualsAnyOf(notnull array<int> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, comparisonValues);

		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition EqualsAnyOf(notnull array<float> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, comparisonValues);

		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition LessThan(int comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL, {comparisonValue});

		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition LessThan(float comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL, {comparisonValue});

		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition LessThanOrEquals(int comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN, {comparisonValue});

		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN_OR_EQUAL, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition LessThanOrEquals(float comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN, {comparisonValue});

		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN_OR_EQUAL, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThan(int comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN_OR_EQUAL, {comparisonValue});

		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThan(float comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN_OR_EQUAL, {comparisonValue});

		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThanOrEquals(int comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN, {comparisonValue});

		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThanOrEquals(float comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN, {comparisonValue});

		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Between(int lowerBound, int upperBound)
	{
		if (m_bInverted)
		{
			return EDF_DbFindOr.Create({
					EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN, {lowerBound}),
					EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN, {upperBound})
			});
		}

		return EDF_DbFindAnd.Create({
				EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL, {lowerBound}),
				EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN_OR_EQUAL, {upperBound})
		});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Between(float lowerBound, float upperBound)
	{
		if (m_bInverted)
		{
			return EDF_DbFindOr.Create({
					EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN, {lowerBound}),
					EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN, {upperBound})
			});
		}

		return EDF_DbFindAnd.Create({
				EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL, {lowerBound}),
				EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN_OR_EQUAL, {upperBound})
		});
	}
};

class EDF_DbFindFieldPrimitiveValueConditonBuilder : EDF_DbFindFieldNumericValueConditonBuilder
{
	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(bool comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldBoolMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, {comparisonValue});

		return EDF_DbFindFieldBoolMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(string comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, {comparisonValue});

		return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(vector comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, {comparisonValue});

		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition EqualsAnyOf(notnull array<string> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, comparisonValues);

		return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition EqualsAnyOf(notnull array<vector> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, comparisonValues);

		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThan(vector comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN_OR_EQUAL, {comparisonValue});

		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition GreaterThanOrEquals(vector comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.LESS_THAN, {comparisonValue});

		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Contains(string comparisonValue)
	{
		if (m_bInverted) return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, {comparisonValue});

		return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, {comparisonValue});
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<string> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues);

		return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldNumericValueConditonBuilder Length()
	{
		_AppendIfNotPresent(EDF_DbFindFieldAnnotations.COUNT);
		return this;
	}
};

class EDF_DbFindFieldAllValueConditonBuilder : EDF_DbFindFieldPrimitiveValueConditonBuilder
{
	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Null()
	{
		return EDF_DbFindCheckFieldNull.Create(m_sFieldPath, !m_bInverted);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Empty()
	{
		return EDF_DbFindCheckFieldEmpty.Create(m_sFieldPath, !m_bInverted);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(typename comparisonValue)
	{
		return Equals(EDF_DbName.Get(comparisonValue));
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition EqualsAnyOf(notnull array<typename> comparisonValues)
	{
		return EqualsAnyOf(_ConvertTypenameArray(comparisonValues));
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<int> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, comparisonValues);

		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<float> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, comparisonValues);

		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<bool> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldBoolMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, comparisonValues);

		return EDF_DbFindFieldBoolMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<string> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, comparisonValues);

		return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<vector> comparisonValues)
	{
		if (m_bInverted) return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_EQUAL, comparisonValues);

		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.EQUAL, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition Equals(notnull array<typename> comparisonValues)
	{
		return Equals(_ConvertTypenameArray(comparisonValues));
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
		return ContainsAnyOf(EDF_DbValues<string>.From({EDF_DbName.Get(comparisonValue)}));
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<int> comparisonValues)
	{
		_AppendIfNotPresent(EDF_DbFindFieldAnnotations.ANY);

		if (m_bInverted) return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues);

		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<float> comparisonValues)
	{
		_AppendIfNotPresent(EDF_DbFindFieldAnnotations.ANY);

		if (m_bInverted) return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues);

		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<bool> comparisonValues)
	{
		_AppendIfNotPresent(EDF_DbFindFieldAnnotations.ANY);

		if (m_bInverted) return EDF_DbFindFieldBoolMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues);

		return EDF_DbFindFieldBoolMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<vector> comparisonValues)
	{
		_AppendIfNotPresent(EDF_DbFindFieldAnnotations.ANY);

		if (m_bInverted) return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues);

		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAnyOf(notnull array<typename> comparisonValues)
	{
		_AppendIfNotPresent(EDF_DbFindFieldAnnotations.ANY);

		return ContainsAnyOf(_ConvertTypenameArray(comparisonValues));
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<int> comparisonValues)
	{
		_AppendIfNotPresent(EDF_DbFindFieldAnnotations.ALL);

		if (m_bInverted) return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues);

		return EDF_DbFindFieldIntMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<float> comparisonValues)
	{
		_AppendIfNotPresent(EDF_DbFindFieldAnnotations.ALL);

		if (m_bInverted) return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues);

		return EDF_DbFindFieldFloatMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<bool> comparisonValues)
	{
		_AppendIfNotPresent(EDF_DbFindFieldAnnotations.ALL);

		if (m_bInverted) return EDF_DbFindFieldBoolMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues);

		return EDF_DbFindFieldBoolMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<string> comparisonValues)
	{
		_AppendIfNotPresent(EDF_DbFindFieldAnnotations.ALL);

		if (m_bInverted) return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues);

		return EDF_DbFindFieldStringMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<vector> comparisonValues)
	{
		_AppendIfNotPresent(EDF_DbFindFieldAnnotations.ALL);

		if (m_bInverted) return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.NOT_CONTAINS, comparisonValues);

		return EDF_DbFindFieldVectorMultiple.Create(m_sFieldPath, EDF_EDbFindOperator.CONTAINS, comparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindCondition ContainsAllOf(notnull array<typename> comparisonValues)
	{
		return ContainsAllOf(_ConvertTypenameArray(comparisonValues));
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldNumericValueConditonBuilder Count()
	{
		_AppendIfNotPresent(EDF_DbFindFieldAnnotations.COUNT);
		return this;
	}
};

class EDF_DbFindFieldMainConditionBuilder : EDF_DbFindFieldAllValueConditonBuilder
{
	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldCollectionHandlingBuilder Field(string fieldPath)
	{
		m_sFieldPath += EDF_DbFindFieldAnnotations.SEPERATOR + fieldPath;
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
	protected static ref array<ref EDF_DbFindFieldBasicCollectionHandlingBuilder>> ALLOC_BUFFER;

	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldMainConditionBuilder Any()
	{
		_AppendIfNotPresent(EDF_DbFindFieldAnnotations.ANY);
		return this;
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldMainConditionBuilder All()
	{
		_AppendIfNotPresent(EDF_DbFindFieldAnnotations.ALL);
		return this;
	}
};

class EDF_DbFindFieldCollectionHandlingBuilder : EDF_DbFindFieldBasicCollectionHandlingBuilder
{
	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldBasicCollectionHandlingBuilder Keys()
	{
		_AppendIfNotPresent(EDF_DbFindFieldAnnotations.KEYS);
		return this;
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldBasicCollectionHandlingBuilder Values()
	{
		_AppendIfNotPresent(EDF_DbFindFieldAnnotations.VALUES);
		return this;
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldMainConditionBuilder At(int index)
	{
		return Field(index.ToString());
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindFieldBasicCollectionHandlingBuilder OfType(typename type)
	{
		return Field(EDF_DbName.Get(type));
	}

	//------------------------------------------------------------------------------------------------
	static EDF_DbFindFieldCollectionHandlingBuilder Create(string fieldPath)
	{
		auto inst = new EDF_DbFindFieldCollectionHandlingBuilder(fieldPath);
		if (!ALLOC_BUFFER) ALLOC_BUFFER = {null};
		ALLOC_BUFFER.Set(0, inst);
		return inst;
	}
};
