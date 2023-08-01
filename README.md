<div align="center">
<picture>
  <source media="(prefers-color-scheme: dark)" width="400" srcset="https://github.com/Arkensor/EnfusionDatabaseFramework/assets/8494013/4ae8ba7f-e129-4e5e-b032-44166597de58">
  <img alt="Everon Life" width="400" src="https://github.com/Arkensor/EnfusionDatabaseFramework/assets/8494013/6bad6033-f07d-42f3-a485-553d8d8b7d76">
</picture>

[![Releases](https://img.shields.io/github/v/release/Arkensor/EnfusionDatabaseFramework?style=flat-square)](https://github.com/Arkensor/EnfusionDatabaseFramework/releases)
[![Arma Reforger Workshop](https://img.shields.io/badge/Workshop-5D6EA74A94173EDF-blue?style=flat-square)](https://reforger.armaplatform.com/workshop/5D6EA74A94173EDF)
[![License MIT](https://img.shields.io/badge/License-MIT-green?style=flat-square)](https://opensource.org/licenses/MIT)
</div>

# Enfusion Database Framework

> **Warning**
> This framework is still in **BETA**. Until version 1.0.0 there is no backward compatibility guarantee! Expect some bugs/performance issues and function signature updates until then. Feedback via [issue](https://github.com/Arkensor/EnfusionDatabaseFramework/issues) or [discussion](https://github.com/Arkensor/EnfusionDatabaseFramework/discussions) is welcome.

**A database framework to connect the Enfusion engine with SQL-, document- and local file databases.**

> **Note**
> Are you trying to persist the entire world state and not just a few self scripted db entites? Consider using [EnfusionPersistenceFramework](https://github.com/Arkensor/EnfusionPersistenceFramework) which is built on top of this project.

## 🚀 Features
- ✅ Easy to setup your DB entities with the provided base classes
- ✅ Built-in [GUID](https://en.wikipedia.org/wiki/GUID) generation for DB entities to avoid expensive roundtrips to the database
- ✅ Powerful query builder to find DB entities by complex conditions
- ✅ Sync (blocking) and Async (non-blocking) APIs for adding/updating, finding, and removing DB entities
- ✅ Pagination and result sorting by nested properties 
- ✅ Optional repository pattern for easy strong typed results and re-useable queries.
- 🚧 Migrations between storage backends e.g. `JsonFile` <-> `Http:MySQL`

### Drivers
- ✅ [`InMemory`](docs/drivers/in-memory.md) for unit testing purposes
- ✅ [`JsonFile`](docs/drivers/json-file.md) local `.json` files for workbench development and small data volumes
- ✅ [`BinaryFile`](docs/drivers/binary-file.md) local `.bin` files, same purpose as JSON but much smaller in filesize.
- 🚧 `BIBackend` local/cloud synced `.bin` files stored in the Bohemia Interactive session backend.
- 🚧 `Http` a web API proxy to other external storage services such as SQL and document databases.
    - ✅ Document Databases [`MongoDB`](docs/drivers/proxy-mongodb.md)
    - 🚧 SQL Databases `SQLite`, `MySQL`, `PostgreSQL`

## 📖 Documentation
Detailed information on the individual classes and best practices can be found [here](docs/index.md).

## ⚡ Quick start
```cs
[EDF_DbName.Automatic()]
class TAG_MyPersistentInfo : EDF_DbEntity
{
    float m_fNumber;
    string m_sText;

    //------------------------------------------------------------------------------------------------
    //! Db entities can not have a constructor with parameters, this is a limitation of the engine.
    //! Consult the docs for more info on this.
    static TAG_MyPersistentInfo Create(float number, string text)
    {
        TAG_MyPersistentInfo instance();
        instance.m_fNumber = number;
        instance.m_sText = text;
        return instance;
    }
};

class EDF_QuickstartAction : ScriptedUserAction
{
    //------------------------------------------------------------------------------------------------
    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
    {
        // Get the connection info as an attribute or parse it from CLI params etc.
        EDF_JsonFileDbConnectionInfo connectInfo();
        connectInfo.m_sDatabaseName = "MyJsonDatabase";

        // Get a db context instance and save it somewhere to re-use in e.g. a singleton
        EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);

        // For convenience interact with the DB context through a repository
        EDF_DbRepository<TAG_MyPersistentInfo> repository = EDF_DbEntityHelper<TAG_MyPersistentInfo>.GetRepository(dbContext);

        // Add some entries
        repository.AddOrUpdateAsync(TAG_MyPersistentInfo.Create(13.37, "Hello"));
        repository.AddOrUpdateAsync(TAG_MyPersistentInfo.Create(42.42, "World!"));

        // Now find hello
        EDF_DbFindCondition condition = EDF_DbFind.Field("m_sText").Contains("Hello");
        EDF_DbFindCallbackSingle<TAG_MyPersistentInfo> helloHandler(this, "FindHelloHandler");
        repository.FindFirstAsync(condition, callback: helloHandler);
    }

    protected void FindHelloHandler(EDF_EDbOperationStatusCode statusCode, TAG_MyPersistentInfo result)
    {
        PrintFormat("FindHelloHandler invoked! - StatusCode: %1",
            typename.EnumToString(EDF_EDbOperationStatusCode, statusCode));

        if (result)
            PrintFormat("Result: %1(id: %2, number: %3, text: %4)",
                result, result.GetId(), result.m_fNumber, result.m_sText)
    }
};
```
You should see this in your script console after executing the user action
> FindHelloHandler invoked! - StatusCode: SUCCESS  
> Result: TAG_MyPersistentInfo<0x0000020D41100670>(id: 646e0c40-0000-0000-32cd-65805811b000, number: 13.37, text: Hello) 

And in your profile find these two files
- `profile/.db/MyJsonDatabase/MyPersistentInfos/646e0c40-0000-0000-32cd-65805811b000.json`
- `profile/.db/MyJsonDatabase/MyPersistentInfos/646e0c40-0000-0001-1a8d-352051eea400.json`

And inside `646e0c40-0000-0000-32cd-65805811b000.json`
```json
{
    "m_sId": "646e0c40-0000-0000-32cd-65805811b000",
    "m_fNumber": 13.36999,
    "m_sText": "Hello"
}
```
