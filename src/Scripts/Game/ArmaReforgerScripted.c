modded class ArmaReforgerScripted
{
	//------------------------------------------------------------------------------------------------
	override void OnAfterInit(BaseWorld world)
	{
		EDF_DbRepositoryRegistration.FlushRegistrations(GetScriptModule());
		super.OnAfterInit(world);
	}
};
