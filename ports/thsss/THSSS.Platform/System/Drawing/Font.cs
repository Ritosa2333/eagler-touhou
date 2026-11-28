namespace System.Drawing
{
	[Flags]
	public enum FontStyle
	{
		Regular = 0,
		Bold = 1,
		Italic = 2,
		Underline = 4,
		Strikeout = 8
	}

	public class Font : IDisposable
	{
		public string Name { get; private set; }

		public float Size { get; private set; }

		public FontStyle Style { get; private set; }

		public bool Bold
		{
			get
			{
				return (this.Style & FontStyle.Bold) != 0;
			}
		}

		public bool Italic
		{
			get
			{
				return (this.Style & FontStyle.Italic) != 0;
			}
		}

		public Font(string family, float size)
			: this(family, size, FontStyle.Regular)
		{
		}

		public Font(string family, float size, FontStyle style)
		{
			this.Name = family;
			this.Size = size;
			this.Style = style;
		}

		public void Dispose()
		{
		}
	}
}
