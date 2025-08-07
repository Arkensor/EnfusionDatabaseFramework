[BaseContainerProps(category: "Autotest")]
class EDF_DbEntityUtilsTests : SCR_AutotestSuiteBase
{
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
	/* private -- Hotfix for 1.0, do not ctor directly */
	void EDF_Test_DbEntityUtilsOtherClassType(float floatValue, string stringValue);
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbEntityUtilsTests")]
class EDF_Test_DbEntityUtils_StructAutoCopy_ValidInput_MatchingOutput : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Arrange
		EDF_Test_DbEntityUtilsSaveStruct saveStruct();
		saveStruct.m_fFloatValue = 42.42;
		saveStruct.m_sStringValue = "Hello World";

		EDF_Test_DbEntityUtilsOtherClassType otherClass = EDF_Test_DbEntityUtilsOtherClassType.Cast(String("EDF_Test_DbEntityUtilsOtherClassType").ToType().Spawn());

		// Act
		EDF_DbEntityUtils.StructAutoCopy(saveStruct, otherClass);

		// Assert
		SetResult(new SCR_AutotestResult(otherClass && (otherClass.m_fFloatValue == saveStruct.m_fFloatValue) && (otherClass.m_sStringValue == saveStruct.m_sStringValue)));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbEntityUtilsTests")]
class EDF_Test_DbEntityUtils_DeepCopy_ValidInput_MatchingOutput : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Arrange
		EDF_Test_DbEntityUtilsSaveStruct saveStruct();
		saveStruct.m_fFloatValue = 42.42;
		saveStruct.m_sStringValue = "Hello World";

		// Act
		auto deepCopy = EDF_Test_DbEntityUtilsSaveStruct.Cast(EDF_DbEntityUtils.DeepCopy(saveStruct));

		// Assert
		SetResult(new SCR_AutotestResult(deepCopy && (deepCopy.m_fFloatValue == saveStruct.m_fFloatValue) && (deepCopy.m_sStringValue == saveStruct.m_sStringValue)));
	}
}
