class EDF_WebProxyDbDriverTests : TestSuite
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

class EDF_Test_WebProxyDbDriver_TestBase : TestBase
{
	bool m_bReady;
	bool m_bCleaned;
	ref EDF_WebProxyDbDriver m_pDriver;
	ref EDF_DbEntity m_pEntity;

	//------------------------------------------------------------------------------------------------
	[Step(EStage.Setup)]
	void Construct()
	{
		EDF_WebProxyConnectionInfoBase connectInfo();
		connectInfo.m_sDatabaseName = "WebProxyDbDriverTests";
		connectInfo.m_sProxyHost = "localhost";
		connectInfo.m_iProxyPort = 5120;

		m_pDriver = new EDF_WebProxyDbDriver();
		m_pDriver.Initialize(connectInfo);
	}
};

class EDF_Test_WebProxyDbDriverEntityA : EDF_DbEntity
{
	float m_fFloatValue;
	string m_sStringValue;

	//------------------------------------------------------------------------------------------------
	static EDF_Test_WebProxyDbDriverEntityA Create(string id, float floatValue = 0, string stringValue = string.Empty)
	{
		EDF_Test_WebProxyDbDriverEntityA entity();
		entity.SetId(id);
		entity.m_fFloatValue = floatValue;
		entity.m_sStringValue = stringValue;
		return entity;
	}
};

class EDF_Test_WebProxyDbDriverEntityB : EDF_DbEntity
{
	float m_fFloat;
	string m_sString;
	vector m_vVector;

	//------------------------------------------------------------------------------------------------
	static EDF_Test_WebProxyDbDriverEntityB Create(string id, float floatValue, string stringValue, vector vetorValue)
	{
		EDF_Test_WebProxyDbDriverEntityB entity();
		entity.SetId(id);
		entity.m_fFloat = floatValue;
		entity.m_sString = stringValue;
		entity.m_vVector = vetorValue;
		return entity;
	}
};

class EDF_Test_WebProxyDbDriverEntityHolder : EDF_DbEntity
{
	ref array<ref EDF_DbEntity> m_aEntities;

	//------------------------------------------------------------------------------------------------
	static EDF_Test_WebProxyDbDriverEntityHolder Create(string id, array<ref EDF_DbEntity> entities)
	{
		EDF_Test_WebProxyDbDriverEntityHolder entity();
		entity.SetId(id);
		entity.m_aEntities = entities;
		return entity;
	}
};

class EDF_Test_WebProxyDbDriverEntityT<Class T> : EDF_DbEntity
{
	T m_Value;

	//------------------------------------------------------------------------------------------------
	static EDF_Test_WebProxyDbDriverEntityT<T> Create(string id, T value)
	{
		EDF_Test_WebProxyDbDriverEntityT<T> entity();
		entity.SetId(id);
		entity.m_Value = value;
		return entity;
	}
};

class EDF_Test_WebProxyDbDriverEntityTArray<Class T> : EDF_DbEntity
{
	ref array<T> m_aValues;

	//------------------------------------------------------------------------------------------------
	static EDF_Test_WebProxyDbDriverEntityTArray<T> Create(string id, array<T> values = null)
	{
		EDF_Test_WebProxyDbDriverEntityTArray<T> entity();
		entity.SetId(id);
		entity.m_aValues = values;
		return entity;
	}
};

//[Test("EDF_WebProxyDbDriverTests")]
class EDF_Test_WebProxyDbDriver_AddOrUpdateAsync_NewEntity_Added : EDF_Test_WebProxyDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Act()
	{
		auto entity = EDF_Test_WebProxyDbDriverEntityA.Create("00000000-0000-0001-0000-000000000001", 42.42, "Hello World");
		EDF_DbOperationStatusOnlyCallback callback(this, "OnResult");
		m_pDriver.AddOrUpdateAsync(entity, callback);

		SetResult(new EDF_TestResult(true));
	}

	//------------------------------------------------------------------------------------------------
	void OnResult(EDF_EDbOperationStatusCode statusCode)
	{
		PrintFormat(Type().ToString()+" Result: %1", statusCode == EDF_EDbOperationStatusCode.SUCCESS);
		//SetResult(new EDF_TestResult(statusCode == EDF_EDbOperationStatusCode.SUCCESS));
	}

	/*
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	bool Asset()
	{
		Print("Assert");
		// Wait until we have a result
		return GetResult() != null;
	}

	//------------------------------------------------------------------------------------------------
	[Step(EStage.TearDown)]
	void Cleanup()
	{
		m_pDriver.RemoveAsync(EDF_Test_WebProxyDbDriverEntityA, "00000000-0000-0001-0000-000000000001");
	}
	*/
};

