/*
class EDF_BufferedDbContext : EDF_DbContext
{
	//------------------------------------------------------------------------------------------------
	void Flush(int maxBatchSize = 50)
	{
		EDF_DbDriverBufferWrapper.Cast(m_Driver).Flush(maxBatchSize);
	}

	//------------------------------------------------------------------------------------------------
	override static EDF_BufferedDbContext Create(notnull EDF_DbConnectionInfoBase connectionInfo)
	{
		EDF_DbContext baseContext = EDF_DbContext.Create(connectionInfo);
		if (baseContext)
			return new EDF_BufferedDbContext(baseContext.m_Driver);

		return null;
	}

	//------------------------------------------------------------------------------------------------
	protected void EDF_BufferedDbContext(notnull EDF_DbDriver driver)
	{
		m_Driver = new EDF_DbDriverBufferWrapper(driver);
	}
};
*/