class EDF_HexHelperTests : TestSuite
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

//------------------------------------------------------------------------------------------------
[Test("EDF_HexHelperTests")]
TestResultBase EDF_Test_Utils_IntToHex_ZeroNoPadding_Zero()
{
	return new EDF_TestResult(EDF_HexHelper.Convert(0) == "0");
}

//------------------------------------------------------------------------------------------------
[Test("EDF_HexHelperTests")]
TestResultBase EDF_Test_Utils_IntToHex_ZeroPadding4_Zero4()
{
	return new EDF_TestResult(EDF_HexHelper.Convert(0, fixedLength: 4) == "0000");
}

//------------------------------------------------------------------------------------------------
[Test("EDF_HexHelperTests")]
TestResultBase EDF_Test_Utils_IntToHex_ZeroPadding10_Zero8()
{
	return new EDF_TestResult(EDF_HexHelper.Convert(0, fixedLength: 10) == "00000000");
}

//------------------------------------------------------------------------------------------------
[Test("EDF_HexHelperTests")]
TestResultBase EDF_Test_Utils_IntToHex_1000_3e8()
{
	return new EDF_TestResult(EDF_HexHelper.Convert(1000) == "3e8");
}

//------------------------------------------------------------------------------------------------
[Test("EDF_HexHelperTests")]
TestResultBase EDF_Test_Utils_IntToHex_1000Upper_3E8()
{
	return new EDF_TestResult(EDF_HexHelper.Convert(1000, true) == "3E8");
}

//------------------------------------------------------------------------------------------------
[Test("EDF_HexHelperTests")]
TestResultBase EDF_Test_Utils_IntToHex_13342UpperPadded_00341E()
{
	return new EDF_TestResult(EDF_HexHelper.Convert(13342, true, 6) == "00341E");
}

//------------------------------------------------------------------------------------------------
[Test("EDF_HexHelperTests")]
TestResultBase EDF_Test_Utils_IntToHex_IntMaxUpper_7FFFFFFF()
{
	return new EDF_TestResult(EDF_HexHelper.Convert(int.MAX, true) == "7FFFFFFF");
}

//------------------------------------------------------------------------------------------------
[Test("EDF_HexHelperTests")]
TestResultBase EDF_Test_Utils_IntToHex_MiddleZeros_b00b()
{
	return new EDF_TestResult(EDF_HexHelper.Convert(45067) == "b00b");
}

//------------------------------------------------------------------------------------------------
[Test("EDF_HexHelperTests")]
TestResultBase EDF_Test_Utils_IntToHex_MiddleZerosPadded_00b00b()
{
	return new EDF_TestResult(EDF_HexHelper.Convert(45067, fixedLength: 7) == "000b00b");
}

//------------------------------------------------------------------------------------------------
[Test("EDF_HexHelperTests")]
TestResultBase EDF_Test_Utils_IntToHex_FixedExeeded_AllPresent()
{
	return new EDF_TestResult(EDF_HexHelper.Convert(4095, fixedLength: 2) == "fff");
}
