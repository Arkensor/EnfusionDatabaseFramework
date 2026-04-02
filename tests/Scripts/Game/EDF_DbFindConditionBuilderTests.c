[BaseContainerProps(category: "Autotest")]
class EDF_DbFindConditionBuilderTests : SCR_AutotestSuiteBase
{
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionBuilderTests")]
class EDF_Test_DbFindConditionBuilder_And_EmptyArgs_EmptyCondition : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Act
		EDF_DbFindAnd condition = EDF_DbFind.And({});

		// Assert
		SetResult(EDF_AutotestResult.FromResult(condition.m_aConditions.Count() == 0));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionBuilderTests")]
class EDF_Test_DbFindConditionBuilder_And_MultipleConditions_MultipleWrapped : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Act
		EDF_DbFindAnd condition = EDF_DbFind.And({new EDF_DbFindCondition(), new EDF_DbFindCondition()});

		// Assert
		SetResult(EDF_AutotestResult.FromResult(condition.m_aConditions.Count() == 2));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionBuilderTests")]
class EDF_Test_DbFindConditionBuilder_Or_EmptyArgs_EmptyCondition : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Act
		EDF_DbFindOr condition = EDF_DbFind.Or({});

		// Assert
		SetResult(EDF_AutotestResult.FromResult(condition.m_aConditions.Count() == 0));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionBuilderTests")]
class EDF_Test_DbFindConditionBuilder_Or_MultipleConditions_MultipleWrapped : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Act
		EDF_DbFindOr condition = EDF_DbFind.Or({new EDF_DbFindCondition(), new EDF_DbFindCondition()});

		// Assert
		SetResult(EDF_AutotestResult.FromResult(condition.m_aConditions.Count() == 2));
	}
}


//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionBuilderTests")]
class EDF_Test_DbFindConditionBuilder_Field_SingleField_ValidBuilder : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Act
		EDF_DbFindFieldCollectionHandlingBuilder builder = EDF_DbFind.Field("fieldName");

		// Assert
		SetResult(EDF_AutotestResult.FromResult(builder.m_sFieldPath == "fieldName" && builder.m_bInverted == false));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionBuilderTests")]
class EDF_Test_DbFindConditionBuilder_Field_SingleFieldMultiValueInverted_ValidBuilder : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Act
		EDF_DbFindFieldAllValueConditonBuilder builder = EDF_DbFind.Field("fieldName.subField").Not();

		// Assert
		SetResult(EDF_AutotestResult.FromResult(builder.m_sFieldPath == "fieldName.subField" && builder.m_bInverted == true));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionBuilderTests")]
class EDF_Test_DbFindConditionBuilder_Field_MultiField_FieldsChained : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Act
		EDF_DbFindFieldCollectionHandlingBuilder builder = EDF_DbFind.Field("fieldName").Field("subField");

		// Assert
		SetResult(EDF_AutotestResult.FromResult(builder.m_sFieldPath == "fieldName.subField"));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionBuilderTests")]
class EDF_Test_DbFindConditionBuilder_Field_Length_ModifierPresent : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Act
		EDF_DbFindFieldNumericValueConditonBuilder builder = EDF_DbFind.Field("stringField").Length();

		// Assert
		SetResult(EDF_AutotestResult.FromResult(builder.m_sFieldPath.EndsWith("stringField" + EDF_DbFindFieldAnnotations.LENGTH)));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionBuilderTests")]
class EDF_Test_DbFindConditionBuilder_Field_Count_ModifierPresent : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Act
		EDF_DbFindFieldNumericValueConditonBuilder builder = EDF_DbFind.Field("collectionName").Count();

		// Assert
		SetResult(EDF_AutotestResult.FromResult(builder.m_sFieldPath.EndsWith("collectionName:count")));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionBuilderTests")]
class EDF_Test_DbFindConditionBuilder_Field_Any_ModifierPresent : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Act
		EDF_DbFindFieldMainConditionBuilder builder = EDF_DbFind.Field("collectionName").Any();

		// Assert
		SetResult(EDF_AutotestResult.FromResult(builder.m_sFieldPath.EndsWith(":any")));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionBuilderTests")]
