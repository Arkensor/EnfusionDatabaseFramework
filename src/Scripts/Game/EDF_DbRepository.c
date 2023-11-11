class EDF_DbRepositoryBase
{
	protected ref EDF_DbContext m_DbContext;

	//------------------------------------------------------------------------------------------------
	EDF_DbContext GetDbContext()
	{
		return m_DbContext;
	}

	//------------------------------------------------------------------------------------------------
	//! Use with caution, normally you should never need to use this!
	void SetDbContext(EDF_DbContext dbContext)
	{
		m_DbContext = dbContext;
	}

	//------------------------------------------------------------------------------------------------
	//! Creation only through EDF_DbRepositoryFactory::GetRepository
	/*protected void EDF_DbRepositoryBase(); --Hotfix for 1.0*/
};

class EDF_DbRepository<Class TEntityType> : EDF_DbRepositoryBase
{
	// ------------------------------------ Sync API -------------------------------------------------

	//------------------------------------------------------------------------------------------------
	EDF_EDbOperationStatusCode AddOrUpdate(notnull TEntityType entity)
	{
		return m_DbContext.AddOrUpdate(entity);
	}

	//------------------------------------------------------------------------------------------------
	EDF_EDbOperationStatusCode Remove(string entityId)
	{
		return m_DbContext.Remove(TEntityType, entityId);
	}

