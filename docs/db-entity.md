# [`EDF_DbEntity`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbEntity.c;1)
The [`EDF_DbContext`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbContext.c;1) API works with a base class `EDF_DbEntity` that represents one distinct entity inside the database.  

## The Entity GUID
Each `EDF_DbEntity` is identified with an [GUID](https://en.wikipedia.org/wiki/GUID) represented as 36 character hexadecimal string, that can be assigned manually via [`EDF_DbEntity::SetId()`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbEntity.c;16) or will be assigned by the [`EDF_DbContext::AddOrUpdate()`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbContext.c;9) variants.  

The GUID is generated in script by the [`EDF_DbEntityIdGenerator`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbEntityIdGenerator.c;1) to avoid any round trips to the database and allow for deterministic async processing of all requests. The GUID is always lowercase. 

| 646e0c40     | - | 0000      | - | 0005     | - | 32cd          | - | 6580         | 5811b000 |
|--------------|---|-----------|---|----------|---|---------------|---|--------------|----------|
| TTTTTTTT     |   | SEQ1      |   | SEQ1     |   | RND1          |   | RND1         | RND2RND2 |

- `TTTTTTTT` UNIX UTC 32-bit date time stamp. Gives the ID general lexograpical sortability by creation time. Minimizes collisions as each ID becomes unique per second.
- `SEQ1` gives guaranteed uniqueness to the ID due to the combination with the timestamp. Starts at 0 per session. Is incremented by one for each generated ID. There can be skips in the sequence number making their way to the database as not all generated ids will end up being used.
- `RND1` and `RND2` aim to minimize predictability by malicious actors by introducing pseudo-random numbers. These are however **NOT** cryptographically safe!

## Creating your own database entity
To create a database entity just inherit from the `EDF_DbEntity` base class.  Optionally you can decorate the class with the [`[EDF_DbName("<Name>")]`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbEntity.c;43) attribute. It is used when the type is stored somewhere as a string e.g. JSON serialization. There is also [`[EDF_DbName.Automatic()]`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbEntity.c;73) which converts any class of the pattern `TAG_<Base?>ActuallyRelevantPart<SaveData?>` into just `ActuallyRelevantPart`.

> **Warning**  
> Because of the serialization support, all DB entity classes MUST have 0 parameters in their constructor if a custom one is defined. This is a limitation of the deserialization of the engine. Instead, consider adding a static `Create` function. Inside, you can still access private or protected members using the code setup below.

```cs
[EDF_DbName("MyCustomRenamedDbEntity")]
class TAG_MyCustomDbEntity : EDF_DbEntity
{
    int m_iValue;

    //------------------------------------------------------------------------------------------------
    static TAG_MyCustomDbEntity Create(int value)
    {
        TAG_MyCustomDbEntity instance();
        instance.m_iValue = value;
        return instance;
    }
};
```
