using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

public class TextureObject
{
	public Texture TXTure { get; set; }

	public int Width => PosRect.Width;

	public int Height => PosRect.Height;

	public int SrcWidth { get; set; }

	public int SrcHeight { get; set; }

	public Rectangle PosRect { get; set; }

	public int OffsetX { get; set; }

	public int OffsetY { get; set; }

	public PointF RotatingCenter => new PointF(PosRect.Width / 2 + OffsetX, PosRect.Height / 2 + OffsetY);

	public PointF LeftTop => new PointF(0f, 0f);

	public PointF RightTop => new PointF(Width, 0f);

	public void Dispose()
	{
		if (!((ComObject)TXTure).Disposed)
		{
			((ComObject)TXTure).Dispose();
		}
	}
}
