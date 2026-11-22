using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

public class Model_Triangle : Model, IModel
{
	public Vector2 LeftTop;

	public Vector2 RightTop;

	public Vector2 LeftBottom;

	public int Width;

	public int Height;

	public Model_Triangle()
	{
	}

	public Model_Triangle(Device DeviceMain, TextureObject TxtureObject, string Name, Vector2 LeftTop, Vector2 RightTop, Vector2 LeftBottom, int Width, int Height)
	{
		//IL_0032: Unknown result type (might be due to invalid IL or missing references)
		//IL_0034: Unknown result type (might be due to invalid IL or missing references)
		//IL_003a: Unknown result type (might be due to invalid IL or missing references)
		//IL_003c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0042: Unknown result type (might be due to invalid IL or missing references)
		//IL_0044: Unknown result type (might be due to invalid IL or missing references)
		//IL_0060: Unknown result type (might be due to invalid IL or missing references)
		//IL_006a: Expected Obj, but got Unknown
		//IL_0076: Unknown result type (might be due to invalid IL or missing references)
		//IL_0080: Expected Obj, but got Unknown
		//IL_0086: Unknown result type (might be due to invalid IL or missing references)
		TransparentValue = 255;
		ColorValue = Color.White;
		base.DeviceMain = DeviceMain;
		base.TxtureObject = TxtureObject;
		base.Name = Name;
		this.LeftTop = LeftTop;
		this.RightTop = RightTop;
		this.LeftBottom = LeftBottom;
		this.Width = Width;
		this.Height = Height;
		vertexDecl = new VertexDeclaration(DeviceMain, TexturedVertex.VertexElements);
		vertexBuffer = new VertexBuffer(DeviceMain, 3 * TexturedVertex.SizeBytes, (Usage)8, (VertexFormat)0, (Pool)1);
		material = default;
		SetupVertex();
	}

	public override void SetupVertex()
	{
		//IL_0029: Unknown result type (might be due to invalid IL or missing references)
		//IL_002f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0051: Unknown result type (might be due to invalid IL or missing references)
		//IL_0057: Unknown result type (might be due to invalid IL or missing references)
		//IL_0079: Unknown result type (might be due to invalid IL or missing references)
		//IL_007f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0099: Unknown result type (might be due to invalid IL or missing references)
		vertexBuffer.Lock(0, 0, (LockFlags)8192).WriteRange<TexturedVertex>(new TexturedVertex[3]
		{
			new TexturedVertex(new Vector3(0f, 0f, 0f), LeftTop),
			new TexturedVertex(new Vector3((float)Width, 0f, 0f), RightTop),
			new TexturedVertex(new Vector3(0f, (float)Height, 0f), LeftBottom)
		});
		vertexBuffer.Unlock();
	}

	public override void SetModel()
	{
		//IL_0013: Unknown result type (might be due to invalid IL or missing references)
		//IL_002b: Unknown result type (might be due to invalid IL or missing references)
		DeviceMain.SetStreamSource(0, vertexBuffer, 0, TexturedVertex.SizeBytes);
		DeviceMain.SetTexture(0, (BaseTexture)(object)TxtureObject.TXTure);
	}

	public override void Draw()
	{
		//IL_000f: Unknown result type (might be due to invalid IL or missing references)
		SetupMaterial();
		DeviceMain.DrawPrimitives((PrimitiveType)4, 0, 1);
	}
}
