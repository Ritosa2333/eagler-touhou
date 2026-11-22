using System;

namespace Shooting;

public class ScoreHistory
{
	public string MyPlaneFullName { get; set; }

	public DifficultLevel Rank { get; set; }

	public string PlayerName { get; set; }

	public long Score { get; set; }

	public string Date { get; set; }

	public string Time { get; set; }

	public string Stage { get; set; }

	public string SlowRate { get; set; }

	public string Data2String()
	{
		return MyPlaneFullName + "\t" + (int)Rank + "\t" + PlayerName + "\t" + Score + "\t" + Date + "\t" + Time + "\t" + Stage + "\t" + SlowRate + "\r\n";
	}

	public string Data2DrawString()
	{
		return PlayerName.PadRight(8) + Score.ToString().PadLeft(11) + Date.PadLeft(12) + Time.PadLeft(6) + Stage.PadLeft(4) + SlowRate.PadLeft(5);
	}

	public string Data2DrawStringSimple()
	{
		return PlayerName.PadRight(8) + Score.ToString().PadLeft(11) + Stage.PadLeft(5) + SlowRate.PadLeft(6);
	}

	public void String2Data(string Data)
	{
		char[] separator = new char[1] { '\t' };
		string[] array = Data.Split(separator);
		MyPlaneFullName = array[0];
		Rank = (DifficultLevel)Convert.ToInt32(array[1]);
		PlayerName = array[2];
		Score = Convert.ToInt64(array[3]);
		Date = array[4];
		Time = array[5];
		Stage = array[6];
		SlowRate = array[7];
	}

	public void Copy(ScoreHistory SH)
	{
		String2Data(SH.Data2String().Replace("\r\n", string.Empty));
	}
}
