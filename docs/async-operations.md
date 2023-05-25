# Async operations
Whenever possible all scripted logic should use async operations. This is because the sync API can and will lock up the server for the entire duration the storage backends needs for the data access. Even a few milliseconds of the server not being able to respond to packages and continue its simulation can result in undesirable gameplay effects such as rubberbanding, desync, or crashes. The sync API only makes sense for blocking load during game world init and blocking save during "mission exit" or host shutdown. For shutdown all async operations are force re-routed to be sync, otherwise, data might get lost.

## FindAllAsync callback classes
If you want to process the result inside a separate callback class you can create one and implement the functions defined in the base class to get already strong typed results. 
Besides [`EDF_DbFindCallbackMultiple<T>`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbOperationResultTypes.c;105) for multiple return values there is also [`EDF_DbFindCallbackSingle<T>`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbOperationResultTypes.c;133) for expecting a single result as well as [`EDF_DbFindCallbackSingleton<T>`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbOperationResultTypes.c;164) to create a result instance if nothing was found.
```cs
class MyCustomDbEntityFindCallback : EDF_DbFindCallbackMultiple<TAG_MyCustomDbEntity>
{
    //------------------------------------------------------------------------------------------------
    override void OnSuccess(array<ref TEntityType> results, Managed context)
    {
        results.Debug();
        Print(context);
    }

    //------------------------------------------------------------------------------------------------
    override void OnFailure(EDF_EDbOperationStatusCode statusCode, Managed context)
    {
        Print(statusCode);
        Print(context);
    }
}
...
void DoFind()
{
    MyCustomDbEntityFindCallback myCallback();
    dbContext.FindAllAsync(TAG_MyCustomDbEntity, callback: myCallback);
}
```

## FindAllAsync callback method
As an alternative to a separate class, the result can be handled by a dedicated method.
```cs
class MyDbLogic
{
    //------------------------------------------------------------------------------------------------
    void DoFind()
    {
        EDF_DbFindCallbackSingle<TAG_MyCustomDbEntity> myCallback(this, "HandleResult");
        dbContext.FindAllAsync(TAG_MyCustomDbEntity, callback: myCallback);
    }

    //------------------------------------------------------------------------------------------------
    protected void HandleResult(EDF_EDbOperationStatusCode code, TAG_MyCustomDbEntity result, Managed context)
    {
        Print(code);
        Print(result);
        Print(context);
    }
}
```

## Callback context
In async operations, there is an optional `Managed` parameter called `context`. This allows for the caller scope to pass down additional "arguments" that are needed when processing the result. The value can be as simple as `TupleN<T>` or a custom context type is used. The callback holds a strong reference to it, so any information in the context is kept alive until the callback completes.

When not needed the context parameter can actually be omitted from a dedicated result handler method signature.  
E.g `protected void HandleResult(EDF_EDbOperationStatusCode code, TAG_MyCustomDbEntity result)`

```cs
class MyContextDbLogic
{
    //------------------------------------------------------------------------------------------------
    void DoFind(string someParam)
    {
        Tuple1<string> context(someParam);
        EDF_DbFindCallbackSingle<TAG_MyCustomDbEntity> myCallback(this, "HandleResult", context);
        dbContext.FindAllAsync(TAG_MyCustomDbEntity, callback: myCallback);
    }

    //------------------------------------------------------------------------------------------------
    protected void HandleResult(EDF_EDbOperationStatusCode code, TAG_MyCustomDbEntity result, Managed context)
    {
        Tuple1<string> typedContext = Tuple1<string>.Cast(context);
        string someParam = typedContext.param1; // We get our DoFind parameter back yay!
    }
}
```
