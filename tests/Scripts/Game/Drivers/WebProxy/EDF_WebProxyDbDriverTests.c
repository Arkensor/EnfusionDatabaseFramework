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

class EDF_Test_WebProxyDbDriverEntityTNestedArray<Class T> : EDF_DbEntity
{
	ref array<ref array<T>> m_aValues;

	//------------------------------------------------------------------------------------------------
	static EDF_Test_WebProxyDbDriverEntityTNestedArray<T> Create(string id, array<ref array<T>> values = null)
	{
		EDF_Test_WebProxyDbDriverEntityTNestedArray<T> entity();
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
		PrintFormat("%1 OnResult: %2", ClassName(), statusCode == EDF_EDbOperationStatusCode.SUCCESS);
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
		PrintFormat("%1 OnResult: %2", ClassName(), statusCode == EDF_EDbOperationStatusCode.SUCCESS);
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
		PrintFormat("%1 OnResult: %2", ClassName(), statusCode == EDF_EDbOperationStatusCode.FAILURE_ID_NOT_FOUND);
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
		bool SUCCESS = statusCode == EDF_EDbOperationStatusCode.SUCCESS &&
			results.Count() == 1 &&
			results.Get(0).GetId() == "00000000-0000-0004-0000-000000000001";

		PrintFormat("%1 OnResult: %2", ClassName(), SUCCESS);
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
		EDF_DbFindCondition condition = EDF_DbFind.Id().Contains("00000000-0000-0005-0000");
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
		bool SUCCESS = results.Count() == 4 &&
			results.Get(0).GetId() == "00000000-0000-0005-0000-000000000006" &&
			results.Get(1).GetId() == "00000000-0000-0005-0000-000000000003" &&
			results.Get(2).GetId() == "00000000-0000-0005-0000-000000000005" &&
			results.Get(3).GetId() == "00000000-0000-0005-0000-000000000002";

		PrintFormat("%1 OnResult: %2", ClassName(), SUCCESS);
	}
};

//[Test("EDF_WebProxyDbDriverTests")]
class EDF_Test_WebProxyDbDriver_FindAllAsync_NullOrDefault_OperatorBased : EDF_Test_WebProxyDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Arrange()
	{
		m_pDriver.AddOrUpdateAsync(EDF_Test_WebProxyDbDriverEntityT<int>.Create("00000000-0000-0006-0000-000000000001", 0));
		m_pDriver.AddOrUpdateAsync(EDF_Test_WebProxyDbDriverEntityT<int>.Create("00000000-0000-0006-0000-000000000002", 42));

		m_pDriver.AddOrUpdateAsync(EDF_Test_WebProxyDbDriverEntityT<float>.Create("00000000-0000-0006-0000-000000000003", 0.0));
		m_pDriver.AddOrUpdateAsync(EDF_Test_WebProxyDbDriverEntityT<float>.Create("00000000-0000-0006-0000-000000000004", 42.42));

		m_pDriver.AddOrUpdateAsync(EDF_Test_WebProxyDbDriverEntityT<bool>.Create("00000000-0000-0006-0000-000000000005", false));
		m_pDriver.AddOrUpdateAsync(EDF_Test_WebProxyDbDriverEntityT<bool>.Create("00000000-0000-0006-0000-000000000006", true));

		m_pDriver.AddOrUpdateAsync(EDF_Test_WebProxyDbDriverEntityT<string>.Create("00000000-0000-0006-0000-000000000007", string.Empty));
		m_pDriver.AddOrUpdateAsync(EDF_Test_WebProxyDbDriverEntityT<string>.Create("00000000-0000-0006-0000-000000000008", "Hello"));

		m_pDriver.AddOrUpdateAsync(EDF_Test_WebProxyDbDriverEntityT<vector>.Create("00000000-0000-0006-0000-000000000009", vector.Zero));
		m_pDriver.AddOrUpdateAsync(EDF_Test_WebProxyDbDriverEntityT<vector>.Create("00000000-0000-0006-0000-000000000010", "1 2 3"),
			new EDF_DbOperationStatusOnlyCallback(this, "Act"));

		SetResult(new EDF_TestResult(true));
	}

	//------------------------------------------------------------------------------------------------
	void Act()
	{
		EDF_DbFindCallbackMultipleUntyped matchCallback(this, "OnMatchResult");

		typename type = String("EDF_Test_WebProxyDbDriverEntityT<int>").ToType();
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").NullOrDefault(), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").Not().NullOrDefault(), callback: matchCallback);

		type = String("EDF_Test_WebProxyDbDriverEntityT<float>").ToType();
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").NullOrDefault(), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").Not().NullOrDefault(), callback: matchCallback);

		type = String("EDF_Test_WebProxyDbDriverEntityT<bool>").ToType();
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").NullOrDefault(), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").Not().NullOrDefault(), callback: matchCallback);

		type = String("EDF_Test_WebProxyDbDriverEntityT<string>").ToType();
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").NullOrDefault(), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").Not().NullOrDefault(), callback: matchCallback);

		type = String("EDF_Test_WebProxyDbDriverEntityT<vector>").ToType();
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").NullOrDefault(), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").Not().NullOrDefault(), callback: matchCallback);

	}

	//------------------------------------------------------------------------------------------------
	void OnResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_DbEntity> results)
	{
		int matches;
		foreach (EDF_DbEntity result : results)
		{
			if (result.GetId().StartsWith("00000000-0000-0006-0000-"))
				matches++;
		}

		PrintFormat("%1 OnResult: %2", ClassName(), matches == 1);
	}
};

