class EDF_DbEntityUtils
{
	//------------------------------------------------------------------------------------------------
	//! Memeber-wise deep copy of data from source to matching named instance variables on the destination
	static bool StructAutoCopy(notnull Managed source, notnull Managed destination)
	{
		JsonSaveContext writer();
		if (!writer.WriteValue("", source))
			return false;

		const string data = writer.SaveToString();

		JsonLoadContext reader();
		if (!reader.LoadFromString(data))
			return false;

		return reader.ReadValue("", destination);
	}

	//------------------------------------------------------------------------------------------------
	static Managed DeepCopy(notnull Managed instance)
	{
		BinarySaveContext writer();
		if (!writer.WriteValue("", instance))
			return null;

		BinaryLoadContext reader();
		if (!reader.LoadFromContainer(writer.SaveToContainer()))
			return null;

		auto deepCopy = instance.Type().Spawn();
		if (!reader.ReadValue("", deepCopy))
			return null;

		return deepCopy;
	}
}
