[EDF_DbConnectionInfoDriverType(EDF_JsonFileDbDriver), BaseContainerProps()]
class EDF_JsonFileDbConnectionInfo : EDF_FileDbDriverInfoBase
{
	[Attribute(defvalue: "1", desc: "Save files prettified (formatted). Primarily for debugging.")]
	bool m_bPrettify;

	//------------------------------------------------------------------------------------------------
	override void ReadOptions(string connectionString)
	{
		super.ReadOptions(connectionString);
		m_bPrettify = connectionString.Contains("prettify=true") || connectionString.Contains("pretty=true");
	}
};

[EDF_DbDriverName({"JsonFile", "Json"})]
class EDF_JsonFileDbDriver : EDF_FileDbDriverBase
{
	protected bool m_bPrettify;

	//------------------------------------------------------------------------------------------------
	override bool Initialize(notnull EDF_DbConnectionInfoBase connectionInfo)
	{
		if (!super.Initialize(connectionInfo))
			return false;

		auto jsonConnectInfo = EDF_JsonFileDbConnectionInfo.Cast(connectionInfo);
		m_bPrettify = jsonConnectInfo.m_bPrettify;
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override protected string GetFileExtension()
	{
		return ".json";
	}

	//------------------------------------------------------------------------------------------------
	override protected EDF_EDbOperationStatusCode WriteToDisk(EDF_DbEntity entity)
	{
		ContainerSerializationSaveContext writer();
		BaseJsonSerializationSaveContainer jsonContainer;
		if (m_bPrettify)
		{
			jsonContainer = new PrettyJsonSaveContainer();
		}
		else
		{
			jsonContainer = new JsonSaveContainer();
		}

		jsonContainer.SetMaxDecimalPlaces(5);
		writer.SetContainer(jsonContainer);

		if (!writer.WriteValue("", entity))
			return EDF_EDbOperationStatusCode.FAILURE_DATA_MALFORMED;

		if (!jsonContainer.SaveToFile(string.Format("%1/%2.json", _GetTypeDirectory(entity.Type()), entity.GetId())))
			return EDF_EDbOperationStatusCode.FAILURE_DB_UNAVAILABLE;

		return EDF_EDbOperationStatusCode.SUCCESS;
	}

	//------------------------------------------------------------------------------------------------
	override protected EDF_EDbOperationStatusCode ReadFromDisk(typename entityType, string entityId, out EDF_DbEntity entity)
	{
		string file = string.Format("%1/%2.json", _GetTypeDirectory(entityType), entityId);
		if (FileIO.FileExists(file))
		{
			SCR_JsonLoadContext reader();
			if (!reader.LoadFromFile(file))
				return EDF_EDbOperationStatusCode.FAILURE_DB_UNAVAILABLE;

			entity = EDF_DbEntity.Cast(entityType.Spawn());
			if (!reader.ReadValue("", entity))
				return EDF_EDbOperationStatusCode.FAILURE_DATA_MALFORMED;
		}

		return EDF_EDbOperationStatusCode.SUCCESS;
	}
};