class EDF_Test_DbFindConditionBuilder_Field_All_ModifierPresent : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Act
		EDF_DbFindFieldMainConditionBuilder builder = EDF_DbFind.Field("collectionName").All();

		// Assert
		SetResult(EDF_AutotestResult.FromResult(builder.m_sFieldPath.EndsWith(":all")));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionBuilderTests")]
class EDF_Test_DbFindConditionBuilder_Field_Keys_ModifierPresent : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Act
		EDF_DbFindFieldBasicCollectionHandlingBuilder builder = EDF_DbFind.Field("collectionName").Keys();

		// Assert
		SetResult(EDF_AutotestResult.FromResult(builder.m_sFieldPath.EndsWith(":keys")));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionBuilderTests")]
class EDF_Test_DbFindConditionBuilder_Field_Values_ModifierPresent : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Act
		EDF_DbFindFieldBasicCollectionHandlingBuilder builder = EDF_DbFind.Field("collectionName").Values();

		// Assert
		SetResult(EDF_AutotestResult.FromResult(builder.m_sFieldPath.EndsWith(":values")));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionBuilderTests")]
class EDF_Test_DbFindConditionBuilder_Field_ValuesAny_ModifiersPresent : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Act
		EDF_DbFindFieldMainConditionBuilder builder = EDF_DbFind.Field("collectionName").Values().Any();

		// Assert
		SetResult(EDF_AutotestResult.FromResult(builder.m_sFieldPath.EndsWith(":values:any")));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionBuilderTests")]
class EDF_Test_DbFindConditionBuilder_Field_At_IndexFieldSet : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Act
		EDF_DbFindFieldMainConditionBuilder builder = EDF_DbFind.Field("collectionName").At(3);

		// Assert
		SetResult(EDF_AutotestResult.FromResult(builder.m_sFieldPath == "collectionName.{3}"));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionBuilderTests")]
class EDF_Test_DbFindConditionBuilder_Field_OfType_ModiferAndTypefilterPresent : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Act
		EDF_DbFindFieldMainConditionBuilder builder = EDF_DbFind.Field("collectionName").OfType(EDF_DbFind);

		// Assert
		SetResult(EDF_AutotestResult.FromResult(builder.m_sFieldPath == "collectionName.{EDF_DbFind}"));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbFindConditionBuilderTests")]
class EDF_Test_DbFindConditionBuilder_Field_ComplexBuild_DebugStringEqual : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Arrange
		EDF_DbFindCondition condition = EDF_DbFind.Or({
			EDF_DbFind.Field("A").Not().NullOrDefault(),
			EDF_DbFind.Field("B").NullOrDefault(),
			EDF_DbFind.And({
				EDF_DbFind.Field("CString").Contains("SubString"),
				EDF_DbFind.Field("DBoolArray").Equals({true, false, true, true}),
				EDF_DbFind.And({
					EDF_DbFind.Field("E.m_Numbers").Contains(100),
					EDF_DbFind.Field("F.m_ComplexWrapperSet").OfType(EDF_DbFind).Any().Field("someNumber").Not().EqualsAnyOf({1, 2})
				}),
				EDF_DbFind.Or({
					EDF_DbFind.Field("G").EqualsAnyOf({12, 13})
				})
			})
		});

		// Act
		string debugString = condition.GetDebugString();
		//Print(debugString);
		debugString.Replace("\t", "");
		debugString.Replace(" ", "");

		// Assert
		string compareString = "Or(\
			CheckNullOrDefault(fieldPath:'A', shouldBeNullOrDefault:false), \
			CheckNullOrDefault(fieldPath:'B', shouldBeNullOrDefault:true), \
			And(\
				Compare(fieldPath:'CString', operator:CONTAINS, values:{'SubString'}), \
				Compare(fieldPath:'DBoolArray', operator:EQUAL, values:{{true, false, true, true}}), \
				And(\
					Compare(fieldPath:'E.m_Numbers', operator:CONTAINS, values:{100}), \
					Compare(fieldPath:'F.m_ComplexWrapperSet.{EDF_DbFind}:any.someNumber', operator:NOT_EQUAL, values:{1, 2})\
				), \
				Or(\
					Compare(fieldPath:'G', operator:EQUAL, values:{12, 13})\
				)\
			)\
		)";

		compareString.Replace("\r", "");
		compareString.Replace("\t", "");
		compareString.Replace(" ", "");

		SetResult(EDF_AutotestResult.FromResult(debugString == compareString));
	}
}
