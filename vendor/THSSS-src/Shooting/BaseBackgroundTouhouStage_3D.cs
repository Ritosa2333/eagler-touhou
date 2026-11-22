using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

internal class BaseBackgroundTouhouStage_3D : BaseObject
{
	public VertexBuffer vertexBuffer;

	public VertexDeclaration vertexDecl;

	public Material material;

	public Device DeviceMain => StageData.DeviceMain;

	public BaseBackgroundTouhouStage_3D(StageDataPackage StageData, string textureName)
		: base(StageData, textureName, new PointF(0f, 0f), 0f, 0.0)
	{
		//IL_0057: Unknown result type (might be due to invalid IL or missing references)
		//IL_0061: Expected Obj, but got Unknown
		OriginalPosition = new PointF(0f, 0f);
		Background3D.BackgroundList.Add(this);
		vertexDecl = new VertexDeclaration(DeviceMain, TexturedVertex.VertexElements);
	}

	public void SetupMaterial()
	{
		//IL_0006: Unknown result type (might be due to invalid IL or missing references)
		//IL_0023: Unknown result type (might be due to invalid IL or missing references)
		//IL_0044: Unknown result type (might be due to invalid IL or missing references)
		material = default;
		material.Diffuse = new Color4(Color.FromArgb(TransparentValue, ColorValue));
		material.Ambient = new Color4(Color.FromArgb(TransparentValue, ColorValue));
	}

	public override bool OutBoundary()
	{
		return false;
	}

	public virtual void SetupVertex()
	{
	}

	public virtual void SetupMatrices()
	{
		//IL_000b: Unknown result type (might be due to invalid IL or missing references)
		//IL_0010: Unknown result type (might be due to invalid IL or missing references)
		DeviceMain.SetTransform((TransformState)256, Matrix.Identity);
	}

	public override void Show()
	{
		//IL_0019: Unknown result type (might be due to invalid IL or missing references)
		//IL_0035: Unknown result type (might be due to invalid IL or missing references)
		//IL_004e: Unknown result type (might be due to invalid IL or missing references)
		//IL_006b: Unknown result type (might be due to invalid IL or missing references)
		SetupVertex();
		SetupMaterial();
		SetupMatrices();
		DeviceMain.Material = material;
		DeviceMain.SetTexture(0, (BaseTexture)(object)TxtureObject.TXTure);
		DeviceMain.SetStreamSource(0, vertexBuffer, 0, TexturedVertex.SizeBytes);
		DeviceMain.VertexDeclaration = vertexDecl;
		DeviceMain.VertexFormat = TexturedVertex.Format;
	}

	public override void Dispose()
	{
		if (vertexBuffer != null)
		{
			((ComObject)vertexBuffer).Dispose();
			((ComObject)vertexDecl).Dispose();
		}
	}
}
