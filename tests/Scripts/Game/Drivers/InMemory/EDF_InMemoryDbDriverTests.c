class EDF_InMemoryDbDriverTests : TestSuite
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
}

class EDF_Test_InMemoryDbDriverEntity : EDF_DbEntity
{
	float m_fFloatValue;
	string m_sStringValue;

	//------------------------------------------------------------------------------------------------
	void EDF_Test_InMemoryDbDriverEntity(string id, float floatValue, string stringValue)
	{
		SetId(id);
		m_fFloatValue = floatValue;
		m_sStringValue = stringValue;
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_InMemoryDbDriverTests")]
TestResultBase EDF_Test_InMemoryDbDriver_AddOrUpdate_NewEntity_Inserted()
{
	// Arrange
	EDF_InMemoryDbDriver driver();
	EDF_InMemoryDbConnectionInfo connectInfo();
	connectInfo.m_sDatabaseName = "Testing";
	driver.Initialize(connectInfo);

	EDF_Test_InMemoryDbDriverEntity entity("TEST0000-0000-0001-0000-000000000001", 42.42, "Hello World");

	// Act
	EDF_EDbOperationStatusCode statusCode = driver.AddOrUpdate(entity);

	// Assert
	if (!statusCode == EDF_EDbOperationStatusCode.SUCCESS) return new EDF_TestResult(false);

	array<ref EDF_DbEntity> results = driver.FindAll(EDF_Test_InMemoryDbDriverEntity, EDF_DbFind.Id().Equals(entity.GetId())).GetEntities();

	if (results.Count() != 1) return new EDF_TestResult(false);

	EDF_Test_InMemoryDbDriverEntity resultEntity = EDF_Test_InMemoryDbDriverEntity.Cast(results.Get(0));

	if (!resultEntity) return new EDF_TestResult(false);

	return new EDF_TestResult(
		resultEntity.GetId() == entity.GetId() &&
		resultEntity.m_fFloatValue == entity.m_fFloatValue &&
		resultEntity.m_sStringValue == entity.m_sStringValue);
}

//------------------------------------------------------------------------------------------------
[Test("EDF_InMemoryDbDriverTests")]
TestResultBase EDF_Test_InMemoryDbDriver_Remove_ExistingId_Removed()
{
	// Arrange
	EDF_InMemoryDbDriver driver();
	EDF_InMemoryDbConnectionInfo connectInfo();
	connectInfo.m_sDatabaseName = "Testing";
	driver.Initialize(connectInfo);

	EDF_Test_InMemoryDbDriverEntity entity("TEST0000-0000-0001-0000-000000000002", 42.42, "Hello World");
	driver.AddOrUpdate(entity);

	// Act
	EDF_EDbOperationStatusCode statusCode = driver.Remove(EDF_Test_InMemoryDbDriverEntity, entity.GetId());

	// Assert
	if (!statusCode == EDF_EDbOperationStatusCode.SUCCESS) return new EDF_TestResult(false);

	array<ref EDF_DbEntity> results = driver.FindAll(EDF_Test_InMemoryDbDriverEntity, EDF_DbFind.Id().Equals(entity.GetId())).GetEntities();

	return new EDF_TestResult(results.Count() == 0);
}

