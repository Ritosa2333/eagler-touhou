using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

public class StageTitle3D2 : StageTitle3D
{
	public StageTitle3D2(StageDataPackage StageData, string TextureName)
		: base(StageData, TextureName)
	{
		OriginalPosition = new PointF(BoundRect.Width / 2, BoundRect.Height / 2 - 80);
	}

	public override void SetupVertex()
	{
		//IL_024d: Unknown result type (might be due to invalid IL or missing references)
		//IL_018c: Unknown result type (might be due to invalid IL or missing references)
		//IL_01a7: Unknown result type (might be due to invalid IL or missing references)
		//IL_01ec: Unknown result type (might be due to invalid IL or missing references)
		//IL_0207: Unknown result type (might be due to invalid IL or missing references)
		DataStream val = vertexBuffer.Lock(0, 0, (LockFlags)8192);
		float num = 0f - TxtureObject.RotatingCenter.X;
		float num2 = (float)TxtureObject.Width - TxtureObject.RotatingCenter.X;
		float num3 = 0f - TxtureObject.RotatingCenter.Y;
		float num4 = (float)TxtureObject.Height - TxtureObject.RotatingCenter.Y;
		float num5 = (float)TxtureObject.PosRect.X / (float)TxtureObject.SrcWidth;
		float num6 = (float)(TxtureObject.PosRect.X + TxtureObject.Width) / (float)TxtureObject.SrcWidth;
		float num7 = (float)TxtureObject.PosRect.Y / (float)TxtureObject.SrcHeight;
		float num8 = (float)(TxtureObject.PosRect.Y + TxtureObject.Height) / (float)TxtureObject.SrcHeight;
		TexturedColoredVertex[] array = new TexturedColoredVertex[2 * Length];
		for (int i = 0; i < Length; i++)
		{
			int num9 = -10 * i + Time * 3;
			if (num9 < 0)
			{
				num9 = 0;
			}
			else if (num9 > TransparentValue)
			{
				num9 = TransparentValue;
			}
			array[2 * i] = new TexturedColoredVertex(new Vector3(num + (num2 - num) * (float)i / (float)Length, num3, 0f), new Vector2(num5 + (num6 - num5) * (float)i / (float)Length, num7), Color.FromArgb(num9, ColorValue).ToArgb());
			array[2 * i + 1] = new TexturedColoredVertex(new Vector3(num + (num2 - num) * (float)i / (float)Length, num4, 0f), new Vector2(num5 + (num6 - num5) * (float)i / (float)Length, num8), Color.FromArgb(num9, ColorValue).ToArgb());
		}
		val.WriteRange<TexturedColoredVertex>(array);
		vertexBuffer.Unlock();
	}
}
