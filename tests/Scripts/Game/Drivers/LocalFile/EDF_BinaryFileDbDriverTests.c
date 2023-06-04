class EDF_BinaryFileDbDriverTests : TestSuite
{
	static const string DB_NAME = "BinaryFileDbDriverTests";

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
	static void WriteEntity(string dbDir, EDF_Test_BinFileDbDriverEntity entity)
	{
		SCR_BinSaveContext writer();
		writer.WriteValue("data", entity);
		FileIO.MakeDirectory(dbDir);
		writer.SaveToFile(string.Format("%1/%2.bin", dbDir, entity.GetId()));
	}

	//------------------------------------------------------------------------------------------------
	static void DeleteEntity(string dbDir, string entityId)
	{
		FileIO.DeleteFile(string.Format("%1/%2.bin", dbDir, entityId));
	}
};

class EDF_Test_BinFileDbDriverEntity : EDF_DbEntity
{
	float m_fFloatValue;
	string m_sStringValue;

	//------------------------------------------------------------------------------------------------
	static EDF_Test_BinFileDbDriverEntity Create(string id, float floatValue, string stringValue)
	{
		EDF_Test_BinFileDbDriverEntity entity();
		entity.SetId(id);
		entity.m_fFloatValue = floatValue;
		entity.m_sStringValue = stringValue;
		return entity;
	}
};

class EDF_Test_BinFileDbDriverEntity_TestBase : TestBase
{
	ref EDF_BinaryFileDbDriver driver;

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
	void Construct()
	{
		driver = new EDF_BinaryFileDbDriver();
	}
};

[Test("EDF_BinaryFileDbDriverTests")]
class EDF_Test_BinaryFileDbDriver_AddOrUpdate_NewEntity_ReadFromFileSuccessfully : EDF_Test_BinFileDbDriverEntity_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
	void Arrange()
	{
		EDF_BinaryFileDbConnectionInfo connectInfo();
		connectInfo.m_sDatabaseName = EDF_BinaryFileDbDriverTests.DB_NAME;
		driver.Initialize(connectInfo);
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		auto entity = EDF_Test_BinFileDbDriverEntity.Create("TEST0000-0000-0001-0000-000000000001", 42.42, "Hello World");
		EDF_EDbOperationStatusCode statusCode = driver.AddOrUpdate(entity);

		// Assert
		if (statusCode != EDF_EDbOperationStatusCode.SUCCESS)
		{
			SetResult(new EDF_TestResult(false));
			return;
		}

		array<ref EDF_DbEntity> results = driver.FindAll(EDF_Test_BinFileDbDriverEntity, EDF_DbFind.Id().Equals(entity.GetId())).GetEntities();

		if (results.Count() != 1)
		{
			SetResult(new EDF_TestResult(false));
			return;
		}

		EDF_Test_BinFileDbDriverEntity resultEntity = EDF_Test_BinFileDbDriverEntity.Cast(results.Get(0));

		if (!resultEntity)
		{
			SetResult(new EDF_TestResult(false));
			return;
		}

		SetResult(new EDF_TestResult(
			resultEntity.GetId() == entity.GetId() &&
			resultEntity.m_fFloatValue == entity.m_fFloatValue &&
			resultEntity.m_sStringValue == entity.m_sStringValue));
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.TearDown)]
	void Cleanup()
	{
		EDF_BinaryFileDbDriverTests.DeleteEntity(driver._GetTypeDirectory(EDF_Test_BinFileDbDriverEntity), "TEST0000-0000-0001-0000-000000000001");
	}
};
