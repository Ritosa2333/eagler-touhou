using System.IO;
using SlimDX.Multimedia;
using SlimDX.XAudio2;

namespace Shooting;

public class XAudio2_Player
{
	private XAudio2 device;

	private SourceVoice sourceVoice;

	private AudioBuffer buffer;

	public float VoicePos { get; set; }

	public bool loadSuccess { get; private set; }

	public long Position
	{
		get
		{
			//IL_0006: Unknown result type (might be due to invalid IL or missing references)
			//IL_000b: Unknown result type (might be due to invalid IL or missing references)
			VoiceState state = sourceVoice.State;
			return state.SamplesPlayed / 44;
		}
	}

	public int Volume
	{
		get
		{
			return (int)(((Voice)sourceVoice).Volume * 200f);
		}
		set
		{
			if (0 <= value && value <= 100)
			{
				((Voice)sourceVoice).Volume = (float)value / 200f;
			}
		}
	}

	public XAudio2_Player(string fileName, XAudio2 device)
	{
		this.device = device;
		loadSuccess = (LoadSound(fileName) ? true : false);
	}

	private bool LoadSound(string fileName)
	{
		//IL_0007: Unknown result type (might be due to invalid IL or missing references)
		//IL_000d: Expected Obj, but got Unknown
		//IL_0013: Unknown result type (might be due to invalid IL or missing references)
		//IL_001d: Expected Obj, but got Unknown
		//IL_0055: Unknown result type (might be due to invalid IL or missing references)
		//IL_005f: Expected Obj, but got Unknown
		try
		{
			FileStream fileStream = File.OpenRead(fileName);
			WaveStream val = new WaveStream((Stream)fileStream);
			fileStream.Close();
			buffer = new AudioBuffer();
			buffer.AudioData = (Stream)(object)val;
			buffer.AudioBytes = (int)((Stream)(object)val).Length;
			buffer.Flags = (BufferFlags)64;
			sourceVoice = new SourceVoice(device, val.Format);
			Volume = 40;
			return true;
		}
		catch
		{
			return false;
		}
	}

	public void Play()
	{
		//IL_0022: Unknown result type (might be due to invalid IL or missing references)
		//IL_0027: Unknown result type (might be due to invalid IL or missing references)
		//IL_0067: Unknown result type (might be due to invalid IL or missing references)
		//IL_007a: Unknown result type (might be due to invalid IL or missing references)
		if (loadSuccess)
		{
			float num = VoicePos * 0.4f + 0.3f;
			VoiceDetails voiceDetails = ((Voice)sourceVoice).VoiceDetails;
			int inputChannels = voiceDetails.InputChannels;
			float[] array = new float[2]
			{
				1f - num,
				num
			};
			if (inputChannels == 2)
			{
				array = new float[3]
				{
					0f,
					1f - num,
					num
				};
			}
			((Voice)sourceVoice).SetOutputMatrix(inputChannels, 2, array);
			Reset();
			sourceVoice.Start((PlayFlags)0);
		}
	}

	public void Play(int Volume)
	{
		int volume = this.Volume;
		this.Volume = Volume;
		Play();
		this.Volume = volume;
	}

	public void Resume()
	{
		//IL_0010: Unknown result type (might be due to invalid IL or missing references)
		if (loadSuccess)
		{
			sourceVoice.Start((PlayFlags)0);
		}
	}

	public void Stop()
	{
		//IL_0010: Unknown result type (might be due to invalid IL or missing references)
		if (!loadSuccess)
		{
			return;
		}
		try
		{
			sourceVoice.Stop();
		}
		catch
		{
		}
	}

	private void Reset()
	{
		//IL_000f: Unknown result type (might be due to invalid IL or missing references)
		//IL_001b: Unknown result type (might be due to invalid IL or missing references)
		//IL_002d: Unknown result type (might be due to invalid IL or missing references)
		if (loadSuccess)
		{
			sourceVoice.Stop();
			sourceVoice.FlushSourceBuffers();
			sourceVoice.SubmitSourceBuffer(buffer);
		}
	}

	public void SetLoop(int LoopBegin, int LoopEnd)
	{
		SetLoop(0, 0, 255, LoopBegin, LoopEnd);
	}

	public void SetLoop(int PlayBegin, int PlayLength, int LoopCount, int LoopBegin, int LoopEnd)
	{
		buffer.PlayBegin = PlayBegin;
		buffer.PlayLength = PlayLength;
		buffer.LoopCount = LoopCount;
		buffer.LoopBegin = LoopBegin;
		if (LoopEnd > LoopBegin)
		{
			buffer.LoopLength = LoopEnd - LoopBegin;
		}
	}

	public void Dispose()
	{
		if (loadSuccess)
		{
			Stop();
			sourceVoice.Dispose();
			buffer.Dispose();
		}
	}
}
