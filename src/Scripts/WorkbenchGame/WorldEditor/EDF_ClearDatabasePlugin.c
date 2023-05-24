[WorkbenchPluginAttribute("Clear Database", "Deletes the .db folder!", "alt+c", "", {"WorldEditor", "ResourceManager"}, "", 0xf1c0)]
class EDF_ClearDatabasePlugin: WorkbenchPlugin
{
	const string DB_BASE_DIR = "$profile:/.db";

	//------------------------------------------------------------------------------------------------
	override void Run()
	{
		System.FindFiles(DeleteFileCallback, DB_BASE_DIR, ".json");
		System.FindFiles(DeleteFileCallback, DB_BASE_DIR, ".bin");
		System.FindFiles(DeleteFileCallback, DB_BASE_DIR, string.Empty);
		FileIO.DeleteFile(DB_BASE_DIR);
		Print("-- DATABASE CLEARED --", LogLevel.WARNING);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DeleteFileCallback(string path, FileAttribute attributes)
	{
		FileIO.DeleteFile(path);
	}
};
