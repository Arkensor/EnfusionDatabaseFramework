/*sealed*/ class EDF_DbRepositoryFactory
{
	private static ref map<string, ref EDF_DbRepositoryBase> s_mRepositoryCache;

	//------------------------------------------------------------------------------------------------
	static EDF_DbRepositoryBase GetRepository(typename repositoryType, notnull EDF_DbContext dbContext)
	{
		EDF_DbRepositoryBase repository = null;

		if (!s_mRepositoryCache)
			s_mRepositoryCache = new map<string, ref EDF_DbRepositoryBase>();

		string cacheKey = string.Format("%1:%2", repositoryType.ToString(), dbContext);

		repository = s_mRepositoryCache.Get(cacheKey);

		// No valid repository found, create a new one
		if (!repository)
		{
			repository = EDF_DbRepositoryBase.Cast(repositoryType.Spawn());

			if (repository)
				repository.SetDbContext(dbContext);
		}

		// Cache repository to be re-used, even if null because second time it would still create an invalid one
		s_mRepositoryCache.Set(cacheKey, repository);

		return repository;
	}

	//------------------------------------------------------------------------------------------------
	static void ResetCache()
	{
		s_mRepositoryCache = null;
	}
};
