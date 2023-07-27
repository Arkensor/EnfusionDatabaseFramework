class EDF_EDbEntitySortDirection
{
	static const string ASCENDING = "ASC";
	static const string DESCENDING = "DESC";
}

class EDF_DbEntitySorter
{
	//------------------------------------------------------------------------------------------------
	static array<ref EDF_DbEntity> GetSorted(notnull array<ref EDF_DbEntity> entities, notnull array<ref TStringArray> orderBy, int orderByIndex = 0)
	{
		if (entities.Count() < 2 || orderByIndex >= orderBy.Count())
			return entities;

		string fieldName = orderBy.Get(orderByIndex).Get(0);
		array<string> fieldSplits();
		fieldName.Split(".", fieldSplits, true);

		// Empty sort condition, nothing to do
		if (fieldSplits.IsEmpty())
			return entities;

		string sortDirection = orderBy.Get(orderByIndex).Get(1);
		sortDirection.ToLower();
		bool descending = sortDirection == "desc";

		ScriptModule scriptModule = GetGame().GetScriptModule();
		Managed sortWrappers = GetSortedWrappers(entities, fieldSplits, descending, scriptModule);

		int sortWrappersCount;
		scriptModule.Call(sortWrappers, "Count", false, sortWrappersCount);

		array<ref EDF_DbEntity> sameEntities();
		bool checkSameEntities = (orderByIndex + 1) < orderBy.Count();

		array<ref EDF_DbEntity> sortedEntities();
		sortedEntities.Reserve(sortWrappersCount);
		for (int nWrapper = 0; nWrapper < sortWrappersCount; nWrapper++)
		{
			EDF_DbEntitySortWrapper sortWrapper;
			scriptModule.Call(sortWrappers, "Get", false, sortWrapper, nWrapper);

			if (checkSameEntities)
			{
				// Collect same order value entities to run sub ordering on them
				sameEntities.Clear();
				int checkNext = nWrapper + 1;
				while (checkNext < sortWrappersCount)
				{
					EDF_DbEntitySortWrapper next;
					scriptModule.Call(sortWrappers, "Get", false, next, checkNext);

					if (sortWrapper.Equals(next))
					{
						sameEntities.Insert(next.m_pEntity);
						checkNext++;
						continue;
					}

					break;
				}

				if (!sameEntities.IsEmpty())
				{
					sameEntities.Insert(sortWrapper.m_pEntity);
					foreach (EDF_DbEntity subSortedEntity : GetSorted(sameEntities, orderBy, orderByIndex + 1))
					{
						sortedEntities.Insert(subSortedEntity);
					}
					nWrapper = checkNext - 1;
					continue;
				}
			}

			sortedEntities.Insert(sortWrapper.m_pEntity);
		}

		return sortedEntities;
	}

	//------------------------------------------------------------------------------------------------
	protected static EDF_DbEntitySortWrapper GetSortWrapper(EDF_DbEntity entity, array<string> fieldSplits)
	{
		EDF_DbEntitySortWrapper wrapper = BuildSortWrapper(entity, fieldSplits, 0);
		wrapper.m_pEntity = entity;
		return wrapper;
	}

	//------------------------------------------------------------------------------------------------
	protected static EDF_DbEntitySortWrapper BuildSortWrapper(Class instance, array<string> fieldSplits, int currentIndex)
	{
		string currentFieldName = fieldSplits.Get(currentIndex);
		EDF_ReflectionVariableInfo variableinfo = EDF_ReflectionVariableInfo.Get(instance, currentFieldName);

		// Expand nested object
		if (currentIndex < fieldSplits.Count() - 1)
		{
			if (variableinfo.m_tVaribleType.IsInherited(array) || variableinfo.m_tVaribleType.IsInherited(set) || variableinfo.m_tVaribleType.IsInherited(map))
			{
				Debug.Error(string.Format("Can not get sort value from collection type '%1' on '%2.%3'", variableinfo.m_tVaribleType, variableinfo.m_tHolderType, currentFieldName));
				return null;
			}
			else if (!variableinfo.m_tVaribleType.IsInherited(Class))
			{
				Debug.Error(string.Format("Can not expand primitive type '%1' on '%2.%3' to read field '%4'", variableinfo.m_tVaribleType, variableinfo.m_tHolderType, currentFieldName, fieldSplits.Get(currentIndex + 1)));
				return null;
			}

			Class complexHolder;
			if (!variableinfo.m_tHolderType.GetVariableValue(instance, variableinfo.m_iVariableIndex, complexHolder))
				return null;

			return BuildSortWrapper(complexHolder, fieldSplits, currentIndex + 1);
		}

		// Read primitive field value and convert to compareable
		switch (variableinfo.m_tVaribleType)
		{
			case int:
			{
				int outVal;
				if (variableinfo.m_tHolderType.GetVariableValue(instance, variableinfo.m_iVariableIndex, outVal))
					return new EDF_DbEntitySortWrapperT<int>(outVal);

				break;
			}

			case float:
			{
				float outVal;
				if (variableinfo.m_tHolderType.GetVariableValue(instance, variableinfo.m_iVariableIndex, outVal))
					return new EDF_DbEntitySortWrapperT<float>(outVal);

				break;
			}

			case bool:
			{
				bool outVal;
				if (variableinfo.m_tHolderType.GetVariableValue(instance, variableinfo.m_iVariableIndex, outVal))
					return new EDF_DbEntitySortWrapperT<bool>(outVal);

				break;
			}

			case string:
			{
				string outVal;
				if (variableinfo.m_tHolderType.GetVariableValue(instance, variableinfo.m_iVariableIndex, outVal))
					return new EDF_DbEntitySortWrapperT<string>(outVal);

				break;
			}

			case vector:
			{
				vector outVal;
				if (variableinfo.m_tHolderType.GetVariableValue(instance, variableinfo.m_iVariableIndex, outVal))
					return new EDF_DbEntitySortWrapperT<vector>(outVal);

				break;
			}

			case typename:
			{
				typename outVal;
				if (variableinfo.m_tHolderType.GetVariableValue(instance, variableinfo.m_iVariableIndex, outVal))
					return new EDF_DbEntitySortWrapperT<typename>(outVal);

				break;
			}
		}

		Debug.Error(string.Format("Can not sort entity collection by field '%1' with non sortable type '%2'.", currentFieldName, variableinfo.m_tVaribleType.ToString()));
		return null;
	}

