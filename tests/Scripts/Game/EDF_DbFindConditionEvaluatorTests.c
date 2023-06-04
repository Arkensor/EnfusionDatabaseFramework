class EDF_DbFindConditionEvaluatorTests : TestSuite
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

class EDF_Test_FindConditionEvaluatorContainerItem : EDF_DbEntity
{
	int m_Int = 1337;
	float m_Float = 42.42;
	bool m_Bool = true;
	string m_String = "hello WoRld";
	vector m_Vector = Vector(1, 3, 7);

	EDF_Test_FindConditionEvaluatorContainerItem m_NullField;

	ref array<int> m_IntArray = {42, 1337};
	ref array<float> m_FloatArray = {42.42, 1337.1337};
	ref array<bool> m_BoolArray = {true, false, true, false};
	ref array<string> m_StringArray = {"HeLlO", "WoRlD"};
	ref array<vector> m_VectorArray = {Vector(1, 3, 7), Vector(1, 0, 1)};

	ref set<float> m_FloatSet;

	//------------------------------------------------------------------------------------------------
	void EDF_Test_FindConditionEvaluatorContainerItem()
	{
		m_FloatSet = new set<float>();
		m_FloatSet.Insert(42.42);
		m_FloatSet.Insert(1337.1337);
	}
};

class EDF_Test_FindConditionEvaluatorContainer : EDF_DbEntity
{
	ref EDF_Test_FindConditionEvaluatorContainerItem m_SingleItem;
	ref array<ref EDF_Test_FindConditionEvaluatorContainerItem> m_MultiItemArray;
	ref set<ref EDF_Test_FindConditionEvaluatorContainerItem> m_MultiItemSet;
	ref map<string, ref EDF_Test_FindConditionEvaluatorContainerItem> m_MultiItemMap;

	ref map<string, int> m_IntMap;

