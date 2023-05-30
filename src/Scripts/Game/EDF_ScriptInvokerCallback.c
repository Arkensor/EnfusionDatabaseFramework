// Needs to have base class without the invoke method otherwise script vm thinks that "callbacks do not support overloaded methods" ...
class EDF_ScriptInvokerCallbackBase
{
	protected static ref set<ref EDF_ScriptInvokerCallbackBase> s_CallbackReferences = new set<ref EDF_ScriptInvokerCallbackBase>();

	ref Managed m_pContext;
	Class m_pInvokeInstance;
	string m_sInvokeMethod;

	//------------------------------------------------------------------------------------------------
	void EDF_ScriptInvokerCallbackBase(Class invokeInstance, string invokeMethod, Managed context = null)
	{
		s_CallbackReferences.Insert(this);
		m_pInvokeInstance = invokeInstance;
		m_sInvokeMethod = invokeMethod;
		m_pContext = context;
	}
};

class EDF_ScriptInvokerCallback : EDF_ScriptInvokerCallbackBase
{
	//------------------------------------------------------------------------------------------------
	sealed void Invoke()
	{
		GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, false, null, m_pContext);
		s_CallbackReferences.RemoveItem(this);
	}
};

class EDF_ScriptInvokerCallback1<Class T1> : EDF_ScriptInvokerCallbackBase
{
	//------------------------------------------------------------------------------------------------
	sealed void Invoke(T1 t1)
	{
		GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, false, null, t1, m_pContext);
		s_CallbackReferences.RemoveItem(this);
	}
};

class EDF_ScriptInvokerCallback2<Class T1, Class T2> : EDF_ScriptInvokerCallbackBase
{
	//------------------------------------------------------------------------------------------------
	sealed void Invoke(T1 t1, T2 t2)
	{
		GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, false, null, t1, t2, m_pContext);
		s_CallbackReferences.RemoveItem(this);
	}
};

class EDF_ScriptInvokerCallback3<Class T1, Class T2, Class T3> : EDF_ScriptInvokerCallbackBase
{
	//------------------------------------------------------------------------------------------------
	sealed void Invoke(T1 t1, T2 t2, T3 t3)
	{
		GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, false, null, t1, t2, t3, m_pContext);
		s_CallbackReferences.RemoveItem(this);
	}
};

class EDF_ScriptInvokerCallback4<Class T1, Class T2, Class T3, Class T4> : EDF_ScriptInvokerCallbackBase
{
	//------------------------------------------------------------------------------------------------
	sealed void Invoke(T1 t1, T2 t2, T3 t3, T4 t4)
	{
		GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, false, null, t1, t2, t3, t4, m_pContext);
		s_CallbackReferences.RemoveItem(this);
	}
};

class EDF_ScriptInvokerCallback5<Class T1, Class T2, Class T3, Class T4, Class T5> : EDF_ScriptInvokerCallbackBase
{
	//------------------------------------------------------------------------------------------------
	sealed void Invoke(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)
	{
		GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, false, null, t1, t2, t3, t4, t5, m_pContext);
		s_CallbackReferences.RemoveItem(this);
	}
};

class EDF_ScriptInvokerCallback6<Class T1, Class T2, Class T3, Class T4, Class T5, Class T6> : EDF_ScriptInvokerCallbackBase
{
	//------------------------------------------------------------------------------------------------
	sealed void Invoke(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6)
	{
		GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, false, null, t1, t2, t3, t4, t5, t6, m_pContext);
		s_CallbackReferences.RemoveItem(this);
	}
};

class EDF_ScriptInvokerCallback7<Class T1, Class T2, Class T3, Class T4, Class T5, Class T6, Class T7> : EDF_ScriptInvokerCallbackBase
{
	//------------------------------------------------------------------------------------------------
	sealed void Invoke(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7)
	{
		GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, false, null, t1, t2, t3, t4, t5, t6, t7, m_pContext);
		s_CallbackReferences.RemoveItem(this);
	}
};

class EDF_ScriptInvokerCallback8<Class T1, Class T2, Class T3, Class T4, Class T5, Class T6, Class T7, Class T8> : EDF_ScriptInvokerCallbackBase
{
	//------------------------------------------------------------------------------------------------
	sealed void Invoke(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8)
	{
		GetGame().GetScriptModule().Call(m_pInvokeInstance, m_sInvokeMethod, false, null, t1, t2, t3, t4, t5, t6, t7, t8, m_pContext);
		s_CallbackReferences.RemoveItem(this);
	}
};