//[Test("EDF_WebProxyDbDriverTests")]
class EDF_Test_WebProxyDbDriver_FindAllAsync_LengthOperator_OperatorBased : EDF_Test_WebProxyDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Arrange()
	{
		m_pDriver.AddOrUpdateAsync(EDF_Test_WebProxyDbDriverEntityT<string>.Create("00000000-0000-0007-0000-000000000001", "MarioE"));

		m_pEntity = EDF_Test_WebProxyDbDriverEntityTArray<string>.Create("00000000-0000-0007-0000-000000000001", {"Hello", "World", "!"});
		m_pDriver.AddOrUpdateAsync(m_pEntity, new EDF_DbOperationStatusOnlyCallback(this, "Act"));

		SetResult(new EDF_TestResult(true));
	}

	//------------------------------------------------------------------------------------------------
	void Act()
	{
		EDF_DbFindCallbackMultipleUntyped matchCallback(this, "OnMatchResult");

		// Should match
		typename type = m_pEntity.Type();
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().Length().GreaterThan(3), callback: matchCallback);

		// maybe for projection stage filter out all fields being accessed for length -> string property or array of strings
			// leave condition in but on evaluate instead of the actual field name do field_lengths?
			// https://www.thecodebuzz.com/mongodb-nested-array-string-field-value-length-query/
			// alternative is regex

		//type = String("EDF_Test_WebProxyDbDriverEntityT<string>").ToType();
		//m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").Length().Equals(6), callback: matchCallback);
	}

	//------------------------------------------------------------------------------------------------
	void OnMatchResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_DbEntity> results)
	{
		int matches;
		foreach (EDF_DbEntity result : results)
		{
			if (result.GetId().StartsWith("00000000-0000-0007-0000-"))
				matches++;
		}

		PrintFormat("%1 OnResult: %2", ClassName(), matches == 1);
	}
};


//[Test("EDF_WebProxyDbDriverTests")]
class EDF_Test_WebProxyDbDriver_FindAllAsync_CountOperator_OperatorBased : EDF_Test_WebProxyDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Arrange()
	{
		m_pEntity = EDF_Test_WebProxyDbDriverEntityTArray<int>.Create("00000000-0000-0008-0000-000000000001", {1, 2, 3});
		m_pDriver.AddOrUpdateAsync(m_pEntity, new EDF_DbOperationStatusOnlyCallback(this, "Act"));

		SetResult(new EDF_TestResult(true));
	}

	//------------------------------------------------------------------------------------------------
	void Act()
	{
		EDF_DbFindCallbackMultipleUntyped matchCallback(this, "OnMatchResult");

		// Should match
		typename type = m_pEntity.Type();
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Count().Equals(3), callback: matchCallback);
	}

	//------------------------------------------------------------------------------------------------
	void OnMatchResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_DbEntity> results)
	{
		int matches;
		foreach (EDF_DbEntity result : results)
		{
			if (result.GetId().StartsWith("00000000-0000-0008-0000-"))
				matches++;
		}

		PrintFormat("%1 OnResult: %2", ClassName(), matches == 1);
	}
};


