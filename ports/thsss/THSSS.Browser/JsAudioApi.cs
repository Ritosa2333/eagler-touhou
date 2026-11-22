using SlimDX.Audio;

namespace Thsss.Browser
{
	public sealed class JsAudioApi : IAudioApi
	{
		public int Prepare(string path)
		{
			return ThsssJs.AudioPrepare(path ?? "");
		}

		public int CreateVoice()
		{
			return ThsssJs.AudioVoice();
		}

		public void DestroyVoice(int voice)
		{
			ThsssJs.AudioDestroy(voice);
		}

		public void Submit(int voice, int buffer, int playBegin, int playLength, int loopCount, int loopBegin, int loopLength)
		{
			ThsssJs.AudioSubmit(voice, buffer, playBegin, playLength, loopCount, loopBegin, loopLength);
		}

		public void Start(int voice)
		{
			ThsssJs.AudioStart(voice);
		}

		public void Stop(int voice)
		{
			ThsssJs.AudioStop(voice);
		}

		public void SetVolume(int voice, float volume)
		{
			ThsssJs.AudioVolume(voice, volume);
		}

		public void SetPan(int voice, float left, float right)
		{
			ThsssJs.AudioPan(voice, left, right);
		}

		public void SetFilter(int voice, bool highpass, float frequency)
		{
			ThsssJs.AudioFilter(voice, highpass, frequency);
		}

		public int SamplesPlayed(int voice)
		{
			return ThsssJs.AudioSamples(voice);
		}

		public int Channels(int buffer)
		{
			return ThsssJs.AudioChannels(buffer);
		}
	}
}
