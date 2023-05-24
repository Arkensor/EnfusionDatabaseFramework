typedef map<string, ref EDF_DbEntity> EDF_InMemoryDatabaseTable;

class EDF_InMemoryDatabase
{
	string m_DbName;
	ref map<string, ref EDF_InMemoryDatabaseTable> m_EntityTables;

	//------------------------------------------------------------------------------------------------
	void AddOrUpdate(notnull EDF_DbEntity entity)
	{
		EDF_InMemoryDatabaseTable table = GetTable(entity.Type());

		if (table)
			table.Set(entity.GetId(), entity);
	}

	//------------------------------------------------------------------------------------------------
	void Remove(typename entityType, string entityId)
	{
		EDF_InMemoryDatabaseTable table = GetTable(entityType);
		if (table)
			table.Remove(entityId);
	}

	//------------------------------------------------------------------------------------------------
	EDF_DbEntity Get(typename entityType, string entityId)
	{
		EDF_InMemoryDatabaseTable table = GetTable(entityType);
		if (table)
			return table.Get(entityId);

		return null;
	}

	//------------------------------------------------------------------------------------------------
	array<ref EDF_DbEntity> GetAll(typename entityType)
	{
		EDF_InMemoryDatabaseTable table = GetTable(entityType);

		array<ref EDF_DbEntity> result();

		if (table)
		{
			for (int nElement = 0; nElement < table.Count(); nElement++)
			{
				result.Insert(table.GetElement(nElement));
			}
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------
	protected EDF_InMemoryDatabaseTable GetTable(typename entityType)
	{
		string typeKey = entityType.ToString();

		EDF_InMemoryDatabaseTable table = m_EntityTables.Get(typeKey);

		if (!table)
		{
			table = new EDF_InMemoryDatabaseTable();
			m_EntityTables.Set(typeKey, table);
		}

		return table;
	}

	//------------------------------------------------------------------------------------------------
	void EDF_InMemoryDatabase(string dbName)
	{
		m_DbName = dbName;
		m_EntityTables = new map<string, ref EDF_InMemoryDatabaseTable>();
	}
}
