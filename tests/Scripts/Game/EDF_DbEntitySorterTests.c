[BaseContainerProps(category: "Autotest")]
class EDF_DbEntitySorterTests : SCR_AutotestSuiteBase
{
}

class EDF_Test_DbEntitySortableEntity : EDF_DbEntity
{
	int m_iIntValue;
	float m_fFloatValue;
	bool m_bBoolValue;
	string m_sStringValue;
	vector m_vVectorValue;

	//------------------------------------------------------------------------------------------------
	void EDF_Test_DbEntitySortableEntity(int intVal = 0, float floatVal = 0.0, bool boolVal = false, string stringVal = "", vector vectorVal = "0 0 0")
	{
		m_iIntValue = intVal;
		m_fFloatValue = floatVal;
		m_bBoolValue = boolVal;
		m_sStringValue = stringVal;
		m_vVectorValue = vectorVal;
	}
}

class EDF_Test_DbEntitySortableEntitySingleWrapper : EDF_DbEntity
{
	int m_iSameIntValue;
	ref EDF_Test_DbEntitySortableEntity m_pEntity;

	//------------------------------------------------------------------------------------------------
	void EDF_Test_DbEntitySortableEntitySingleWrapper(int intVal = 0, float floatVal = 0.0, bool boolVal = false, string stringVal = "", vector vectorVal = "0 0 0")
	{
		m_iSameIntValue = 1337;
		m_pEntity = new EDF_Test_DbEntitySortableEntity(intVal, floatVal, boolVal, stringVal, vectorVal);
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbEntitySorterTests")]
class EDF_Test_DbEntitySorter_GetSorted_ArrayInt_AscSorted : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Arrange
		array<ref EDF_DbEntity> entities = {
			new EDF_Test_DbEntitySortableEntity(intVal: 5),
			new EDF_Test_DbEntitySortableEntity(intVal: 50),
			new EDF_Test_DbEntitySortableEntity(intVal: 1)
		};
	
		// Act
		array<ref EDF_DbEntity> sorted = EDF_DbEntitySorter.GetSorted(entities, {{"m_iIntValue", EDF_EDbEntitySortDirection.ASCENDING}});
	
		// Assert
		SetResult(EDF_AutotestResult.FromResult(
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(0)).m_iIntValue == 1 &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(1)).m_iIntValue == 5 &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(2)).m_iIntValue == 50));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbEntitySorterTests")]
class EDF_Test_DbEntitySorter_GetSorted_ArrayInt_DescSorted : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Arrange
		array<ref EDF_DbEntity> entities = {
			new EDF_Test_DbEntitySortableEntity(intVal: 5),
			new EDF_Test_DbEntitySortableEntity(intVal: 50),
			new EDF_Test_DbEntitySortableEntity(intVal: 1)
		};
	
		// Act
		array<ref EDF_DbEntity> sorted = EDF_DbEntitySorter.GetSorted(entities, {{"m_iIntValue", EDF_EDbEntitySortDirection.DESCENDING}});
	
		// Assert
		SetResult(EDF_AutotestResult.FromResult(
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(0)).m_iIntValue == 50 &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(1)).m_iIntValue == 5 &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(2)).m_iIntValue == 1));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbEntitySorterTests")]
class EDF_Test_DbEntitySorter_GetSorted_ArrayFloat_AscSorted : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Arrange
		array<ref EDF_DbEntity> entities = {
			new EDF_Test_DbEntitySortableEntity(floatVal: 5.2),
			new EDF_Test_DbEntitySortableEntity(floatVal: 50.3),
			new EDF_Test_DbEntitySortableEntity(floatVal: 1.1)
		};
	
		// Act
		array<ref EDF_DbEntity> sorted = EDF_DbEntitySorter.GetSorted(entities, {{"m_fFloatValue", EDF_EDbEntitySortDirection.ASCENDING}});
	
		// Assert
		SetResult(EDF_AutotestResult.FromResult(
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(0)).m_fFloatValue == 1.1 &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(1)).m_fFloatValue == 5.2 &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(2)).m_fFloatValue == 50.3));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbEntitySorterTests")]
class EDF_Test_DbEntitySorter_GetSorted_ArrayFloat_DescSorted : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Arrange
		array<ref EDF_DbEntity> entities = {
			new EDF_Test_DbEntitySortableEntity(floatVal: 5.2),
			new EDF_Test_DbEntitySortableEntity(floatVal: 50.3),
			new EDF_Test_DbEntitySortableEntity(floatVal: 1.1)
		};
	
		// Act
		array<ref EDF_DbEntity> sorted = EDF_DbEntitySorter.GetSorted(entities, {{"m_fFloatValue", EDF_EDbEntitySortDirection.DESCENDING}});
	
		// Assert
		SetResult(EDF_AutotestResult.FromResult(
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(0)).m_fFloatValue == 50.3 &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(1)).m_fFloatValue == 5.2 &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(2)).m_fFloatValue == 1.1));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbEntitySorterTests")]
class EDF_Test_DbEntitySorter_GetSorted_WrappedIntFloatDirectionInvariant_DescSorted : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Arrange
		array<ref EDF_DbEntity> entities = {
			new EDF_Test_DbEntitySortableEntitySingleWrapper(floatVal: 5.2),
			new EDF_Test_DbEntitySortableEntitySingleWrapper(floatVal: 50.3),
			new EDF_Test_DbEntitySortableEntitySingleWrapper(floatVal: 1.1)
		};
	
		// Act
		array<ref EDF_DbEntity> sorted = EDF_DbEntitySorter.GetSorted(entities, {{"m_iSameIntValue", "Asc"}, {"m_pEntity.m_fFloatValue", "deSC"}});
	
		// Assert
		SetResult(EDF_AutotestResult.FromResult(
			EDF_Test_DbEntitySortableEntitySingleWrapper.Cast(sorted.Get(0)).m_pEntity.m_fFloatValue == 50.3 &&
			EDF_Test_DbEntitySortableEntitySingleWrapper.Cast(sorted.Get(1)).m_pEntity.m_fFloatValue == 5.2 &&
			EDF_Test_DbEntitySortableEntitySingleWrapper.Cast(sorted.Get(2)).m_pEntity.m_fFloatValue == 1.1));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbEntitySorterTests")]
