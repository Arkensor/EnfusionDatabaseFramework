//! Base class for scripted callbacks. Primarily used for async operations
class EDF_Callback
{
	ref Managed m_pContext;
	Class m_pInvokeInstance;
	string m_sInvokeMethod;

	//------------------------------------------------------------------------------------------------
	sealed void Invoke()
	{
		GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, false, null, m_pContext);
	}

	//------------------------------------------------------------------------------------------------
	void EDF_Callback(Class invokeInstance = null, string invokeMethod = string.Empty, Managed context = null)
	{
		m_pInvokeInstance = invokeInstance;
		m_sInvokeMethod = invokeMethod;
		m_pContext = context;
	}
};

class EDF_DataCallbackSingle<Class T> : EDF_Callback
{
	//------------------------------------------------------------------------------------------------
	void OnComplete(T data, Managed context);

	//------------------------------------------------------------------------------------------------
	sealed void Invoke(T data)
	{
		if (!m_pInvokeInstance ||
			!m_sInvokeMethod ||
			!GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, false, null, data, m_pContext))
		{
			OnComplete(data, m_pContext);
		}
	}
};

class EDF_DataCallbackMultiple<Class T> : EDF_Callback
{
	//------------------------------------------------------------------------------------------------
	void OnComplete(array<T> data, Managed context);

	//------------------------------------------------------------------------------------------------
	sealed void Invoke(array<T> data)
	{
		if (!m_pInvokeInstance ||
			!m_sInvokeMethod ||
			!GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, false, null, data, m_pContext))
		{
			OnComplete(data, m_pContext);
		}
	}
};
