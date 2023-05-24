class EDF_DbEntityUtilsTests : TestSuite
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

class EDF_Test_DbEntityUtilsSaveStruct : EDF_DbEntity
{
	float m_fFloatValue;
	string m_sStringValue;
}

class EDF_Test_DbEntityUtilsOtherClassType
{
	float m_fFloatValue;
	string m_sStringValue;

	//------------------------------------------------------------------------------------------------
	private void EDF_Test_DbEntityUtilsOtherClassType(float floatValue, string stringValue);
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbEntityUtilsTests")]
TestResultBase EDF_Test_DbEntityUtils_StructAutoCopy_ValidInput_MatchingOutput()
{
	// Arrange
	EDF_Test_DbEntityUtilsSaveStruct saveStruct();
	saveStruct.m_fFloatValue = 42.42;
	saveStruct.m_sStringValue = "Hello World";

	EDF_Test_DbEntityUtilsOtherClassType otherClass = EDF_Test_DbEntityUtilsOtherClassType.Cast(String("EDF_Test_DbEntityUtilsOtherClassType").ToType().Spawn());

	// Act
	EDF_DbEntityUtils.StructAutoCopy(saveStruct, otherClass);

	// Assert
	return new EDF_TestResult(otherClass && (otherClass.m_fFloatValue == saveStruct.m_fFloatValue) && (otherClass.m_sStringValue == saveStruct.m_sStringValue));
}
