using System;

namespace SlimDX.Multimedia
{
	public enum WaveFormatTag
	{
		Pcm = 1,
		Extensible = 65534,
		WMAudio2 = 353
	}

	[Flags]
	public enum Speakers
	{
		None = 0,
		FrontLeft = 1,
		FrontRight = 2
	}

	public class WaveFormat
	{
		public WaveFormatTag FormatTag { get; set; }

		public short Channels { get; set; }

		public int SamplesPerSecond { get; set; }

		public int AverageBytesPerSecond { get; set; }

		public short BlockAlignment { get; set; }

		public short BitsPerSample { get; set; }

		public WaveFormat()
		{
			this.FormatTag = WaveFormatTag.Pcm;
			this.Channels = 2;
			this.SamplesPerSecond = 44100;
			this.BitsPerSample = 16;
			this.BlockAlignment = 4;
			this.AverageBytesPerSecond = 176400;
		}
	}

	public class WaveFormatExtensible : WaveFormat
	{
		public short ValidBitsPerSample { get; set; }

		public Speakers ChannelMask { get; set; }

		public Guid SubFormat { get; set; }
	}
}
