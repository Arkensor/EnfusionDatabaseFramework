modded class SCR_BaseGameMode
{
	//------------------------------------------------------------------------------------------------
	override void OnGameEnd()
	{
		// On game end before any final flushes, hard map the async api into sync to make those operations blocking and not loose data
		EDF_DbDriver.s_bForceBlocking = true;
		super.OnGameEnd();
	}

	//------------------------------------------------------------------------------------------------
	void ~SCR_BaseGameMode()
	{
		// TODO: Find better event for "session" teardown that works on dedicated servers, workbench and player hosted missions.
		EDF_DbDriver.s_bForceBlocking = false;
		EDF_DbEntityIdGenerator.Reset();
		EDF_WebProxyDbDriverCallback.Reset();
		EDF_DbRepositoryFactory.ResetCache();
	}
}
