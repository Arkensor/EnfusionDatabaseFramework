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
}

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
		connectInfo.m_iProxyPort = 8008;

		m_pDriver = new EDF_WebProxyDbDriver();
		m_pDriver.Initialize(connectInfo);
	}
}

class EDF_Test_WebProxyDbDriverEntityA : EDF_DbEntity
{
	float m_fFloatValue;
	string m_sStringValue;

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		saveContext.WriteValue("_type", Type().ToString());
		WriteId(saveContext);
		saveContext.WriteValue("m_fFloatValue", m_fFloatValue);
		saveContext.WriteValue("m_sStringValue", m_sStringValue);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static EDF_Test_WebProxyDbDriverEntityA Create(string id, float floatValue = 0, string stringValue = string.Empty)
	{
		EDF_Test_WebProxyDbDriverEntityA entity();
		entity.SetId(id);
		entity.m_fFloatValue = floatValue;
		entity.m_sStringValue = stringValue;
		return entity;
	}
}

class EDF_Test_WebProxyDbDriverEntityB : EDF_DbEntity
{
	float m_fFloat;
	string m_sString;
	vector m_vVector;

	//------------------------------------------------------------------------------------------------
	protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		saveContext.WriteValue("_type", Type().ToString());
		WriteId(saveContext);
		saveContext.WriteValue("m_fFloat", m_fFloat);
		saveContext.WriteValue("m_sString", m_sString);
		saveContext.WriteValue("m_vVector", m_vVector);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	static EDF_Test_WebProxyDbDriverEntityB Create(string id, float floatValue = 0, string stringValue = string.Empty, vector vetorValue = vector.Zero)
	{
		EDF_Test_WebProxyDbDriverEntityB entity();
		entity.SetId(id);
		entity.m_fFloat = floatValue;
		entity.m_sString = stringValue;
		entity.m_vVector = vetorValue;
		return entity;
	}
}

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
}

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
}

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
}

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
}

class EDF_Test_WebProxyDbDriverComplexUnwrapEntityInnerWrapper
{
	ref EDF_Test_WebProxyDbDriverEntityTNestedArray<string> m_pNestedArrayHolder;
}

class EDF_Test_WebProxyDbDriverComplexUnwrapEntityMiddleWrapperSecond
{
	ref EDF_Test_WebProxyDbDriverComplexUnwrapEntityInnerWrapper m_pInnerWrapper;
}

class EDF_Test_WebProxyDbDriverComplexUnwrapEntityMiddleWrapperFirst
{
	string m_sArma = "ArmA"; // Take this Lou >:)
	ref EDF_Test_WebProxyDbDriverComplexUnwrapEntityMiddleWrapperSecond m_pInnerWrapper;
}

class EDF_Test_WebProxyDbDriverComplexUnwrapEntityOuterWrapper
{
	ref array<ref array<ref EDF_Test_WebProxyDbDriverComplexUnwrapEntityMiddleWrapperFirst>> m_aMiddleWrappers;
}

class EDF_Test_WebProxyDbDriverComplexUnwrapEntity : EDF_DbEntity
{
	ref array<string> m_aStrings;
	ref array<ref array<string>> m_aOtherArray;
	ref map<int, ref map<int, string>> m_mNestedMap;
	ref array<ref array<ref map<int, ref array<ref array<string>>>>> m_aValues;
	ref array<ref array<ref EDF_Test_WebProxyDbDriverEntityTNestedArray<string>>> m_aNestedValues;

	ref EDF_Test_WebProxyDbDriverComplexUnwrapEntityOuterWrapper m_pOuterWrapper;

	//------------------------------------------------------------------------------------------------
	static EDF_Test_WebProxyDbDriverComplexUnwrapEntity Create(
		string id,
		array<string> strings,
		array<ref array<string>> otherArray,
		map<int, ref map<int, string>> nestedMap,
		array<ref array<ref map<int, ref array<ref array<string>>>>> values,
		array<ref array<ref EDF_Test_WebProxyDbDriverEntityTNestedArray<string>>> nestedValues,
		EDF_Test_WebProxyDbDriverComplexUnwrapEntityOuterWrapper outerWrapper)
	{
		EDF_Test_WebProxyDbDriverComplexUnwrapEntity entity();
		entity.SetId(id);
		entity.m_aStrings = strings;
		entity.m_aOtherArray = otherArray;
		entity.m_mNestedMap = nestedMap;
		entity.m_aValues = values;
		entity.m_aNestedValues = nestedValues;
		entity.m_pOuterWrapper = outerWrapper;
		return entity;
	}
}

[Test("EDF_WebProxyDbDriverTests")]
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
}

[Test("EDF_WebProxyDbDriverTests")]
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
}

