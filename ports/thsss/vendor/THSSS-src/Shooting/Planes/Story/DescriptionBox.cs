using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting.Planes.Story;

internal class DescriptionBox : BaseStoryItem
{
	private Texture StringTexture;

	private Font sysfont;

	private Font DXfont { get; set; }

	public Color FontColor { get; set; }

	public string Text { get; set; }

	public int OffsetX { get; set; }

	public int OffsetY { get; set; }

	public bool Shadowed { get; set; }

	public DescriptionBox(StageDataPackage StageData)
		: base(StageData)
	{
		//IL_00d0: Unknown result type (might be due to invalid IL or missing references)
		//IL_00da: Expected Obj, but got Unknown
		//IL_00e7: Unknown result type (might be due to invalid IL or missing references)
		//IL_00f1: Expected Obj, but got Unknown
		TxtureObject = TextureObjectDictionary["对话框"];
		Position = new PointF(BoundRect.Left, BoundRect.Bottom - TxtureObject.Height * BoundRect.Width / TxtureObject.Width);
		Velocity = 0f;
		Direction = 0.0;
		TransparentValueF = 0f;
		MaxTransparent = 200;
		TransparentVelocity = 10f;
		OffsetX = 20;
		OffsetY = 20;
		sysfont = new Font(StageData.GlobalData.FontType, 16f * GlobalDataPackage.FontScale, (FontStyle)1);
		DXfont = new Font(StageData.DeviceMain, sysfont);
		FontColor = Color.White;
	}

	public override void Ctrl()
	{
		if (StringTexture != null)
		{
			((ComObject)StringTexture).Dispose();
		}
		StringTexture = StageData.DrawString(Text, sysfont, Brushes.White, 1024, 256);
		base.Ctrl();
	}

	public override void Show()
	{
		SizeF destinationSize = new SizeF(BoundRect.Width, TxtureObject.Height * BoundRect.Width / TxtureObject.Width);
		SpriteMain.Draw2D(TxtureObject.TXTure, TxtureObject.PosRect, destinationSize, new PointF(0f, 0f), 0f, Position, Color.FromArgb(TransparentValue, Color.White));
		if (Text != null && StringTexture != null)
		{
			if (Shadowed)
			{
				SpriteMain.Draw2D(StringTexture, new Rectangle(0, 0, 1024, 256), new SizeF(1024f, 256f), new PointF((int)Position.X + 1 + OffsetX, (int)Position.Y + 1 + (int)((float)OffsetY * destinationSize.Height / 90f)), Color.FromArgb(150, 0, 0, 0));
			}
			SpriteMain.Draw2D(StringTexture, new Rectangle(0, 0, 1024, 256), new SizeF(1024f, 256f), new PointF((int)Position.X + OffsetX, (int)Position.Y + (int)((float)OffsetY * destinationSize.Height / 90f)), Color.White);
		}
	}

	public override void Dispose()
	{
		((ComObject)DXfont).Dispose();
		if (StringTexture != null)
		{
			((ComObject)StringTexture).Dispose();
		}
	}
}
