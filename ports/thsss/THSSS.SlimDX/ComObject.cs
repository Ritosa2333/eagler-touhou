using System;

namespace SlimDX
{
	public class ComObject : IDisposable
	{
		public bool Disposed { get; private set; }

		public void Dispose()
		{
			if (!this.Disposed)
			{
				this.Disposed = true;
				this.Dispose(true);
			}
		}

		protected virtual void Dispose(bool disposing)
		{
		}
	}
}
