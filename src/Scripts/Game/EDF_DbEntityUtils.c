class EDF_DbEntityUtils
{
	//------------------------------------------------------------------------------------------------
	//! Memeber-wise deep copy of data from source to matching named instance variables on the destination
	static bool StructAutoCopy(notnull Managed source, notnull Managed destination)
	{
		SCR_JsonSaveContext writer();
		if (!writer.WriteValue("", source))
			return false;

		string data = writer.ExportToString();

		SCR_JsonLoadContext reader();
		if (!reader.ImportFromString(data))
			return false;

		return reader.ReadValue("", destination);
	}

	//------------------------------------------------------------------------------------------------
	static Managed DeepCopy(notnull Managed instance)
	{
		SCR_BinSaveContext writer();
		if (!writer.WriteValue("", instance))
			return null;

		SCR_BinLoadContext reader();
		if (!reader.LoadFromContainer(writer.SaveToContainer()))
			return null;

		auto deepCopy = instance.Type().Spawn();
		if (!reader.ReadValue("", deepCopy))
			return null;

		return deepCopy;
	}
}
