using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

internal class StoryBox : BaseStoryItem
{
	private string text;

	private Texture StringTexture;

	private Font sysfont;

	private char[] TextArray;

	private string text2;

	private Texture StringTexture2;

	public Color FontColor2;

	public string DrawText2;

	private char[] TextArray2;

	private Font DXfont { get; set; }

	public Color FontColor { get; set; }

	public int OffsetX { get; set; }

	public int OffsetY { get; set; }

	public bool Shadowed { get; set; }

	public string DrawText { get; set; }

	public string Text
	{
		get
		{
			return text;
		}
		set
		{
			if (value != text)
			{
				text = value;
				TextArray = text.ToCharArray();
				DrawText = "";
				Time = 0;
			}
		}
	}

	public string Text2
	{
		get
		{
			return text2;
		}
		set
		{
			if (value != text2)
			{
				text2 = value;
				TextArray2 = text2.ToCharArray();
				DrawText2 = "";
				Time = 0;
			}
		}
	}

	public StoryBox(StageDataPackage StageData)
		: base(StageData)
	{
		//IL_00ce: Unknown result type (might be due to invalid IL or missing references)
		//IL_00d8: Expected Obj, but got Unknown
		//IL_00e5: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ef: Expected Obj, but got Unknown
		TxtureObject = TextureObjectDictionary["对话框"];
		Position = new PointF(BoundRect.Left, BoundRect.Bottom - TxtureObject.Height * BoundRect.Width / TxtureObject.Width - 8);
		Velocity = 0f;
		Direction = 0.0;
		TransparentValueF = 0f;
		TransparentVelocity = 10f;
		OffsetX = 20;
		OffsetY = 21;
		Shadowed = true;
		sysfont = new Font(StageData.GlobalData.FontType, 16f * GlobalDataPackage.FontScale, (FontStyle)1);
		DXfont = new Font(StageData.DeviceMain, sysfont);
		FontColor = Color.White;
	}

	public override void Ctrl()
	{
		if (DrawText != null && DrawText.Replace("\r\n", "") == text)
		{
			TransparentValueF = MaxTransparent;
			return;
		}
		if (TextArray != null && Time < TextArray.Length)
		{
			if ((float)DXfont.MeasureString(SpriteMain.sprite, DrawText + TextArray[Time], (DrawTextFormat)0).Width / 1.15f > (float)(TxtureObject.Width - OffsetX))
			{
				DrawText += "\r\n";
			}
			DrawText += TextArray[Time];
		}
		if (StringTexture != null)
		{
			((ComObject)StringTexture).Dispose();
		}
		StringTexture = StageData.DrawString(DrawText, sysfont, Brushes.White, 512, 256);
		if (TextArray2 != null && Time < TextArray2.Length)
		{
			if ((float)DXfont.MeasureString(SpriteMain.sprite, DrawText2 + TextArray2[Time], (DrawTextFormat)0).Width / 1.15f > (float)(TxtureObject.Width - OffsetX))
			{
				DrawText2 += "\r\n";
			}
			DrawText2 += TextArray2[Time];
		}
		if (StringTexture2 != null)
		{
			((ComObject)StringTexture2).Dispose();
		}
		StringTexture2 = StageData.DrawString(DrawText2, sysfont, Brushes.White, 512, 256);
		base.Ctrl();
	}

	public void DrawFullText()
	{
		if (TextArray != null)
		{
			while (Time < TextArray.Length)
			{
				if ((float)DXfont.MeasureString(SpriteMain.sprite, DrawText + TextArray[Time], (DrawTextFormat)0).Width / 1.15f > (float)(TxtureObject.Width - OffsetX))
				{
					DrawText += "\r\n";
				}
				DrawText += TextArray[Time];
				Time++;
			}
		}
		if (StringTexture != null)
		{
			((ComObject)StringTexture).Dispose();
		}
		StringTexture = StageData.DrawString(DrawText, sysfont, Brushes.White, 512, 256);
		if (TextArray2 != null)
		{
			while (Time < TextArray2.Length)
			{
				if ((float)DXfont.MeasureString(SpriteMain.sprite, DrawText2 + TextArray2[Time], (DrawTextFormat)0).Width / 1.15f > (float)(TxtureObject.Width - OffsetX))
				{
					DrawText2 += "\r\n";
				}
				DrawText2 += TextArray2[Time];
				Time++;
			}
		}
		if (StringTexture2 != null)
		{
			((ComObject)StringTexture2).Dispose();
		}
		StringTexture2 = StageData.DrawString(DrawText2, sysfont, Brushes.White, 512, 256);
	}

	public void SetFont(float FontSize)
	{
		//IL_0034: Unknown result type (might be due to invalid IL or missing references)
		//IL_003e: Expected Obj, but got Unknown
		//IL_0050: Unknown result type (might be due to invalid IL or missing references)
		//IL_005a: Expected Obj, but got Unknown
		if (sysfont != null)
		{
			FontSize *= GlobalDataPackage.FontScale;
			if (sysfont.Size != FontSize)
			{
				sysfont = new Font(StageData.GlobalData.FontType, FontSize, (FontStyle)1);
				DXfont = new Font(StageData.DeviceMain, sysfont);
			}
		}
	}

	public override void Show()
	{
		SizeF destinationSize = new SizeF(TxtureObject.Width, 116f);
		SpriteMain.Draw2D(TxtureObject.TXTure, TxtureObject.PosRect, destinationSize, TxtureObject.LeftTop, 0f, Position, Color.FromArgb(TransparentValue, Color.White));
		if (Text != null && StringTexture != null)
		{
			if (Shadowed)
			{
				SpriteMain.Draw2D(StringTexture, new Rectangle(0, 0, 512, 256), new SizeF(512f, 256f), new PointF((int)Position.X + 2 + OffsetX, (int)Position.Y + 2 + (int)((float)OffsetY * destinationSize.Height / 90f)), Color.FromArgb(150, 0, 0, 0));
			}
			SpriteMain.Draw2D(StringTexture, new Rectangle(0, 0, 512, 256), new SizeF(512f, 256f), new PointF((int)Position.X + OffsetX, (int)Position.Y + (int)((float)OffsetY * destinationSize.Height / 90f)), FontColor);
			if (StringTexture2 != null)
			{
				SpriteMain.Draw2D(StringTexture2, new Rectangle(0, 0, 512, 256), new SizeF(512f, 256f), new PointF((int)Position.X + OffsetX, (int)Position.Y + (int)((float)OffsetY * destinationSize.Height / 90f)), FontColor2);
			}
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
