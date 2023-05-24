class EDF_DbRepositoryTests : TestSuite
{
	static ref EDF_DbContext m_pDbContext;

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
	void Setup()
	{
		EDF_InMemoryDbConnectionInfo connectInfo();
		connectInfo.m_sDatabaseName = "DbRepositoryTests";
		m_pDbContext = EDF_DbContext.Create(connectInfo);
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.TearDown)]
	void TearDown()
	{
		m_pDbContext = null;
	}
};

class EDF_Test_DbEntityRepositoryEntity : EDF_DbEntity
{
	int m_iIntValue;

	//------------------------------------------------------------------------------------------------
	void EDF_Test_DbEntityRepositoryEntity(string id, int intValue)
	{
		SetId(id);
		m_iIntValue = intValue;
	}
};

[EDF_DbRepositoryRegistration()]
class EDF_Test_DbEntityRepositoryEntityRepository : EDF_DbRepository<EDF_Test_DbEntityRepositoryEntity>
{
	EDF_DbFindResultSingle<EDF_Test_DbEntityRepositoryEntity> FindByIntValue(int value)
	{
		return FindFirst(EDF_DbFind.Field("m_iIntValue").Equals(value));
	}
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbRepositoryTests")]
TestResultBase EDF_Test_DbEntityRepository_AddOrUpdate_NewEntityFindByIntValue_Found()
{
	// Arrange
	EDF_Test_DbEntityRepositoryEntityRepository repository = EDF_DbRepositoryHelper<EDF_Test_DbEntityRepositoryEntityRepository>.Get(EDF_DbRepositoryTests.m_pDbContext);

	EDF_Test_DbEntityRepositoryEntity entity("TEST0000-0000-0001-0000-000000000001", 1001)

	// Act
	repository.AddOrUpdate(entity);

	// Assert
	EDF_TestResult result(repository.FindByIntValue(1001).GetEntity().GetId() == "TEST0000-0000-0001-0000-000000000001");

	// Cleanup
	repository.Remove(entity);

	return result;
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbRepositoryTests")]
TestResultBase EDF_Test_DbEntityRepository_Remove_ByInstance_Removed()
{
	// Arrange
	EDF_DbRepository<EDF_Test_DbEntityRepositoryEntity> repository = EDF_DbEntityHelper<EDF_Test_DbEntityRepositoryEntity>.GetRepository(EDF_DbRepositoryTests.m_pDbContext);

	EDF_Test_DbEntityRepositoryEntity entity("TEST0000-0000-0001-0000-000000000002", 1002);
	repository.AddOrUpdate(entity);

	// Act
	EDF_EDbOperationStatusCode statusCode = repository.Remove(entity);

	// Assert
	EDF_TestResult result(
		statusCode == EDF_EDbOperationStatusCode.SUCCESS &&
		!repository.Find("TEST0000-0000-0001-0000-000000000002").GetEntity());

	// Cleanup
	repository.Remove(entity);

	return result;
};
