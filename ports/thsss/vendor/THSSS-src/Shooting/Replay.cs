using System;
using System.IO;
using System.Text;

namespace Shooting;

public class Replay
{
	private FileStream ReplayData { get; set; }

	private string FileName { get; set; }

	public long DataPosition
	{
		get
		{
			return ReplayData.Position;
		}
		set
		{
			ReplayData.Position = value;
		}
	}

	public bool CanRead => ReplayData.CanRead;

	public static ReplayInfo ReadTitle(string fileName)
	{
		string[] array = File.ReadAllLines(fileName, Encoding.Default);
		try
		{
			int num = 0;
			while (array[++num] != "ReplayInformation" && num < array.Length - 1)
			{
			}
			ReplayInfo replayInfo = new ReplayInfo
			{
				Version = array[num + 1],
				PlayerName = array[num + 2],
				Date = array[num + 3],
				Time = array[num + 4],
				MyPlaneName = array[num + 5],
				WeaponType = array[num + 6],
				Rank = (DifficultLevel)Convert.ToInt32(array[num + 7]),
				StartStage = array[num + 8],
				LastStage = array[num + 9],
				SlowRate = array[num + 10]
			};
			for (int i = num + 11; i < array.Length; i++)
			{
				char[] separator = new char[1] { '\t' };
				string[] array2 = array[i].Split(separator);
				MyPlaneInfo item = new MyPlaneInfo
				{
					Life = Convert.ToInt32(array2[0]),
					Spell = Convert.ToInt32(array2[1]),
					Power = Convert.ToInt32(array2[2]),
					Score = Convert.ToInt64(array2[3]),
					Graze = Convert.ToInt32(array2[4]),
					PosX = (float)Convert.ToDouble(array2[5]),
					PosY = (float)Convert.ToDouble(array2[6]),
					LifeChip = Convert.ToInt32(array2[7]),
					SpellChip = Convert.ToInt32(array2[8]),
					LifeUpCount = Convert.ToInt32(array2[9]),
					StarPoint = Convert.ToInt32(array2[10]),
					HighItemScore = Convert.ToInt32(array2[11]),
					Rate = (float)Convert.ToDouble(array2[12]),
					LastColor = (EnchantmentType)Convert.ToInt32(array2[13]),
					DataPosition = Convert.ToInt64(array2[14])
				};
				replayInfo.MyPlaneData.Add(item);
			}
			return replayInfo;
		}
		catch
		{
			return new ReplayInfo();
		}
	}

	public void WriteKey(int KeyValue)
	{
		byte value = (byte)(KeyValue & 0xFF);
		ReplayData.WriteByte(value);
		value = (byte)(KeyValue >> 8);
		value &= 0xFF;
		ReplayData.WriteByte(value);
	}

	public int ReadKey()
	{
		return ReplayData.ReadByte() + (ReplayData.ReadByte() << 8);
	}

	public void CreatRpy()
	{
		ReplayData = new FileStream(".\\Replay\\AutoSave.rpy", FileMode.Create);
	}

	public void SaveRpy(string fileName, ReplayInfo repInfo)
	{
		if (ReplayData.CanRead)
		{
			ReplayData.WriteByte(240);
			ReplayData.WriteByte(15);
			string tmp = null;
			tmp += "\r\n";
			tmp += "ReplayInformation";
			tmp += "\r\n";
			tmp += "ver 1.00";
			tmp += "\r\n";
			tmp += repInfo.PlayerName;
			tmp += "\r\n";
			tmp += DateTime.Now.Date.ToString("yy'/'MM'/'dd");
			tmp += "\r\n";
			tmp += DateTime.Now.ToShortTimeString();
			tmp += "\r\n";
			tmp += repInfo.MyPlaneName;
			tmp += "\r\n";
			tmp += repInfo.WeaponType.ToString();
			tmp += "\r\n";
			string text = tmp;
			int rank = (int)repInfo.Rank;
			tmp = text + rank;
			tmp += "\r\n";
			tmp += repInfo.StartStage;
			tmp += "\r\n";
			tmp += repInfo.LastStage;
			tmp += "\r\n";
			tmp += repInfo.SlowRate;
			repInfo.MyPlaneData.ForEach((MyPlaneInfo x) =>
			{
				tmp += "\r\n";
				string[] array = new string[30]
				{
					tmp,
					x.Life.ToString(),
					"\t",
					x.Spell.ToString(),
					"\t",
					x.Power.ToString(),
					"\t",
					x.Score.ToString(),
					"\t",
					x.Graze.ToString(),
					"\t",
					x.PosX.ToString(),
					"\t",
					x.PosY.ToString(),
					"\t",
					x.LifeChip.ToString(),
					"\t",
					x.SpellChip.ToString(),
					"\t",
					x.LifeUpCount.ToString(),
					"\t",
					x.StarPoint.ToString(),
					"\t",
					x.HighItemScore.ToString(),
					"\t",
					x.Rate.ToString(),
					"\t",
					null,
					null,
					null
				};
				int lastColor = (int)x.LastColor;
				array[27] = lastColor.ToString();
				array[28] = "\t";
				array[29] = x.DataPosition.ToString();
				tmp = string.Concat(array);
			});
			StreamWriter streamWriter = new StreamWriter(ReplayData);
			streamWriter.Write(tmp);
			streamWriter.Close();
			ReplayData.Close();
		}
		if (fileName != ".\\Replay\\AutoSave.rpy")
		{
			File.Copy(".\\Replay\\AutoSave.rpy", fileName, overwrite: true);
		}
	}

	public void LoadRpy(string fileName)
	{
		FileName = fileName;
		FileInfo fileInfo = new FileInfo(fileName);
		ReplayData = fileInfo.OpenRead();
	}

	public void ReloadRpy()
	{
		FileInfo fileInfo = new FileInfo(FileName);
		ReplayData = fileInfo.OpenRead();
	}

	public void CloseRpy()
	{
		if (ReplayData != null)
		{
			ReplayData.Close();
		}
	}

	public void Dispose()
	{
		if (ReplayData != null)
		{
			ReplayData.Close();
			ReplayData.Dispose();
		}
	}
}
