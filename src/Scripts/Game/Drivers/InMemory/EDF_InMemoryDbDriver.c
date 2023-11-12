[EDF_DbConnectionInfoDriverType(EDF_InMemoryDbDriver), BaseContainerProps()]
class EDF_InMemoryDbConnectionInfo : EDF_DbConnectionInfoBase
{
};

[EDF_DbDriverName({"InMemory"})]
class EDF_InMemoryDbDriver : EDF_DbDriver
{
	protected static ref map<string, ref EDF_InMemoryDatabase> s_mDatabases;
	protected EDF_InMemoryDatabase m_pDb;

	//------------------------------------------------------------------------------------------------
	override bool Initialize(notnull EDF_DbConnectionInfoBase connectionInfo)
	{
		// Only create the db holder if at least one driver is initialized. (Avoids allocation on clients)
		if (!s_mDatabases)
			s_mDatabases = new map<string, ref EDF_InMemoryDatabase>();

		string dbName = connectionInfo.m_sDatabaseName;
		m_pDb = s_mDatabases.Get(dbName);

		// Init db if driver was the first one to trying to access it
		if (!m_pDb)
		{
			s_mDatabases.Set(dbName, new EDF_InMemoryDatabase(dbName));
			m_pDb = s_mDatabases.Get(dbName); // Strong ref held by map so get it there
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override EDF_EDbOperationStatusCode AddOrUpdate(notnull EDF_DbEntity entity)
	{
		if (!entity.HasId())
			return EDF_EDbOperationStatusCode.FAILURE_ID_NOT_SET;

		// Make a copy so after insert you can not accidently change anything on the instance passed into the driver later.
		EDF_DbEntity deepCopy = EDF_DbEntity.Cast(entity.Type().Spawn());
		EDF_DbEntityUtils<EDF_DbEntity>.StructAutoCopy(entity, deepCopy);

		m_pDb.AddOrUpdate(deepCopy);
		return EDF_EDbOperationStatusCode.SUCCESS;
	}

	//------------------------------------------------------------------------------------------------
	override EDF_EDbOperationStatusCode Remove(typename entityType, string entityId)
	{
		if (!entityId)
			return EDF_EDbOperationStatusCode.FAILURE_ID_NOT_SET;

		if (!m_pDb.Get(entityType, entityId))
			return EDF_EDbOperationStatusCode.FAILURE_ID_NOT_FOUND;

		m_pDb.Remove(entityType, entityId);
		return EDF_EDbOperationStatusCode.SUCCESS;
	}

	//------------------------------------------------------------------------------------------------
	override EDF_DbFindResultMultiple<EDF_DbEntity> FindAll(typename entityType, EDF_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1)
	{
		array<ref EDF_DbEntity> entities;

		// See if we can only load selected few entities by id or we need the entire collection to search through
		set<string> loadIds(), skipIds();
		bool needsFilter = false;
		if (EDF_DbFindConditionEvaluator.CollectConditionIds(condition, loadIds, skipIds) &&
			!loadIds.IsEmpty() && // There must be something to load explictly
			skipIds.IsEmpty()) // and no "load xxx but skip these"
		{
			entities = {};
			foreach (string relevantId : loadIds)
			{
				EDF_DbEntity entity = m_pDb.Get(entityType, relevantId);
				if (entity)
					entities.Insert(entity);
			}
		}
		else
		{
			entities = m_pDb.GetAll(entityType);
			needsFilter = true;
		}

		if (needsFilter && condition)
			entities = EDF_DbFindConditionEvaluator.GetFiltered(entities, condition);

		if (orderBy)
			entities = EDF_DbEntitySorter.GetSorted(entities, orderBy);

		array<ref EDF_DbEntity> resultEntites();

		foreach (int idx, EDF_DbEntity entity : entities)
		{
			// Respect output limit is specified
			if (limit != -1 && resultEntites.Count() >= limit)
				break;

			// Skip the first n records if offset specified (for paginated loading together with limit)
			if (offset != -1 && idx < offset)
				continue;

			// Return a deep copy so you can not accidentially change the db reference instance in the result handling code
			EDF_DbEntity deepCopy = EDF_DbEntity.Cast(entityType.Spawn());
			EDF_DbEntityUtils<EDF_DbEntity>.StructAutoCopy(entity, deepCopy);

			resultEntites.Insert(deepCopy);
		}

		return new EDF_DbFindResultMultiple<EDF_DbEntity>(EDF_EDbOperationStatusCode.SUCCESS, resultEntites);
	}

	//------------------------------------------------------------------------------------------------
	override void AddOrUpdateAsync(notnull EDF_DbEntity entity, EDF_DbOperationStatusOnlyCallback callback = null)
	{
		// In memory is blocking, re-use sync api
		EDF_EDbOperationStatusCode statusCode = AddOrUpdate(entity);
		if (callback)
			callback.Invoke(statusCode);
	}

	//------------------------------------------------------------------------------------------------
	override void RemoveAsync(typename entityType, string entityId, EDF_DbOperationStatusOnlyCallback callback = null)
	{
		// In memory is blocking, re-use sync api
		EDF_EDbOperationStatusCode statusCode = Remove(entityType, entityId);
		if (callback)
			callback.Invoke(statusCode);
	}

	//------------------------------------------------------------------------------------------------
	override void FindAllAsync(typename entityType, EDF_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1, EDF_DbFindCallbackBase callback = null)
	{
		// In memory is blocking, re-use sync api
		EDF_DbFindResultMultiple<EDF_DbEntity> findResults = FindAll(entityType, condition, orderBy, limit, offset);
		if (callback)
			callback.Invoke(findResults.GetStatusCode(), findResults.GetEntities());
	}

	//------------------------------------------------------------------------------------------------
	void ~EDF_InMemoryDbDriver()
	{
		if	(s_mDatabases && m_pDb)
			s_mDatabases.Remove(m_pDb.m_DbName);
	}
};
