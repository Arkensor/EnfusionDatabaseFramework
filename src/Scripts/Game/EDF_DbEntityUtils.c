class EDF_DbEntityUtils<Class TDesition>
{
	//------------------------------------------------------------------------------------------------
	static bool StructAutoCopy(notnull Managed from, notnull TDesition to)
	{
		// TODO: Remove template hack after C++ side bug is fixed.
		// TODO: Use binary serailizer for speeeeed booooost as soon as we can read back from it in memory without file io
		
		SCR_JsonSaveContext writer();
		if (!writer.WriteValue("", from))
			return false;

		string data = writer.ExportToString();

		SCR_JsonLoadContext reader();
		if (!reader.ImportFromString(data))
			return false;

		return reader.ReadValue("", to);
	}
}
