# ConnectionInfo
The connection info used by the [`EDF_DbContext`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbContext.c;1) / [`EDF_DbDriver`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/Drivers/EDF_DbDriver.c;1) is based on [`EDF_DbConnectionInfoBase`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/Drivers/EDF_DbConnectionInfo.c;2).

Connection info objects can be created directly from script.
```cs
EDF_JsonFileDbConnectionInfo connectInfo();
connectInfo.m_sDatabaseName = "MyDatabase";
connectInfo.m_bPrettify = true;
```

They can also be parsed from CLI params or other string sources using [`EDF_DbConnectionInfoBase::Parse()`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/Drivers/EDF_DbConnectionInfo.c;8).  
The format for this is `<driver-name>://<database-name>?<option-name>=<option-value>&<another-option-name>=<another-option-value>`.
```cs
// ArmaReforgerServer.exe "-ConnectionString=JsonFile://MyDatabase?prettify=true"
protected EDF_DbConnectionInfoBase GetConnectionInfo()
{
    string connectionString;
    if (!System.GetCLIParam("ConnectionString", connectionString))
        return null;

    return EDF_DbConnectionInfoBase.Parse(connectionString);
}
```
The available connection options can be found on the individual [driver](drivers/index.md) pages.
