# [`EDF_DbFind`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbFindCondition.c;1)
[`EDF_DbEntity`](db-entity.md)s are queried from the [`EDF_DbContext`](db-context.md) using a [`EDF_DbFindCondition`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/EDF_DbFindCondition.c;29). These find conditions or "queries" are constructed through the `EDF_DbFind` builder.

# Find by ID
A simple find by ID can look like this:
```cs
EDF_DbFindCondition condition = EDF_DbFind.Id().Equals("GUID-GOES-HERE");
EDF_DbFindResults<EDF_DbEntity> results = dbContext.FindAll(<type>, condition, limit: 1);
```

# Find by field
Besides the `Id()` shortcut any other field can be queried using the `Field()` method on the builder.
```cs
EDF_DbFindCondition condition = EDF_DbFind.Field("m_Name").Equals("Foo");
```
The field name supports "dot-notation" so you can navigate complex types as well as collections using it.  
It is recommended to use the builder variant, instead of manually setting up the path for compatiblity reasons.
- `EDF_DbFind.Field("fieldName.subField")` / `EDF_DbFind.Field("fieldName").Field("subField")`
- `EDF_DbFind.Field("floatArray.0")` / `EDF_DbFind.Field("floatArray").At(0)` evaluates the first element of the array on the entity
- `EDF_DbFind.Field("classArray.FilteredType")` / `EDF_DbFind.Field("classArray").OfType(FilteredType)` evaluate elements of a complex type array that inherit from the provided typename

## Field condition builder
Summary of the available filed condition builder functions:
| Function            | Used on              | Description                                                        |
|---------------------|----------------------|--------------------------------------------------------------------|
| Not                 | *                    | Inverts the next statement                                         |
| Null                | Complex/Collection   | Checks if the field is null                                        |
| Empty               | Primitive/Collection | Checks if the field is empty collection or primitive value default |
| Equals              | *                    | Full equality check                                                |
| EqualsAnyOf         | Primitive            | Equality check comparison against multiple values                  |
| LessThan            | Numeric              | `<` operator                                                       |
| LessThanOrEquals    | Numeric              | `<=` operator                                                      |
| GreaterThan         | Numeric              | `>` operator                                                       |
| GreaterThanOrEquals | Numeric              | `>=` operator                                                      |
| Between             | Numeric              | LOWER `<` VALUE `<` UPPER                                          |
| Contains            | String               | Case-sensitive string in string search                             |
| Contains            | Collection           | Value inside collection                                            |
| ContainsAnyOf       | String               | Same as `Contains` with multiple comparison values                 |
| ContainsAnyOf       | Collection           | Same as `Contains` with multiple comparison values                 |
| ContainsAllOf       | Collection           | Collection must at least contain all the comparison values         |
| Length              | String               | Character count of string comparison                               |
| Count               | Collection           | Number of elements inside the collection                           |
| Any                 | Collection           | True if any collection item matches the condition                  |
| All                 | Collection           | True if all collection items match the condition                   |
| At                  | Collection           | Get collection item at index                                       |
| OfType              | Collection           | Evaluates collection items that matches the type                   |
| Keys                | Collection           | Get the key collection of a map                                    |
| Values              | Collection           | Get the value collection of a map                                  |

To combine multiple conditions together you can use `And()` and `Or()` to build like so
```cs
EDF_DbFind.And({
	condition1,
	condition2,
	conditionN
});
```

Combining all of this together provides various ways to write precise database queries:
```cs
EDF_DbFindCondition condition = EDF_DbFind.Or({
    EDF_DbFind.Field("A").Not().Null(),
    EDF_DbFind.Field("B").Empty(),
    EDF_DbFind.And({
        EDF_DbFind.Field("CString").Contains("SubString"),
        EDF_DbFind.Field("DBoolArray").Equals(EDF_DbValues<bool>.From({true, false, true, true})),
        EDF_DbFind.And({
            EDF_DbFind.Field("E.m_Numbers").Contains(100),
            EDF_DbFind.Field("F.m_ComplexWrapperSet").OfType(Class).Any().Field("someNumber").Not().EqualsAnyOf(EDF_DbValues<int>.From({1, 2}))
        }),
        EDF_DbFind.Or({
            EDF_DbFind.Field("G").EqualsAnyOf(EDF_DbValues<int>.From({12, 13}))
        })
    })
});
```

> **Note**
> If you get an error like "formula too complex" you need to split up your conditions into a few sub-conditions and join them together separately, as the script VM has some limits on how nested the builder can be.

### Performance considerations 
If possible make the condition a constant class member, that way it is created only once. For this to be possible all search values must be known constants as well.
You can also cache more complex conditions as class members if you build them dynamically once and re-use them after.  
```cs
const ref EDF_DbFindCondition s_ConstNameEqualsFoo = EDF_DbFind.Field("m_Name").Equals("Foo");

void MyFunction()
{
    // Do something with s_ConstNameEqualsFoo
	...
}
```
### EL_DbValues wrapper
> **Warning**
> The following information only applies to versions used before AR patch 0.9.9.X. The value wrapper will be deprecated afterwards.

In some of the examples the utility class `EDF_DbValues<T>` is used. Especially for const allocations, this ensures that the array data you pass into the builder is handled correctly.
It is generally only needed for the static initializer syntax `{...}`. An already strong typed initialized array from previous code can be passed without the wrapper class safely.
The compiler will complain or the built condition will have no array value if it was not used in a situation where it should have been.
