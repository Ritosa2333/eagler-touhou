using System.IO;
using SlimDX.Multimedia;
using SlimDX.XAudio2;

namespace Shooting;

public class XAudio2_PlayerV2
{
	private XAudio2 device;

	private SourceVoice sourceVoice;

	private AudioBuffer buffer;

	private bool xwmaMode;

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
			return (int)(((Voice)sourceVoice).Volume * 100f);
		}
		set
		{
			if (0 <= value && value <= 100)
			{
				((Voice)sourceVoice).Volume = (float)value / 100f;
			}
		}
	}

	public XAudio2_PlayerV2(string fileName, XAudio2 device)
	{
		this.device = device;
		loadSuccess = (LoadSound(fileName) ? true : false);
	}

	private bool LoadSound(string fileName)
	{
		//IL_0021: Unknown result type (might be due to invalid IL or missing references)
		//IL_002b: Expected Obj, but got Unknown
		//IL_0065: Unknown result type (might be due to invalid IL or missing references)
		//IL_006f: Expected Obj, but got Unknown
		//IL_008b: Unknown result type (might be due to invalid IL or missing references)
		//IL_0091: Expected Obj, but got Unknown
		//IL_0092: Unknown result type (might be due to invalid IL or missing references)
		//IL_009c: Expected Obj, but got Unknown
		//IL_00d6: Unknown result type (might be due to invalid IL or missing references)
		//IL_00e0: Expected Obj, but got Unknown
		try
		{
			FileInfo fileInfo = new FileInfo(fileName);
			if (fileInfo.Extension == ".xwma")
			{
				xWMAStream xWMAStream2 = new xWMAStream(fileName);
				buffer = new AudioBuffer();
				buffer.AudioData = xWMAStream2;
				buffer.AudioBytes = (int)xWMAStream2.Length;
				buffer.Flags = (BufferFlags)64;
				sourceVoice = new SourceVoice(device, xWMAStream2.Format, (VoiceFlags)24);
				xwmaMode = true;
			}
			else if (fileInfo.Extension == ".wav")
			{
				WaveStream val = new WaveStream(fileName);
				buffer = new AudioBuffer();
				buffer.AudioData = (Stream)(object)val;
				buffer.AudioBytes = (int)((Stream)(object)val).Length;
				buffer.Flags = (BufferFlags)64;
				sourceVoice = new SourceVoice(device, val.Format, (VoiceFlags)24);
			}
			Volume = 100;
			return true;
		}
		catch
		{
			return false;
		}
	}

	public void Play()
	{
		//IL_0016: Unknown result type (might be due to invalid IL or missing references)
		if (loadSuccess)
		{
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
		//IL_0016: Unknown result type (might be due to invalid IL or missing references)
		if (!loadSuccess)
		{
			return;
		}
		try
		{
			SourceVoice val = sourceVoice;
			if (val != null)
			{
				val.Stop();
			}
		}
		catch
		{
		}
	}

	public void Reset()
	{
		//IL_003f: Unknown result type (might be due to invalid IL or missing references)
		//IL_004b: Unknown result type (might be due to invalid IL or missing references)
		//IL_005d: Unknown result type (might be due to invalid IL or missing references)
		//IL_0032: Unknown result type (might be due to invalid IL or missing references)
		if (loadSuccess)
		{
			if (xwmaMode)
			{
				sourceVoice.SubmitSourceBuffer(buffer, ((xWMAStream)buffer.AudioData).DecodedPacketCumulativeBytes);
				return;
			}
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

	public void FilterON()
	{
		//IL_0002: Unknown result type (might be due to invalid IL or missing references)
		//IL_002e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0052: Unknown result type (might be due to invalid IL or missing references)
		FilterParameters filterParameters = default;
		filterParameters.Type = (FilterType)2;
		filterParameters.Frequency = 0.5f;
		filterParameters.OneOverQ = 2f;
		((Voice)sourceVoice).FilterParameters = filterParameters;
		((Voice)sourceVoice).SetOutputMatrix(2, 2, new float[3] { 0f, 4f, 4f });
	}

	public void FilterOFF()
	{
		//IL_0002: Unknown result type (might be due to invalid IL or missing references)
		//IL_002e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0052: Unknown result type (might be due to invalid IL or missing references)
		FilterParameters filterParameters = default;
		filterParameters.Type = (FilterType)2;
		filterParameters.Frequency = 0f;
		filterParameters.OneOverQ = 2f;
		((Voice)sourceVoice).FilterParameters = filterParameters;
		((Voice)sourceVoice).SetOutputMatrix(2, 2, new float[3] { 0f, 1f, 1f });
	}

	public void Dispose()
	{
		if (loadSuccess)
		{
			Stop();
			sourceVoice.Dispose();
			buffer.AudioData.Dispose();
			buffer.Dispose();
		}
	}
}