//[Test("EDF_WebProxyDbDriverTests")]
class EDF_Test_WebProxyDbDriver_RemoveAsync_ExitingEntity_Removed : EDF_Test_WebProxyDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Arrange()
	{
		auto entity = EDF_Test_WebProxyDbDriverEntityA.Create("00000000-0000-0002-0000-000000000001", 42.42, "Hello World");
		EDF_DbOperationStatusOnlyCallback callback(this, "Act");
		m_pDriver.AddOrUpdateAsync(entity, callback);

		SetResult(new EDF_TestResult(true));
	}

	//------------------------------------------------------------------------------------------------
	//[Step(EStage.Main)]
	void Act()
	{
		EDF_DbOperationStatusOnlyCallback callback(this, "OnResult");
		m_pDriver.RemoveAsync(EDF_Test_WebProxyDbDriverEntityA, "00000000-0000-0002-0000-000000000001", callback);
	}

	//------------------------------------------------------------------------------------------------
	void OnResult(EDF_EDbOperationStatusCode statusCode)
	{
		PrintFormat(Type().ToString()+" Result: %1", statusCode == EDF_EDbOperationStatusCode.SUCCESS);
	}
};

//[Test("EDF_WebProxyDbDriverTests")]
class EDF_Test_WebProxyDbDriver_RemoveAsync_UnknownEntity_NotFound : EDF_Test_WebProxyDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	bool Act()
	{
		EDF_DbOperationStatusOnlyCallback callback(this, "OnResult");
		m_pDriver.RemoveAsync(EDF_Test_WebProxyDbDriverEntityA, "00000000-0000-0003-0000-000000000001", callback);

		SetResult(new EDF_TestResult(true));
		return false;
	}

	//------------------------------------------------------------------------------------------------
	void OnResult(EDF_EDbOperationStatusCode statusCode)
	{
		PrintFormat(Type().ToString()+" Result: %1", statusCode == EDF_EDbOperationStatusCode.FAILURE_ID_NOT_FOUND);
	}
};

//[Test("EDF_WebProxyDbDriverTests")]
class EDF_Test_WebProxyDbDriver_FindAllAsync_ExitingId_Returned : EDF_Test_WebProxyDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Arrange()
	{
		auto entity = EDF_Test_WebProxyDbDriverEntityA.Create("00000000-0000-0004-0000-000000000001", 42.42, "Hello World");
		EDF_DbOperationStatusOnlyCallback callback(this, "Act");
		m_pDriver.AddOrUpdateAsync(entity, callback);

		SetResult(new EDF_TestResult(true));
	}

	//------------------------------------------------------------------------------------------------
	//[Step(EStage.Main)]
	void Act()
	{
		EDF_DbFindCondition condition = EDF_DbFind.Id().Equals("00000000-0000-0004-0000-000000000001");
		EDF_DbFindCallbackMultiple<EDF_Test_WebProxyDbDriverEntityA> callback(this, "OnResult");
		m_pDriver.FindAllAsync(EDF_Test_WebProxyDbDriverEntityA, condition, callback: callback);
	}

	//------------------------------------------------------------------------------------------------
	void OnResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_Test_WebProxyDbDriverEntityA> results)
	{
		bool succes = statusCode == EDF_EDbOperationStatusCode.SUCCESS &&
			results.Count() == 1 &&
			results.Get(0).GetId() == "00000000-0000-0004-0000-000000000001";

		PrintFormat(Type().ToString()+" Result: %1", succes);
	}
};

//[Test("EDF_WebProxyDbDriverTests")]
class EDF_Test_WebProxyDbDriver_FindAllAsync_OrderedLimitedOffset_CorrectResultsOrdered : EDF_Test_WebProxyDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Arrange()
	{
		m_pDriver.AddOrUpdateAsync(EDF_Test_WebProxyDbDriverEntityA.Create("00000000-0000-0005-0000-000000000001", 1, "B"));
		m_pDriver.AddOrUpdateAsync(EDF_Test_WebProxyDbDriverEntityA.Create("00000000-0000-0005-0000-000000000002", 1, "A"));
		m_pDriver.AddOrUpdateAsync(EDF_Test_WebProxyDbDriverEntityA.Create("00000000-0000-0005-0000-000000000003", 3));
		m_pDriver.AddOrUpdateAsync(EDF_Test_WebProxyDbDriverEntityA.Create("00000000-0000-0005-0000-000000000004", 5));
		m_pDriver.AddOrUpdateAsync(EDF_Test_WebProxyDbDriverEntityA.Create("00000000-0000-0005-0000-000000000005", 2));

		EDF_DbOperationStatusOnlyCallback callback(this, "Act");
		m_pDriver.AddOrUpdateAsync(EDF_Test_WebProxyDbDriverEntityA.Create("00000000-0000-0005-0000-000000000006", 4), callback);

		SetResult(new EDF_TestResult(true));
	}

	//------------------------------------------------------------------------------------------------
	void Act()
	{
		EDF_DbFindCondition condition = EDF_DbFind.Id().Contains("0000-0005-0000");
		array<ref TStringArray> orderBy = {{"m_fFloatValue", EDF_EDbEntitySortDirection.DESCENDING}, {"m_sStringValue", "asc"}};
		int limit = 4;
		int offset = 1;

		/*
			5 -> exclude through offset
			4
			3
			2
			1 - A
			1 - B -> exclude trough limit
		*/

		EDF_DbFindCallbackMultiple<EDF_Test_WebProxyDbDriverEntityA> callback(this, "OnResult");
		m_pDriver.FindAllAsync(EDF_Test_WebProxyDbDriverEntityA, condition, orderBy, limit, offset, callback);
	}

	//------------------------------------------------------------------------------------------------
	void OnResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_Test_WebProxyDbDriverEntityA> results)
	{
		bool succes = true;
		PrintFormat(Type().ToString()+" Result: %1", succes);
	}
};

