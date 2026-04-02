sealed class EDF_AutotestResult : SCR_AutotestResult
{
	sealed static SCR_AutotestResult FromResult(bool success)
	{
		if (success)
			return SCR_AutotestResult.AsSuccess();
		
		return SCR_AutotestResult.AsFailure("");
	}
}
