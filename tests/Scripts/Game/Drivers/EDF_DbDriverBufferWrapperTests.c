/*
class EDF_DbDriverBufferWrapperTests : TestSuite
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
	void Setup()
	{
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.TearDown)]
	void TearDown()
	{
	}
};

class EDF_Test_DbDriverBufferWrapperEntity : EDF_DbEntity
{
	int m_iValue;

	//------------------------------------------------------------------------------------------------
	static EDF_Test_DbDriverBufferWrapperEntity Create(string id, int value)
	{
		EDF_Test_DbDriverBufferWrapperEntity instance();
		instance.SetId(id);
		instance.m_iValue = value;
		return instance;
	}
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbDriverBufferWrapperTests")]
TestResultBase EDF_Test_DbDriverBufferWrapper_AddOrUpdateFindById_NotFlushed_Returned()
{
	// Arrange
	EDF_InMemoryDbDriver driver();
	EDF_InMemoryDbConnectionInfo connectInfo();
	connectInfo.m_sDatabaseName = "Testing";
	driver.initialize(connectInfo);
	EDF_DbDriverBufferWrapper bufferedDriver(driver);

	auto entity = EDF_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000001", 42);

	// Act
	EDF_EDbOperationStatusCode statusCode = bufferedDriver.AddOrUpdate(entity);

	// Assert
	if (statusCode != EDF_EDbOperationStatusCode.SUCCESS)
		return new EDF_TestResult(false);

	array<ref EDF_DbEntity> results = bufferedDriver.FindAll(EDF_Test_DbDriverBufferWrapperEntity, EDF_DbFind.Id().Equals(entity.GetId())).GetEntities();
	if (results.Count() != 1)
		return new EDF_TestResult(false);

	EDF_Test_DbDriverBufferWrapperEntity resultEntity = EDF_Test_DbDriverBufferWrapperEntity.Cast(results.Get(0));
	return new EDF_TestResult(
		resultEntity.GetId() == entity.GetId() &&
		resultEntity.m_iValue == entity.m_iValue);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbDriverBufferWrapperTests")]
TestResultBase EDF_Test_DbDriverBufferWrapper_AddOrUpdateInPlaceTwice_Flushed_Returned()
{
	// Arrange
	EDF_InMemoryDbDriver driver();
	EDF_InMemoryDbConnectionInfo connectInfo();
	connectInfo.m_sDatabaseName = "Testing";
	driver.initialize(connectInfo);
	EDF_DbDriverBufferWrapper bufferedDriver(driver);

	auto entity = EDF_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000001", 42);
	auto updatedEntity = EDF_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000001", 1337);

	// Act
	EDF_EDbOperationStatusCode statusCode1 = bufferedDriver.AddOrUpdate(entity);
	EDF_EDbOperationStatusCode statusCode2 = bufferedDriver.AddOrUpdate(updatedEntity);
	bufferedDriver.Flush(forceBlocking: true);

	// Assert
	if (statusCode1 != EDF_EDbOperationStatusCode.SUCCESS||
		statusCode2 != EDF_EDbOperationStatusCode.SUCCESS)
	{
		return new EDF_TestResult(false);
	}

	array<ref EDF_DbEntity> results = bufferedDriver.FindAll(EDF_Test_DbDriverBufferWrapperEntity, EDF_DbFind.Id().Equals(updatedEntity.GetId())).GetEntities();
	if (results.Count() != 1)
		return new EDF_TestResult(false);

	EDF_Test_DbDriverBufferWrapperEntity resultEntity = EDF_Test_DbDriverBufferWrapperEntity.Cast(results.Get(0));
	return new EDF_TestResult(
		resultEntity.GetId() == updatedEntity.GetId() &&
		resultEntity.m_iValue == updatedEntity.m_iValue);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbDriverBufferWrapperTests")]
TestResultBase EDF_Test_DbDriverBufferWrapper_AddOrUpdateFindAllLimited_NotFlushed_ReturnedOnlyInLimit()
{
	// Arrange
	EDF_InMemoryDbDriver driver();
	EDF_InMemoryDbConnectionInfo connectInfo();
	connectInfo.m_sDatabaseName = "Testing";
	driver.initialize(connectInfo);
	EDF_DbDriverBufferWrapper bufferedDriver(driver);

	auto entity1 = EDF_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000001", 42);
	auto entity2 = EDF_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000002", 43);
	auto entity3 = EDF_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000003", 44);

	// Act
	EDF_EDbOperationStatusCode statusCode1 = bufferedDriver.AddOrUpdate(entity1);
	EDF_EDbOperationStatusCode statusCode2 = bufferedDriver.AddOrUpdate(entity2);
	EDF_EDbOperationStatusCode statusCode3 = bufferedDriver.AddOrUpdate(entity3);

	// Assert
	if (statusCode1 != EDF_EDbOperationStatusCode.SUCCESS||
		statusCode2 != EDF_EDbOperationStatusCode.SUCCESS||
		statusCode3 != EDF_EDbOperationStatusCode.SUCCESS)
	{
		return new EDF_TestResult(false);
	}

	array<ref EDF_DbEntity> results = bufferedDriver.FindAll(EDF_Test_DbDriverBufferWrapperEntity, limit: 2).GetEntities();
	if (results.Count() != 2)
		return new EDF_TestResult(false);

	EDF_Test_DbDriverBufferWrapperEntity resultEntity1 = EDF_Test_DbDriverBufferWrapperEntity.Cast(results.Get(0));
	EDF_Test_DbDriverBufferWrapperEntity resultEntity2 = EDF_Test_DbDriverBufferWrapperEntity.Cast(results.Get(1));
	return new EDF_TestResult(
		resultEntity1.GetId() == entity1.GetId() &&
		resultEntity2.GetId() == entity2.GetId());
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbDriverBufferWrapperTests")]
TestResultBase EDF_Test_DbDriverBufferWrapper_AddOrUpdateFindMultipleByIdDesc_NotFlushed_Returned()
{
	// Arrange
	EDF_InMemoryDbDriver driver();
	EDF_InMemoryDbConnectionInfo connectInfo();
	connectInfo.m_sDatabaseName = "Testing";
	driver.initialize(connectInfo);
	EDF_DbDriverBufferWrapper bufferedDriver(driver);

	auto entity1 = EDF_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000001", 42);
	auto entity2 = EDF_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000002", 43);
	auto entity3 = EDF_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000003", 44);

	// Act
	EDF_EDbOperationStatusCode statusCode1 = bufferedDriver.AddOrUpdate(entity1);
	EDF_EDbOperationStatusCode statusCode2 = bufferedDriver.AddOrUpdate(entity2);
	EDF_EDbOperationStatusCode statusCode3 = bufferedDriver.AddOrUpdate(entity3);

	// Assert
	if (statusCode1 != EDF_EDbOperationStatusCode.SUCCESS||
		statusCode2 != EDF_EDbOperationStatusCode.SUCCESS||
		statusCode3 != EDF_EDbOperationStatusCode.SUCCESS)
	{
		return new EDF_TestResult(false);
	}

	array<ref EDF_DbEntity> results = bufferedDriver.FindAll(
		EDF_Test_DbDriverBufferWrapperEntity,
		EDF_DbFind.Id().EqualsAnyOf(EDF_DbValues<string>.From({"TEST0000-0000-0001-0000-000000000002", "TEST0000-0000-0001-0000-000000000003"})),
		orderBy: {{"m_iValue", EDF_EDbEntitySortDirection.DESCENDING}}
	).GetEntities();

	if (results.Count() != 2)
		return new EDF_TestResult(false);

	EDF_Test_DbDriverBufferWrapperEntity resultEntity1 = EDF_Test_DbDriverBufferWrapperEntity.Cast(results.Get(0));
	EDF_Test_DbDriverBufferWrapperEntity resultEntity2 = EDF_Test_DbDriverBufferWrapperEntity.Cast(results.Get(1));
	return new EDF_TestResult(
		resultEntity1.GetId() == entity3.GetId() &&
		resultEntity2.GetId() == entity2.GetId());
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbDriverBufferWrapperTests")]
TestResultBase EDF_Test_DbDriverBufferWrapper_AddRemove_NotFlushed_NotRetured()
{
	// Arrange
	EDF_InMemoryDbDriver driver();
	EDF_InMemoryDbConnectionInfo connectInfo();
	connectInfo.m_sDatabaseName = "Testing";
	driver.initialize(connectInfo);
	EDF_DbDriverBufferWrapper bufferedDriver(driver);

	auto entity = EDF_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000001", 42);

	// Act
	EDF_EDbOperationStatusCode statusCode1 = bufferedDriver.AddOrUpdate(entity);
	EDF_EDbOperationStatusCode statusCode2 = bufferedDriver.Remove(EDF_Test_DbDriverBufferWrapperEntity, entity.GetId());

	// Assert
	if (statusCode1 != EDF_EDbOperationStatusCode.SUCCESS ||
		statusCode2 != EDF_EDbOperationStatusCode.SUCCESS)
	{
		return new EDF_TestResult(false);
	}

	array<ref EDF_DbEntity> results = bufferedDriver.FindAll(EDF_Test_DbDriverBufferWrapperEntity, EDF_DbFind.Id().Equals(entity.GetId())).GetEntities();
	return new EDF_TestResult(results.Count() == 0);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbDriverBufferWrapperTests")]
TestResultBase EDF_Test_DbDriverBufferWrapper_AddRemove_Flushed_NotRetured()
{
	// Arrange
	EDF_InMemoryDbDriver driver();
	EDF_InMemoryDbConnectionInfo connectInfo();
	connectInfo.m_sDatabaseName = "Testing";
	driver.initialize(connectInfo);
	EDF_DbDriverBufferWrapper bufferedDriver(driver);

	auto entity = EDF_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000001", 42);

	// Act
	EDF_EDbOperationStatusCode statusCode1 = bufferedDriver.AddOrUpdate(entity);
	EDF_EDbOperationStatusCode statusCode2 = bufferedDriver.Remove(EDF_Test_DbDriverBufferWrapperEntity, entity.GetId());
	bufferedDriver.Flush(forceBlocking: true);

	// Assert
	if (statusCode1 != EDF_EDbOperationStatusCode.SUCCESS ||
		statusCode2 != EDF_EDbOperationStatusCode.SUCCESS)
	{
		return new EDF_TestResult(false);
	}

	array<ref EDF_DbEntity> results = bufferedDriver.FindAll(EDF_Test_DbDriverBufferWrapperEntity, EDF_DbFind.Id().Equals(entity.GetId())).GetEntities();
	return new EDF_TestResult(results.Count() == 0);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbDriverBufferWrapperTests")]
TestResultBase EDF_Test_DbDriverBufferWrapper_AddFlushRemove_NotFlushed_NotRetured()
{
	// Arrange
	EDF_InMemoryDbDriver driver();
	EDF_InMemoryDbConnectionInfo connectInfo();
	connectInfo.m_sDatabaseName = "Testing";
	driver.initialize(connectInfo);
	EDF_DbDriverBufferWrapper bufferedDriver(driver);

	auto entity = EDF_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000001", 42);

	// Act
	EDF_EDbOperationStatusCode statusCode1 = bufferedDriver.AddOrUpdate(entity);
	bufferedDriver.Flush(forceBlocking: true);
	EDF_EDbOperationStatusCode statusCode2 = bufferedDriver.Remove(EDF_Test_DbDriverBufferWrapperEntity, entity.GetId());

	// Assert
	if (statusCode1 != EDF_EDbOperationStatusCode.SUCCESS ||
		statusCode2 != EDF_EDbOperationStatusCode.SUCCESS)
	{
		return new EDF_TestResult(false);
	}

	array<ref EDF_DbEntity> bufferedResults = bufferedDriver.FindAll(EDF_Test_DbDriverBufferWrapperEntity, EDF_DbFind.Id().Equals(entity.GetId())).GetEntities();
	array<ref EDF_DbEntity> dbResults = driver.FindAll(EDF_Test_DbDriverBufferWrapperEntity, EDF_DbFind.Id().Equals(entity.GetId())).GetEntities();
	return new EDF_TestResult(bufferedResults.Count() == 0 && dbResults.Count() == 1);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbDriverBufferWrapperTests")]
TestResultBase EDF_Test_DbDriverBufferWrapper_FindAllPaginatedOrdered_HalfFlushed_CorrectOrder()
{
	// Arrange
	EDF_InMemoryDbDriver driver();
	EDF_InMemoryDbConnectionInfo connectInfo();
	connectInfo.m_sDatabaseName = "Testing";
	driver.initialize(connectInfo);
	EDF_DbDriverBufferWrapper bufferedDriver(driver);

	auto entity1 = EDF_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000001", 11);
	auto entity2 = EDF_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000002", 22);
	auto entity3 = EDF_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000003", 33); // Deleted, Not flushed
	auto entity4 = EDF_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000004", 44); // Deleted, Not flushed
	auto entity5 = EDF_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000005", 55);
	auto entity6 = EDF_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000006", 66);
	auto entity7 = EDF_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000007", 77);
	auto entity8 = EDF_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000008", 88); //Not flushed
	auto entity9 = EDF_Test_DbDriverBufferWrapperEntity.Create("TEST0000-0000-0001-0000-000000000009", 99); //Not flushed

	// Act
	bufferedDriver.AddOrUpdate(entity1);
	bufferedDriver.AddOrUpdate(entity2);
	//bufferedDriver.AddOrUpdate(entity3);
	//bufferedDriver.AddOrUpdate(entity4);
	bufferedDriver.AddOrUpdate(entity5);
	bufferedDriver.AddOrUpdate(entity6);
	bufferedDriver.AddOrUpdate(entity7);
	bufferedDriver.Flush(forceBlocking: true);
	//bufferedDriver.Remove(EDF_Test_DbDriverBufferWrapperEntity, entity3.GetId());
	//bufferedDriver.Remove(EDF_Test_DbDriverBufferWrapperEntity, entity4.GetId());
	bufferedDriver.AddOrUpdate(entity3);
	bufferedDriver.AddOrUpdate(entity4);
	bufferedDriver.AddOrUpdate(entity8);
	bufferedDriver.AddOrUpdate(entity9);

	// Assert

	EDF_DbFindCondition condition = null; //EDF_DbFind.Id().Equals(entity.GetId())
	array<ref TStringArray> orderBy = {{"m_iValue", EDF_EDbEntitySortDirection.ASCENDING}}; //null;

	array<ref EDF_DbEntity> page1 = bufferedDriver.FindAll(EDF_Test_DbDriverBufferWrapperEntity, condition, orderBy, offset: 0, limit: 3).GetEntities();
	array<ref EDF_DbEntity> page2 = bufferedDriver.FindAll(EDF_Test_DbDriverBufferWrapperEntity, condition, orderBy, offset: 3, limit: 3).GetEntities();
	array<ref EDF_DbEntity> page3 = bufferedDriver.FindAll(EDF_Test_DbDriverBufferWrapperEntity, condition, orderBy, offset: 6, limit: 3).GetEntities();

	//array<ref EDF_DbEntity> page1 = bufferedDriver.FindAll(EDF_Test_DbDriverBufferWrapperEntity, condition, orderBy, offset: 0, limit: 2).GetEntities();
	//array<ref EDF_DbEntity> page2 = bufferedDriver.FindAll(EDF_Test_DbDriverBufferWrapperEntity, condition, orderBy, offset: 2, limit: 2).GetEntities();
	//array<ref EDF_DbEntity> page3 = bufferedDriver.FindAll(EDF_Test_DbDriverBufferWrapperEntity, condition, orderBy, offset: 4, limit: 2).GetEntities();
	//array<ref EDF_DbEntity> page4 = bufferedDriver.FindAll(EDF_Test_DbDriverBufferWrapperEntity, condition, orderBy, offset: 6, limit: 2).GetEntities();
	//array<ref EDF_DbEntity> page5 = bufferedDriver.FindAll(EDF_Test_DbDriverBufferWrapperEntity, condition, orderBy, offset: 8, limit: 2).GetEntities();

	return new EDF_TestResult(true);
};
*/
