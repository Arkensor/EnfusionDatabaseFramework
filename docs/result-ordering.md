# Result ordering
If multiple results are returned by the operation they can be automatically ordered by field values, again using ["dot-notation"](query-builder.md#find-by-field) on the `orderBy` parameter.
The sorting order is given via a nested array, with the second, third, ... n-th element only being used if the ordering based on the previous element left two or more entities with the same sorting result.
Options for the sort direction are `ASC`, `DESC` as invariant strings or the [`EDF_EDbEntitySortDirection`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbEntitySorter.c;1) "enum".
```cs
dbContext.FindAll(..., orderBy: {{"child.subField", "ASC"}, {"thenByField", EDF_EDbEntitySortDirection.DESCENDING}});
```