//[Test("EDF_WebProxyDbDriverTests")]
class EDF_Test_WebProxyDbDriver_FindAllAsync_IntSingleOperators_OperatorBased : EDF_Test_WebProxyDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Arrange()
	{
		m_pEntity = EDF_Test_WebProxyDbDriverEntityT<int>.Create("00000000-0000-0006-0000-000000000001", 1337);
		m_pDriver.AddOrUpdateAsync(m_pEntity, new EDF_DbOperationStatusOnlyCallback(this, "Act"));

		SetResult(new EDF_TestResult(true));
	}

	//------------------------------------------------------------------------------------------------
	void Act()
	{
		typename type = m_pEntity.Type();
		EDF_DbFindCallbackMultiple<EDF_Test_WebProxyDbDriverEntityT<int>> matchCallback(this, "OnMatchResult");
		EDF_DbFindCallbackMultiple<EDF_Test_WebProxyDbDriverEntityT<int>> noMatchCallback(this, "OnNoMatchResult");

		// Should match operations
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").Equals(1337), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").Not().Equals(42), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").LessThan(2000), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").LessThanOrEquals(1337), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").GreaterThan(42), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").GreaterThanOrEquals(1337), callback: matchCallback);
		
		// Should not match
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").Equals(42), callback: noMatchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").Not().Equals(1337), callback: noMatchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").LessThan(42), callback: noMatchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").LessThanOrEquals(42), callback: noMatchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").GreaterThan(2000), callback: noMatchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").GreaterThanOrEquals(2000), callback: noMatchCallback);
	}

	//------------------------------------------------------------------------------------------------
	void OnMatchResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_Test_WebProxyDbDriverEntityT<int>> results)
	{
		bool succes = statusCode == EDF_EDbOperationStatusCode.SUCCESS &&
			results.Count() == 1 &&
			results.Get(0).GetId() == "00000000-0000-0006-0000-000000000001";

		PrintFormat(Type().ToString() + "OnMatchResult: %1", succes);
	}

	//------------------------------------------------------------------------------------------------
	void OnNoMatchResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_Test_WebProxyDbDriverEntityT<int>> results)
	{
		bool succes = statusCode == EDF_EDbOperationStatusCode.SUCCESS && results.Count() == 0;
		PrintFormat(Type().ToString() + "OnNoMatchResult: %1", succes);
	}
};

[Test("EDF_WebProxyDbDriverTests")]
class EDF_Test_WebProxyDbDriver_FindAllAsync_IntArrayOperators_OperatorBased : EDF_Test_WebProxyDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Arrange()
	{
		m_pEntity = EDF_Test_WebProxyDbDriverEntityTArray<int>.Create("00000000-0000-0006-0000-000000000001", {1337, 42});
		m_pDriver.AddOrUpdateAsync(m_pEntity, new EDF_DbOperationStatusOnlyCallback(this, "Act"));

		SetResult(new EDF_TestResult(true));
	}

	//------------------------------------------------------------------------------------------------
	void Act()
	{
		typename type = m_pEntity.Type();
		EDF_DbFindCallbackMultiple<EDF_Test_WebProxyDbDriverEntityTArray<int>> matchCallback(this, "OnMatchResult");
		EDF_DbFindCallbackMultiple<EDF_Test_WebProxyDbDriverEntityTArray<int>> noMatchCallback(this, "OnNoMatchResult");
		
		//m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Values").any, callback: matchCallback);
	}

	//------------------------------------------------------------------------------------------------
	void OnMatchResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_Test_WebProxyDbDriverEntityT<int>> results)
	{
		bool succes = statusCode == EDF_EDbOperationStatusCode.SUCCESS &&
			results.Count() == 1 &&
			results.Get(0).GetId() == "00000000-0000-0006-0000-000000000001";

		PrintFormat(Type().ToString() + "OnMatchResult: %1", succes);
	}

	//------------------------------------------------------------------------------------------------
	void OnNoMatchResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_Test_WebProxyDbDriverEntityT<int>> results)
	{
		bool succes = statusCode == EDF_EDbOperationStatusCode.SUCCESS && results.Count() == 0;
		PrintFormat(Type().ToString() + "OnNoMatchResult: %1", succes);
	}
};
