class EDF_DbEntityHelper<Class TEntityType>
{
	//------------------------------------------------------------------------------------------------
	//! Get the repository responsible to handling the templated entity type
	//! \param dbContext Database context to wrap with the repository
	//! \return database repository instance or null if no type could be found.
	static EDF_DbRepository<TEntityType> GetRepository(notnull EDF_DbContext dbContext)
	{
		typename repositoryType = EDF_DbRepositoryRegistration.Get(TEntityType); // Can not be inlined or else illegal read happens because of bug in scriptvm.
		if (!repositoryType)
		{
			string repositoryTypeStr = string.Format("EDF_DbRepository<%1>", TEntityType);
			Debug.Error(string.Format("Tried to get unknown entity repository type '%1'. Make sure you use it somewhere in your code e.g.: '%1 repository = ...;'", repositoryTypeStr));
		}

		return EDF_DbRepository<TEntityType>.Cast(EDF_DbRepositoryFactory.GetRepository(repositoryType, dbContext));
	}
};

class EDF_DbRepositoryHelper<Class TRepositoryType>
{
	//------------------------------------------------------------------------------------------------
	static TRepositoryType Get(notnull EDF_DbContext dbContext)
	{
		return TRepositoryType.Cast(EDF_DbRepositoryFactory.GetRepository(TRepositoryType, dbContext));
	}
};