//[Test("EDF_WebProxyDbDriverTests")]
class EDF_Test_WebProxyDbDriver_FindAllAsync_IntSingleOperators_OperatorBased : EDF_Test_WebProxyDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Arrange()
	{
		m_pEntity = EDF_Test_WebProxyDbDriverEntityT<int>.Create("00000000-0000-0009-0000-000000000001", 1337);
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
		int matches;
		foreach (EDF_DbEntity result : results)
		{
			if (result.GetId().StartsWith("00000000-0000-0009-0000-"))
				matches++;
		}

		PrintFormat("%1 OnResult: %2", ClassName(), matches == 1);
	}

	//------------------------------------------------------------------------------------------------
	void OnNoMatchResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_Test_WebProxyDbDriverEntityT<int>> results)
	{
		int matches;
		foreach (EDF_DbEntity result : results)
		{
			if (result.GetId().StartsWith("00000000-0000-0009-0000-"))
				matches++;
		}

		PrintFormat("%1 OnResult: %2", ClassName(), matches == 0);
	}
};

//[Test("EDF_WebProxyDbDriverTests")]
class EDF_Test_WebProxyDbDriver_FindAllAsync_IntArrayOperators_OperatorBased : EDF_Test_WebProxyDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Arrange()
	{
		m_pEntity = EDF_Test_WebProxyDbDriverEntityTArray<int>.Create("00000000-0000-0010-0000-000000000001", {1337, 42});
		m_pDriver.AddOrUpdateAsync(m_pEntity, new EDF_DbOperationStatusOnlyCallback(this, "Act"));

		SetResult(new EDF_TestResult(true));
	}

	//------------------------------------------------------------------------------------------------
	void Act()
	{
		typename type = m_pEntity.Type();
		EDF_DbFindCallbackMultipleUntyped matchCallback(this, "OnMatchResult");
		EDF_DbFindCallbackMultipleUntyped noMatchCallback(this, "OnNoMatchResult");

		// Should match
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Not().NullOrDefault(), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Contains(1337), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Contains(42), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").ContainsAnyOf(EDF_DbValues<int>.From({41, 42, 43})), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").ContainsAllOf(EDF_DbValues<int>.From({42, 1337})), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Equals(EDF_DbValues<int>.From({1337, 42})), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().GreaterThan(69), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").All().LessThan(2000), callback: matchCallback);

		// Should not match
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").NullOrDefault(), callback: noMatchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Contains(69), callback: noMatchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").ContainsAnyOf(EDF_DbValues<int>.From({69, 96})), callback: noMatchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").ContainsAllOf(EDF_DbValues<int>.From({42, 69})), callback: noMatchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Equals(EDF_DbValues<int>.From({42, 1337})), callback: noMatchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().GreaterThanOrEquals(1338), callback: noMatchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").All().LessThanOrEquals(40), callback: noMatchCallback);
	}

	//------------------------------------------------------------------------------------------------
	void OnMatchResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_DbEntity> results)
	{
		int matches;
		foreach (EDF_DbEntity result : results)
		{
			if (result.GetId().StartsWith("00000000-0000-0010-0000-"))
				matches++;
		}

		PrintFormat("%1 OnResult: %2", ClassName(), matches == 1);
	}

	//------------------------------------------------------------------------------------------------
	void OnNoMatchResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_DbEntity> results)
	{
		int matches;
		foreach (EDF_DbEntity result : results)
		{
			if (result.GetId().StartsWith("00000000-0000-0010-0000-"))
				matches++;
		}

		PrintFormat("%1 OnResult: %2", ClassName(), matches == 0);
	}
};

