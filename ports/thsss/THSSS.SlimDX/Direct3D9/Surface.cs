using System.IO;

namespace SlimDX.Direct3D9
{
	public class Surface : ComObject
	{
		public int GlTextureId { get; set; }

		public int Width { get; set; }

		public int Height { get; set; }

		public bool IsDepth { get; set; }

		public static Surface CreateDepthStencil(Device device, int width, int height, Format format, MultisampleType multisample, int quality, bool discard)
		{
			return new Surface
			{
				Width = width,
				Height = height,
				IsDepth = true
			};
		}

		public static void ToFile(Surface surface, string fileName, ImageFileFormat format)
		{
			var text = Path.GetDirectoryName(fileName);
			if (!string.IsNullOrEmpty(text))
			{
				Directory.CreateDirectory(text);
			}
			File.WriteAllBytes(fileName, new byte[] { 0xFF, 0xD8, 0xFF, 0xD9 });
		}
	}
}
