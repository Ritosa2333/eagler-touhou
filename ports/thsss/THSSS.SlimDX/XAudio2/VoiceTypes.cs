using System;

namespace SlimDX.XAudio2
{
	[Flags]
	public enum BufferFlags
	{
		None = 0,
		EndOfStream = 1
	}

	[Flags]
	public enum PlayFlags
	{
		None = 0
	}

	[Flags]
	public enum VoiceFlags
	{
		None = 0,
		Music = 1,
		UseFilter = 2
	}

	public enum FilterType
	{
		LowPassFilter = 0,
		HighPassFilter = 1
	}

	public struct FilterParameters
	{
		public FilterType Type;

		public float Frequency;

		public float OneOverQ;
	}

	public struct VoiceState
	{
		public long SamplesPlayed;
	}

	public struct VoiceDetails
	{
		public int InputChannels;
	}
}