	//------------------------------------------------------------------------------------------------
	protected static Managed GetSortedWrappers(array<ref EDF_DbEntity> entities, array<string> fieldSplits, bool descending, ScriptModule scriptModule)
	{
		Managed sortWrappers;
		typename valueType;

		foreach (EDF_DbEntity entity : entities)
		{
			EDF_DbEntitySortWrapper wrapper = GetSortWrapper(entity, fieldSplits);

			if (!sortWrappers)
			{
				valueType = wrapper.GetType();
				switch (valueType)
				{
					case int:
					{
						sortWrappers = new array<ref EDF_DbEntitySortWrapperT<int>>();
						break;
					}

					case float:
					{
						sortWrappers = new array<ref EDF_DbEntitySortWrapperT<float>>();
						break;
					}

					case bool:
					{
						sortWrappers = new array<ref EDF_DbEntitySortWrapperT<bool>>();
						break;
					}

					case string:
					{
						sortWrappers = new array<ref EDF_DbEntitySortWrapperT<string>>();
						break;
					}

					case vector:
					{
						sortWrappers = new array<ref EDF_DbEntitySortWrapperT<vector>>();
						break;
					}

					case typename:
					{
						sortWrappers = new array<ref EDF_DbEntitySortWrapperT<typename>>();
						break;
					}
				}

				scriptModule.Call(sortWrappers, "Reserve", false, null, entities.Count());
			}

			switch (valueType)
			{
				case int:
				{
					array<ref EDF_DbEntitySortWrapperT<int>>.Cast(sortWrappers).Insert(EDF_DbEntitySortWrapperT<int>.Cast(wrapper));
					break;
				}

				case float:
				{
					array<ref EDF_DbEntitySortWrapperT<float>>.Cast(sortWrappers).Insert(EDF_DbEntitySortWrapperT<float>.Cast(wrapper));
					break;
				}

				case bool:
				{
					array<ref EDF_DbEntitySortWrapperT<bool>>.Cast(sortWrappers).Insert(EDF_DbEntitySortWrapperT<bool>.Cast(wrapper));
					break;
				}

				case string:
				{
					array<ref EDF_DbEntitySortWrapperT<string>>.Cast(sortWrappers).Insert(EDF_DbEntitySortWrapperT<string>.Cast(wrapper));
					break;
				}

				case vector:
				{
					array<ref EDF_DbEntitySortWrapperT<vector>>.Cast(sortWrappers).Insert(EDF_DbEntitySortWrapperT<vector>.Cast(wrapper));
					break;
				}

				case typename:
				{
					array<ref EDF_DbEntitySortWrapperT<typename>>.Cast(sortWrappers).Insert(EDF_DbEntitySortWrapperT<typename>.Cast(wrapper));
					break;
				}
			}
		}

		auto dbgbefore = array<ref EDF_DbEntitySortWrapperT<vector>>.Cast(sortWrappers);
		if (dbgbefore)
		{
			foreach (auto wrapperbefore : dbgbefore)
			{
				Print(wrapperbefore.m_SortValue);
			}
		}

		scriptModule.Call(sortWrappers, "Sort", false, null, descending);

		auto dbgafter = array<ref EDF_DbEntitySortWrapperT<vector>>.Cast(sortWrappers);
		if (dbgafter)
		{
			foreach (auto wrapperafter : dbgafter)
			{
				Print(wrapperafter.m_SortValue);
			}
		}

		return sortWrappers;
	}

}

class EDF_DbEntitySortWrapper
{
	ref EDF_DbEntity m_pEntity;

	//------------------------------------------------------------------------------------------------
	bool Equals(EDF_DbEntitySortWrapper other);

	//------------------------------------------------------------------------------------------------
	typename GetType();
}

class EDF_DbEntitySortWrapperT<Class T> : EDF_DbEntitySortWrapper
{
	[SortAttribute()]
	T m_SortValue;

	//------------------------------------------------------------------------------------------------
	override bool Equals(EDF_DbEntitySortWrapper other)
	{
		return m_SortValue == EDF_DbEntitySortWrapperT<T>.Cast(other).m_SortValue;
	}

	//------------------------------------------------------------------------------------------------
	override typename GetType()
	{
		return T;
	}

	//------------------------------------------------------------------------------------------------
	void EDF_DbEntitySortWrapperT(T sortValue)
	{
		m_SortValue = sortValue;
	}
}
