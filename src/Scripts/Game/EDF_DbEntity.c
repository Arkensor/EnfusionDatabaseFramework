class EDF_DbEntity
{
	static const string FIELD_ID = "m_sId";

	private string m_sId;

	//------------------------------------------------------------------------------------------------
	//! Get the unique id of the db entity. Guaranteed to remain a string in any future version
	string GetId()
	{
		return m_sId;
	}

	//------------------------------------------------------------------------------------------------
	//! Set the unique id on the db entity. Must always be a string!
	void SetId(string id)
	{
		m_sId = id;
	}

	//------------------------------------------------------------------------------------------------
	//! Check if an id has been assigned
	bool HasId()
	{
		return m_sId;
	}

	//------------------------------------------------------------------------------------------------
	//! Utility function to write id to serializer
	void WriteId(notnull SaveContext saveContext)
	{
		saveContext.WriteValue(FIELD_ID, m_sId);
	}

	//------------------------------------------------------------------------------------------------
	//! Utility function to read id from serializer
	void ReadId(notnull LoadContext loadContext)
	{
		loadContext.ReadValue(FIELD_ID, m_sId);
	}
}

class EDF_DbName
{
	protected static const string AUTO_GENERATE = "AUTO_GENERATE";

	protected static ref map<typename, string> s_mMapping;
	protected static ref map<string, typename> s_mReverseMapping;

	//------------------------------------------------------------------------------------------------
	static string Get(typename entityType)
	{
		string name;
		if (!s_mMapping || !s_mMapping.Find(entityType, name))
			name = entityType.ToString();

		return name;
	}

	//------------------------------------------------------------------------------------------------
	static typename GetTypeByName(string name)
	{
		typename resultType;
		if (!s_mReverseMapping || !s_mReverseMapping.Find(name, resultType))
			resultType = name.ToType();

		return resultType;
	}

	//------------------------------------------------------------------------------------------------
	[Obsolete("Use EDF_DbName() without any arguments instead! Support stops with AR 1.7.")]
	static EDF_DbName Automatic()
	{
		return new EDF_DbName(AUTO_GENERATE);
	}

	//------------------------------------------------------------------------------------------------
	//! Set the "pretty" name used for database tables etc, if left empty it is automatically generated.
	void EDF_DbName(string name = AUTO_GENERATE)
	{
		typename entityType = EDF_ReflectionUtils.GetAttributeParent();

		if (!s_mMapping)
			s_mMapping = new map<typename, string>();

		if (!s_mReverseMapping)
			s_mReverseMapping = new map<string, typename>();

		if (name == AUTO_GENERATE)
		{
			name = entityType.ToString();

			int tagIdx = name.IndexOf("_");
			if (tagIdx != -1 && (tagIdx + 1) < name.Length())
				name = name.Substring(tagIdx + 1, name.Length() - (tagIdx + 1));

			if (name.StartsWith("Base"))
				name = name.Substring(4, name.Length() - 4);

			if (name.EndsWith("SaveData"))
				name = name.Substring(0, name.Length() - 8);

			s_mMapping.Set(entityType, name);
			s_mReverseMapping.Set(name, entityType);
		}

		s_mMapping.Set(entityType, name);
		s_mReverseMapping.Set(name, entityType);
	}
}
