using System.Drawing.Text;

namespace System.Drawing
{
	public abstract class Brush : System.IDisposable
	{
		public void Dispose()
		{
		}
	}

	public class SolidBrush : Brush
	{
		public Color Color { get; set; }

		public SolidBrush(Color color)
		{
			this.Color = color;
		}
	}

	public class Graphics : System.IDisposable
	{
		private readonly Bitmap bitmap;

		public TextRenderingHint TextRenderingHint { get; set; }

		private Graphics(Bitmap bitmap)
		{
			this.bitmap = bitmap;
		}

		public static Graphics FromImage(Image image)
		{
			return new Graphics(image as Bitmap);
		}

		public void DrawString(string text, Font font, Brush brush, float x, float y)
		{
			if (this.bitmap == null || string.IsNullOrEmpty(text) || GlyphRaster.Draw == null)
			{
				return;
			}
			var num = font != null && font.Size > 0f ? font.Size : 16f;
			var solidBrush = brush as SolidBrush;
			var num2 = solidBrush != null ? solidBrush.Color.ToArgb() : -1;
			var text2 = font != null ? font.Name : "sans-serif";
			var array = GlyphRaster.Draw(text, this.bitmap.Width, this.bitmap.Height, num, font != null && font.Bold, font != null && font.Italic, num2, text2);
			this.bitmap.ReplaceRgba(array);
		}

		public void Dispose()
		{
		}
	}
}
