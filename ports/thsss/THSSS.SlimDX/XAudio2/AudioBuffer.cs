using System;
using System.IO;

namespace SlimDX.XAudio2
{
	public class AudioBuffer : IDisposable
	{
		public Stream AudioData { get; set; }

		public int AudioBytes { get; set; }

		public BufferFlags Flags { get; set; }

		public int PlayBegin { get; set; }

		public int PlayLength { get; set; }

		public int LoopCount { get; set; }

		public int LoopBegin { get; set; }

		public int LoopLength { get; set; }

		public void Dispose()
		{
		}
	}
}
