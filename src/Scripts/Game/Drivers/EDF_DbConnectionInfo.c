[BaseContainerProps()]
class EDF_DbConnectionInfoBase
{
	[Attribute()]
	string m_sDatabaseName;

	//------------------------------------------------------------------------------------------------
	void ReadOptions(string connectionString)
	{
		int until = connectionString.IndexOf("?");
		if (until == -1)
			until = connectionString.Length();

		m_sDatabaseName = connectionString.Substring(0, until);
	}

	//------------------------------------------------------------------------------------------------
	static EDF_DbConnectionInfoBase Parse(string connectionString)
	{
		int driverEndIdx = connectionString.IndexOf("://");
		if (driverEndIdx == -1)
		{
			Debug.Error(string.Format("Invalid -ConnectionString=... parameter value '%1'.", connectionString));
			return null;
		}

		string driverName = connectionString.Substring(0, driverEndIdx);
		string connectionInfoString = connectionString.Substring(driverEndIdx + 3, connectionString.Length() - (driverName.Length() + 3));

		typename driverType = EDF_DbDriverRegistry.Get(driverName);
		if (!driverType.IsInherited(EDF_DbDriver))
		{
			Debug.Error(string.Format("Incompatible database driver type '%1'.", driverType));
			return null;
		}

		typename connectionInfoType = EDF_DbConnectionInfoDriverType.GetConnectionInfoType(driverType);
		EDF_DbConnectionInfoBase connectionInfo = EDF_DbConnectionInfoBase.Cast(connectionInfoType.Spawn());
		if (!connectionInfo)
			return null;

		connectionInfo.Parse(connectionInfoString);

		return connectionInfo;
	}
};

class EDF_DbConnectionInfoDriverType
{
	protected static ref map<typename, typename> s_mMapping;
	protected static ref map<typename, typename> s_mReverseMapping;

	//------------------------------------------------------------------------------------------------
	static typename GetDriverType(typename connectionInfoType)
	{
		if (s_mMapping)
			return s_mMapping.Get(connectionInfoType);

		return typename.Empty;
	}

	//------------------------------------------------------------------------------------------------
	static typename GetConnectionInfoType(typename driverType)
	{
		if (s_mReverseMapping)
			return s_mReverseMapping.Get(driverType);

		return typename.Empty;
	}

	//------------------------------------------------------------------------------------------------
	void EDF_DbConnectionInfoDriverType(typename driver)
	{
		typename connectionInfoType = EDF_ReflectionUtils.GetAttributeParent();

		if (!s_mMapping)
			s_mMapping = new map<typename, typename>();

		if (!s_mReverseMapping)
			s_mReverseMapping = new map<typename, typename>();

		s_mMapping.Set(connectionInfoType, driver);
		s_mReverseMapping.Set(driver, connectionInfoType);
	}
};
