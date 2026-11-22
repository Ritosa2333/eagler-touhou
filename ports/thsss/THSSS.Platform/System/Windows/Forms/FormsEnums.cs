namespace System.Windows.Forms
{
	public enum AutoScaleMode
	{
		None,
		Font,
		Dpi
	}

	public enum FormBorderStyle
	{
		None,
		FixedSingle,
		Fixed3D,
		FixedDialog,
		Sizable
	}

	public enum FormStartPosition
	{
		Manual,
		CenterScreen,
		WindowsDefaultLocation,
		CenterParent
	}

	public enum ImageLayout
	{
		None,
		Tile,
		Center,
		Stretch,
		Zoom
	}

	public enum BorderStyle
	{
		None,
		FixedSingle,
		Fixed3D
	}

	public enum DockStyle
	{
		None,
		Top,
		Bottom,
		Left,
		Right,
		Fill
	}

	[Flags]
	public enum AnchorStyles
	{
		None = 0,
		Top = 1,
		Bottom = 2,
		Left = 4,
		Right = 8
	}

	public enum HorizontalAlignment
	{
		Left,
		Right,
		Center
	}

	public enum DialogResult
	{
		None,
		OK,
		Cancel,
		Yes,
		No
	}

	public enum MessageBoxButtons
	{
		OK,
		OKCancel,
		YesNo
	}

	public enum MessageBoxIcon
	{
		None,
		Hand,
		Error,
		Warning,
		Information
	}

	public struct Padding
	{
		public int Left;

		public int Top;

		public int Right;

		public int Bottom;

		public Padding(int all)
		{
			this.Left = all;
			this.Top = all;
			this.Right = all;
			this.Bottom = all;
		}

		public Padding(int left, int top, int right, int bottom)
		{
			this.Left = left;
			this.Top = top;
			this.Right = right;
			this.Bottom = bottom;
		}
	}
}
