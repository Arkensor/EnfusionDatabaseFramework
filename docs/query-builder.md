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
It is recommended to use the builder variant, instead of manually setting up the path to ensure future compatiblity.
- `EDF_DbFind.Field("fieldName").Field("subField")` / `EDF_DbFind.Field("fieldName.subField")`
- `EDF_DbFind.Field("floatArray").At(0)` / `EDF_DbFind.Field("floatArray.0")` evaluates the first element of the collection
- `EDF_DbFind.Field("floatArray").At({0, 1})` / `EDF_DbFind.Field("floatArray.{0, 1}")` evaluates the first and second element of the collection
- `EDF_DbFind.Field("classArray").OfType(FilteredType)` / `EDF_DbFind.Field("classArray.FilteredType")` evaluate collection collection elements of a complex type that inherit from the provided typename
- `EDF_DbFind.Field("classArray").OfTypes({FilteredType, AnotherType})` / `EDF_DbFind.Field("classArray.{FilteredType, AnotherType}")` evaluate collection collection elements of a complex type that inherit from the provided typenames

## Field condition builder
Summary of the available filed condition builder functions (`set<T>` counts as `array<T>`):
| Function            | Used on          | Description                                              |
|---------------------|------------------|----------------------------------------------------------|
| Not                 | *                | Inverts the next statement                               |
| NullOrDefault       | *                | Field is null, an empty array or primitive value default |
| Equals              | *                | Full equality check                                      |
| EqualsAnyOf         | Primitive        | Equality check comparison against multiple values        |
| LessThan            | Numeric          | `<` operator                                             |
| LessThanOrEquals    | Numeric          | `<=` operator                                            |
| GreaterThan         | Numeric          | `>` operator                                             |
| GreaterThanOrEquals | Numeric          | `>=` operator                                            |
| Between             | Numeric          | LOWER `<` VALUE `<` UPPER                                |
| Invariant           | String           | Makes the condition case-insensitive                     |
| Length              | String           | Length of string                                         |
| Count               | Array/Map        | Number of elements inside the collection                 |
| Contains            | String/Array/Map | Contains the value                                       |
| ContainsAnyOf       | String/Array/Map | Contains at least one of the values                      |
| ContainsAllOf       | String/Array/Map | Contains at least all the values                         |
| Any                 | Array            | True if any array item matches the condition             |
| All                 | Array            | True if all array items match the condition              |
| At                  | Array            | Get array item at index                                  |
| OfType              | Array            | Evaluates array items that match the type                |
| Keys                | Map              | Get the key array of a map                               |
| Values              | Map              | Get the value array of a map                             |

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
        EDF_DbFind.Field("DBoolArray").Equals({true, false, true, true}),
        EDF_DbFind.And({
            EDF_DbFind.Field("E.m_Numbers").Contains(100),
            EDF_DbFind.Field("F.m_ComplexWrapperSet").OfType(SomeType).Any().Field("someNumber").Not().EqualsAnyOf({1, 2})
        }),
        EDF_DbFind.Or({
            EDF_DbFind.Field("G").EqualsAnyOf({12, 13})
        })
    })
});
```
> **Note**
> If you get an error like "formula too complex" you need to split up your conditions into a few sub-conditions and join them together separately, as the script VM has some limits on how nested the builder can be.

### Query cheat sheet

#### Collection only contains certain values:
```cs
EDF_DbFind.Field("Collection").All().EqualsAnyOf({a, b, c})
```

#### Collection unordered equality:
```cs
EDF_DbFind.And({
    EDF_DbFind.Field("Collection").ContainsAllOf({a, b, c}), 
    DF_DbFind.Field("Collection").Count().Equals(3)
})
```

### Polymorphism limitation
Right now it is required to pass in the entire inheritance tree to match during an `OfType()` filtering. In later versions once https://feedback.bistudio.com/T172647 is implemented, it should be possible to just pass a base class and still match all inherited types.
Given the classes `BaseType`, `InheritedTypeA : BaseType` and `InheritedTypeB : BaseType` a filter to match all of these needs to look like this currently:
```cs
EDF_DbFind.Field("someCollection").OfTypes({BaseType, InheritedTypeA, InheritedTypeB})
```
and later should only become
```cs
EDF_DbFind.Field("someCollection").OfType(BaseType)
```

### Performance considerations 
If possible make the condition a constant class member, that way it is created only once. For this to be possible all search values must be known constants as well.
You can also cache more complex conditions as class members if you build them dynamically once and re-use them after.  
```cs
static const ref EDF_DbFindCondition s_ConstNameEqualsFoo = EDF_DbFind.Field("m_Name").Equals("Foo");

void MyFunction()
{
    // Do something with s_ConstNameEqualsFoo
	...
}
```
