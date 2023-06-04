enum EDF_DbFindFieldPathSegmentFlags
{
	NUMBER = 1,
	TYPENAME = 2,
	ANY = 4,
	ALL = 8,
	COUNT = 16,
	LENGTH = 32,
	KEYS = 64,
	VALUES = 128
};

class EDF_DbFindFieldPathSegment
{
	string m_sFieldName;
	int m_iFlags;

	//------------------------------------------------------------------------------------------------
	void EDF_DbFindFieldPathSegment(string fieldName, int flags)
	{
		m_sFieldName = fieldName;
		m_iFlags = flags;
	}
};

class EDF_DbFindConditionEvaluator
{
	//------------------------------------------------------------------------------------------------
	static array<ref EDF_DbEntity> GetFiltered(notnull array<ref EDF_DbEntity> entities, notnull EDF_DbFindCondition condition)
	{
		array<ref EDF_DbEntity> conditionMatched();

		foreach (EDF_DbEntity entity : entities)
		{
			if (EDF_DbFindConditionEvaluator.Evaluate(entity, condition))
			{
				conditionMatched.Insert(entity);
			}
		}

		return conditionMatched;
	}

	//------------------------------------------------------------------------------------------------
	static bool Evaluate(notnull EDF_DbEntity entity, notnull EDF_DbFindCondition condition)
	{
		switch (condition.Type())
		{
			case EDF_DbFindAnd:
			{
				foreach (EDF_DbFindCondition checkCondition : EDF_DbFindAnd.Cast(condition).m_aConditions)
				{
					if (!Evaluate(entity, checkCondition))
						return false;
				}

				return true;
			}

			case EDF_DbFindOr:
			{
				foreach (EDF_DbFindCondition checkCondition : EDF_DbFindOr.Cast(condition).m_aConditions)
				{
					if (Evaluate(entity, checkCondition))
						return true;
				}

				return false;
			}

			default:
			{
				EDF_DbFindFieldCondition fieldCondition = EDF_DbFindFieldCondition.Cast(condition);

				if (!fieldCondition)
					return false;

				array<ref EDF_DbFindFieldPathSegment> segments = ParseSegments(fieldCondition);
				return EvaluateField(entity, fieldCondition, segments, 0);
			}
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	protected static array<ref EDF_DbFindFieldPathSegment> ParseSegments(EDF_DbFindFieldCondition fieldCondition)
	{
		array<ref EDF_DbFindFieldPathSegment> resultSegments();

		array<string> segments();
		fieldCondition.m_sFieldPath.Split(EDF_DbFindFieldAnnotations.SEPERATOR, segments, true);

		resultSegments.Reserve(segments.Count());

		foreach (string segment : segments)
		{
			int flags = 0; // Explicit reset to 0 is required

			while (true)
			{
				if (segment.Replace(EDF_DbFindFieldAnnotations.ANY, "") > 0)
				{
					flags |= EDF_DbFindFieldPathSegmentFlags.ANY;
					continue;
				}

				if (segment.Replace(EDF_DbFindFieldAnnotations.ALL, "") > 0)
				{
					flags |= EDF_DbFindFieldPathSegmentFlags.ALL;
					continue;
				}

				if (segment.Replace(EDF_DbFindFieldAnnotations.KEYS, "") > 0)
				{
					flags |= EDF_DbFindFieldPathSegmentFlags.KEYS;
					continue;
				}

				if (segment.Replace(EDF_DbFindFieldAnnotations.VALUES, "") > 0)
				{
					flags |= EDF_DbFindFieldPathSegmentFlags.VALUES;
					continue;
				}

				if (segment.Replace(EDF_DbFindFieldAnnotations.LENGTH, "") > 0)
				{
					flags |= EDF_DbFindFieldPathSegmentFlags.LENGTH;
					continue;
				}

				if (segment.Replace(EDF_DbFindFieldAnnotations.COUNT, "") > 0)
				{
					flags |= EDF_DbFindFieldPathSegmentFlags.COUNT;
					continue;
				}

				break;
			}

			int asIntValue = segment.ToInt();
			if (asIntValue != 0 || segment == "0")
			{
				flags |= EDF_DbFindFieldPathSegmentFlags.NUMBER;
			}
			else if (segment.ToType())
			{
				flags |= EDF_DbFindFieldPathSegmentFlags.TYPENAME;
			}

			resultSegments.Insert(new EDF_DbFindFieldPathSegment(segment, flags));
		}

		return resultSegments;
	}

	//------------------------------------------------------------------------------------------------
	protected static bool EvaluateField(Class instance, EDF_DbFindFieldCondition fieldCondition, array<ref EDF_DbFindFieldPathSegment> pathSegments, int currentSegmentIndex)
	{
		if (currentSegmentIndex >= pathSegments.Count())
		{
			Debug.Error(string.Format("Failed to evaluate invalid rule '%1' on '%2'.", fieldCondition.m_sFieldPath, instance));
			return false;
		}

		ScriptModule scriptModule = GetGame().GetScriptModule();

		EDF_DbFindFieldPathSegment currentSegment = pathSegments.Get(currentSegmentIndex);
		EDF_ReflectionVariableInfo variableInfo = EDF_ReflectionVariableInfo.Get(instance, currentSegment.m_sFieldName);

		// Expand complex/collection type as this is not yet the final path segment
		if (currentSegmentIndex < pathSegments.Count() - 1)
		{
			if (variableInfo.m_tVaribleType.IsInherited(Class))
			{
				Class complexFieldValue;
				if (!instance.Type().GetVariableValue(instance, variableInfo.m_iVariableindex, complexFieldValue))
				{
					Debug.Error(string.Format("Failed to read field '%1' of type '%2' on '%3'.", currentSegment.m_sFieldName, variableInfo.m_tVaribleType, instance));
					return false;
				}

				// Expand collections
				if (variableInfo.m_eCollectionType != EDF_ReflectionVariableType.NONE)
				{
					int collectionCount;
					scriptModule.Call(complexFieldValue, "Count", false, collectionCount);

					int nextSegmentIndex = currentSegmentIndex + 1;
					EDF_DbFindFieldPathSegment nextSegment = pathSegments.Get(nextSegmentIndex);

					// Handle collection<Class>.typename access operator
					typename filterType = typename.Empty;
					if (nextSegment.m_iFlags & EDF_DbFindFieldPathSegmentFlags.TYPENAME)
					{
						filterType = EDF_DbName.GetTypeByName(nextSegment.m_sFieldName);
						nextSegmentIndex++; //Skip source.filterType. to condition after the filter
						currentSegment = nextSegment; // move current segment because :any/:all modifier is on the typename segement
					}

					// Handle collection<...>.N access operator
					int collectionStartIndex = 0;
					bool indexAccessorMode = false;
					if (nextSegment.m_iFlags & EDF_DbFindFieldPathSegmentFlags.NUMBER)
					{
						collectionStartIndex = nextSegment.m_sFieldName.ToInt();

						if (collectionStartIndex >= collectionCount)
						{
							Debug.Error(string.Format("Tried to access ilegal collection index '%1' of type '%2' on '%3'. Collection only contained '%4' items.", collectionStartIndex, variableInfo.m_tVaribleType, instance, collectionCount));
							return false;
						}

						indexAccessorMode = true;
						nextSegmentIndex++; // Continue on the segment after the index information
					}

					for (int nValue = collectionStartIndex; nValue < collectionCount; nValue++)
					{
						Class collectionValueItem;

						string getFnc = "Get";
						if (variableInfo.m_eCollectionType == EDF_ReflectionVariableType.MAP)
						{
							// Access n-th map element by key(default) or value
							if (currentSegment.m_iFlags & EDF_DbFindFieldPathSegmentFlags.VALUES) //Only if Values() was explicitly set
							{
								getFnc = "GetElement";
							}
							else
							{
								getFnc = "GetKey";
							}
						}

						if (!scriptModule.Call(complexFieldValue, getFnc, false, collectionValueItem, nValue) || !collectionValueItem)
						{
							Debug.Error(string.Format("Failed to get collection value at index '%1' on collection '%2' on '%3'.", nValue, complexFieldValue, instance));
							return false;
						}

						if (filterType && !collectionValueItem.IsInherited(filterType))
							continue;

						bool evaluationResult = EvaluateField(collectionValueItem, fieldCondition, pathSegments, nextSegmentIndex);

						// A specific index of the collection was checked, just directly return that result what ever it is.
						if (indexAccessorMode)
							return evaluationResult;

						if ((currentSegment.m_iFlags & EDF_DbFindFieldPathSegmentFlags.ALL))
						{
							// All must be true, but at least one was false, so total result is false
							if (!evaluationResult)
								return false;

							// All elements were iterated and none of them returned false, so the overall result is true
							if (nValue == collectionCount - 1)
								return true;
						}

						//Any(default) must be true, and we got one match, so we can abort and return true
						if (evaluationResult)
							return true;
					}

					//None of the typename filters matched or none of the any-conditions returned true
					return false;
				}

				// Expand complex type
				return EvaluateField(complexFieldValue, fieldCondition, pathSegments, currentSegmentIndex + 1);
			}
			else
			{
				Debug.Error(string.Format("Reading field '%1' of a primtive type '%2' on '%3' is not possible.", currentSegment.m_sFieldName, variableInfo.m_tVaribleType, instance));
				return false;
			}
		}

		// Apply condition to result field value
		switch (fieldCondition.Type())
		{
			case EDF_DbFindCheckFieldNull:
			{
				if (variableInfo.m_tVaribleType.IsInherited(Class))
				{
					Class fieldValue;
					if (!instance.Type().GetVariableValue(instance, variableInfo.m_iVariableindex, fieldValue)) return false;
					return fieldValue == null;
				}

				Debug.Error(string.Format("Can no null check field '%1' of a primtive type '%2' on '%3' is not possible.", currentSegment.m_sFieldName, variableInfo.m_tVaribleType, instance));
				return false;
			}

			case EDF_DbFindCheckFieldEmpty:
			{
				switch (variableInfo.m_tVaribleType)
				{
					case int: return EDF_DbFindFielEmptyChecker<int>.Evaluate(instance, EDF_DbFindCheckFieldEmpty.Cast(fieldCondition), currentSegment, variableInfo);
					case float: return EDF_DbFindFielEmptyChecker<float>.Evaluate(instance, EDF_DbFindCheckFieldEmpty.Cast(fieldCondition), currentSegment, variableInfo);
					case bool: return EDF_DbFindFielEmptyChecker<bool>.Evaluate(instance, EDF_DbFindCheckFieldEmpty.Cast(fieldCondition), currentSegment, variableInfo);
					case string: return EDF_DbFindFielEmptyChecker<string>.Evaluate(instance, EDF_DbFindCheckFieldEmpty.Cast(fieldCondition), currentSegment, variableInfo);
					case vector: return EDF_DbFindFielEmptyChecker<vector>.Evaluate(instance, EDF_DbFindCheckFieldEmpty.Cast(fieldCondition), currentSegment, variableInfo);
				}

				return EDF_DbFindFielEmptyChecker<Class>.Evaluate(instance, EDF_DbFindCheckFieldEmpty.Cast(fieldCondition), currentSegment, variableInfo);
			}

			case EDF_DbFindFieldIntMultiple:
			{
				return EDF_DbFindFieldValueTypedEvaluator<int>.Evaluate(instance, EDF_DbFindFieldIntMultiple.Cast(fieldCondition), currentSegment, variableInfo);
			}

			case EDF_DbFindFieldFloatMultiple:
			{
				return EDF_DbFindFieldValueTypedEvaluator<float>.Evaluate(instance, EDF_DbFindFieldFloatMultiple.Cast(fieldCondition), currentSegment, variableInfo);
			}

			case EDF_DbFindFieldBoolMultiple:
			{
				return EDF_DbFindFieldValueTypedEvaluator<bool>.Evaluate(instance, EDF_DbFindFieldBoolMultiple.Cast(fieldCondition), currentSegment, variableInfo);
			}

			case EDF_DbFindFieldStringMultiple:
			{
				return EDF_DbFindFieldValueTypedEvaluator<string>.Evaluate(instance, EDF_DbFindFieldStringMultiple.Cast(fieldCondition), currentSegment, variableInfo);
			}

			case EDF_DbFindFieldVectorMultiple:
			{
				return EDF_DbFindFieldValueTypedEvaluator<vector>.Evaluate(instance, EDF_DbFindFieldVectorMultiple.Cast(fieldCondition), currentSegment, variableInfo);
			}

			case EDF_DbFindFieldTypenameMultiple:
			{
				return EDF_DbFindFieldValueTypedEvaluator<typename>.Evaluate(instance, EDF_DbFindFieldTypenameMultiple.Cast(fieldCondition), currentSegment, variableInfo);
			}

			default:
			{
				Debug.Error(string.Format("Unknown condition type '%1'.", fieldCondition.Type().ToString()));
				return false;
			}
		}

		// Fall through
		return false;
	}
};

class EDF_DbFindFielEmptyChecker<Class TValueType>
{
	//------------------------------------------------------------------------------------------------
	static bool Evaluate(Class instance, EDF_DbFindCheckFieldEmpty valueCondition, EDF_DbFindFieldPathSegment currentSegment, EDF_ReflectionVariableInfo fieldInfo)
	{
		TValueType fieldValue;
		if (!instance.Type().GetVariableValue(instance, fieldInfo.m_iVariableindex, fieldValue)) return false;

		return IsEmptyOrDefault(fieldValue);
	}

	//------------------------------------------------------------------------------------------------
	protected static bool IsEmptyOrDefault(int value)
	{
		return value == 0;
	}

	//------------------------------------------------------------------------------------------------
	protected static bool IsEmptyOrDefault(float value)
	{
		return float.AlmostEqual(value, 0.0);
	}

	//------------------------------------------------------------------------------------------------
	protected static bool IsEmptyOrDefault(bool value)
	{
		return value == false;
	}

	//------------------------------------------------------------------------------------------------
	protected static bool IsEmptyOrDefault(string value)
	{
		value.Replace(" ", "");
		return value.IsEmpty();
	}

	//------------------------------------------------------------------------------------------------
	protected static bool IsEmptyOrDefault(vector value)
	{
		return float.AlmostEqual(vector.Distance(value, "0 0 0"), 0.0);
	}

	//------------------------------------------------------------------------------------------------
	protected static bool IsEmptyOrDefault(Class value)
	{
		if (value.Type().IsInherited(array) || value.Type().IsInherited(set) || value.Type().IsInherited(map))
		{
			int collectionCount;
			if (!GetGame().GetScriptModule().Call(value, "Count", false, collectionCount))
				return false;

			return collectionCount == 0;
		}

		return value == null;
	}
};

class EDF_DbFindFieldValueTypedEvaluator<Class TValueType>
{
	//------------------------------------------------------------------------------------------------
	static bool Evaluate(Class instance, EDF_DbFindCompareFieldValues<TValueType> valueCondition, EDF_DbFindFieldPathSegment currentSegment, EDF_ReflectionVariableInfo fieldInfo)
	{
		if (valueCondition.m_aComparisonValues.IsEmpty())
		{
			Debug.Error(string.Format("Can not compare field '%1' on '%2' with empty condition.", currentSegment.m_sFieldName, instance));
			return false;
		}

		PreprocessComparisonValues(valueCondition.m_eComparisonOperator, valueCondition.m_aComparisonValues);

		ScriptModule scriptModule = GetGame().GetScriptModule();

		// Handle collection comparison
		if (fieldInfo.m_eCollectionType != EDF_ReflectionVariableType.NONE)
		{
			Class collectionHolder;
			if (!instance.Type().GetVariableValue(instance, fieldInfo.m_iVariableindex, collectionHolder))
				return false;

			int collectionCount;
			if (!GetGame().GetScriptModule().Call(collectionHolder, "Count", false, collectionCount))
				return false;

			// Handle count of collection comparison
			if (currentSegment.m_iFlags & EDF_DbFindFieldPathSegmentFlags.COUNT)
				return CompareCollectionCount(collectionCount, valueCondition.m_eComparisonOperator, valueCondition.m_aComparisonValues);

			EDF_EDbFindOperator comparisonOperator = valueCondition.m_eComparisonOperator;
			bool exactOrderedMatch = comparisonOperator == EDF_EDbFindOperator.ARR_EQUAL;
			if (exactOrderedMatch)
				comparisonOperator = EDF_EDbFindOperator.EQUAL;

			// If the count missmatches on full match it can not be equal
			if (exactOrderedMatch && (collectionCount != valueCondition.m_aComparisonValues.Count()))
				return false;

			for (int nItem = 0; nItem < collectionCount; nItem++)
			{
				TValueType fieldValue;

				string getFnc = "Get";

				if (fieldInfo.m_eCollectionType == EDF_ReflectionVariableType.MAP)
				{
					if (currentSegment.m_iFlags & EDF_DbFindFieldPathSegmentFlags.VALUES) //Only if Values() was explicitly set
					{
						getFnc = "GetElement";
					}
					else
					{
						getFnc = "GetKey";
					}
				}

				if (TValueType == typename && fieldInfo.m_tCollectionValueType != typename)
				{
					// Special handling to read array<Class> and compare to array<typename>
					Class holder;
					if (!scriptModule.Call(collectionHolder, getFnc, false, holder, nItem) ||
						!scriptModule.Call(holder, "Type", false, fieldValue, holder))
						return false;
				}
				else if (!scriptModule.Call(collectionHolder, getFnc, false, fieldValue, nItem))
				{
					return false;
				}

				array<TValueType> compareValues = valueCondition.m_aComparisonValues;
				if (exactOrderedMatch)
					compareValues = {valueCondition.m_aComparisonValues.Get(nItem)};

				bool comparisonMatches = Compare(fieldValue, comparisonOperator, compareValues);

				if (exactOrderedMatch || (currentSegment.m_iFlags & EDF_DbFindFieldPathSegmentFlags.ALL))
				{
					// All must match the operator comparison, if one of them failes the final result is false
					if (!comparisonMatches)
						return false;

					// All including the last item in the collection matched the comparison, so we return true
					if (nItem == collectionCount - 1)
						return true;
				}
				else
				{
					// Any of the item matched, so we can return true early
					if (comparisonMatches)
						return true;

					// None, including the last item, matched the condition so return false
					if (nItem == collectionCount - 1)
						return false;
				}
			}

			// Fall through
			return false;
		}

		// Special case of string lenth which is int value condition but string holder
		if (currentSegment.m_iFlags & EDF_DbFindFieldPathSegmentFlags.LENGTH)
		{
			string stringData;
			if (!instance.Type().GetVariableValue(instance, fieldInfo.m_iVariableindex, stringData))
				return false;

			Managed arr = valueCondition.m_aComparisonValues;
			return Compare(stringData.Length(), valueCondition.m_eComparisonOperator, array<int>.Cast(arr));
		}

		// Compare primitive value
		TValueType fieldValue;
		if (!instance.Type().GetVariableValue(instance, fieldInfo.m_iVariableindex, fieldValue))
			return false;

		return Compare(fieldValue, valueCondition.m_eComparisonOperator, valueCondition.m_aComparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	// Default do nothing implementation
	protected static void PreprocessComparisonValues(EDF_EDbFindOperator operator, Class comparisonValues);

	//------------------------------------------------------------------------------------------------
	protected static void PreprocessComparisonValues(EDF_EDbFindOperator operator, array<string> comparisonValues)
	{
		foreach (int idx, string comparisonValue : comparisonValues)
		{
			comparisonValue.ToLower();
			comparisonValues.Set(idx, comparisonValue);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected static bool CompareCollectionCount(int collectionCount, EDF_EDbFindOperator operator, Class comparisonValues)
	{
		array<int> strongTypedComparisonValues = array<int>.Cast(comparisonValues);
		if (!strongTypedComparisonValues)
			return false;

		return Compare(collectionCount, operator, strongTypedComparisonValues);
	}

	//------------------------------------------------------------------------------------------------
	protected static bool Compare(int fieldValue, EDF_EDbFindOperator operator, array<int> comparisonValues)
	{
		switch (operator)
		{
			case EDF_EDbFindOperator.CONTAINS:
			case EDF_EDbFindOperator.EQUAL:
			{
				return comparisonValues.Contains(fieldValue);
			}

			case EDF_EDbFindOperator.NOT_CONTAINS:
			case EDF_EDbFindOperator.NOT_EQUAL:
			{
				return !comparisonValues.Contains(fieldValue);
			}

			case EDF_EDbFindOperator.LESS_THAN:
			{
				foreach (int compare : comparisonValues)
				{
					if (fieldValue < compare)
						return true;
				}

				return false;
			}

			case EDF_EDbFindOperator.LESS_THAN_OR_EQUAL:
			{
				foreach (int compare : comparisonValues)
				{
					if (fieldValue <= compare)
						return true;
				}

				return false;
			}

			case EDF_EDbFindOperator.GREATER_THAN:
			{
				foreach (int compare : comparisonValues)
				{
					if (fieldValue > compare)
						return true;
				}

				return false;
			}

			case EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL:
			{
				foreach (int compare : comparisonValues)
				{
					if (fieldValue >= compare)
						return true;
				}

				return false;
			}
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	protected static bool Compare(float fieldValue, EDF_EDbFindOperator operator, array<float> comparisonValues)
	{
		switch (operator)
		{
			case EDF_EDbFindOperator.CONTAINS:
			case EDF_EDbFindOperator.EQUAL:
			{
				foreach (float compare : comparisonValues)
				{
					if (float.AlmostEqual(fieldValue, compare))
						return true;
				}

				return false;
			}

			case EDF_EDbFindOperator.NOT_CONTAINS:
			case EDF_EDbFindOperator.NOT_EQUAL:
			{
				foreach (float compare : comparisonValues)
				{
					if (float.AlmostEqual(fieldValue, compare))
						return false;
				}

				return true;
			}

			case EDF_EDbFindOperator.LESS_THAN:
			{
				foreach (float compare : comparisonValues)
				{
					if (fieldValue < compare)
						return true;
				}

				return false;
			}

			case EDF_EDbFindOperator.LESS_THAN_OR_EQUAL:
			{
				foreach (float compare : comparisonValues)
				{
					if ((fieldValue < compare) || float.AlmostEqual(fieldValue, compare))
						return true;
				}

				return false;
			}

			case EDF_EDbFindOperator.GREATER_THAN:
			{
				foreach (float compare : comparisonValues)
				{
					if (fieldValue > compare)
						return true;
				}

				return false;
			}

			case EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL:
			{
				foreach (float compare : comparisonValues)
				{
					if ((fieldValue > compare) || float.AlmostEqual(fieldValue, compare))
						return true;
				}

				return false;
			}
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	protected static bool Compare(bool fieldValue, EDF_EDbFindOperator operator, array<bool> comparisonValues)
	{
		switch (operator)
		{
			case EDF_EDbFindOperator.CONTAINS:
			case EDF_EDbFindOperator.EQUAL:
			{
				return comparisonValues.Contains(fieldValue);
			}

			case EDF_EDbFindOperator.NOT_CONTAINS:
			case EDF_EDbFindOperator.NOT_EQUAL:
			{
				return !comparisonValues.Contains(fieldValue);
			}
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	protected static bool Compare(string fieldValue, EDF_EDbFindOperator operator, array<string> comparisonValues)
	{
		fieldValue.ToLower();

		switch (operator)
		{
			case EDF_EDbFindOperator.EQUAL:
			{
				return comparisonValues.Contains(fieldValue);
			}

			case EDF_EDbFindOperator.NOT_EQUAL:
			{
				return !comparisonValues.Contains(fieldValue);
			}

			case EDF_EDbFindOperator.CONTAINS:
			{
				foreach (string compare : comparisonValues)
				{
					if (fieldValue.Contains(compare)) return true;
				}

				return false;
			}

			case EDF_EDbFindOperator.NOT_CONTAINS:
			{
				foreach (string compare : comparisonValues)
				{
					if (fieldValue.Contains(compare)) return false;
				}

				return true;
			}
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	protected static bool Compare(vector fieldValue, EDF_EDbFindOperator operator, array<vector> comparisonValues)
	{
		switch (operator)
		{
			case EDF_EDbFindOperator.CONTAINS:
			case EDF_EDbFindOperator.EQUAL:
			{
				foreach (vector compare : comparisonValues)
				{
					if (float.AlmostEqual(vector.Distance(fieldValue, compare), 0))
						return true;
				}

				return false;
			}

			case EDF_EDbFindOperator.NOT_CONTAINS:
			case EDF_EDbFindOperator.NOT_EQUAL:
			{
				foreach (vector compare : comparisonValues)
				{
					if (float.AlmostEqual(vector.Distance(fieldValue, compare), 0))
						return false;
				}

				return true;
			}

			case EDF_EDbFindOperator.LESS_THAN:
			{
				foreach (vector compare : comparisonValues)
				{
					if ((fieldValue[0] < compare[0]) &&
						(fieldValue[1] < compare[1]) &&
						(fieldValue[2] < compare[2])) return true;
				}

				return false;
			}

			case EDF_EDbFindOperator.LESS_THAN_OR_EQUAL:
			{
				foreach (vector compare : comparisonValues)
				{
					if (((fieldValue[0] < compare[0]) || float.AlmostEqual(fieldValue[0], compare[0])) &&
						((fieldValue[1] < compare[1]) || float.AlmostEqual(fieldValue[1], compare[1])) &&
						((fieldValue[2] < compare[2]) || float.AlmostEqual(fieldValue[2], compare[2]))) return true;
				}

				return false;
			}

			case EDF_EDbFindOperator.GREATER_THAN:
			{
				foreach (vector compare : comparisonValues)
				{
					if ((fieldValue[0] > compare[0]) &&
						(fieldValue[1] > compare[1]) &&
						(fieldValue[2] > compare[2])) return true;
				}

				return false;
			}

			case EDF_EDbFindOperator.GREATER_THAN_OR_EQUAL:
			{
				foreach (vector compare : comparisonValues)
				{
					if (((fieldValue[0] > compare[0]) || float.AlmostEqual(fieldValue[0], compare[0])) &&
						((fieldValue[1] > compare[1]) || float.AlmostEqual(fieldValue[1], compare[1])) &&
						((fieldValue[2] > compare[2]) || float.AlmostEqual(fieldValue[2], compare[2]))) return true;
				}

				return false;
			}
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	protected static bool Compare(typename fieldValue, EDF_EDbFindOperator operator, array<typename> comparisonValues)
	{
		switch (operator)
		{
			case EDF_EDbFindOperator.EQUAL:
			case EDF_EDbFindOperator.CONTAINS:
			{
				foreach (typename compare : comparisonValues)
				{
					if (fieldValue.IsInherited(compare))
						return true;
				}

				return false;
			}

			case EDF_EDbFindOperator.NOT_EQUAL:
			case EDF_EDbFindOperator.NOT_CONTAINS:
			{
				foreach (typename compare : comparisonValues)
				{
					if (fieldValue.IsInherited(compare))
						return false;
				}

				return true;
			}
		}

		return false;
	}
};