	//------------------------------------------------------------------------------------------------
	void EDF_Test_FindConditionEvaluatorContainer()
	{
		m_SingleItem = new EDF_Test_FindConditionEvaluatorContainerItem();

		m_MultiItemArray = new array<ref EDF_Test_FindConditionEvaluatorContainerItem>();
		m_MultiItemArray.Insert(new EDF_Test_FindConditionEvaluatorContainerItem());
		m_MultiItemArray.Insert(new EDF_Test_FindConditionEvaluatorContainerItem());

		m_MultiItemSet = new set<ref EDF_Test_FindConditionEvaluatorContainerItem>();
		m_MultiItemSet.Insert(new EDF_Test_FindConditionEvaluatorContainerItem());
		m_MultiItemSet.Insert(new EDF_Test_FindConditionEvaluatorContainerItem());

		m_MultiItemMap = new map<string, ref EDF_Test_FindConditionEvaluatorContainerItem>();
		m_MultiItemMap.Set("key1", new EDF_Test_FindConditionEvaluatorContainerItem());
		m_MultiItemMap.Set("key2", new EDF_Test_FindConditionEvaluatorContainerItem());

		m_IntMap = new map<string, int>();
		m_IntMap.Set("1337", 1337);
		m_IntMap.Set("42", 42);
	}
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Equals_CorrectInt_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_Int").Equals(1337);

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Equals_IncorrectInt_False()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_Int").Equals(1000);

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(!matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Equals_CorrectFloat_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_Float").Equals(42.42);

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Equals_IncorrectFloat_False()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_Int").Equals(10.13);

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(!matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Equals_CorrectBool_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_Bool").Equals(true);

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Equals_IncorrectBool_False()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_Bool").Equals(false);

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(!matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Equals_CorrectString_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_String").Equals("Hello World");

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Equals_IncorrectString_False()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_String").Equals("Noot noot.");

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(!matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Equals_CorrectVector_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_Vector").Equals(Vector(1, 3, 7));

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Equals_IncorrectVector_False()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_Vector").Equals(Vector(6, 6, 6));

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(!matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Contains_StringField_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_String").Contains("Hello");

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_And_OneTrue_False()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.And({
		EDF_DbFind.Field("m_Bool").Equals(true),
		EDF_DbFind.Field("m_Int").Equals(100),
	});

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(!matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_And_BothTrue_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.And({
		EDF_DbFind.Field("m_Bool").Equals(true),
		EDF_DbFind.Field("m_Int").Equals(1337),
	});

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Or_NoneTrue_False()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Or({
		EDF_DbFind.Field("m_Bool").Equals(false),
		EDF_DbFind.Field("m_Int").Equals(100),
	});

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(!matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Or_OneTrue_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Or({
		EDF_DbFind.Field("m_Bool").Equals(false),
		EDF_DbFind.Field("m_Int").Equals(1337),
	});

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_EqualsAnyOf_OneTrue_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_Float").EqualsAnyOf(EDF_DbValues<float>.From({3.14, 2.22, 42.42}));

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_EqualsAnyOf_NoneTrue_False()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_Float").EqualsAnyOf(EDF_DbValues<float>.From({3.14, 2.22, 3.33}));

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(!matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_LessThan_FloatLess_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_Float").LessThan(100.5);

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_LessThanOrEqual_IntGreater_False()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_Int").LessThanOrEquals(100);

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(!matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_GreaterThanOrEqual_VectorLess_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_Vector").GreaterThanOrEquals(Vector(1, 1, 1));

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Between_InRange_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_Int").Between(1300, 1400);

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Not_Matching_False()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_Bool").Not().Equals(true);

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(!matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Null_NullField_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_NullField").Null();

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Empty_FilledCollection_False()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_BoolArray").Empty();

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(!matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Count_AnyOfIncludingTwo_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_IntArray").Count().EqualsAnyOf(EDF_DbValues<int>.From({1, 2, 3}));

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Length_Equals_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_String").Length().Equals(11);

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Any_MatchingValue_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_IntArray").Any().GreaterThanOrEquals(1300);

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_All_OneNotMatching_False()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_IntArray").All().LessThanOrEquals(100);

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(!matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Keys_OneMatching_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainer entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_MultiItemMap").Keys().Any().Equals("key2");

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Values_OneMatching_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainer entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_IntMap").Values().Any().EqualsAnyOf(EDF_DbValues<int>.From({10, 1300, 42}));

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_At_IdxZeroNestedVectorContained_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainer entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_MultiItemArray").At(0).Field("m_VectorArray").Contains(Vector(1, 0, 1));

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_ContainsAnyOf_PartialIntersect_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_FloatSet").ContainsAnyOf(EDF_DbValues<float>.From({666.666, 1337.1337}));

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_ContainsAllOf_PartialIntersect_False()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_FloatSet").ContainsAllOf(EDF_DbValues<float>.From({666.666, 1337.1337}));

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(!matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Equals_ArrayMatches_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_FloatArray").Equals(EDF_DbValues<float>.From({42.42, 1337.1337}));

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_Contains_StringArrayItemMatches_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorContainerItem entity();

	EDF_DbFindCondition conditon = EDF_DbFind.Field("m_StringArray").Contains("World");

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(entity, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

class EDF_Test_FindConditionEvaluatorPolymorphBase : EDF_DbEntity
{
	ref array<ref EDF_Test_FindConditionEvaluatorPolymorphBase> m_NestedArrayItems = {};
};

class EDF_Test_FindConditionEvaluatorPolymorphDerivedA : EDF_Test_FindConditionEvaluatorPolymorphBase
{
	int m_intFieldA;

	//------------------------------------------------------------------------------------------------
	void EDF_Test_FindConditionEvaluatorPolymorphDerivedA(int value = 0)
	{
		m_intFieldA = value;
	}
};

class EDF_Test_FindConditionEvaluatorPolymorphDerivedB : EDF_Test_FindConditionEvaluatorPolymorphBase
{
	float m_floatFieldB;

	//------------------------------------------------------------------------------------------------
	void EDF_Test_FindConditionEvaluatorPolymorphDerivedB(float value = 0.0)
	{
		m_floatFieldB = value;
	}
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
TestResultBase EDF_Test_DbFindConditionEvaluator_FirstOf_Matches_True()
{
	// Arrange
	EDF_Test_FindConditionEvaluatorPolymorphBase container();
	container.m_NestedArrayItems.Insert(new EDF_Test_FindConditionEvaluatorPolymorphDerivedB());
	container.m_NestedArrayItems.Insert(new EDF_Test_FindConditionEvaluatorPolymorphDerivedA(10));
	container.m_NestedArrayItems.Insert(new EDF_Test_FindConditionEvaluatorPolymorphDerivedA(1337));
	container.m_NestedArrayItems.Insert(new EDF_Test_FindConditionEvaluatorPolymorphDerivedB());
	container.m_NestedArrayItems.Insert(new EDF_Test_FindConditionEvaluatorPolymorphDerivedA(20));
	container.m_NestedArrayItems.Insert(new EDF_Test_FindConditionEvaluatorPolymorphDerivedB());

	EDF_DbFindCondition conditon = EDF_DbFind
		.Field("m_NestedArrayItems")
		.OfType(EDF_Test_FindConditionEvaluatorPolymorphDerivedA)
		.Any()
		.Field("m_intFieldA")
		.Equals(10);

	// Act
	bool matches = EDF_DbFindConditionEvaluator.Evaluate(container, conditon);

	// Assert
	return new EDF_TestResult(matches);
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
class EDF_Test_DbFindConditionEvaluator_AllOf_ConstMatches_True : TestBase
{
	const ref EDF_DbFindCondition s_Condition = EDF_DbFind
		.Field("m_NestedArrayItems")
		.OfType(EDF_Test_FindConditionEvaluatorPolymorphDerivedB)
		.All()
		.Field("m_floatFieldB")
		.GreaterThanOrEquals(42.0);

	[Step(EStage.Main)]
	void DoTest()
	{
		// Arrange
		EDF_Test_FindConditionEvaluatorPolymorphBase container();
		container.m_NestedArrayItems.Insert(new EDF_Test_FindConditionEvaluatorPolymorphDerivedB(42.1));
		container.m_NestedArrayItems.Insert(new EDF_Test_FindConditionEvaluatorPolymorphDerivedA());
		container.m_NestedArrayItems.Insert(new EDF_Test_FindConditionEvaluatorPolymorphDerivedA());
		container.m_NestedArrayItems.Insert(new EDF_Test_FindConditionEvaluatorPolymorphDerivedB(42.2));
		container.m_NestedArrayItems.Insert(new EDF_Test_FindConditionEvaluatorPolymorphDerivedA());
		container.m_NestedArrayItems.Insert(new EDF_Test_FindConditionEvaluatorPolymorphDerivedB(42.42));

		// Act
		bool matches = EDF_DbFindConditionEvaluator.Evaluate(container, s_Condition);

		// Assert
		SetResult(new EDF_TestResult(matches));
	}
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
class EDF_Test_DbFindConditionEvaluator_AnyEqualsTypename_Match_True : TestBase
{
	[Step(EStage.Main)]
	void DoTest()
	{
		EDF_DbFindCondition condition = EDF_DbFind.Field("m_NestedArrayItems")
			.Any()
			.Equals(EDF_Test_FindConditionEvaluatorPolymorphDerivedB);

		// Arrange
		EDF_Test_FindConditionEvaluatorPolymorphBase container();
		container.m_NestedArrayItems.Insert(new EDF_Test_FindConditionEvaluatorPolymorphDerivedA());
		container.m_NestedArrayItems.Insert(new EDF_Test_FindConditionEvaluatorPolymorphDerivedB());

		// Act
		bool matches = EDF_DbFindConditionEvaluator.Evaluate(container, condition);

		// Assert
		SetResult(new EDF_TestResult(matches));
	}
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
class EDF_Test_DbFindConditionEvaluator_AllEqualsTypename_MixedTypes_False : TestBase
{
	[Step(EStage.Main)]
	void DoTest()
	{
		EDF_DbFindCondition condition = EDF_DbFind.Field("m_NestedArrayItems")
			.All()
			.Equals(EDF_Test_FindConditionEvaluatorPolymorphDerivedB);

		// Arrange
		EDF_Test_FindConditionEvaluatorPolymorphBase container();
		container.m_NestedArrayItems.Insert(new EDF_Test_FindConditionEvaluatorPolymorphDerivedA());
		container.m_NestedArrayItems.Insert(new EDF_Test_FindConditionEvaluatorPolymorphDerivedB());

		// Act
		bool matches = EDF_DbFindConditionEvaluator.Evaluate(container, condition);

		// Assert
		SetResult(new EDF_TestResult(!matches));
	}
};

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
class EDF_Test_DbFindConditionEvaluator_ContainsTypename_Match_True : TestBase
{
	[Step(EStage.Main)]
	void DoTest()
	{
		EDF_DbFindCondition condition = EDF_DbFind.Field("m_NestedArrayItems")
			.Contains(EDF_Test_FindConditionEvaluatorPolymorphDerivedB);

		// Arrange
		EDF_Test_FindConditionEvaluatorPolymorphBase container();
		container.m_NestedArrayItems.Insert(new EDF_Test_FindConditionEvaluatorPolymorphDerivedA());
		container.m_NestedArrayItems.Insert(new EDF_Test_FindConditionEvaluatorPolymorphDerivedB());

		// Act
		bool matches = EDF_DbFindConditionEvaluator.Evaluate(container, condition);

		// Assert
		SetResult(new EDF_TestResult(matches));
	}
};


//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
class EDF_Test_DbFindConditionEvaluator_NotContainsTypename_SingleType_True : TestBase
{
	[Step(EStage.Main)]
	void DoTest()
	{
		EDF_DbFindCondition condition = EDF_DbFind.Field("m_NestedArrayItems")
			.Not()
			.Contains(EDF_Test_FindConditionEvaluatorPolymorphDerivedB);

		// Arrange
		EDF_Test_FindConditionEvaluatorPolymorphBase container();
		container.m_NestedArrayItems.Insert(new EDF_Test_FindConditionEvaluatorPolymorphDerivedA());
		container.m_NestedArrayItems.Insert(new EDF_Test_FindConditionEvaluatorPolymorphDerivedA());

		// Act
		bool matches = EDF_DbFindConditionEvaluator.Evaluate(container, condition);

		// Assert
		SetResult(new EDF_TestResult(matches));
	}
};


//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionEvaluatorTests")]
class EDF_Test_DbFindConditionEvaluator_ContainsTypename_Empty_False : TestBase
{
	[Step(EStage.Main)]
	void DoTest()
	{
		EDF_DbFindCondition condition = EDF_DbFind.Field("m_NestedArrayItems")
			.Contains(EDF_Test_FindConditionEvaluatorPolymorphDerivedB);

		// Arrange
		EDF_Test_FindConditionEvaluatorPolymorphBase container();

		// Act
		bool matches = EDF_DbFindConditionEvaluator.Evaluate(container, condition);

		// Assert
		SetResult(new EDF_TestResult(!matches));
	}
};
