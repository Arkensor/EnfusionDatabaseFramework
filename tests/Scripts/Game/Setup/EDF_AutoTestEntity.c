class EDF_AutoTestEntityClass : GenericEntityClass
{
};

class EDF_AutoTestEntity : GenericEntity
{
	//------------------------------------------------------------------------------------------------
	protected void EDF_AutoTestEntity(IEntitySource src, IEntity parent)
	{
		if (!GetWorld() || GetWorld().IsEditMode())
			return;

		Run();
		
		GetGame().RequestClose();
	}

	//------------------------------------------------------------------------------------------------
	static void Run()
	{
		// Run tests and wait until finished
		TestHarness.Begin();
		int start = System.GetTickCount();
		while (!TestHarness.Run() && (System.GetTickCount() - start) < 10000)
		{
		}
		TestHarness.End();

		// Get and process results
		string testResults = TestHarness.Report();

		int year, month, day, hour, minute, second;
		System.GetYearMonthDayUTC(year, month, day);
		System.GetHourMinuteSecondUTC(hour, minute, second);
		string resultFile = string.Format("TestResults/Run %1-%2-%3 %4_%5_%6.xml", year, month, day, hour, minute, second);

		string hintText;

		if (!testResults.Contains("<failure ") && !testResults.Contains("<error "))
		{
			Print("All test run successfully!");
		}
		else
		{
			Print(string.Format("Some tests failed! For details check MyGames/ArmaReforger/profile/%1", resultFile), LogLevel.ERROR);
		}

		// Write results file
		FileIO.MakeDirectory("$profile:TestResults");

		FileHandle handle = FileIO.OpenFile(string.Format("$profile:%1", resultFile), FileMode.WRITE);
		if (!handle)
			return;

		handle.Write(testResults);
		handle.Close();
	}
}
