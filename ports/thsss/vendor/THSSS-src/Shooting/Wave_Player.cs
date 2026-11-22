using SlimDX.XAudio2;

namespace Shooting;

public class Wave_Player
{
	private XAudio2_PlayerV2[] x2p = new XAudio2_PlayerV2[2];

	private byte AudioIndex;

	private XAudio2 device;

	private string[] fileName = new string[2];

	public bool OnPause { get; set; }

	public string FileName
	{
		get
		{
			return fileName[AudioIndex];
		}
		set
		{
			if (fileName[1 - AudioIndex] == value)
			{
				AudioIndex = (byte)(1 - AudioIndex);
				return;
			}
			int volume = 100;
			fileName[AudioIndex] = value;
			if (x2p[AudioIndex] != null)
			{
				volume = x2p[AudioIndex].Volume;
				x2p[AudioIndex].Dispose();
			}
			x2p[AudioIndex] = new XAudio2_PlayerV2(fileName[AudioIndex], device);
			x2p[AudioIndex].Volume = volume;
		}
	}

	public long CurrentPosition => x2p[AudioIndex].Position;

	public int Volume
	{
		get
		{
			if (x2p[AudioIndex] != null)
			{
				return x2p[AudioIndex].Volume;
			}
			return 100;
		}
		set
		{
			if (x2p[AudioIndex] != null)
			{
				x2p[AudioIndex].Volume = value;
			}
		}
	}

	public Wave_Player(XAudio2 device)
	{
		Volume = 100;
		this.device = device;
	}

	public void Play()
	{
		x2p[AudioIndex].Play();
	}

	public void PlayRepeat()
	{
		x2p[AudioIndex].SetLoop(0, 0);
		x2p[AudioIndex].Play();
	}

	public void PlayRepeat(int LoopBegin, int LoopEnd)
	{
		PlayRepeat(0, 0, 255, LoopBegin, LoopEnd);
	}

	public void PlayRepeat(int PlayBegin, int PlayLength, int LoopCount, int LoopBegin, int LoopEnd)
	{
		x2p[AudioIndex].Stop();
		x2p[AudioIndex].SetLoop(PlayBegin, PlayLength, LoopCount, LoopBegin, LoopEnd);
		x2p[AudioIndex].Play();
	}

	public void Stop()
	{
		if (x2p[AudioIndex] != null)
		{
			x2p[AudioIndex].Stop();
		}
	}

	public void Puase()
	{
		OnPause = true;
		Stop();
	}

	public void Resume()
	{
		OnPause = false;
		x2p[AudioIndex].Resume();
	}

	public void PreLoad(string FileName)
	{
		this.FileName = FileName;
		x2p[AudioIndex].Reset();
	}

	public void FilterON()
	{
		x2p[AudioIndex].FilterON();
	}

	public void FilterOFF()
	{
		x2p[AudioIndex].FilterOFF();
	}

	public void Dispose()
	{
		for (int i = 0; i < x2p.Length; i++)
		{
			if (x2p[AudioIndex] != null)
			{
				x2p[AudioIndex].Dispose();
			}
		}
	}
}
