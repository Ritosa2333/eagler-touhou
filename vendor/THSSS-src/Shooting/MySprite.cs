using System;
using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

public class MySprite
{
	public Sprite sprite { get; set; }

	public MySprite(Device device)
	{
		//IL_0008: Unknown result type (might be due to invalid IL or missing references)
		//IL_0012: Expected Obj, but got Unknown
		sprite = new Sprite(device);
	}

	public void Begin(SpriteFlags flags)
	{
		//IL_0006: Unknown result type (might be due to invalid IL or missing references)
		//IL_0007: Unknown result type (might be due to invalid IL or missing references)
		sprite.Begin(flags);
	}

	public void End()
	{
		//IL_0006: Unknown result type (might be due to invalid IL or missing references)
		sprite.End();
	}

	public void Draw2D(Texture srcTexture, PointF rotationCenter, float rotationAngle, PointF position, Color color)
	{
		//IL_0015: Unknown result type (might be due to invalid IL or missing references)
		//IL_002f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0038: Unknown result type (might be due to invalid IL or missing references)
		//IL_004d: Unknown result type (might be due to invalid IL or missing references)
		//IL_0058: Unknown result type (might be due to invalid IL or missing references)
		//IL_0068: Unknown result type (might be due to invalid IL or missing references)
		//IL_006d: Unknown result type (might be due to invalid IL or missing references)
		//IL_0079: Unknown result type (might be due to invalid IL or missing references)
		//IL_007e: Unknown result type (might be due to invalid IL or missing references)
		//IL_008f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0095: Unknown result type (might be due to invalid IL or missing references)
		//IL_009b: Unknown result type (might be due to invalid IL or missing references)
		//IL_009c: Unknown result type (might be due to invalid IL or missing references)
		//IL_00a8: Unknown result type (might be due to invalid IL or missing references)
		Vector3 value = new Vector3(rotationCenter.X, rotationCenter.Y, 0f);
		Vector3 value2 = new Vector3(position.X, position.Y, 0f);
		Color4 val = new Color4(color);
		Vector2 val2 = new Vector2(position.X, position.Y);
		sprite.Transform = Matrix.Transformation2D(val2, 0f, new Vector2(1f, 1f), val2, rotationAngle, new Vector2(0f, 0f));
		sprite.Draw(srcTexture, (Nullable)value, (Nullable)value2, val);
		sprite.Transform = Matrix.Identity;
	}

	public void Draw2D(Texture srcTexture, Rectangle srcRectangle, SizeF destinationSize, PointF position, Color color)
	{
		//IL_0015: Unknown result type (might be due to invalid IL or missing references)
		//IL_001e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0033: Unknown result type (might be due to invalid IL or missing references)
		//IL_005a: Unknown result type (might be due to invalid IL or missing references)
		//IL_0065: Unknown result type (might be due to invalid IL or missing references)
		//IL_006b: Unknown result type (might be due to invalid IL or missing references)
		//IL_006c: Unknown result type (might be due to invalid IL or missing references)
		//IL_007c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0081: Unknown result type (might be due to invalid IL or missing references)
		//IL_00a7: Unknown result type (might be due to invalid IL or missing references)
		//IL_00b1: Unknown result type (might be due to invalid IL or missing references)
		//IL_00b7: Unknown result type (might be due to invalid IL or missing references)
		//IL_00b8: Unknown result type (might be due to invalid IL or missing references)
		//IL_00c4: Unknown result type (might be due to invalid IL or missing references)
		Vector3 value = new Vector3(position.X, position.Y, 0f);
		Color4 val = new Color4(color);
		Vector2 val2 = new Vector2(position.X, position.Y);
		Vector2 val3 = new Vector2(destinationSize.Width / (float)srcRectangle.Width, destinationSize.Height / (float)srcRectangle.Height);
		sprite.Transform = Matrix.Transformation2D(val2, 0f, val3, val2, 0f, new Vector2(0f, 0f));
		sprite.Draw(srcTexture, (Rectangle?)srcRectangle, (Nullable)new Vector3(0f, 0f, 0f), (Nullable)value, val);
		sprite.Transform = Matrix.Identity;
	}

