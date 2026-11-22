namespace SlimDX.Direct3D9
{
	public class VertexBuffer : ComObject
	{
		public byte[] Data { get; private set; }

		internal float[] Decoded;

		internal int DecodedCount;

		internal VertexDeclaration DecodedDecl;

		internal bool DecodeDirty = true;

		public VertexBuffer(Device device, int sizeInBytes, Usage usage, VertexFormat format, Pool pool)
		{
			this.Data = new byte[sizeInBytes > 0 ? sizeInBytes : 0];
		}

		public DataStream Lock(int offset, int size, LockFlags flags)
		{
			if (size <= 0)
			{
				size = this.Data.Length - offset;
			}
			if (offset + size > this.Data.Length)
			{
				var array = new byte[offset + size];
				System.Buffer.BlockCopy(this.Data, 0, array, 0, this.Data.Length);
				this.Data = array;
			}
			this.DecodeDirty = true;
			return new DataStream(this.Data);
		}

		public void Unlock()
		{
			this.DecodeDirty = true;
		}
	}
}
