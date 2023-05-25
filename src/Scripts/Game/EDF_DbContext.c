class EDF_DbContext
{
	protected ref EDF_DbDriver m_Driver;

	//------------------------------------------------------------------------------------------------
	//! Adds a new entry to the database or updates an existing one
	//! \param entity database entity to add or update
	//! \return status code of the operation
	EDF_EDbOperationStatusCode AddOrUpdate(notnull EDF_DbEntity entity)
	{
		if (!entity.HasId())
			entity.SetId(EDF_DbEntityIdGenerator.Generate());

		return m_Driver.AddOrUpdate(entity);
	}

	//------------------------------------------------------------------------------------------------
	//! Remove an existing database entity
	//! \param entity database to remove
	//! \return status code of the operation, will fail if entity did not exist
	EDF_EDbOperationStatusCode Remove(notnull EDF_DbEntity entity)
	{
		// Save as vars because script vm invalid pointer bug if passed diretly
		typename type = entity.Type();
		string id = entity.GetId();
		return m_Driver.Remove(type, id);
	}

	//------------------------------------------------------------------------------------------------
	//! Remove an existing database entity
	//! \param entityType typename of the database entity
	//! \param entityId unique id of the entity to remove
	//! \return status code of the operation, will fail if entity did not exist
	EDF_EDbOperationStatusCode Remove(typename entityType, string entityId)
	{
		return m_Driver.Remove(entityType, entityId);
	}

	//------------------------------------------------------------------------------------------------
	//! Find database entities
	//! \param entityType typename of the database entity
	//! \param condition find condition to search by
	//! \param orderBy field paths in dotnotation to order by e.g. {{"child.subField", "ASC"}, {"thenByField", "DESC"}}
	//! \param limit maximum amount of returned. Limit is applied on those that matched the conditions
	//! \param offset used together with limit to offset the result limit count. Can be used to paginate the loading.
	//! \return find result buffer containing status code and result entities on success
	EDF_DbFindResultMultiple<EDF_DbEntity> FindAll(typename entityType, EDF_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1)
	{
		return m_Driver.FindAll(entityType, condition, orderBy, limit, offset);
	}

	//------------------------------------------------------------------------------------------------
	//! Adds a new entry to the database or updates an existing one asynchronously
	//! \param entity database entity to add or update
	//! \param callback optional callback to handle the operation result
	void AddOrUpdateAsync(notnull EDF_DbEntity entity, EDF_DbOperationStatusOnlyCallback callback = null)
	{
		if (!entity.HasId())
			entity.SetId(EDF_DbEntityIdGenerator.Generate());

		m_Driver.AddOrUpdateAsync(entity, callback);
	}

	//------------------------------------------------------------------------------------------------
	//! Remove an existing database entity asynchronously
	//! \param entity database to remove
	//! \param callback optional callback to handle the operation result
	void RemoveAsync(notnull EDF_DbEntity entity, EDF_DbOperationStatusOnlyCallback callback = null)
	{
		// Save as vars because script vm invalid pointer bug if passed diretly
		typename entityType = entity.Type();
		string entityId = entity.GetId();

		m_Driver.RemoveAsync(entityType, entityId, callback);
	}

	//------------------------------------------------------------------------------------------------
	//! Remove an existing database entity asynchronously
	//! \param entityType typename of the database entity
	//! \param entityId unique id of the entity to remove
	//! \param callback optional callback to handle the operation result
	void RemoveAsync(typename entityType, string entityId, EDF_DbOperationStatusOnlyCallback callback = null)
	{
		m_Driver.RemoveAsync(entityType, entityId, callback);
	}

	//------------------------------------------------------------------------------------------------
	//! Find database entities asynchronously
	//! \param entityType typename of the database entity
	//! \param condition find condition to search by
	//! \param orderBy field paths in dotnotation to order by e.g. {{"child.subField", "ASC"}, {"thenByField", "DESC"}}
	//! \param limit maximum amount of returned. Limit is applied on those that matched the conditions
	//! \param offset used together with limit to offset the result limit count. Can be used to paginate the loading.
	//! \param callback optional callback to handle the operation result
	void FindAllAsync(typename entityType, EDF_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1, EDF_DbFindCallbackBase callback = null)
	{
		m_Driver.FindAllAsync(entityType, condition, orderBy, limit, offset, callback);
	}

	//------------------------------------------------------------------------------------------------
	static EDF_DbContext Create(notnull EDF_DbConnectionInfoBase connectionInfo)
	{
		typename driverType = EDF_DbConnectionInfoDriverType.GetDriverType(connectionInfo.Type());
		EDF_DbDriver driver = EDF_DbDriver.Cast(driverType.Spawn());
		if (!driver || !driver.Initialize(connectionInfo))
		{
			Debug.Error(string.Format("Unable to initialize database driver of type '%1'.", driverType));
			return null;
		}

		return new EDF_DbContext(driver);
	}

	//------------------------------------------------------------------------------------------------
	//! Use EDF_DbContext::Create(EDF_DbConnectionInfoBase) to get a context instance.
	protected void EDF_DbContext(notnull EDF_DbDriver driver)
	{
		m_Driver = driver;
	}

	//------------------------------------------------------------------------------------------------
	void ~EDF_DbContext()
	{
		m_Driver = null;
	}
};
