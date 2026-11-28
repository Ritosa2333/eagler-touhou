using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

internal class BackgroundStage1_3D : BaseObject
{
	private VertexBuffer vertexBuffer;

	private VertexDeclaration vertexDecl;

	private float deltaY;

	public Device DeviceMain => StageData.DeviceMain;

	public BackgroundStage1_3D(StageDataPackage StageData)
		: base(StageData, "Background_Stage1", new PointF(0f, 0f), 0f, 0.0)
	{
		//IL_0046: Unknown result type (might be due to invalid IL or missing references)
		//IL_0050: Expected Obj, but got Unknown
		//IL_0061: Unknown result type (might be due to invalid IL or missing references)
		//IL_006b: Expected Obj, but got Unknown
		Background3D.BackgroundList.Add(this);
		vertexDecl = new VertexDeclaration(DeviceMain, TexturedVertex.VertexElements);
		vertexBuffer = new VertexBuffer(DeviceMain, 4 * TexturedVertex.SizeBytes, (Usage)8, (VertexFormat)0, (Pool)1);
		Velocity = 1f;
	}

	public override void Ctrl()
	{
		Time++;
		if (Time == 660)
		{
			Velocity = 5f;
		}
		deltaY += Velocity;
		if (deltaY > 1000f)
		{
			deltaY = 0f;
		}
	}

	public override bool OutBoundary()
	{
		return false;
	}

	private void SetupVertex()
	{
		//IL_0025: Unknown result type (might be due to invalid IL or missing references)
		//IL_0034: Unknown result type (might be due to invalid IL or missing references)
		//IL_0054: Unknown result type (might be due to invalid IL or missing references)
		//IL_0063: Unknown result type (might be due to invalid IL or missing references)
		//IL_0083: Unknown result type (might be due to invalid IL or missing references)
		//IL_0092: Unknown result type (might be due to invalid IL or missing references)
		//IL_00b2: Unknown result type (might be due to invalid IL or missing references)
		//IL_00c1: Unknown result type (might be due to invalid IL or missing references)
		//IL_00db: Unknown result type (might be due to invalid IL or missing references)
		vertexBuffer.Lock(0, 0, (LockFlags)0).WriteRange<TexturedVertex>(new TexturedVertex[4]
		{
			new TexturedVertex(new Vector3(-1000f, -4000f, 0f), new Vector2(0f, 4f)),
			new TexturedVertex(new Vector3(1000f, -4000f, 0f), new Vector2(1f, 4f)),
			new TexturedVertex(new Vector3(-1000f, 0f, 0f), new Vector2(0f, 0f)),
			new TexturedVertex(new Vector3(1000f, 0f, 0f), new Vector2(1f, 0f))
		});
		vertexBuffer.Unlock();
	}

	private void SetupMatrices()
	{
		//IL_000b: Unknown result type (might be due to invalid IL or missing references)
		//IL_0010: Unknown result type (might be due to invalid IL or missing references)
		//IL_0033: Unknown result type (might be due to invalid IL or missing references)
		//IL_004e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0062: Unknown result type (might be due to invalid IL or missing references)
		//IL_0067: Unknown result type (might be due to invalid IL or missing references)
		//IL_006c: Unknown result type (might be due to invalid IL or missing references)
		DeviceMain.SetTransform((TransformState)256, Matrix.Identity);
		DeviceMain.SetTransform((TransformState)2, Matrix.LookAtLH(new Vector3(0f, -1400f - deltaY, 150f), new Vector3(0f, -1500f - deltaY, 0f), new Vector3(0f, -1f, 0f)));
	}

	public override void Show()
	{
		//IL_001e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0037: Unknown result type (might be due to invalid IL or missing references)
		//IL_0046: Unknown result type (might be due to invalid IL or missing references)
		SetupVertex();
		SetupMatrices();
		DeviceMain.SetTexture(0, (BaseTexture)(object)TxtureObject.TXTure);
		DeviceMain.SetStreamSource(0, vertexBuffer, 0, TexturedVertex.SizeBytes);
		DeviceMain.DrawPrimitives((PrimitiveType)5, 0, 2);
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