[Test("EDF_WebProxyDbDriverTests")]
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
		PrintFormat("%1 OnResult: %2", ClassName(), statusCode == EDF_EDbOperationStatusCode.FAILURE_UNKNOWN);
	}
}

[Test("EDF_WebProxyDbDriverTests")]
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
}

[Test("EDF_WebProxyDbDriverTests")]
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
}

[Test("EDF_WebProxyDbDriverTests")]
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
}

[Test("EDF_WebProxyDbDriverTests")]
class EDF_Test_WebProxyDbDriver_FindAllAsync_LengthOperator_OperatorBased : EDF_Test_WebProxyDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Arrange()
	{
		m_pDriver.AddOrUpdateAsync(EDF_Test_WebProxyDbDriverEntityT<string>.Create("00000000-0000-0007-0000-000000000001", "MarioE"));

		m_pEntity = EDF_Test_WebProxyDbDriverEntityTArray<string>.Create("00000000-0000-0007-0000-000000000002", {"Hello", "World", "!"});
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

		type = String("EDF_Test_WebProxyDbDriverEntityT<string>").ToType();
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_Value").Length().Equals(6), callback: matchCallback);
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
}

[Test("EDF_WebProxyDbDriverTests")]
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
}

[Test("EDF_WebProxyDbDriverTests")]
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
}

[Test("EDF_WebProxyDbDriverTests")]
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
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").ContainsAnyOf({41, 42, 43}), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").ContainsAllOf({42, 1337}), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Equals({1337, 42}), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().GreaterThan(69), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").All().LessThan(2000), callback: matchCallback);

		// Should not match
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").NullOrDefault(), callback: noMatchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Contains(69), callback: noMatchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").ContainsAnyOf({69, 96}), callback: noMatchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").ContainsAllOf({42, 69}), callback: noMatchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Equals({42, 1337}), callback: noMatchCallback);
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
}

[Test("EDF_WebProxyDbDriverTests")]
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
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").All().ContainsAnyOf({42, 69}), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().Equals({69, 96}), callback: matchCallback);

		// Should not match
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().Contains(666), callback: noMatchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").All().ContainsAnyOf({41, 42, 43}), callback: noMatchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").All().Equals({69, 96}), callback: noMatchCallback);
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
}

[Test("EDF_WebProxyDbDriverTests")]
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
}

[Test("EDF_WebProxyDbDriverTests")]
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
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().Equals({true, false}), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().Any().Equals(false), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().All().NullOrDefault(), callback: matchCallback);

		// Should not match
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").All().Equals({true, true}), callback: noMatchCallback);
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
}

[Test("EDF_WebProxyDbDriverTests")]
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
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().Invariant().Contains("HeLlO"), callback: matchCallback);
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
}