//[Test("EDF_WebProxyDbDriverTests")]
class EDF_Test_WebProxyDbDriver_FindAllAsync_IntNestedArrayOperators_OperatorBased : EDF_Test_WebProxyDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Arrange()
	{
		array<ref array<int>> values();
		values.Insert({1337, 42});
		values.Insert({69, 96});

		m_pEntity = EDF_Test_WebProxyDbDriverEntityTNestedArray<int>.Create("00000000-0000-0011-0000-000000000001", values);
		m_pDriver.AddOrUpdateAsync(m_pEntity, new EDF_DbOperationStatusOnlyCallback(this, "Act"));

		SetResult(new EDF_TestResult(true));
	}

	//------------------------------------------------------------------------------------------------
	void Act()
	{
		typename type = m_pEntity.Type();
		EDF_DbFindCallbackMultipleUntyped matchCallback(this, "OnMatchResult");
		EDF_DbFindCallbackMultipleUntyped noMatchCallback(this, "OnNoMatchResult");

		// Should match
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().Contains(69), callback: matchCallback);
		//m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").All().ContainsAnyOf(EDF_DbValues<int>.From({42, 69})), callback: matchCallback);
		//m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().Equals(EDF_DbValues<int>.From({69, 96})), callback: matchCallback);

		// Should not match
		//m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().Contains(666), callback: noMatchCallback);
		//m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").All().ContainsAnyOf(EDF_DbValues<int>.From({41, 42, 43})), callback: noMatchCallback);
		//m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").All().Equals(EDF_DbValues<int>.From({69, 96})), callback: noMatchCallback);
	}

	//------------------------------------------------------------------------------------------------
	void OnMatchResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_DbEntity> results)
	{
		int matches;
		foreach (EDF_DbEntity result : results)
		{
			if (result.GetId().StartsWith("00000000-0000-0011-0000-"))
				matches++;
		}

		PrintFormat("%1 OnResult: %2", ClassName(), matches == 1);
	}

	//------------------------------------------------------------------------------------------------
	void OnNoMatchResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_DbEntity> results)
	{
		int matches;
		foreach (EDF_DbEntity result : results)
		{
			if (result.GetId().StartsWith("00000000-0000-0011-0000-"))
				matches++;
		}

		PrintFormat("%1 OnResult: %2", ClassName(), matches == 0);
	}
};

//[Test("EDF_WebProxyDbDriverTests")]
class EDF_Test_WebProxyDbDriver_FindAllAsync_FloatNestedArrayOperators_OperatorBased : EDF_Test_WebProxyDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Arrange()
	{
		array<ref array<float>> values();
		values.Insert({13.37, 42.42});
		values.Insert({69.69, 96.96});

		m_pEntity = EDF_Test_WebProxyDbDriverEntityTNestedArray<float>.Create("00000000-0000-0012-0000-000000000001", values);
		m_pDriver.AddOrUpdateAsync(m_pEntity, new EDF_DbOperationStatusOnlyCallback(this, "Act"));

		SetResult(new EDF_TestResult(true));
	}

	//------------------------------------------------------------------------------------------------
	void Act()
	{
		typename type = m_pEntity.Type();
		EDF_DbFindCallbackMultipleUntyped matchCallback(this, "OnMatchResult");
		EDF_DbFindCallbackMultipleUntyped noMatchCallback(this, "OnNoMatchResult");

		// Should match
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().Contains(13.37), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().Any().GreaterThanOrEquals(96.96), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").All().All().LessThan(100), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").All().All().Not().NullOrDefault(), callback: matchCallback);

		// Should not match
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().Contains(66.66), callback: noMatchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().Any().GreaterThan(97), callback: noMatchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().Any().NullOrDefault(), callback: noMatchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").All().All().Equals(404), callback: noMatchCallback);
	}

	//------------------------------------------------------------------------------------------------
	void OnMatchResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_DbEntity> results)
	{
		int matches;
		foreach (EDF_DbEntity result : results)
		{
			if (result.GetId().StartsWith("00000000-0000-0012-0000-"))
				matches++;
		}

		PrintFormat("%1 OnResult: %2", ClassName(), matches == 1);
	}

	//------------------------------------------------------------------------------------------------
	void OnNoMatchResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_DbEntity> results)
	{
		int matches;
		foreach (EDF_DbEntity result : results)
		{
			if (result.GetId().StartsWith("00000000-0000-0012-0000-"))
				matches++;
		}

		PrintFormat("%1 OnResult: %2", ClassName(), matches == 0);
	}
};

