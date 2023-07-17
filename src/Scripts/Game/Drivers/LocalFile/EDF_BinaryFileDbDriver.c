[EDF_DbConnectionInfoDriverType(EDF_BinaryFileDbDriver), BaseContainerProps()]
class EDF_BinaryFileDbConnectionInfo : EDF_FileDbDriverInfoBase
{
};

[EDF_DbDriverName({"BinaryFile", "BinFile", "Bin"})]
class EDF_BinaryFileDbDriver : EDF_FileDbDriverBase
{
	//------------------------------------------------------------------------------------------------
	override protected string GetFileExtension()
	{
		return ".bin";
	}

	//------------------------------------------------------------------------------------------------
	override protected EDF_EDbOperationStatusCode WriteToDisk(EDF_DbEntity entity)
	{
		SCR_BinSaveContext writer();
		if (!writer.WriteValue("", entity))
			return EDF_EDbOperationStatusCode.FAILURE_DATA_MALFORMED;

		if (!writer.SaveToFile(string.Format("%1/%2.bin", _GetTypeDirectory(entity.Type()), entity.GetId())))
			return EDF_EDbOperationStatusCode.FAILURE_DB_UNAVAILABLE;

		return EDF_EDbOperationStatusCode.SUCCESS;
	}

	//------------------------------------------------------------------------------------------------
	override protected EDF_EDbOperationStatusCode ReadFromDisk(typename entityType, string entityId, out EDF_DbEntity entity)
	{
		string file = string.Format("%1/%2.bin", _GetTypeDirectory(entityType), entityId);
		if (FileIO.FileExists(file))
		{
			SCR_BinLoadContext reader();
			if (!reader.LoadFromFile(file))
				return EDF_EDbOperationStatusCode.FAILURE_DB_UNAVAILABLE;
			
			entity = EDF_DbEntity.Cast(entityType.Spawn());
			if (!reader.ReadValue("", entity))
				return EDF_EDbOperationStatusCode.FAILURE_DATA_MALFORMED;
		}

		return EDF_EDbOperationStatusCode.SUCCESS;
	}
};
