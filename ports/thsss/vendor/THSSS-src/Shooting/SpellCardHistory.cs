using System;

namespace Shooting;

public class SpellCardHistory
{
	public string MyPlaneFullName { get; set; }

	public DifficultLevel Rank { get; set; }

	public int Index { get; set; }

	public string Name { get; set; }

	public int Recorded { get; set; }

	public int History { get; set; }

	public string Data2String()
	{
		return Index + "\t" + MyPlaneFullName + "\t" + (int)Rank + "\t" + Recorded + "\t" + History + "\r\n";
	}

	public void String2Data(string Data)
	{
		char[] separator = new char[1] { '\t' };
		string[] array = Data.Split(separator);
		Index = Convert.ToInt32(array[0]);
		MyPlaneFullName = array[1];
		Rank = (DifficultLevel)Convert.ToInt32(array[2]);
		Recorded = Convert.ToInt32(array[3]);
		History = Convert.ToInt32(array[4]);
	}
}
