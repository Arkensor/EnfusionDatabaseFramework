[EDF_DbConnectionInfoDriverType(EDF_JsonFileDbDriver), BaseContainerProps()]
class EDF_FileDbDriverInfoBase : EDF_DbConnectionInfoBase
{
	[Attribute(defvalue: "1", desc: "Cache read results from disk to reduce file IO operations on consecutive queries.")]
	bool m_bUseCache;

	//------------------------------------------------------------------------------------------------
	override void ReadOptions(string connectionString)
	{
		super.ReadOptions(connectionString);
		connectionString.ToLower();
		connectionString.Replace(" = ", "=");
		m_bUseCache = connectionString.Contains("cache=true");
	}
};

class EDF_FileDbDriverBase : EDF_DbDriver
{
	const string DB_BASE_DIR = "$profile:/.db"; //Can be changed through modded class if you want to!

	// Static props that can be shared across all driver instances
	protected ref EDF_DbEntityCache m_pEntityCache;
	protected ref map<typename, ref set<string>> m_mEntityIdsyCache;

	protected string m_sDbDir;
	protected bool m_bUseCache;

	//------------------------------------------------------------------------------------------------
	override bool Initialize(notnull EDF_DbConnectionInfoBase connectionInfo)
	{
		if (!m_pEntityCache)
			m_pEntityCache = new EDF_DbEntityCache();

		if (!m_mEntityIdsyCache)
			m_mEntityIdsyCache = new map<typename, ref set<string>>();

		auto fileConnectInfo = EDF_FileDbDriverInfoBase.Cast(connectionInfo);
		m_bUseCache = fileConnectInfo.m_bUseCache;

		string dbName = fileConnectInfo.m_sDatabaseName;
		m_sDbDir = string.Format("%1/%2", DB_BASE_DIR, dbName);
		FileIO.MakeDirectory(DB_BASE_DIR);
		FileIO.MakeDirectory(m_sDbDir);

		return FileIO.FileExists(m_sDbDir);
	}

	//------------------------------------------------------------------------------------------------
	override EDF_EDbOperationStatusCode AddOrUpdate(notnull EDF_DbEntity entity)
	{
		if (!entity.HasId())
			return EDF_EDbOperationStatusCode.FAILURE_ID_NOT_SET;

		FileIO.MakeDirectory(_GetTypeDirectory(entity.Type()));

		EDF_EDbOperationStatusCode statusCode = WriteToDisk(entity);
		if (statusCode != EDF_EDbOperationStatusCode.SUCCESS)
			return statusCode;

		if (m_bUseCache)
			m_pEntityCache.Add(entity);

		// Add id to pool of all known ids
		GetIdsByType(entity.Type()).Insert(entity.GetId());

		return EDF_EDbOperationStatusCode.SUCCESS;
	}

	//------------------------------------------------------------------------------------------------
	override EDF_EDbOperationStatusCode Remove(typename entityType, string entityId)
	{
		if (!entityId) return EDF_EDbOperationStatusCode.FAILURE_ID_NOT_SET;

		EDF_EDbOperationStatusCode statusCode = DeleteFromDisk(entityType, entityId);
		if (statusCode != EDF_EDbOperationStatusCode.SUCCESS)
			return statusCode;

		if (m_bUseCache)
			m_pEntityCache.Remove(entityId);

		// Remove id from pool of all known ids
		set<string> ids = GetIdsByType(entityType);
		ids.RemoveItem(entityId);

		// If collection of that entity type is empty remove the folder too to keep the file structure clean
		if (ids.IsEmpty())
			FileIO.DeleteFile(_GetTypeDirectory(entityType));

		return EDF_EDbOperationStatusCode.SUCCESS;
	}

	//------------------------------------------------------------------------------------------------
	override EDF_DbFindResultMultiple<EDF_DbEntity> FindAll(typename entityType, EDF_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1)
	{
		// See if we can only load selected few entities by id or we need the entire collection to search through
		set<string> loadIds(), skipIds();
		bool needsFilter = false;
		if (!EDF_DbFindConditionEvaluator.CollectConditionIds(condition, loadIds, skipIds) || loadIds.IsEmpty() || !skipIds.IsEmpty())
		{
			// Condition(s) require more information than just ids so all need to be loaded and also filtered by condition
			loadIds = GetIdsByType(entityType, skipIds);
			needsFilter = true;
		}

		array<ref EDF_DbEntity> entities();

		foreach (string entityId : loadIds)
		{
			EDF_DbEntity entity;

			if (m_bUseCache)
				entity = m_pEntityCache.Get(entityId);

			if (!entity)
			{
				EDF_EDbOperationStatusCode statusCode = ReadFromDisk(entityType, entityId, entity);

				if (statusCode != EDF_EDbOperationStatusCode.SUCCESS || !entity)
					continue;

				if (m_bUseCache)
					m_pEntityCache.Add(entity);
			}

			entities.Insert(entity);
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

			resultEntites.Insert(entity);
		}

		return new EDF_DbFindResultMultiple<EDF_DbEntity>(EDF_EDbOperationStatusCode.SUCCESS, resultEntites);
	}

