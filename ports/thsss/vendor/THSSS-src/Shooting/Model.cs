using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

public class Model : IModel
{
	public VertexBuffer vertexBuffer;

	public VertexDeclaration vertexDecl;

	public Material material;

	public string Name { get; set; }

	public TextureObject TxtureObject { get; set; }

	public Device DeviceMain { get; set; }

	public int TransparentValue { get; set; }

	public Color ColorValue { get; set; }

	public Model()
	{
	}

	public Model(Device DeviceMain, TextureObject TxtureObject, string Name)
	{
		//IL_0038: Unknown result type (might be due to invalid IL or missing references)
		//IL_0042: Expected Obj, but got Unknown
		//IL_004e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0058: Expected Obj, but got Unknown
		//IL_005e: Unknown result type (might be due to invalid IL or missing references)
		TransparentValue = 255;
		ColorValue = Color.White;
		this.DeviceMain = DeviceMain;
		this.TxtureObject = TxtureObject;
		this.Name = Name;
		vertexDecl = new VertexDeclaration(DeviceMain, TexturedVertex.VertexElements);
		vertexBuffer = new VertexBuffer(DeviceMain, 4 * TexturedVertex.SizeBytes, (Usage)8, (VertexFormat)0, (Pool)1);
		material = default;
		SetupVertex();
	}

	public virtual void SetupVertex()
	{
		//IL_0139: Unknown result type (might be due to invalid IL or missing references)
		//IL_0142: Unknown result type (might be due to invalid IL or missing references)
		//IL_015a: Unknown result type (might be due to invalid IL or missing references)
		//IL_0163: Unknown result type (might be due to invalid IL or missing references)
		//IL_017b: Unknown result type (might be due to invalid IL or missing references)
		//IL_0184: Unknown result type (might be due to invalid IL or missing references)
		//IL_019c: Unknown result type (might be due to invalid IL or missing references)
		//IL_01a5: Unknown result type (might be due to invalid IL or missing references)
		//IL_01bf: Unknown result type (might be due to invalid IL or missing references)
		DataStream val = vertexBuffer.Lock(0, 0, (LockFlags)8192);
		float num = 0f - TxtureObject.RotatingCenter.X;
		float num2 = (float)TxtureObject.Width - TxtureObject.RotatingCenter.X;
		float num3 = 0f - TxtureObject.RotatingCenter.Y;
		float num4 = (float)TxtureObject.Height - TxtureObject.RotatingCenter.Y;
		float num5 = (float)TxtureObject.PosRect.X / (float)TxtureObject.SrcWidth;
		float num6 = (float)(TxtureObject.PosRect.X + TxtureObject.Width) / (float)TxtureObject.SrcWidth;
		float num7 = (float)TxtureObject.PosRect.Y / (float)TxtureObject.SrcHeight;
		float num8 = (float)(TxtureObject.PosRect.Y + TxtureObject.Height) / (float)TxtureObject.SrcHeight;
		val.WriteRange<TexturedVertex>(new TexturedVertex[4]
		{
			new TexturedVertex(new Vector3(num, num3, 0f), new Vector2(num5, num7)),
			new TexturedVertex(new Vector3(num2, num3, 0f), new Vector2(num6, num7)),
			new TexturedVertex(new Vector3(num, num4, 0f), new Vector2(num5, num8)),
			new TexturedVertex(new Vector3(num2, num4, 0f), new Vector2(num6, num8))
		});
		vertexBuffer.Unlock();
	}

	public void SetupMaterial()
	{
		//IL_0017: Unknown result type (might be due to invalid IL or missing references)
		//IL_0038: Unknown result type (might be due to invalid IL or missing references)
		//IL_0049: Unknown result type (might be due to invalid IL or missing references)
		material.Diffuse = new Color4(Color.FromArgb(TransparentValue, ColorValue));
		material.Ambient = new Color4(Color.FromArgb(TransparentValue, ColorValue));
		DeviceMain.Material = material;
	}

	public virtual void SetModel()
	{
		//IL_0013: Unknown result type (might be due to invalid IL or missing references)
		//IL_002b: Unknown result type (might be due to invalid IL or missing references)
		DeviceMain.SetStreamSource(0, vertexBuffer, 0, TexturedVertex.SizeBytes);
		DeviceMain.SetTexture(0, (BaseTexture)(object)TxtureObject.TXTure);
	}

	public void SetVertexFormat()
	{
		//IL_0017: Unknown result type (might be due to invalid IL or missing references)
		DeviceMain.VertexDeclaration = vertexDecl;
		DeviceMain.VertexFormat = TexturedVertex.Format;
	}

	public virtual void Draw()
	{
		//IL_000f: Unknown result type (might be due to invalid IL or missing references)
		SetupMaterial();
		DeviceMain.DrawPrimitives((PrimitiveType)5, 0, 2);
	}

	public virtual void Dispose()
	{
		if (vertexBuffer != null)
		{
			((ComObject)vertexBuffer).Dispose();
			((ComObject)vertexDecl).Dispose();
		}
	}
}