	//------------------------------------------------------------------------------------------------
	EDF_EDbOperationStatusCode Remove(notnull TEntityType entity)
	{
		return m_DbContext.Remove(TEntityType, entity.GetId());
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindResultSingle<TEntityType> Find(string entityId)
	{
		return FindFirst(EDF_DbFind.Id().Equals(entityId));
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindResultSingle<TEntityType> FindSingleton()
	{
		EDF_DbFindResultSingle<TEntityType> findResult = FindFirst();

		if (!findResult.IsSuccess() || findResult.GetEntity())
			return findResult;

		typename spawnType = TEntityType;
		return new EDF_DbFindResultSingle<TEntityType>(EDF_EDbOperationStatusCode.SUCCESS, TEntityType.Cast(spawnType.Spawn()));
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindResultSingle<TEntityType> FindFirst(EDF_DbFindCondition condition = null, array<ref array<string>> orderBy = null)
	{
		EDF_DbFindResultMultiple<EDF_DbEntity> findResults = m_DbContext.FindAll(TEntityType, condition, orderBy, 1);

		if (!findResults.IsSuccess())
			return new EDF_DbFindResultSingle<TEntityType>(findResults.GetStatusCode());

		TEntityType entity = null;
		if (findResults.GetEntities().Count() > 0)
			entity = TEntityType.Cast(findResults.GetEntities().Get(0));

		return new EDF_DbFindResultSingle<TEntityType>(findResults.GetStatusCode(), entity);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbFindResultMultiple<TEntityType> FindAll(EDF_DbFindCondition condition = null, array<ref array<string>> orderBy = null, int limit = -1, int offset = -1)
	{
		EDF_DbFindResultMultiple<EDF_DbEntity> findResults = m_DbContext.FindAll(TEntityType, condition, orderBy, limit, offset);
		return new EDF_DbFindResultMultiple<TEntityType>(findResults.GetStatusCode(), EDF_RefArrayCaster<EDF_DbEntity, TEntityType>.Convert(findResults.GetEntities()));
	}

	// ------------------------------------------ ASYNC API ------------------------------------------

	//------------------------------------------------------------------------------------------------
	void AddOrUpdateAsync(notnull TEntityType entity, EDF_DbOperationStatusOnlyCallback callback = null)
	{
		m_DbContext.AddOrUpdateAsync(entity, callback);
	}

	//------------------------------------------------------------------------------------------------
	void RemoveAsync(string entityId, EDF_DbOperationStatusOnlyCallback callback = null)
	{
		m_DbContext.RemoveAsync(TEntityType, entityId, callback);
	}

	//------------------------------------------------------------------------------------------------
	void RemoveAsync(TEntityType entity, EDF_DbOperationStatusOnlyCallback callback = null)
	{
		m_DbContext.RemoveAsync(TEntityType, entity.GetId(), callback);
	}

	//------------------------------------------------------------------------------------------------
	void FindAsync(string entityId, notnull EDF_DbFindCallbackSingle<TEntityType> callback)
	{
		m_DbContext.FindAllAsync(TEntityType, EDF_DbFind.Id().Equals(entityId), null, 1, -1, callback);
	}

	//------------------------------------------------------------------------------------------------
	void FindSingletonAsync(notnull EDF_DbFindCallbackSingleton<TEntityType> callback)
	{
		m_DbContext.FindAllAsync(TEntityType, null, null, 1, -1, callback);
	}

	//------------------------------------------------------------------------------------------------
	void FindFirstAsync(EDF_DbFindCondition condition = null, array<ref array<string>> orderBy = null, EDF_DbFindCallbackSingle<TEntityType> callback = null)
	{
		m_DbContext.FindAllAsync(TEntityType, condition, orderBy, 1, -1, callback);
	}

	//------------------------------------------------------------------------------------------------
	void FindAllAsync(EDF_DbFindCondition condition = null, array<ref array<string>> orderBy = null, int limit = -1, int offset = -1, notnull EDF_DbFindCallbackMultiple<TEntityType> callback = null)
	{
		m_DbContext.FindAllAsync(TEntityType, condition, orderBy, limit, offset, callback);
	}
	
	//------------------------------------------------------------------------------------------------
	/*sealed*/ static typename GetEntityType()
	{
		return TEntityType;
	}
};

class EDF_DbRepositoryRegistration
{
	protected static ref map<typename, typename> s_mMapping;
	protected static ref array<typename> s_aRegistrationQueue;

	//------------------------------------------------------------------------------------------------
	static typename Get(typename entityType)
	{
		if (!s_mMapping)
			s_mMapping = new map<typename, typename>();

		typename result = s_mMapping.Get(entityType);
		if (!result)
		{
			string repositoryTypeStr = string.Format("EDF_DbRepository<%1>", entityType.ToString());

			result = repositoryTypeStr.ToType();

			if (result)
			{
				// Save default implementation repository to cache
				s_mMapping.Set(entityType, result);
			}
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------
	static void FlushRegistrations(ScriptModule scriptModule)
	{
		if (!s_aRegistrationQueue || !scriptModule)
			return;

		if (!s_mMapping)
			s_mMapping = new map<typename, typename>();

		foreach (typename repositoryType : s_aRegistrationQueue)
		{
			auto reflectionInst = repositoryType.Spawn();
			if (!reflectionInst)
				continue;

			typename entityType;
			scriptModule.Call(reflectionInst, "GetEntityType", false, entityType);
			reflectionInst = null;

			typename expectedBase = string.Format("EDF_DbRepository<%1>", entityType.ToString()).ToType();
			if (!repositoryType.IsInherited(expectedBase))
			{
				Debug.Error(string.Format("Failed to register '%1' as repository for '%2'. '%1' must inherit from '%3'.", repositoryType, entityType, expectedBase));
				continue;
			}

			if (!entityType.IsInherited(EDF_DbEntity))
			{
				Debug.Error(string.Format("Db repository '%1' created with entity type '%2' that does not inherit from '%3'. Results will be invalid!", repositoryType, entityType, EDF_DbEntity));
				continue;
			}

			s_mMapping.Set(entityType, repositoryType);
		}
	}

	//------------------------------------------------------------------------------------------------
	static void EDF_DbRepositoryRegistration()
	{
		// TODO: Retrire registration attribute when we can can get all inherited types from our repository base classes

		typename repositoryType = EDF_ReflectionUtils.GetAttributeParent();

		if (!s_aRegistrationQueue)
		{
			s_aRegistrationQueue = {repositoryType};
			return;
		}

		s_aRegistrationQueue.Insert(repositoryType);
	}
};
