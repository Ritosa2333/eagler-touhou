using System.Drawing.Imaging;

namespace System.Drawing
{
	public abstract class Image : IDisposable
	{
		public int Width { get; protected set; }

		public int Height { get; protected set; }

		public PixelFormat PixelFormat { get; protected set; }

		public static Image FromFile(string filename)
		{
			return new Bitmap(filename);
		}

		public void Dispose()
		{
		}
	}

	public class Bitmap : Image
	{
		private readonly byte[] rgba;

		public Bitmap(int width, int height)
			: this(width, height, PixelFormat.Format32bppArgb)
		{
		}

		public Bitmap(int width, int height, PixelFormat format)
		{
			this.Width = Math.Max(1, width);
			this.Height = Math.Max(1, height);
			this.PixelFormat = format;
			this.rgba = new byte[this.Width * this.Height * 4];
		}

		public Bitmap(string filename)
		{
			int num;
			int num2;
			var array = SlimDX.Imaging.ImageFiles.DecodeRgba(filename, out num, out num2);
			this.Width = num;
			this.Height = num2;
			this.PixelFormat = PixelFormat.Format32bppArgb;
			this.rgba = array;
		}

		internal void ReplaceRgba(byte[] pixels)
		{
			if (pixels == null || this.rgba == null)
			{
				return;
			}
			var num = Math.Min(this.rgba.Length, pixels.Length);
			Buffer.BlockCopy(pixels, 0, this.rgba, 0, num);
		}

		public Color GetPixel(int x, int y)
		{
			if (this.rgba == null || x < 0 || y < 0 || x >= this.Width || y >= this.Height)
			{
				return Color.Transparent;
			}
			var num = (y * this.Width + x) * 4;
			return Color.FromArgb(this.rgba[num + 3], this.rgba[num], this.rgba[num + 1], this.rgba[num + 2]);
		}

		public BitmapData LockBits(Rectangle rect, ImageLockMode flags, PixelFormat format)
		{
			var array = new byte[Math.Max(1, this.Width * this.Height * 4)];
			if (this.rgba != null)
			{
				var num = Math.Min(array.Length, this.rgba.Length);
				for (var i = 0; i < num / 4; i++)
				{
					array[i * 4] = this.rgba[i * 4 + 2];
					array[i * 4 + 1] = this.rgba[i * 4 + 1];
					array[i * 4 + 2] = this.rgba[i * 4];
					array[i * 4 + 3] = this.rgba[i * 4 + 3];
				}
			}
			return new BitmapData
			{
				Width = this.Width,
				Height = this.Height,
				PixelFormat = format,
				Stride = this.Width * 4,
				Pixels = array
			};
		}

		public void UnlockBits(BitmapData data)
		{
			if (data != null)
			{
				data.FreePin();
			}
		}
	}

	public class BitmapData
	{
		public int Width { get; set; }

		public int Height { get; set; }

		public int Stride { get; set; }

		public PixelFormat PixelFormat { get; set; }

		public byte[] Pixels { get; set; }

		public IntPtr Scan0
		{
			get
			{
				if (this.Pixels == null)
				{
					return IntPtr.Zero;
				}
				if (this.handle.IsAllocated)
				{
					return this.handle.AddrOfPinnedObject();
				}
				this.handle = System.Runtime.InteropServices.GCHandle.Alloc(this.Pixels, System.Runtime.InteropServices.GCHandleType.Pinned);
				return this.handle.AddrOfPinnedObject();
			}
		}

		internal void FreePin()
		{
			if (this.handle.IsAllocated)
			{
				this.handle.Free();
			}
		}

		~BitmapData()
		{
			this.FreePin();
		}

		private System.Runtime.InteropServices.GCHandle handle;
	}

	public class Icon
	{
	}
}