//[Test("EDF_WebProxyDbDriverTests")]
class EDF_Test_WebProxyDbDriver_FindAllAsync_BoolNestedArrayOperators_OperatorBased : EDF_Test_WebProxyDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Arrange()
	{
		array<ref array<bool>> values();
		values.Insert({true, true});
		values.Insert({true, false});
		values.Insert({false, false});

		m_pEntity = EDF_Test_WebProxyDbDriverEntityTNestedArray<bool>.Create("00000000-0000-0013-0000-000000000001", values);
		m_pDriver.AddOrUpdateAsync(m_pEntity, new EDF_DbOperationStatusOnlyCallback(this, "Act"));

		SetResult(new EDF_TestResult(true));
	}

	//------------------------------------------------------------------------------------------------
	void Act()
	{
		typename type = m_pEntity.Type();
		EDF_DbFindCallbackMultipleUntyped matchCallback(this, "OnMatchResult");
		EDF_DbFindCallbackMultipleUntyped noMatchCallback(this, "OnNoMatchResult");

		// Should match
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().Contains(true), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().Equals(EDF_DbValues<bool>.From({true, false})), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().Any().Equals(false), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().All().NullOrDefault(), callback: matchCallback);

		// Should not match
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").All().Equals(EDF_DbValues<bool>.From({true, true})), callback: noMatchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").All().All().NullOrDefault(), callback: noMatchCallback);
	}

	//------------------------------------------------------------------------------------------------
	void OnMatchResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_DbEntity> results)
	{
		int matches;
		foreach (EDF_DbEntity result : results)
		{
			if (result.GetId().StartsWith("00000000-0000-0013-0000-"))
				matches++;
		}

		PrintFormat("%1 OnResult: %2", ClassName(), matches == 1);
	}

	//------------------------------------------------------------------------------------------------
	void OnNoMatchResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_DbEntity> results)
	{
		int matches;
		foreach (EDF_DbEntity result : results)
		{
			if (result.GetId().StartsWith("00000000-0000-0013-0000-"))
				matches++;
		}

		PrintFormat("%1 OnResult: %2", ClassName(), matches == 0);
	}
};

//[Test("EDF_WebProxyDbDriverTests")]
class EDF_Test_WebProxyDbDriver_FindAllAsync_StringNestedArrayOperators_OperatorBased : EDF_Test_WebProxyDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Arrange()
	{
		array<ref array<string>> values();
		values.Insert({"Hello", "World", "!"});
		values.Insert({"ArmA", "Reforger"});

		m_pEntity = EDF_Test_WebProxyDbDriverEntityTNestedArray<string>.Create("00000000-0000-0014-0000-000000000001", values);
		m_pDriver.AddOrUpdateAsync(m_pEntity, new EDF_DbOperationStatusOnlyCallback(this, "Act"));

		SetResult(new EDF_TestResult(true));
	}

	//------------------------------------------------------------------------------------------------
	void Act()
	{
		typename type = m_pEntity.Type();
		EDF_DbFindCallbackMultipleUntyped matchCallback(this, "OnMatchResult");
		EDF_DbFindCallbackMultipleUntyped noMatchCallback(this, "OnNoMatchResult");

		// Should match
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().Contains("HeLlO"), callback: matchCallback);

	}

	//------------------------------------------------------------------------------------------------
	void OnMatchResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_DbEntity> results)
	{
		int matches;
		foreach (EDF_DbEntity result : results)
		{
			if (result.GetId().StartsWith("00000000-0000-0014-0000-"))
				matches++;
		}

		PrintFormat("%1 OnResult: %2", ClassName(), matches == 1);
	}

	//------------------------------------------------------------------------------------------------
	void OnNoMatchResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_DbEntity> results)
	{
		int matches;
		foreach (EDF_DbEntity result : results)
		{
			if (result.GetId().StartsWith("00000000-0000-0014-0000-"))
				matches++;
		}

		PrintFormat("%1 OnResult: %2", ClassName(), matches == 0);
	}
};