	//------------------------------------------------------------------------------------------------
	override void AddOrUpdateAsync(notnull EDF_DbEntity entity, EDF_DbOperationStatusOnlyCallback callback = null)
	{
		// FileIO is blocking, re-use sync api
		EDF_EDbOperationStatusCode statusCode = AddOrUpdate(entity);
		if (callback)
			callback.Invoke(statusCode);
	}

	//------------------------------------------------------------------------------------------------
	override void RemoveAsync(typename entityType, string entityId, EDF_DbOperationStatusOnlyCallback callback = null)
	{
		// FileIO is blocking, re-use sync api
		EDF_EDbOperationStatusCode statusCode = Remove(entityType, entityId);
		if (callback)
			callback.Invoke(statusCode);
	}

	//------------------------------------------------------------------------------------------------
	override void FindAllAsync(typename entityType, EDF_DbFindCondition condition = null, array<ref TStringArray> orderBy = null, int limit = -1, int offset = -1, EDF_DbFindCallbackBase callback = null)
	{
		// FileIO is blocking, re-use sync api
		EDF_DbFindResultMultiple<EDF_DbEntity> findResults = FindAll(entityType, condition, orderBy, limit, offset);
		if (callback)
			callback.Invoke(findResults.GetStatusCode(), findResults.GetEntities());
	}

	//------------------------------------------------------------------------------------------------
	protected set<string> GetIdsByType(typename entityType, set<string> skipIds = null)
	{
		set<string> ids = m_mEntityIdsyCache.Get(entityType);
		if (!ids)
		{
			ids = GetIdsOnDisk(entityType, skipIds);
			m_mEntityIdsyCache.Set(entityType, ids);
		}
		return ids;
	}

	//------------------------------------------------------------------------------------------------
	protected set<string> GetIdsOnDisk(typename entityType, set<string> skipIds = null)
	{
		EDF_FileDbDriverFindIdsCallback callback();
		System.FindFiles(callback.AddFile, _GetTypeDirectory(entityType), GetFileExtension());

		set<string> ids();
		foreach (string id : callback.m_Ids)
		{
			if (!skipIds || !skipIds.Contains(id))
				ids.Insert(id);
		}
		return ids;
	}

	//------------------------------------------------------------------------------------------------
	string _GetTypeDirectory(typename entityType)
	{
		string entityName = EDF_DbName.Get(entityType);

		if (entityName.EndsWith("y"))
		{
			entityName = string.Format("%1ies", entityName.Substring(0, entityName.Length() - 1));
		}
		else if(!entityName.EndsWith("s"))
		{
			entityName += "s";
		}

		return string.Format("%1/%2", m_sDbDir, entityName);
	}

	//------------------------------------------------------------------------------------------------
	protected string GetFileExtension();

	//------------------------------------------------------------------------------------------------
	protected EDF_EDbOperationStatusCode WriteToDisk(EDF_DbEntity entity);

	//------------------------------------------------------------------------------------------------
	protected EDF_EDbOperationStatusCode ReadFromDisk(typename entityType, string entityId, out EDF_DbEntity entity);

	//------------------------------------------------------------------------------------------------
	protected EDF_EDbOperationStatusCode DeleteFromDisk(typename entityType, string entityId)
	{
		string file = string.Format("%1/%2%3", _GetTypeDirectory(entityType), entityId, GetFileExtension());

		if (!FileIO.FileExists(file))
			return EDF_EDbOperationStatusCode.FAILURE_ID_NOT_FOUND;

		if (!FileIO.DeleteFile(file))
			return EDF_EDbOperationStatusCode.FAILURE_DB_UNAVAILABLE;

		return EDF_EDbOperationStatusCode.SUCCESS;
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbEntityCache _GetEntityCache()
	{
		return m_pEntityCache;
	}

	//------------------------------------------------------------------------------------------------
	map<typename, ref set<string>> _GetEntityIds()
	{
		return m_mEntityIdsyCache;
	}
};

class EDF_FileDbDriverFindIdsCallback
{
	ref array<string> m_Ids = {};

	//------------------------------------------------------------------------------------------------
	void AddFile(string fileName, FileAttribute attributes = 0, string filesystem = string.Empty)
	{
		fileName = FilePath.StripPath(fileName);
		fileName = FilePath.StripExtension(fileName);

		// Not a UUID of 36 chars length
		if (fileName.IsEmpty() || fileName.Length() != 36)
			return;

		m_Ids.Insert(fileName);
	}
};
