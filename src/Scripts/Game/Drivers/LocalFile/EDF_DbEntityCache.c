class EDF_DbEntityCache
{
	ref map<string, ref EDF_DbEntity> m_EntityInstances;

	// TODO sorted linked list with last queried for, drop by id from cache if not asked for in X time

	//------------------------------------------------------------------------------------------------
	void Add(EDF_DbEntity entity)
	{
		m_EntityInstances.Set(entity.GetId(), entity);
	}

	//------------------------------------------------------------------------------------------------
	void Remove(string entityId)
	{
		m_EntityInstances.Remove(entityId);
	}


	EDF_DbEntity Get(string entityId)
	{
		return m_EntityInstances.Get(entityId);
	}

	//------------------------------------------------------------------------------------------------
	void EDF_DbEntityCache()
	{
		m_EntityInstances = new map<string, ref EDF_DbEntity>();
	}
}
