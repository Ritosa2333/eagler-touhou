using System.Drawing;

namespace Shooting;

public class StarPointLineG : BaseObject
{
	protected string type;

	public StarPointLineG(StageDataPackage StageData, PointF OriginalPosition)
	{
		base.StageData = StageData;
		base.OriginalPosition = OriginalPosition;
		TransparentValueF = 0f;
		TransparentVelocity = 10f;
		type = "C";
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (MyPlane.OriginalPosition.X < 100f && MyPlane.OriginalPosition.Y > (float)(BoundRect.Height - 140))
		{
			TransparentVelocity = -10f;
		}
		else
		{
			TransparentVelocity = 10f;
		}
		if (TransparentValue < 50)
		{
			TransparentValueF = 50f;
		}
		TxtureObject = TextureObjectDictionary["border" + type + "2"];
		if (MyPlane.LastColor == EnchantmentType.Green && Story == null)
		{
			DestPoint = new PointF(66f, 404f);
		}
		else
		{
			DestPoint = new PointF(-6f, 404f);
		}
		Velocity = 3f;
	}

	public override void Show()
	{
		if (TxtureObject != null)
		{
			SpriteMain.Draw2D(TextureObjectDictionary["border" + type + "1"], 1f, 0f, OriginalPosition, Color.FromArgb(TransparentValue, Color.White));
			int num = TxtureObject.Height * MyPlane.StarPoint / MyPlane.maxStarPoint;
			num = ((num <= 1) ? 1 : num);
			Size size = new Size(TxtureObject.Width, num);
			Rectangle srcRectangle = new Rectangle(new Point(TxtureObject.PosRect.X, TxtureObject.PosRect.Y + TxtureObject.Height - num), size);
			SpriteMain.Draw2D(TxtureObject.TXTure, srcRectangle, size, TxtureObject.RotatingCenter, 0f, new PointF(OriginalPosition.X, OriginalPosition.Y + (float)TxtureObject.Height - (float)num), Color.FromArgb(TransparentValue * 8 / 10, Color.White));
			SpriteMain.Draw2D(TextureObjectDictionary["border" + type + "0"], 1f, 0f, OriginalPosition, Color.FromArgb(TransparentValue, Color.White));
		}
	}
}