[Test("EDF_WebProxyDbDriverTests")]
class EDF_Test_WebProxyDbDriver_FindAllAsync_StringNestedNestedArrayOperators_OperatorBased : EDF_Test_WebProxyDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Arrange()
	{
		array<ref array<string>> values1();
		values1.Insert({"Hello", "World", "!"});
		values1.Insert({"ArmA", "Reforger"});

		array<ref array<string>> values2();
		values2.Insert({"Berlin", "Prague"});
		values2.Insert({"Hype", "Train"});

		map<int, ref array<ref array<string>>> values();
		values.Set(42, values1);
		values.Insert(1337, values2);

		array<ref map<int, ref array<ref array<string>>>> maps();
		maps.Insert(values);

		array<ref array<ref map<int, ref array<ref array<string>>>>> mapHolders();
		mapHolders.Insert(maps);

		array<ref array<string>> secondArrayArray();
		secondArrayArray.Insert({"End", "My", "Suffering"});

		map<int, string> childMap1();
		childMap1.Set(0, "The");
		childMap1.Set(1, "real");
		childMap1.Set(2, "slim");
		childMap1.Set(3, "shady");

		map<int, string> childMap2();
		childMap2.Set(0, "A");
		childMap2.Set(1, "B");
		childMap2.Set(2, "C");
		childMap2.Set(3, "D");

		map<int, ref map<int, string>> nestedMap();
		nestedMap.Set(1001, childMap1);
		nestedMap.Set(1002, childMap2);

		array<ref EDF_Test_WebProxyDbDriverEntityTNestedArray<string>> nestedArray1();
		nestedArray1.Insert(EDF_Test_WebProxyDbDriverEntityTNestedArray<string>.Create("00000000-0000-0015-0000-000000000XXX", values1));
		array<ref array<ref EDF_Test_WebProxyDbDriverEntityTNestedArray<string>>> nestedArray();
		nestedArray.Insert(nestedArray1);

		EDF_Test_WebProxyDbDriverComplexUnwrapEntityOuterWrapper outerWrapper();
		EDF_Test_WebProxyDbDriverComplexUnwrapEntityMiddleWrapperFirst middleFirst1();
		EDF_Test_WebProxyDbDriverComplexUnwrapEntityMiddleWrapperFirst middleFirst2();
		outerWrapper.m_aMiddleWrappers = {{middleFirst1, middleFirst2}};

		EDF_Test_WebProxyDbDriverComplexUnwrapEntityMiddleWrapperSecond middleSecond1();
		EDF_Test_WebProxyDbDriverComplexUnwrapEntityMiddleWrapperSecond middleSecond2();
		middleFirst1.m_pInnerWrapper = middleSecond1;
		middleFirst2.m_pInnerWrapper = middleSecond2;

		EDF_Test_WebProxyDbDriverComplexUnwrapEntityInnerWrapper inner1();
		EDF_Test_WebProxyDbDriverComplexUnwrapEntityInnerWrapper inner2();
		middleSecond1.m_pInnerWrapper = inner1;
		middleSecond2.m_pInnerWrapper = inner2;

		inner1.m_pNestedArrayHolder = EDF_Test_WebProxyDbDriverEntityTNestedArray<string>.Create("00000000-0000-0015-0000-000000000XXX", {{"Hello", "Reforger"}});
		inner2.m_pNestedArrayHolder = EDF_Test_WebProxyDbDriverEntityTNestedArray<string>.Create("00000000-0000-0015-0000-000000000XXX", {{"Arma", "World", "!"}});

		array<string> strings = {"Just", "normal", "Strings"};

		m_pEntity = EDF_Test_WebProxyDbDriverComplexUnwrapEntity.Create("00000000-0000-0015-0000-000000000001", strings, secondArrayArray, nestedMap, mapHolders, nestedArray, outerWrapper);
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

		EDF_DbFindCondition nestedMapCondition = EDF_DbFind
			.Field("m_aValues")
			.Any()
			.Any()
			.Values()
			.Any()
			.Any()
			.All()
			.Length()
			.Equals(6);

		EDF_DbFindCondition nestedCondition = EDF_DbFind
			.Field("m_aNestedValues")
			.Any()
			.Any()
			.Field("m_aValues")
			.Any()
			.Invariant()
			.ContainsAllOf({"arma", "reforger"});

		EDF_DbFindCondition nestedPathWrapperCondition = EDF_DbFind
			.Field("m_pOuterWrapper")
			.Field("m_aMiddleWrappers")
			.Any()
			.Any()
			.Field("m_pInnerWrapper")
			.Field("m_pInnerWrapper")
			.Field("m_pNestedArrayHolder")
			.Field("m_aValues")
			.Any()
			.Any()
			.Length()
			.Equals(1);

		EDF_DbFindCondition nestedPathMultiFieldCondition = EDF_DbFind
			.Field("m_pOuterWrapper.m_aMiddleWrappers")
			.Any()
			.Any()
			.Field("m_sArma")
			.Length()
			.GreaterThan(1);

		// todo change first to all to see if it still works
		EDF_DbFindCondition condition = EDF_DbFind.And({
			EDF_DbFind.Field("m_aStrings").Any().Length().Equals(4),
			EDF_DbFind.Field("m_aOtherArray").Any().Invariant().Contains("Suffering"),
			EDF_DbFind.Field("m_aOtherArray").All().Count().Equals(3),
			EDF_DbFind.Field("m_mNestedMap").Values().Values().Invariant().Contains("ShAdY"),
			nestedMapCondition,
			nestedCondition,
			nestedPathWrapperCondition,
			nestedPathMultiFieldCondition
		});

		m_pDriver.FindAllAsync(type, condition, callback: matchCallback);
	}

	//------------------------------------------------------------------------------------------------
	void OnMatchResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_DbEntity> results)
	{
		int matches;
		foreach (EDF_DbEntity result : results)
		{
			if (result.GetId().StartsWith("00000000-0000-0015-0000-"))
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
			if (result.GetId().StartsWith("00000000-0000-0015-0000-"))
				matches++;
		}

		PrintFormat("%1 OnResult: %2", ClassName(), matches == 0);
	}
}