class EDF_Test_DbEntitySorter_GetSorted_ArrayBool_AscSorted : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Arrange
		array<ref EDF_DbEntity> entities = {
			new EDF_Test_DbEntitySortableEntity(boolVal: false),
			new EDF_Test_DbEntitySortableEntity(boolVal: true),
			new EDF_Test_DbEntitySortableEntity(boolVal: false)
		};
	
		// Act
		array<ref EDF_DbEntity> sorted = EDF_DbEntitySorter.GetSorted(entities, {{"m_bBoolValue", EDF_EDbEntitySortDirection.ASCENDING}});
	
		// Assert
		SetResult(EDF_AutotestResult.FromResult(
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(0)).m_bBoolValue == false &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(1)).m_bBoolValue == false &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(2)).m_bBoolValue == true));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbEntitySorterTests")]
class EDF_Test_DbEntitySorter_GetSorted_ArrayBool_DescSorted : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Arrange
		array<ref EDF_DbEntity> entities = {
			new EDF_Test_DbEntitySortableEntity(boolVal: false),
			new EDF_Test_DbEntitySortableEntity(boolVal: true),
			new EDF_Test_DbEntitySortableEntity(boolVal: false)
		};
	
		// Act
		array<ref EDF_DbEntity> sorted = EDF_DbEntitySorter.GetSorted(entities, {{"m_bBoolValue", EDF_EDbEntitySortDirection.DESCENDING}});
	
		// Assert
		SetResult(EDF_AutotestResult.FromResult(
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(0)).m_bBoolValue == true &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(1)).m_bBoolValue == false &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(2)).m_bBoolValue == false));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbEntitySorterTests")]
class EDF_Test_DbEntitySorter_GetSorted_ArrayString_AscSorted : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Arrange
		array<ref EDF_DbEntity> entities = {
			new EDF_Test_DbEntitySortableEntity(stringVal: "Hello World"),
			new EDF_Test_DbEntitySortableEntity(stringVal: "987 ZyaD"),
			new EDF_Test_DbEntitySortableEntity(stringVal: "876 AmrA"),
			new EDF_Test_DbEntitySortableEntity(stringVal: "Hello Arma")
		};
	
		// Act
		array<ref EDF_DbEntity> sorted = EDF_DbEntitySorter.GetSorted(entities, {{"m_sStringValue", EDF_EDbEntitySortDirection.ASCENDING}});
	
		// Assert
		SetResult(EDF_AutotestResult.FromResult(
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(0)).m_sStringValue == "876 AmrA" &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(1)).m_sStringValue == "987 ZyaD" &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(2)).m_sStringValue == "Hello Arma" &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(3)).m_sStringValue == "Hello World"));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbEntitySorterTests")]
class EDF_Test_DbEntitySorter_GetSorted_ArrayString_DescSorted : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Arrange
		array<ref EDF_DbEntity> entities = {
			new EDF_Test_DbEntitySortableEntity(stringVal: "Hello World"),
			new EDF_Test_DbEntitySortableEntity(stringVal: "987 ZyaD"),
			new EDF_Test_DbEntitySortableEntity(stringVal: "876 AmrA"),
			new EDF_Test_DbEntitySortableEntity(stringVal: "Hello Arma")
		};
	
		// Act
		array<ref EDF_DbEntity> sorted = EDF_DbEntitySorter.GetSorted(entities, {{"m_sStringValue", EDF_EDbEntitySortDirection.DESCENDING}});
	
		// Assert
		SetResult(EDF_AutotestResult.FromResult(
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(0)).m_sStringValue == "Hello World" &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(1)).m_sStringValue == "Hello Arma" &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(2)).m_sStringValue == "987 ZyaD" &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(3)).m_sStringValue == "876 AmrA"));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbEntitySorterTests")]
class EDF_Test_DbEntitySorter_GetSorted_ArrayVector_AscSorted : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Arrange
		array<ref EDF_DbEntity> entities = {
			new EDF_Test_DbEntitySortableEntity(vectorVal: Vector(1, 9, 1)),
			new EDF_Test_DbEntitySortableEntity(vectorVal: Vector(1, 2, 1)),
			new EDF_Test_DbEntitySortableEntity(vectorVal: Vector(0, 9, 9)),
			new EDF_Test_DbEntitySortableEntity(vectorVal: Vector(3, 0, 0))
		};
	
		// Act
		array<ref EDF_DbEntity> sorted = EDF_DbEntitySorter.GetSorted(entities, {{"m_vVectorValue", EDF_EDbEntitySortDirection.ASCENDING}});
	
		// Assert
		SetResult(EDF_AutotestResult.FromResult(
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(0)).m_vVectorValue == Vector(0, 9, 9) &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(1)).m_vVectorValue == Vector(1, 2, 1) &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(2)).m_vVectorValue == Vector(1, 9, 1) &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(3)).m_vVectorValue == Vector(3, 0, 0)));
	}
}

