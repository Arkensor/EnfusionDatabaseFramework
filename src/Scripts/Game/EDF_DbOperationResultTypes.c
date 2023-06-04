enum EDF_EDbOperationStatusCode
{
	SUCCESS,

	// Framework failure
	FAILURE_NOT_IMPLEMENTED,
	
	// Database failure
	FAILURE_DB_UNAVAILABLE,
	FAILURE_RESPONSE_MALFORMED,

	// User failure
	FAILURE_DATA_MALFORMED,
	FAILURE_ID_NOT_SET,
	FAILURE_ID_NOT_FOUND,

	// Unknown
	FAILURE_UNKNOWN
};

class EDF_DbFindResultBase
{
	protected EDF_EDbOperationStatusCode m_eStatusCode;

	//------------------------------------------------------------------------------------------------
	EDF_EDbOperationStatusCode GetStatusCode()
	{
		return m_eStatusCode;
	}

	//------------------------------------------------------------------------------------------------
	bool IsSuccess()
	{
		return m_eStatusCode == EDF_EDbOperationStatusCode.SUCCESS;
	}
};

class EDF_DbFindResultMultiple<Class TEntityType> : EDF_DbFindResultBase
{
	protected ref array<ref TEntityType> m_aEntities;

	//------------------------------------------------------------------------------------------------
	array<ref TEntityType> GetEntities()
	{
		return m_aEntities;
	}

	//------------------------------------------------------------------------------------------------
	void EDF_DbFindResultMultiple(EDF_EDbOperationStatusCode statusCode, array<ref TEntityType> entities = null)
	{
		m_eStatusCode = statusCode;
		m_aEntities = entities;
	}
};

class EDF_DbFindResultSingle<Class TEntityType> : EDF_DbFindResultBase
{
	protected ref TEntityType m_pEntity;

	//------------------------------------------------------------------------------------------------
	TEntityType GetEntity()
	{
		return m_pEntity;
	}

	//------------------------------------------------------------------------------------------------
	void EDF_DbFindResultSingle(EDF_EDbOperationStatusCode statusCode, TEntityType entity = null)
	{
		m_eStatusCode = statusCode;
		m_pEntity = entity;
	}
};

class EDF_DbOperationCallback : EDF_Callback
{
};

class EDF_DbOperationStatusOnlyCallback : EDF_DbOperationCallback
{
	//------------------------------------------------------------------------------------------------
	void OnSuccess(Managed context);

	//------------------------------------------------------------------------------------------------
	void OnFailure(EDF_EDbOperationStatusCode statusCode, Managed context);

	//------------------------------------------------------------------------------------------------
	sealed void Invoke(EDF_EDbOperationStatusCode code)
	{
		if (m_pInvokeInstance &&
			m_sInvokeMethod &&
			GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, false, null, code, m_pContext)) return;

		if (code == EDF_EDbOperationStatusCode.SUCCESS)
		{
			OnSuccess(m_pContext);
		}
		else
		{
			OnFailure(code, m_pContext);
		}
	}
};

class EDF_DbFindCallbackBase : EDF_DbOperationCallback
{
	//------------------------------------------------------------------------------------------------
	void Invoke(EDF_EDbOperationStatusCode code, array<ref EDF_DbEntity> findResults);
};

class EDF_DbFindCallbackMultipleUntyped : EDF_DbFindCallbackBase
{
	//------------------------------------------------------------------------------------------------
	void OnSuccess(array<ref EDF_DbEntity> results, Managed context);

	//------------------------------------------------------------------------------------------------
	void OnFailure(EDF_EDbOperationStatusCode statusCode, Managed context);

	//------------------------------------------------------------------------------------------------
	sealed override void Invoke(EDF_EDbOperationStatusCode code, array<ref EDF_DbEntity> findResults)
	{
		if (m_pInvokeInstance &&
			m_sInvokeMethod &&
			GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, false, null, code, findResults, m_pContext)) return;

		if (code == EDF_EDbOperationStatusCode.SUCCESS)
		{
			OnSuccess(findResults, m_pContext);
		}
		else
		{
			OnFailure(code, m_pContext);
		}
	}
};

class EDF_DbFindCallbackMultiple<Class TEntityType> : EDF_DbFindCallbackBase
{
	//------------------------------------------------------------------------------------------------
	void OnSuccess(array<ref TEntityType> results, Managed context);

	//------------------------------------------------------------------------------------------------
	void OnFailure(EDF_EDbOperationStatusCode statusCode, Managed context);

	//------------------------------------------------------------------------------------------------
	sealed override void Invoke(EDF_EDbOperationStatusCode code, array<ref EDF_DbEntity> findResults)
	{
		array<ref TEntityType> strongTypedResults = EDF_RefArrayCaster<EDF_DbEntity, TEntityType>.Convert(findResults);

		if (m_pInvokeInstance &&
			m_sInvokeMethod &&
			GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, false, null, code, strongTypedResults, m_pContext)) return;

		if (code == EDF_EDbOperationStatusCode.SUCCESS)
		{
			OnSuccess(strongTypedResults, m_pContext);
		}
		else
		{
			OnFailure(code, m_pContext);
		}
	}
};

class EDF_DbFindCallbackSingle<Class TEntityType> : EDF_DbFindCallbackBase
{
	//------------------------------------------------------------------------------------------------
	void OnSuccess(TEntityType result, Managed context);

	//------------------------------------------------------------------------------------------------
	void OnFailure(EDF_EDbOperationStatusCode statusCode, Managed context);

	//------------------------------------------------------------------------------------------------
	sealed override void Invoke(EDF_EDbOperationStatusCode code, array<ref EDF_DbEntity> findResults)
	{
		TEntityType typedResult;

		if (findResults.Count() > 0)
			typedResult = TEntityType.Cast(findResults.Get(0));

		if (m_pInvokeInstance &&
			m_sInvokeMethod &&
			GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, false, null, code, typedResult, m_pContext)) return;

		if (code == EDF_EDbOperationStatusCode.SUCCESS)
		{
			OnSuccess(typedResult, m_pContext);
		}
		else
		{
			OnFailure(code, m_pContext);
		}
	}
};

class EDF_DbFindCallbackSingleton<Class TEntityType> : EDF_DbFindCallbackBase
{
	//------------------------------------------------------------------------------------------------
	void OnSuccess(TEntityType result, Managed context);

	//------------------------------------------------------------------------------------------------
	void OnFailure(EDF_EDbOperationStatusCode statusCode, Managed context);

	//------------------------------------------------------------------------------------------------
	sealed override void Invoke(EDF_EDbOperationStatusCode code, array<ref EDF_DbEntity> findResults)
	{
		TEntityType typedResult;

		if (findResults.Count() > 0)
		{
			typedResult = TEntityType.Cast(findResults.Get(0));
		}
		else
		{
			typename spawnType = TEntityType;
			typedResult = TEntityType.Cast(spawnType.Spawn());
		}

		if (m_pInvokeInstance &&
			m_sInvokeMethod &&
			GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, true, false, code, typedResult, m_pContext)) return;

		if (code == EDF_EDbOperationStatusCode.SUCCESS)
		{
			OnSuccess(typedResult, m_pContext);
		}
		else
		{
			OnFailure(code, m_pContext);
		}
	}
};
