class EDF_HexHelper
{
	//------------------------------------------------------------------------------------------------
	//! Convert integer to hexadeciamal string
	//! \param value Input integer
	//! \param upperCase Decide if output should be upper or lower case
	//! \param fixedLength Add leading zeros for a minimum length output
	//! \return result hexadecimal string
	static string Convert(int value, bool upperCase = false, int fixedLength = -1)
	{
		array<string> resultChars = {"0", "0", "0", "0", "0", "0", "0", "0"};

		int asciiOffset = 87;
		if (upperCase) asciiOffset = 55;

		int padUntil = 7;
		if (fixedLength != -1) padUntil = 8 - Math.Min(fixedLength, 8);

		int resultIdx = 7;

		while (value)
		{
			int remainder = value % 16;

			if (remainder < 10)
			{
				resultChars.Set(resultIdx--, remainder.ToString());
			}
			else
			{
				resultChars.Set(resultIdx--, (remainder + asciiOffset).AsciiToString());
			}

			value /= 16;
		}

		string result;
		bool nonZero;

		foreach (int nChar, string char : resultChars)
		{
			if (char == "0" && nChar < padUntil && !nonZero) continue;
			nonZero = true;
			result += char;
		}

		return result;
	}
};
