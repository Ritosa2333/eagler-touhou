using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

internal class BackgroundTouhouStage02_3D : BaseBackgroundTouhouStage_3D
{
	public BackgroundTouhouStage02_3D(StageDataPackage StageData)
		: base(StageData, " ")
	{
		//IL_001e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0028: Expected Obj, but got Unknown
		vertexBuffer = new VertexBuffer(DeviceMain, 14 * TexturedVertex.SizeBytes, (Usage)8, (VertexFormat)0, (Pool)1);
	}

	public override void SetupVertex()
	{
		//IL_002a: Unknown result type (might be due to invalid IL or missing references)
		//IL_0039: Unknown result type (might be due to invalid IL or missing references)
		//IL_0059: Unknown result type (might be due to invalid IL or missing references)
		//IL_0068: Unknown result type (might be due to invalid IL or missing references)
		//IL_0088: Unknown result type (might be due to invalid IL or missing references)
		//IL_0097: Unknown result type (might be due to invalid IL or missing references)
		//IL_00b7: Unknown result type (might be due to invalid IL or missing references)
		//IL_00c6: Unknown result type (might be due to invalid IL or missing references)
		//IL_00e6: Unknown result type (might be due to invalid IL or missing references)
		//IL_00f5: Unknown result type (might be due to invalid IL or missing references)
		//IL_0115: Unknown result type (might be due to invalid IL or missing references)
		//IL_0124: Unknown result type (might be due to invalid IL or missing references)
		//IL_0144: Unknown result type (might be due to invalid IL or missing references)
		//IL_0153: Unknown result type (might be due to invalid IL or missing references)
		//IL_0173: Unknown result type (might be due to invalid IL or missing references)
		//IL_0182: Unknown result type (might be due to invalid IL or missing references)
		//IL_01a2: Unknown result type (might be due to invalid IL or missing references)
		//IL_01b1: Unknown result type (might be due to invalid IL or missing references)
		//IL_01d2: Unknown result type (might be due to invalid IL or missing references)
		//IL_01e1: Unknown result type (might be due to invalid IL or missing references)
		//IL_0202: Unknown result type (might be due to invalid IL or missing references)
		//IL_0211: Unknown result type (might be due to invalid IL or missing references)
		//IL_0232: Unknown result type (might be due to invalid IL or missing references)
		//IL_0241: Unknown result type (might be due to invalid IL or missing references)
		//IL_0262: Unknown result type (might be due to invalid IL or missing references)
		//IL_0271: Unknown result type (might be due to invalid IL or missing references)
		//IL_0292: Unknown result type (might be due to invalid IL or missing references)
		//IL_02a1: Unknown result type (might be due to invalid IL or missing references)
		//IL_02bb: Unknown result type (might be due to invalid IL or missing references)
		vertexBuffer.Lock(0, 0, (LockFlags)8192).WriteRange<TexturedVertex>(new TexturedVertex[14]
		{
			new TexturedVertex(new Vector3(-500f, 6000f, 0f), new Vector2(0f, 0f)),
			new TexturedVertex(new Vector3(500f, 6000f, 0f), new Vector2(1f, 0f)),
			new TexturedVertex(new Vector3(-500f, 5000f, 0f), new Vector2(0f, 1f)),
			new TexturedVertex(new Vector3(500f, 5000f, 0f), new Vector2(1f, 1f)),
			new TexturedVertex(new Vector3(-500f, 4000f, 0f), new Vector2(0f, 0f)),
			new TexturedVertex(new Vector3(500f, 4000f, 0f), new Vector2(1f, 0f)),
			new TexturedVertex(new Vector3(-500f, 3000f, 0f), new Vector2(0f, 1f)),
			new TexturedVertex(new Vector3(500f, 3000f, 0f), new Vector2(1f, 1f)),
			new TexturedVertex(new Vector3(-500f, 2000f, 0f), new Vector2(0f, 0f)),
			new TexturedVertex(new Vector3(500f, 2000f, 0f), new Vector2(1f, 0f)),
			new TexturedVertex(new Vector3(-500f, 1000f, 0f), new Vector2(0f, 1f)),
			new TexturedVertex(new Vector3(500f, 1000f, 0f), new Vector2(1f, 1f)),
			new TexturedVertex(new Vector3(-500f, 0f, 0f), new Vector2(0f, 0f)),
			new TexturedVertex(new Vector3(500f, 0f, 0f), new Vector2(1f, 0f))
		});
		vertexBuffer.Unlock();
	}

	public override void Show()
	{
		//IL_0010: Unknown result type (might be due to invalid IL or missing references)
		base.Show();
		DeviceMain.DrawPrimitives((PrimitiveType)5, 0, 12);
	}
}
