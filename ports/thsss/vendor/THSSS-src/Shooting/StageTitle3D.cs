using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

public class StageTitle3D : BaseParticle3D
{
	protected VertexBuffer vertexBuffer;

	protected VertexDeclaration vertexDecl;

	protected Material material;

	protected int Length = 40;

	public StageTitle3D(StageDataPackage StageData, string TextureName)
	{
		//IL_0074: Unknown result type (might be due to invalid IL or missing references)
		//IL_007e: Expected Obj, but got Unknown
		//IL_0096: Unknown result type (might be due to invalid IL or missing references)
		//IL_00a0: Expected Obj, but got Unknown
		//IL_00a6: Unknown result type (might be due to invalid IL or missing references)
		base.StageData = StageData;
		OriginalPosition = new PointF(BoundRect.Width / 2, BoundRect.Height / 2);
		if (TextureName != null)
		{
			TxtureObject = TextureObjectDictionary[TextureName];
		}
		StageData.Particle3D.ParticleList.Add(this);
		vertexDecl = new VertexDeclaration(DeviceMain, TexturedColoredVertex.VertexElements);
		vertexBuffer = new VertexBuffer(DeviceMain, 2 * Length * TexturedColoredVertex.SizeBytes, (Usage)8, (VertexFormat)0, (Pool)1);
		material = default;
		SetupVertex();
		LifeTime = 300;
		TransparentValueF = 0f;
		ColorValue = Color.White;
		Active = false;
	}

	public virtual void SetupVertex()
	{
		//IL_024e: Unknown result type (might be due to invalid IL or missing references)
		//IL_018d: Unknown result type (might be due to invalid IL or missing references)
		//IL_01a8: Unknown result type (might be due to invalid IL or missing references)
		//IL_01ed: Unknown result type (might be due to invalid IL or missing references)
		//IL_0208: Unknown result type (might be due to invalid IL or missing references)
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
			array[2 * i] = new TexturedColoredVertex(new Vector3(num, num3 + (num4 - num3) * (float)i / (float)Length, 0f), new Vector2(num5, num7 + (num8 - num7) * (float)i / (float)Length), Color.FromArgb(num9, ColorValue).ToArgb());
			array[2 * i + 1] = new TexturedColoredVertex(new Vector3(num2, num3 + (num4 - num3) * (float)i / (float)Length, 0f), new Vector2(num6, num7 + (num8 - num7) * (float)i / (float)Length), Color.FromArgb(num9, ColorValue).ToArgb());
		}
		val.WriteRange<TexturedColoredVertex>(array);
		vertexBuffer.Unlock();
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time < LifeTime / 3)
		{
			TransparentValueF += (float)MaxTransparent * 3f / (float)LifeTime;
		}
		else if (Time > LifeTime * 2 / 3)
		{
			TransparentValueF -= (float)MaxTransparent * 3f / (float)LifeTime;
		}
	}

	public override void Show()
	{
		//IL_0008: Unknown result type (might be due to invalid IL or missing references)
		//IL_001a: Unknown result type (might be due to invalid IL or missing references)
		//IL_0037: Unknown result type (might be due to invalid IL or missing references)
		//IL_005a: Unknown result type (might be due to invalid IL or missing references)
		//IL_0072: Unknown result type (might be due to invalid IL or missing references)
		//IL_00a0: Unknown result type (might be due to invalid IL or missing references)
		//IL_008f: Unknown result type (might be due to invalid IL or missing references)
		//IL_00bd: Unknown result type (might be due to invalid IL or missing references)
		//IL_00de: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ef: Unknown result type (might be due to invalid IL or missing references)
		//IL_010b: Unknown result type (might be due to invalid IL or missing references)
		//IL_0119: Unknown result type (might be due to invalid IL or missing references)
		//IL_012b: Unknown result type (might be due to invalid IL or missing references)
		//IL_0152: Unknown result type (might be due to invalid IL or missing references)
		DeviceMain.EnableLight(0, false);
		DeviceMain.SetRenderState((RenderState)137, false);
		DeviceMain.VertexDeclaration = vertexDecl;
		DeviceMain.VertexFormat = TexturedColoredVertex.Format;
		SetupVertex();
		DeviceMain.SetStreamSource(0, vertexBuffer, 0, TexturedColoredVertex.SizeBytes);
		DeviceMain.SetTexture(0, (BaseTexture)(object)TxtureObject.TXTure);
		SetupMatrices();
		if (Active)
		{
			DeviceMain.SetRenderState<Blend>((RenderState)20, (Blend)2);
		}
		else
		{
			DeviceMain.SetRenderState<Blend>((RenderState)20, (Blend)6);
		}
		material.Diffuse = new Color4(Color.FromArgb(TransparentValue, ColorValue));
		material.Ambient = new Color4(Color.FromArgb(TransparentValue, ColorValue));
		DeviceMain.Material = material;
		DeviceMain.DrawPrimitives((PrimitiveType)5, 0, 2 * Length - 2);
		DeviceMain.EnableLight(0, true);
		DeviceMain.SetRenderState((RenderState)137, true);
		DeviceMain.VertexDeclaration = StageData.Particle3D.vertexDecl;
		DeviceMain.VertexFormat = TexturedVertex.Format;
	}
}
