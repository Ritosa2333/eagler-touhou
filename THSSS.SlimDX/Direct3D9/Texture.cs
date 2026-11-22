using System;
using System.Drawing;
using SlimDX.Gl;
using SlimDX.Imaging;

namespace SlimDX.Direct3D9
{
	public class Texture : BaseTexture
	{
		public int GlId { get; private set; }

		public int Width { get; private set; }

		public int Height { get; private set; }

		public byte[] Rgba { get; private set; }

		public bool IsRenderTarget { get; private set; }

		private byte[] lockBgra;

		private readonly bool ownsGl;

		public Texture(Device device, int width, int height, int levelCount, Usage usage, Format format, Pool pool)
		{
			this.Width = ClampEdge(width);
			this.Height = ClampEdge(height);
			this.ownsGl = true;
			this.IsRenderTarget = (usage & Usage.RenderTarget) != 0;
			int num;
			byte[] array;
			if (!this.IsRenderTarget && TexturePool.Take(this.Width, this.Height, out num, out array))
			{
				this.GlId = num;
				this.Rgba = array;
				return;
			}
			this.Rgba = new byte[this.Width * this.Height * 4];
			for (var i = 3; i < this.Rgba.Length; i += 4)
			{
				this.Rgba[i] = byte.MaxValue;
			}
			if (this.IsRenderTarget)
			{
				this.GlId = GlHost.Need().CreateRenderTarget(this.Width, this.Height);
			}
			else
			{
				this.GlId = GlHost.Need().CreateTexture(this.Width, this.Height, this.Rgba);
			}
		}

		private Texture(int glId, int width, int height, bool owner)
		{
			this.GlId = glId;
			this.Width = width;
			this.Height = height;
			this.ownsGl = owner;
		}

		internal static Texture AllocateEmpty(int width, int height)
		{
			var num = ClampEdge(width);
			var num2 = ClampEdge(height);
			var num3 = GlHost.Need().CreateTexture(num, num2, Array.Empty<byte>());
			return new Texture(num3, num, num2, true);
		}

		public static Texture Wrap(int glId, int width, int height)
		{
			return new Texture(glId, width, height, false);
		}

		internal static int ClampEdge(int value)
		{
			if (value < 1)
			{
				return 1;
			}
			if (value > 2048)
			{
				return 2048;
			}
			return value;
		}

		public static Texture FromFile(Device device, string fileName)
		{
			return FromFile(device, fileName, 0, 0, 0, Usage.None, Format.Unknown, Pool.Managed, Filter.Linear, Filter.None, 0);
		}

		public static Texture FromFile(Device device, string fileName, int width, int height, int levelCount, Usage usage, Format format, Pool pool, Filter filter, Filter mipFilter, int colorKey)
		{
			int num;
			int num2;
			var array = ImageFiles.DecodeRgba(fileName, out num, out num2);
			if (colorKey != 0)
			{
				var b = (byte)((colorKey >> 16) & 255);
				var b2 = (byte)((colorKey >> 8) & 255);
				var b3 = (byte)(colorKey & 255);
				for (var i = 0; i < array.Length; i += 4)
				{
					if (array[i] == b && array[i + 1] == b2 && array[i + 2] == b3)
					{
						array[i + 3] = 0;
					}
				}
			}
			var texture = new Texture(device, num, num2, 1, Usage.None, Format.A8R8G8B8, Pool.Managed);
			texture.Rgba = array;
			GlHost.Need().UpdateTexture(texture.GlId, num, num2, array);
			return texture;
		}

		public Surface GetSurfaceLevel(int level)
		{
			return new Surface
			{
				GlTextureId = this.GlId,
				Width = this.Width,
				Height = this.Height
			};
		}

		public SurfaceDescription GetLevelDescription(int level)
		{
			var surfaceDescription = default(SurfaceDescription);
			surfaceDescription.Width = this.Width;
			surfaceDescription.Height = this.Height;
			surfaceDescription.Format = Format.A8R8G8B8;
			return surfaceDescription;
		}

		public DataRectangle LockRectangle(int level, Rectangle rect, LockFlags flags)
		{
			var num = rect.Width;
			var num2 = rect.Height;
			if (num < 1 || num2 < 1 || (long)num * num2 > (long)this.Width * this.Height)
			{
				num = this.Width;
				num2 = this.Height;
			}
			var num3 = num * num2 * 4;
			if (this.lockBgra == null || this.lockBgra.Length != num3)
			{
				this.lockBgra = new byte[num3];
			}
			return new DataRectangle
			{
				Data = new DataStream(this.lockBgra)
			};
		}

		public void UnlockRectangle(int level)
		{
			if (this.lockBgra == null)
			{
				return;
			}
			var num = Math.Min(this.Rgba.Length / 4, this.lockBgra.Length / 4);
			for (var i = 0; i < num; i++)
			{
				this.Rgba[i * 4] = this.lockBgra[i * 4 + 2];
				this.Rgba[i * 4 + 1] = this.lockBgra[i * 4 + 1];
				this.Rgba[i * 4 + 2] = this.lockBgra[i * 4];
				this.Rgba[i * 4 + 3] = this.lockBgra[i * 4 + 3];
			}
			if (!this.IsRenderTarget && this.GlId != 0)
			{
				GlHost.Need().UpdateTexture(this.GlId, this.Width, this.Height, this.Rgba);
			}
		}

		protected override void Dispose(bool disposing)
		{
			if (!this.ownsGl || this.GlId == 0)
			{
				this.GlId = 0;
				return;
			}
			if (!this.IsRenderTarget && TexturePool.Return(this.GlId, this.Width, this.Height, this.Rgba))
			{
				this.GlId = 0;
				this.Rgba = null;
				this.lockBgra = null;
				return;
			}
			GlHost.Need().DeleteTexture(this.GlId);
			this.GlId = 0;
			this.Rgba = null;
			this.lockBgra = null;
		}
	}

	internal static class TexturePool
	{
		private const int Limit = 16;

		private struct Entry
		{
			public int GlId;

			public int Width;

			public int Height;

			public byte[] Rgba;
		}

		private static readonly System.Collections.Generic.List<Entry> entries = new System.Collections.Generic.List<Entry>();

		public static bool Take(int width, int height, out int glId, out byte[] rgba)
		{
			for (var i = entries.Count - 1; i >= 0; i--)
			{
				var entry = entries[i];
				if (entry.Width == width && entry.Height == height && entry.Rgba != null && entry.Rgba.Length == width * height * 4)
				{
					entries.RemoveAt(i);
					glId = entry.GlId;
					rgba = entry.Rgba;
					return true;
				}
			}
			glId = 0;
			rgba = null;
			return false;
		}

		public static bool Return(int glId, int width, int height, byte[] rgba)
		{
			if (glId == 0 || rgba == null || entries.Count >= Limit)
			{
				return false;
			}
			var entry = default(Entry);
			entry.GlId = glId;
			entry.Width = width;
			entry.Height = height;
			entry.Rgba = rgba;
			entries.Add(entry);
			return true;
		}
	}
}
