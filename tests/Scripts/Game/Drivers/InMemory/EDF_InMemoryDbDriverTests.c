[BaseContainerProps(category: "Autotest")]
class EDF_InMemoryDbDriverTests : SCR_AutotestSuiteBase
{
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
class EDF_Test_InMemoryDbDriver_AddOrUpdate_NewEntity_Inserted : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
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
		if (statusCode == EDF_EDbOperationStatusCode.SUCCESS)
		{
			array<ref EDF_DbEntity> results = driver.FindAll(EDF_Test_InMemoryDbDriverEntity, EDF_DbFind.Id().Equals(entity.GetId())).GetEntities();
			if (results.Count() == 1)
			{
				EDF_Test_InMemoryDbDriverEntity resultEntity = EDF_Test_InMemoryDbDriverEntity.Cast(results.Get(0));
				if (resultEntity)
				{
					SetResult(EDF_AutotestResult.FromResult(
						resultEntity.GetId() == entity.GetId() &&
						resultEntity.m_fFloatValue == entity.m_fFloatValue &&
						resultEntity.m_sStringValue == entity.m_sStringValue));

					return;
				}
			}
		}

		SetResult(EDF_AutotestResult.FromResult(false));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_InMemoryDbDriverTests")]
class EDF_Test_InMemoryDbDriver_Remove_ExistingId_Removed : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
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
		bool success = false;

		if (statusCode == EDF_EDbOperationStatusCode.SUCCESS)
		{
			array<ref EDF_DbEntity> results = driver.FindAll(EDF_Test_InMemoryDbDriverEntity, EDF_DbFind.Id().Equals(entity.GetId())).GetEntities();
			success = results.Count() == 0;
		}

		SetResult(EDF_AutotestResult.FromResult(success));
	}
}
