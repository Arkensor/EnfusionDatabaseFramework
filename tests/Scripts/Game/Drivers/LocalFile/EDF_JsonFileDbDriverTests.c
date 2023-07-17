class EDF_JsonFileDbDriverTests : TestSuite
{
	static const string DB_NAME = "JsonFileDbDriverTests";

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
    void Setup()
    {
    }

	//------------------------------------------------------------------------------------------------
    [Step(EStage.TearDown)]
    void TearDown()
    {
		string dir = string.Format("%1/%2", EDF_FileDbDriverBase.DB_BASE_DIR, DB_NAME);

		array<string> paths = {};
		System.FindFiles(paths.Insert, dir, "");

		foreach (string path : paths)
		{
			FileIO.DeleteFile(path);
		}

		FileIO.DeleteFile(dir);
    }

	//------------------------------------------------------------------------------------------------
	static void WriteEntity(string dbDir, EDF_Test_JsonFileDbDriverEntity entity)
	{
		SCR_JsonSaveContext writer();
		writer.WriteValue("", entity);
		FileIO.MakeDirectory(dbDir);
		writer.SaveToFile(string.Format("%1/%2.json", dbDir, entity.GetId()));
	}

	//------------------------------------------------------------------------------------------------
	static void DeleteEntity(string dbDir, string entityId)
	{
		FileIO.DeleteFile(string.Format("%1/%2.json", dbDir, entityId));
	}
}

class EDF_Test_JsonFileDbDriverEntity : EDF_DbEntity
{
	float m_fFloatValue;
	string m_sStringValue;

	//------------------------------------------------------------------------------------------------
	void EDF_Test_JsonFileDbDriverEntity(string id, float floatValue, string stringValue)
	{
		SetId(id);
		m_fFloatValue = floatValue;
		m_sStringValue = stringValue;
	}
}

class EDF_Test_JsonFileDbDriver_TestBase : TestBase
{
	ref EDF_JsonFileDbDriver driver;

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
	void Construct()
	{
		driver = new EDF_JsonFileDbDriver();
	}
}

[Test("EDF_JsonFileDbDriverTests")]
class EDF_Test_JsonFileDbDriver_AddOrUpdate_NewEntity_ReadFromFileSuccessfully : EDF_Test_JsonFileDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
	void Arrange()
	{
		EDF_JsonFileDbConnectionInfo connectInfo();
		connectInfo.m_sDatabaseName = EDF_JsonFileDbDriverTests.DB_NAME;
		driver.Initialize(connectInfo);
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		EDF_Test_JsonFileDbDriverEntity entity("TEST0000-0000-0001-0000-000000000001", 42.42, "Hello World");
		EDF_EDbOperationStatusCode statusCode = driver.AddOrUpdate(entity);

		// Assert
		if (statusCode != EDF_EDbOperationStatusCode.SUCCESS)
		{
			SetResult(new EDF_TestResult(false));
			return;
		}

		array<ref EDF_DbEntity> results = driver.FindAll(EDF_Test_JsonFileDbDriverEntity, EDF_DbFind.Id().Equals(entity.GetId())).GetEntities();

		if (results.Count() != 1)
		{
			SetResult(new EDF_TestResult(false));
			return;
		}

		EDF_Test_JsonFileDbDriverEntity resultEntity = EDF_Test_JsonFileDbDriverEntity.Cast(results.Get(0));

		if (!resultEntity)
		{
			SetResult(new EDF_TestResult(false));
			return;
		}

		SetResult(new EDF_TestResult(
			resultEntity.GetId() == entity.GetId() &&
			float.AlmostEqual(resultEntity.m_fFloatValue, entity.m_fFloatValue) &&
			resultEntity.m_sStringValue == entity.m_sStringValue));
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.TearDown)]
	void Cleanup()
	{
		EDF_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000000001");
	}
}

[Test("EDF_JsonFileDbDriverTests")]
class EDF_Test_JsonFileDbDriver_Remove_ExistingEntity_FileDeleted : EDF_Test_JsonFileDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
	void Arrange()
	{
		EDF_JsonFileDbConnectionInfo connectInfo();
		connectInfo.m_sDatabaseName = EDF_JsonFileDbDriverTests.DB_NAME;
		driver.Initialize(connectInfo);
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		EDF_Test_JsonFileDbDriverEntity entity("TEST0000-0000-0001-0000-000000000002", 42.42, "Hello World");
		driver.AddOrUpdate(entity);

		EDF_EDbOperationStatusCode statusCode = driver.Remove(EDF_Test_JsonFileDbDriverEntity, entity.GetId());

		// Assert
		if (statusCode != EDF_EDbOperationStatusCode.SUCCESS)
		{
			SetResult(new EDF_TestResult(false));
			return;
		}

		string file = string.Format("%1/%2.json", driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), entity.GetId());
		SetResult(new EDF_TestResult(!FileIO.FileExists(file)));
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.TearDown)]
	void Cleanup()
	{
		EDF_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000000002");
	}
}

