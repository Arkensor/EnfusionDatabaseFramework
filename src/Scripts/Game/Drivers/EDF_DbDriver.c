class EDF_DbDriver
{
	static bool s_bForceBlocking; //Used to forced drivers to rely on sync api during session teardown

	//------------------------------------------------------------------------------------------------
	bool Initialize(notnull EDF_DbConnectionInfoBase connectionInfo);

	//------------------------------------------------------------------------------------------------
	EDF_EDbOperationStatusCode AddOrUpdate(notnull EDF_DbEntity entity);

	//------------------------------------------------------------------------------------------------
	EDF_EDbOperationStatusCode Remove(typename entityType, string entityId);

	//------------------------------------------------------------------------------------------------
	EDF_DbFindResultMultiple<EDF_DbEntity> FindAll(typename entityType, EDF_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1);

	//------------------------------------------------------------------------------------------------
	void AddOrUpdateAsync(notnull EDF_DbEntity entity, EDF_DbOperationStatusOnlyCallback callback = null);

	//------------------------------------------------------------------------------------------------
	void RemoveAsync(typename entityType, string entityId, EDF_DbOperationStatusOnlyCallback callback = null);

	//------------------------------------------------------------------------------------------------
	void FindAllAsync(typename entityType, EDF_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1, EDF_DbFindCallbackBase callback = null);
};

class EDF_DbDriverName
{
	//------------------------------------------------------------------------------------------------
	void EDF_DbDriverName(TStringArray driverAliases = null)
	{
		typename driverType = EDF_ReflectionUtils.GetAttributeParent();

		EDF_DbDriverRegistry.Register(driverType.ToString(), driverType);

		if (driverAliases)
		{
			foreach (string alias : driverAliases)
			{
				EDF_DbDriverRegistry.Register(alias, driverType);
			}
		}
	}
};

class EDF_DbDriverRegistry
{
	protected static ref map<string, typename> s_mDrivers;

	//------------------------------------------------------------------------------------------------
	static void Register(string driverName, typename driverType)
	{
		if (!s_mDrivers)
			s_mDrivers = new map<string, typename>();

		string driverNameInvariant = driverName;
		driverNameInvariant.ToLower();

		s_mDrivers.Set(driverNameInvariant, driverType);
	}

	//------------------------------------------------------------------------------------------------
	static void Unregister(string driverName)
	{
		if (!s_mDrivers)
			return;

		string driverNameInvariant = driverName;
		driverNameInvariant.ToLower();

		s_mDrivers.Remove(driverNameInvariant);
	}

	//------------------------------------------------------------------------------------------------
	static typename Get(string driverName)
	{
		string driverNameInvariant = driverName;
		driverNameInvariant.ToLower();

		if (!s_mDrivers || !s_mDrivers.Contains(driverNameInvariant))
			return typename.Empty;

		return s_mDrivers.Get(driverNameInvariant);
	}

	//------------------------------------------------------------------------------------------------
	static set<typename> GetAll()
	{
		set<typename> result();

		if (s_mDrivers)
		{
			for (int nElement = 0; nElement < s_mDrivers.Count(); nElement++)
			{
				result.Insert(s_mDrivers.GetElement(nElement));
			}
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------
	static void Reset()
	{
		delete s_mDrivers;
	}
};
