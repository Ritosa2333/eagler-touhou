using System;
using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

public class Model_Circle : Model, IModel
{
	private int Length = 64;

	private int Width = 16;

	private int Radius = 100;

	public Model_Circle(Device DeviceMain, TextureObject TxtureObject, string Name)
	{
		//IL_0050: Unknown result type (might be due to invalid IL or missing references)
		//IL_005a: Expected Obj, but got Unknown
		//IL_006f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0079: Expected Obj, but got Unknown
		//IL_007f: Unknown result type (might be due to invalid IL or missing references)
		TransparentValue = 255;
		ColorValue = Color.White;
		base.DeviceMain = DeviceMain;
		base.TxtureObject = TxtureObject;
		base.Name = Name;
		vertexDecl = new VertexDeclaration(DeviceMain, TexturedVertex.VertexElements);
		vertexBuffer = new VertexBuffer(DeviceMain, 2 * (Length + 1) * TexturedVertex.SizeBytes, (Usage)8, (VertexFormat)0, (Pool)1);
		material = default;
		SetupVertex();
	}

	public override void SetupVertex()
	{
		//IL_00cc: Unknown result type (might be due to invalid IL or missing references)
		//IL_00d4: Unknown result type (might be due to invalid IL or missing references)
		//IL_0118: Unknown result type (might be due to invalid IL or missing references)
		//IL_0120: Unknown result type (might be due to invalid IL or missing references)
		//IL_0160: Unknown result type (might be due to invalid IL or missing references)
		float num = (float)TxtureObject.PosRect.X / (float)TxtureObject.SrcWidth;
		float num2 = (float)(TxtureObject.PosRect.X + TxtureObject.Width) / (float)TxtureObject.SrcWidth;
		TexturedVertex[] array = new TexturedVertex[2 * (Length + 1)];
		for (int i = 0; i <= Length; i++)
		{
			double num3 = (double)(i * 2) * Math.PI / (double)Length;
			int num4 = Radius - Width / 2;
			float num5 = (float)(i % (Length / 3)) / (float)(Length / 3);
			array[2 * i] = new TexturedVertex(new Vector3((float)num4 * (float)Math.Cos(num3), (float)num4 * (float)Math.Sin(num3), 0f), new Vector2(num, num5));
			num4 = Radius + Width / 2;
			array[2 * i + 1] = new TexturedVertex(new Vector3((float)num4 * (float)Math.Cos(num3), (float)num4 * (float)Math.Sin(num3), 0f), new Vector2(num2, num5));
		}
		vertexBuffer.Lock(0, 0, (LockFlags)8192).WriteRange<TexturedVertex>(array);
		vertexBuffer.Unlock();
	}

	public override void Draw()
	{
		//IL_0016: Unknown result type (might be due to invalid IL or missing references)
		SetupMaterial();
		DeviceMain.DrawPrimitives((PrimitiveType)5, 0, 2 * Length);
	}
}
