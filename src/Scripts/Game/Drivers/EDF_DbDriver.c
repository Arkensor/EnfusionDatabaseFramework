class EDF_DbDriver
{
	static bool s_bForceBlocking; //Used to forced drivers to rely on sync api during session teardown

	//------------------------------------------------------------------------------------------------
	bool Initalize(notnull EDF_DbConnectionInfoBase connectionInfo);

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
}
