namespace SlimDX.Audio
{
	public interface IAudioApi
	{
		int Prepare(string path);

		int CreateVoice();

		void DestroyVoice(int voice);

		void Submit(int voice, int buffer, int playBegin, int playLength, int loopCount, int loopBegin, int loopLength);

		void Start(int voice);

		void Stop(int voice);

		void SetVolume(int voice, float volume);

		void SetPan(int voice, float left, float right);

		void SetFilter(int voice, bool highpass, float frequency);

		int SamplesPlayed(int voice);

		int Channels(int buffer);
	}

	public static class AudioHost
	{
		public static IAudioApi Api { get; set; }
	}
}
