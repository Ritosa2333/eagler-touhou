using System;

namespace Shooting;

public class ClearHistory
{
	public string MyPlaneFullName { get; set; }

	public DifficultLevel Rank { get; set; }

	public int StartTimes { get; set; }

	public long PlayingTime { get; set; }

	public int ClearTimes { get; set; }

	public int NoContinueClearTimes { get; set; }

	public int PracticeLevel { get; set; }

	public string Data2String()
	{
		return MyPlaneFullName + "\t" + (int)Rank + "\t" + StartTimes + "\t" + PlayingTime + "\t" + ClearTimes + "\t" + NoContinueClearTimes + "\t" + PracticeLevel + "\r\n";
	}

	public void String2Data(string Data)
	{
		char[] separator = new char[1] { '\t' };
		string[] array = Data.Split(separator);
		MyPlaneFullName = array[0];
		Rank = (DifficultLevel)Convert.ToInt32(array[1]);
		StartTimes = Convert.ToInt32(array[2]);
		PlayingTime = Convert.ToInt32(array[3]);
		ClearTimes = Convert.ToInt32(array[4]);
		NoContinueClearTimes = Convert.ToInt32(array[5]);
		PracticeLevel = Convert.ToInt32(array[6]);
	}
}
