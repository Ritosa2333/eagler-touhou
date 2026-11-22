using System;
using System.Runtime.InteropServices;

namespace Shooting;

public class MP3_Player
{
	public enum State
	{
		mPlaying = 1,
		mPuase,
		mStop
	}

	public struct structMCI
	{
		public bool bMut;

		public int iDur;

		public int iPos;

		public int iVol;

		public int iBal;

		public string iName;

		public State state;
	}

	[MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)]
	private string Name = "";

	[MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
	private string durLength = "";

	[MarshalAs(UnmanagedType.LPTStr)]
	private string TemStr = "";

	private int ilong;

	private int volumn;

	public structMCI mc;

	public string FileName
	{
		get
		{
			return mc.iName;
		}
		set
		{
			try
			{
				TemStr = "";
				TemStr = TemStr.PadLeft(127, Convert.ToChar(" "));
				Name = Name.PadLeft(260, Convert.ToChar(" "));
				mc.iName = value;
				ilong = APIClass.GetShortPathName(mc.iName, Name, Name.Length);
				Name = GetCurrPath(Name);
				Name = "open " + Convert.ToChar(34) + Name + Convert.ToChar(34) + " alias media";
				ilong = APIClass.mciSendString("close all", TemStr, TemStr.Length, 0);
				ilong = APIClass.mciSendString(Name, TemStr, TemStr.Length, 0);
				ilong = APIClass.mciSendString("set media time format milliseconds", TemStr, TemStr.Length, 0);
				mc.state = State.mStop;
			}
			catch
			{
			}
		}
	}

	public int Duration
	{
		get
		{
			durLength = "";
			durLength = durLength.PadLeft(128, Convert.ToChar(" "));
			APIClass.mciSendString("status media length", durLength, durLength.Length, 0);
			durLength = durLength.Trim();
			if (durLength == "")
			{
				return 0;
			}
			return (int)(Convert.ToDouble(durLength) / 1000.0);
		}
	}

	public int CurrentPosition
	{
		get
		{
			durLength = "";
			durLength = durLength.PadLeft(128, Convert.ToChar(" "));
			APIClass.mciSendString("status media position", durLength, durLength.Length, 0);
			mc.iPos = (int)Convert.ToDouble(durLength.Replace("\0", ""));
			return mc.iPos;
		}
	}

	public int Volumn
	{
		get
		{
			return volumn;
		}
		set
		{
			if (value < 0)
			{
				volumn = 0;
			}
			else if (value > 100)
			{
				volumn = 100;
			}
			else
			{
				volumn = value;
			}
			TemStr = "";
			TemStr = TemStr.PadLeft(127, Convert.ToChar(" "));
			APIClass.mciSendString("setaudio media volume to " + value * 10, TemStr, TemStr.Length, 0);
		}
	}

	public MP3_Player()
	{
		Volumn = 100;
	}

	public void Play()
	{
		TemStr = "";
		TemStr = TemStr.PadLeft(127, Convert.ToChar(" "));
		APIClass.mciSendString("play media", TemStr, TemStr.Length, 0);
		mc.state = State.mPlaying;
	}

	public void PlayRepeat()
	{
		TemStr = "";
		TemStr = TemStr.PadLeft(127, Convert.ToChar(" "));
		APIClass.mciSendString("play media repeat", TemStr, TemStr.Length, 0);
		mc.state = State.mPlaying;
	}

	public void Stop()
	{
		TemStr = "";
		TemStr = TemStr.PadLeft(128, Convert.ToChar(" "));
		ilong = APIClass.mciSendString("close media", TemStr, 128, 0);
		ilong = APIClass.mciSendString("close all", TemStr, 128, 0);
		mc.state = State.mStop;
	}

	public void Puase()
	{
		TemStr = "";
		TemStr = TemStr.PadLeft(128, Convert.ToChar(" "));
		ilong = APIClass.mciSendString("pause media", TemStr, TemStr.Length, 0);
		mc.state = State.mPuase;
	}

	public void Resume()
	{
		if (mc.state == State.mPuase)
		{
			TemStr = "";
			TemStr = TemStr.PadLeft(127, Convert.ToChar(" "));
			ilong = APIClass.mciSendString("resume media", TemStr, TemStr.Length, 0);
			mc.state = State.mPlaying;
		}
	}

	public void SetStartEnd(int t1, int t2)
	{
		TemStr = "";
		TemStr = TemStr.PadLeft(128, Convert.ToChar(" "));
		string text = t1.ToString();
		string text2 = t2.ToString();
		APIClass.mciSendString("play media from " + text + " to " + text2 + " repeat", TemStr, TemStr.Length, 0);
	}

	public void SetStartEnd(float t1, float t2)
	{
		SetStartEnd((int)t1 * 1000, (int)t2 * 1000);
	}

	private string GetCurrPath(string name)
	{
		if (name.Length < 1)
		{
			return "";
		}
		name = name.Trim();
		name = name.Substring(0, name.Length - 1);
		return name;
	}
}
