class EDF_RefArrayCaster<Class TSourceType, Class TResultType>
{
	//------------------------------------------------------------------------------------------------
	//! Cast an array by converting all individual items. Allocates a new array of the input size.
	//! \param sourceArray Input array
	//! \return casted result array
	static array<ref TResultType> Convert(array<ref TSourceType> sourceArray)
	{
		if (!sourceArray)
			return null;

		array<ref TResultType> castedResult();
		castedResult.Reserve(sourceArray.Count());

		foreach (TSourceType element : sourceArray)
		{
			TResultType castedElement = TResultType.Cast(element);
			if (castedElement)
				castedResult.Insert(castedElement);
		}

		return castedResult;
	}
};
