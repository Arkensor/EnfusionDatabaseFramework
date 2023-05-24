class EDF_TestResult : TestResultBase
{
	protected bool m_Value;

	//------------------------------------------------------------------------------------------------
	void EDF_TestResult(bool value)
	{
		m_Value = value;
	}

	//------------------------------------------------------------------------------------------------
	override bool Failure()
	{
		return !m_Value;
	}

	//------------------------------------------------------------------------------------------------
	override string FailureText()
	{
		return "<failure type=\"EDF_TestResult\">Failed</failure>";
	}
};
