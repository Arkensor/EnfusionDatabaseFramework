# Result ordering
If multiple results are returned by the operation they can be automatically ordered by field values using "dot-notation" to navigate nested properties.
The sorting order is given via a nested array as the `orderBy` parameter, with the second, third, ... n-th element only being used if the ordering based on the previous element left two or more entities with the same sorting result.
Options for the sort direction are `ASC`, `DESC` as invariant strings or the [`EDF_EDbEntitySortDirection`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbEntitySorter.c;1) "enum".
```cs
dbContext.FindAll(..., orderBy: {{"child.subField", "ASC"}, {"thenByField", EDF_EDbEntitySortDirection.DESCENDING}});
```

> **Note**
> It is not possible to sort nested properties that are collections (array/set/map). Only primitive fields can be the sort value.
