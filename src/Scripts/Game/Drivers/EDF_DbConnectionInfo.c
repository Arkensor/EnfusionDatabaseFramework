[BaseContainerProps()]
class EDF_DbConnectionInfoBase
{
	[Attribute()]
	string m_sDatabaseName;

	//------------------------------------------------------------------------------------------------
	void Parse(string connectionString)
	{
		int until = connectionString.IndexOf("?");
		if (until == -1)
			until = connectionString.Length();

		m_sDatabaseName = connectionString.Substring(0, until);
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