[Test("EDF_WebProxyDbDriverTests")]
class EDF_Test_WebProxyDbDriver_FindAllAsync_TypenameOfTypeAny_Matches : EDF_Test_WebProxyDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Arrange()
	{
		m_pEntity = EDF_Test_WebProxyDbDriverEntityHolder.Create("00000000-0000-0016-0000-000000000001", {
			EDF_Test_WebProxyDbDriverEntityA.Create("00000000-0000-0016-0000-000000000002", 42.0),
			EDF_Test_WebProxyDbDriverEntityB.Create("00000000-0000-0016-0000-000000000003", 1337.0)
		});
		m_pDriver.AddOrUpdateAsync(m_pEntity, new EDF_DbOperationStatusOnlyCallback(this, "Act"));

		SetResult(new EDF_TestResult(true));
	}

	//------------------------------------------------------------------------------------------------
	void Act()
	{
		EDF_DbFindCallbackMultipleUntyped matchCallback(this, "OnMatchResult");

		// Should match
		typename type = m_pEntity.Type();
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aEntities").OfType(EDF_Test_WebProxyDbDriverEntityB).Any().Field("m_fFloat").Equals(1337.0), callback: matchCallback);
	}

	//------------------------------------------------------------------------------------------------
	void OnMatchResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_DbEntity> results)
	{
		int matches;
		foreach (EDF_DbEntity result : results)
		{
			if (result.GetId().StartsWith("00000000-0000-0016-0000-"))
				matches++;
		}

		PrintFormat("%1 OnResult: %2", ClassName(), matches == 1);
	}
}

[Test("EDF_WebProxyDbDriverTests")]
class EDF_Test_WebProxyDbDriver_FindAllAsync_TypenameArrayEquals_Matches : EDF_Test_WebProxyDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Arrange()
	{
		// TODO: Change back when https://feedback.bistudio.com/T174003 is fixed.
		m_pEntity = EDF_Test_WebProxyDbDriverEntityTArray<string>.Create("00000000-0000-0017-0000-000000000001", {"EDF_Test_WebProxyDbDriverEntityA", "EDF_Test_WebProxyDbDriverEntityB"});
		m_pDriver.AddOrUpdateAsync(m_pEntity, new EDF_DbOperationStatusOnlyCallback(this, "Act"));

		SetResult(new EDF_TestResult(true));
	}

	//------------------------------------------------------------------------------------------------
	void Act()
	{
		EDF_DbFindCallbackMultipleUntyped matchCallback(this, "OnMatchResult");

		// Should match
		typename type = m_pEntity.Type();
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Any().Equals(EDF_Test_WebProxyDbDriverEntityA), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aValues").Equals({EDF_Test_WebProxyDbDriverEntityA, EDF_Test_WebProxyDbDriverEntityB}), callback: matchCallback);
	}

	//------------------------------------------------------------------------------------------------
	void OnMatchResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_DbEntity> results)
	{
		int matches;
		foreach (EDF_DbEntity result : results)
		{
			if (result.GetId().StartsWith("00000000-0000-0017-0000-"))
				matches++;
		}

		PrintFormat("%1 OnResult: %2", ClassName(), matches == 1);
	}
}

[Test("EDF_WebProxyDbDriverTests")]
class EDF_Test_WebProxyDbDriver_FindAllAsync_ObjectArrayTypenameCheck_Matches : EDF_Test_WebProxyDbDriver_TestBase
{
	//------------------------------------------------------------------------------------------------
	[Step(EStage.Main)]
	void Arrange()
	{
		m_pEntity = EDF_Test_WebProxyDbDriverEntityHolder.Create("00000000-0000-0018-0000-000000000001", {
			EDF_Test_WebProxyDbDriverEntityA.Create("00000000-0000-0018-0000-000000000002"),
			EDF_Test_WebProxyDbDriverEntityB.Create("00000000-0000-0018-0000-000000000003")
		});
		m_pDriver.AddOrUpdateAsync(m_pEntity, new EDF_DbOperationStatusOnlyCallback(this, "Act"));

		SetResult(new EDF_TestResult(true));
	}

	//------------------------------------------------------------------------------------------------
	void Act()
	{
		EDF_DbFindCallbackMultipleUntyped matchCallback(this, "OnMatchResult");

		// Should match
		typename type = m_pEntity.Type();
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aEntities").Any().Equals(EDF_Test_WebProxyDbDriverEntityA), callback: matchCallback);
		m_pDriver.FindAllAsync(type, EDF_DbFind.Field("m_aEntities").Equals({EDF_Test_WebProxyDbDriverEntityA, EDF_Test_WebProxyDbDriverEntityB}), callback: matchCallback);
	}

	//------------------------------------------------------------------------------------------------
	void OnMatchResult(EDF_EDbOperationStatusCode statusCode, array<ref EDF_DbEntity> results)
	{
		int matches;
		foreach (EDF_DbEntity result : results)
		{
			if (result.GetId().StartsWith("00000000-0000-0018-0000-"))
				matches++;
		}

		PrintFormat("%1 OnResult: %2", ClassName(), matches == 1);
	}
}
