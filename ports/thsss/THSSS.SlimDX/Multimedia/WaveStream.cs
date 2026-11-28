using System;
using System.IO;

namespace SlimDX.Multimedia
{
	public class WaveStream : Stream
	{
		private readonly byte[] data;

		private int position;

		public WaveFormat Format { get; private set; }

		public WaveStream(Stream stream)
		{
			this.Format = new WaveFormat();
			if (stream == null)
			{
				this.data = new byte[0];
				return;
			}
			using (var memoryStream = new MemoryStream())
			{
				stream.CopyTo(memoryStream);
				this.data = memoryStream.ToArray();
			}
		}

		public WaveStream(string path)
			: this(File.OpenRead(path))
		{
		}

		public override bool CanRead
		{
			get
			{
				return true;
			}
		}

		public override bool CanSeek
		{
			get
			{
				return true;
			}
		}

		public override bool CanWrite
		{
			get
			{
				return false;
			}
		}

		public override long Length
		{
			get
			{
				return this.data.LongLength;
			}
		}

		public override long Position
		{
			get
			{
				return this.position;
			}
			set
			{
				this.position = (int)value;
			}
		}

		public override void Flush()
		{
		}

		public override int Read(byte[] buffer, int offset, int count)
		{
			var num = Math.Min(count, this.data.Length - this.position);
			if (num <= 0)
			{
				return 0;
			}
			System.Buffer.BlockCopy(this.data, this.position, buffer, offset, num);
			this.position += num;
			return num;
		}

		public override long Seek(long offset, SeekOrigin origin)
		{
			if (origin == SeekOrigin.Begin)
			{
				this.position = (int)offset;
			}
			else if (origin == SeekOrigin.Current)
			{
				this.position += (int)offset;
			}
			else
			{
				this.position = this.data.Length + (int)offset;
			}
			return this.position;
		}

		public override void SetLength(long value)
		{
		}

		public override void Write(byte[] buffer, int offset, int count)
		{
		}
	}
}