//------------------------------------------------------------------------------------------------
[Test("EDF_DbEntitySorterTests")]
class EDF_Test_DbEntitySorter_GetSorted_ArrayVector_DescSorted : SCR_AutotestCaseBase
{
	[Step(EStage.Main)]
	void ActAndAsset()
	{
		// Arrange
		array<ref EDF_DbEntity> entities = {
			new EDF_Test_DbEntitySortableEntity(vectorVal: Vector(1, 9, 1)),
			new EDF_Test_DbEntitySortableEntity(vectorVal: Vector(1, 2, 1)),
			new EDF_Test_DbEntitySortableEntity(vectorVal: Vector(0, 9, 9)),
			new EDF_Test_DbEntitySortableEntity(vectorVal: Vector(3, 0, 0))
		};
	
		// Act
		array<ref EDF_DbEntity> sorted = EDF_DbEntitySorter.GetSorted(entities, {{"m_vVectorValue", EDF_EDbEntitySortDirection.DESCENDING}});
	
		// Assert
		
		SetResult(EDF_AutotestResult.FromResult(
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(0)).m_vVectorValue == Vector(3, 0, 0) &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(1)).m_vVectorValue == Vector(1, 9, 1) &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(2)).m_vVectorValue == Vector(1, 2, 1) &&
			EDF_Test_DbEntitySortableEntity.Cast(sorted.Get(3)).m_vVectorValue == Vector(0, 9, 9)));
	}
}
