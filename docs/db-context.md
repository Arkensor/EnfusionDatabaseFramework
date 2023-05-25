# [`EDF_DbContext`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbContext.c;1)
The `EDF_DbContext` API works with a base class [`EDF_DbEntity`](db-entity.md) that represents one distinct entity inside the database. The
core operations performed through it are [`AddOrUpdate`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbContext.c;9), [`Remove`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbContext.c;21) and [`FindAll`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbContext.c;47).

## Constructing and managing your context
To create a new context use [`EDF_DbContext.Create()`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbContext.c;101) and pass it the [connection info](connection-info.md) you want to use. It is recommended to re-use the instance for as long as possible to benefit from any under-the-hood optimizations that are added later such as caching and buffering. One possibility is to use a singleton for it.
```cs
class TAG_MyDbContextSingleton
{
    protected static ref EDF_DbContext s_pInstance;

    //------------------------------------------------------------------------------------------------
    static EDF_DbContext GetInstance()
    {
        EDF_DbConnectionInfoBase connectionInfo = ...; // Get it from config or somewhere else.
        if (!s_pInstance)
            s_pInstance = EDF_DbContext.Create(connectionInfo);

        return s_pInstance;
    }
};

EDF_DbContext context =  TAG_MyDbContextSingleton.GetInstance();
```

## Using the context
The DB context offers both a sync and [async (non-blocking)](async-operations.md) API for direct interactions. The usage is documented inline in the source code.
For convenience, there is also the possibility of setting up a [repository](repositories.md) for your DB entity types.

> **Note**
> A general **important performance consideration** is to try and only do query by ID whenever possible if the project should be compatible with all possible storage backends. If it is only intended to be used together with e.g. `Http:MySQL` then query performance is not a concern of the scripted logic and is purely influenced by how the "real" database is configured. One way to optimize the usage for maximum compatibility is to try and remember relations by ID. Instead of querying all groups to see if a player is in it, store a group id on the player's save-data. On load, a condition can still be added to check if the player is actually still in the group and if it even exists. The performance diagnostics can aid in finding inefficient db interactions.

Details for how to build queries can be found here [Query builder](query-builder.md).

## Simple FindAll example
`FindAll` returns an `EDF_DbFindResults<EL_DbEntity>` instance which contains the status code enum as well as any results from the operation if it was successful. Depending on the use case, during development, when this simple example works, consider switching to [async operations](async-operations.md) as a best practice.
```cs
EDF_DbFindResults<EL_DbEntity> result = dbContext.FindAll(...);

//Something went wrong
if (result.GetStatusCode() != EL_EDbOperationStatusCode.SUCCESS) 
    return; 

//same as above, for convenience
if (!result.Success()) 
    return; 

array<ref EL_DbEntity> entities = results.GetEntities();
entities.Debug();
...
```
