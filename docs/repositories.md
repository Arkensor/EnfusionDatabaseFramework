# Repositories
To make the handling of database entities easier the framework comes with a utility wrapper class called [`EDF_DbRepository<T>`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbRepository.c;23). 
It contains a few commonly used methods such as `Find()` by id, `FindSingleton()`, `FindFirst()` by condition, as well as providing already casted results of the target entity type.
All DB entities can be handled automatically through the default repository implementation. To get a repository for an entity there is a utility class:
```cs
EDF_DbRepository<TAG_MyCustomDbEntity> repository = EDF_DbEntityHelper<TAG_MyCustomDbEntity>.GetRepository(dbContext);
```

## Custom repositories
Adding a customized implementation of a repository is also possible. In it, frequently used DB operations can be stored as re-useable methods.
> **Note**
> The [`EDF_DbRepositoryRegistration`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbRepository.c;136) attribute is a temporary requirement to make the repository known the framework ahead of any manual instantiation. This attribute will be deprecated in the future when there are better ways to get all repositories.
```cs
[EDF_DbRepositoryRegistration()]
class TAG_MyCustomDbEntityRepository : EDF_DbRepository<TAG_MyCustomDbEntity>
{
    EDF_DbFindResultSingle<TAG_MyCustomDbEntity> FindByIntValue(int value)
    {
        return FindFirst(EDF_DbFind.Field("m_iIntValue").Equals(value));
    }
};
```

An instance of this repository will be returned by the [`EDF_DbEntityHelper<T>::GetRepository()`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbRepositoryHelper.c;7) method, however to also strong cast the resulting repository, a different utility class [`EDF_DbRepositoryHelper<T>`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbRepositoryHelper.c;20) can be used.
```cs
TAG_MyCustomDbEntityRepository repository = EDF_DbRepositoryHelper<TAG_MyCustomDbEntityRepository>.Get(dbContext);
```

Manually getting/creating a repository instance is possible through the [`EDF_DbRepositoryFactory::GetRepository()`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbRepositoryFactory.c;6) function. The factory has the advantage of returning cached instances of the repository on the same db context.