[Test("EDF_JsonFileDbDriverTests")]
class EDF_Test_JsonFileDbDriver_FindAll_IdOnly_ExactLoadAndCache : EDF_Test_JsonFileDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
	void Arrange()
	{
		EDF_JsonFileDbConnectionInfo connectInfo();
		connectInfo.m_sDatabaseName = EDF_JsonFileDbDriverTests.DB_NAME;
		connectInfo.m_bUseCache = true;
		driver.Initialize(connectInfo);

		EDF_JsonFileDbDriverTests.WriteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), new EDF_Test_JsonFileDbDriverEntity("TEST0000-0000-0001-0000-000000001001", 41.1, "Existing 1001"));
		EDF_JsonFileDbDriverTests.WriteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), new EDF_Test_JsonFileDbDriverEntity("TEST0000-0000-0001-0000-000000001002", 41.2, "Existing 1002"));
		EDF_JsonFileDbDriverTests.WriteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), new EDF_Test_JsonFileDbDriverEntity("TEST0000-0000-0001-0000-000000001003", 41.3, "Existing 1003"));
		EDF_JsonFileDbDriverTests.WriteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), new EDF_Test_JsonFileDbDriverEntity("TEST0000-0000-0001-0000-000000001004", 41.4, "Existing 1004"));
		EDF_JsonFileDbDriverTests.WriteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), new EDF_Test_JsonFileDbDriverEntity("TEST0000-0000-0001-0000-000000001005", 41.5, "Existing 1005"));
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		EDF_DbFindCondition condition = EDF_DbFind.Id().EqualsAnyOf({"TEST0000-0000-0001-0000-000000001001", "TEST0000-0000-0001-0000-000000001005"});

		// Act
		array<ref EDF_DbEntity> results = driver.FindAll(EDF_Test_JsonFileDbDriverEntity, condition).GetEntities();

		// Assert
		SetResult(new EDF_TestResult((results.Count() == 2) && (driver._GetEntityCache().m_EntityInstances.Count() == 2)));
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.TearDown)]
	void Cleanup()
	{
		EDF_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000001001");
		EDF_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000001002");
		EDF_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000001003");
		EDF_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000001004");
		EDF_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000001005");
	}
}

[Test("EDF_JsonFileDbDriverTests")]
class EDF_Test_JsonFileDbDriver_FindAll_ContentField_AllLoadedAndCached : EDF_Test_JsonFileDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
	void Arrange()
	{
		EDF_JsonFileDbConnectionInfo connectInfo();
		connectInfo.m_sDatabaseName = EDF_JsonFileDbDriverTests.DB_NAME;
		connectInfo.m_bUseCache = true;
		driver.Initialize(connectInfo);

		EDF_JsonFileDbDriverTests.WriteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), new EDF_Test_JsonFileDbDriverEntity("TEST0000-0000-0001-0000-000000002001", 42.1, "Existing 2001"));
		EDF_JsonFileDbDriverTests.WriteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), new EDF_Test_JsonFileDbDriverEntity("TEST0000-0000-0001-0000-000000002002", 42.2, "Existing 2002"));
		EDF_JsonFileDbDriverTests.WriteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), new EDF_Test_JsonFileDbDriverEntity("TEST0000-0000-0001-0000-000000002003", 42.3, "Existing 2003"));
		EDF_JsonFileDbDriverTests.WriteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), new EDF_Test_JsonFileDbDriverEntity("TEST0000-0000-0001-0000-000000002004", 42.4, "Existing 2004"));
		EDF_JsonFileDbDriverTests.WriteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), new EDF_Test_JsonFileDbDriverEntity("TEST0000-0000-0001-0000-000000002005", 42.5, "Existing 2005"));
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		EDF_DbFindCondition condition = EDF_DbFind.Or({
			EDF_DbFind.Field("m_sStringValue").Contains("2001"),
			EDF_DbFind.Field("m_fFloatValue").GreaterThanOrEquals(42.5)
		});

		array<ref EDF_DbEntity> results = driver.FindAll(EDF_Test_JsonFileDbDriverEntity, condition).GetEntities();

		// Assert
		SetResult(new EDF_TestResult((results.Count() == 2) && (driver._GetEntityCache().m_EntityInstances.Count() == 5)));
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.TearDown)]
	void Cleanup()
	{
		EDF_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000002001");
		EDF_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000002002");
		EDF_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000002003");
		EDF_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000002004");
		EDF_JsonFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EDF_Test_JsonFileDbDriverEntity), "TEST0000-0000-0001-0000-000000002005");
	}
}