	public void Draw2D(Texture srcTexture, Rectangle srcRectangle, SizeF destinationSize, PointF rotationCenter, float rotationAngle, PointF position, Color color)
	{
		//IL_0015: Unknown result type (might be due to invalid IL or missing references)
		//IL_002f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0038: Unknown result type (might be due to invalid IL or missing references)
		//IL_004b: Unknown result type (might be due to invalid IL or missing references)
		//IL_0060: Unknown result type (might be due to invalid IL or missing references)
		//IL_0065: Unknown result type (might be due to invalid IL or missing references)
		//IL_0066: Unknown result type (might be due to invalid IL or missing references)
		//IL_008e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0099: Unknown result type (might be due to invalid IL or missing references)
		//IL_009f: Unknown result type (might be due to invalid IL or missing references)
		//IL_00a1: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ae: Unknown result type (might be due to invalid IL or missing references)
		//IL_00b3: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ca: Unknown result type (might be due to invalid IL or missing references)
		//IL_00d0: Unknown result type (might be due to invalid IL or missing references)
		//IL_00d6: Unknown result type (might be due to invalid IL or missing references)
		//IL_00d7: Unknown result type (might be due to invalid IL or missing references)
		//IL_00e3: Unknown result type (might be due to invalid IL or missing references)
		Vector3 value = new Vector3(rotationCenter.X, rotationCenter.Y, 0f);
		Vector3 value2 = new Vector3(position.X, position.Y, 0f);
		Color4 val = new Color4(color);
		Vector2 val2 = new Vector2(rotationCenter.X, rotationCenter.Y);
		Vector2 val3 = new Vector2(position.X, position.Y);
		_ = val2 + val3;
		Vector2 val4 = new Vector2(destinationSize.Width / (float)srcRectangle.Width, destinationSize.Height / (float)srcRectangle.Height);
		sprite.Transform = Matrix.Transformation2D(val3, 0f, val4, val3, rotationAngle, new Vector2(0f, 0f));
		sprite.Draw(srcTexture, (Rectangle?)srcRectangle, (Nullable)value, (Nullable)value2, val);
		sprite.Transform = Matrix.Identity;
	}

	public void Draw2D(TextureObject txtureObject, float scale, float rotationAngle, PointF position, int transpanrent)
	{
		Draw2D(txtureObject, scale, rotationAngle, position, Color.FromArgb(transpanrent, Color.White));
	}

	public void Draw2D(TextureObject txtureObject, float scale, float rotationAngle, PointF position, Color color)
	{
		Draw2D(txtureObject, scale, rotationAngle, position, txtureObject.RotatingCenter, color);
	}

	public void Draw2D(TextureObject txtureObject, float scale, float rotationAngle, PointF position, PointF rotationCenter, Color color)
	{
		Draw2D(txtureObject, scale, rotationAngle, position, rotationCenter, color, mirrored: false);
	}

	public void Draw2D(TextureObject txtureObject, float scale, float rotationAngle, PointF position, PointF rotationCenter, Color color, bool mirrored)
	{
		Draw2D(txtureObject, scale, scale, rotationAngle, position, rotationCenter, color, mirrored);
	}

	public void Draw2D(TextureObject txtureObject, float scaleX, float scaleY, float rotationAngle, PointF position, Color color, bool mirrored)
	{
		Draw2D(txtureObject, scaleX, scaleY, rotationAngle, position, txtureObject.RotatingCenter, color, mirrored);
	}

	public void Draw2D(TextureObject txtureObject, float scaleX, float scaleY, float rotationAngle, PointF position, PointF rotationCenter, Color color, bool mirrored)
	{
		//IL_0015: Unknown result type (might be due to invalid IL or missing references)
		//IL_002f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0038: Unknown result type (might be due to invalid IL or missing references)
		//IL_004b: Unknown result type (might be due to invalid IL or missing references)
		//IL_0060: Unknown result type (might be due to invalid IL or missing references)
		//IL_0065: Unknown result type (might be due to invalid IL or missing references)
		//IL_0066: Unknown result type (might be due to invalid IL or missing references)
		//IL_007c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0072: Unknown result type (might be due to invalid IL or missing references)
		//IL_0081: Unknown result type (might be due to invalid IL or missing references)
		//IL_0089: Unknown result type (might be due to invalid IL or missing references)
		//IL_008f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0091: Unknown result type (might be due to invalid IL or missing references)
		//IL_009e: Unknown result type (might be due to invalid IL or missing references)
		//IL_00a3: Unknown result type (might be due to invalid IL or missing references)
		//IL_00c4: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ca: Unknown result type (might be due to invalid IL or missing references)
		//IL_00d0: Unknown result type (might be due to invalid IL or missing references)
		//IL_00d1: Unknown result type (might be due to invalid IL or missing references)
		//IL_00dd: Unknown result type (might be due to invalid IL or missing references)
		Vector3 value = new Vector3(rotationCenter.X, rotationCenter.Y, 0f);
		Vector3 value2 = new Vector3(position.X, position.Y, 0f);
		Color4 val = new Color4(color);
		Vector2 val2 = new Vector2(rotationCenter.X, rotationCenter.Y);
		Vector2 val3 = new Vector2(position.X, position.Y);
		_ = val2 + val3;
		Vector2 val4 = (mirrored ? new Vector2(0f - scaleX, scaleY) : new Vector2(scaleX, scaleY));
		sprite.Transform = Matrix.Transformation2D(val3, 0f, val4, val3, rotationAngle, new Vector2(0f, 0f));
		sprite.Draw(txtureObject.TXTure, (Rectangle?)txtureObject.PosRect, (Nullable)value, (Nullable)value2, val);
		sprite.Transform = Matrix.Identity;
	}

	public void Dispose()
	{
		((ComObject)sprite).Dispose();
	}
}
