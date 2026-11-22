using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

internal class BackgroundTouhouStage01B_3D : BaseBackgroundTouhouStage_3D
{
	public int Height { get; set; }

	public BackgroundTouhouStage01B_3D(StageDataPackage StageData, string textureName)
		: base(StageData, textureName)
	{
		//IL_0019: Unknown result type (might be due to invalid IL or missing references)
		//IL_0023: Expected Obj, but got Unknown
		vertexBuffer = new VertexBuffer(DeviceMain, 6 * TexturedVertex.SizeBytes, (Usage)8, (VertexFormat)0, (Pool)1);
	}

	public override void SetupVertex()
	{
		//IL_0029: Unknown result type (might be due to invalid IL or missing references)
		//IL_0038: Unknown result type (might be due to invalid IL or missing references)
		//IL_0058: Unknown result type (might be due to invalid IL or missing references)
		//IL_0067: Unknown result type (might be due to invalid IL or missing references)
		//IL_0087: Unknown result type (might be due to invalid IL or missing references)
		//IL_0096: Unknown result type (might be due to invalid IL or missing references)
		//IL_00b6: Unknown result type (might be due to invalid IL or missing references)
		//IL_00c5: Unknown result type (might be due to invalid IL or missing references)
		//IL_00e5: Unknown result type (might be due to invalid IL or missing references)
		//IL_00f4: Unknown result type (might be due to invalid IL or missing references)
		//IL_0114: Unknown result type (might be due to invalid IL or missing references)
		//IL_0123: Unknown result type (might be due to invalid IL or missing references)
		//IL_013d: Unknown result type (might be due to invalid IL or missing references)
		vertexBuffer.Lock(0, 0, (LockFlags)8192).WriteRange<TexturedVertex>(new TexturedVertex[6]
		{
			new TexturedVertex(new Vector3(-2000f, 0f, 0f), new Vector2(0f, 0f)),
			new TexturedVertex(new Vector3(-2000f, 1000f, 0f), new Vector2(0f, 1f)),
			new TexturedVertex(new Vector3(0f, 0f, 0f), new Vector2(1f, 0f)),
			new TexturedVertex(new Vector3(0f, 1000f, 0f), new Vector2(1f, 1f)),
			new TexturedVertex(new Vector3(2000f, 0f, 0f), new Vector2(0f, 0f)),
			new TexturedVertex(new Vector3(2000f, 1000f, 0f), new Vector2(0f, 1f))
		});
		vertexBuffer.Unlock();
	}

	public override void SetupMatrices()
	{
		//IL_0000: Unknown result type (might be due to invalid IL or missing references)
		//IL_0005: Unknown result type (might be due to invalid IL or missing references)
		//IL_0006: Unknown result type (might be due to invalid IL or missing references)
		//IL_002a: Unknown result type (might be due to invalid IL or missing references)
		//IL_002f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0034: Unknown result type (might be due to invalid IL or missing references)
		//IL_0040: Unknown result type (might be due to invalid IL or missing references)
		//IL_0041: Unknown result type (might be due to invalid IL or missing references)
		Matrix identity = Matrix.Identity;
		identity *= Matrix.Translation(OriginalPosition.X, OriginalPosition.Y, (float)Height);
		DeviceMain.SetTransform((TransformState)256, identity);
	}

	public override void Show()
	{
		//IL_000f: Unknown result type (might be due to invalid IL or missing references)
		base.Show();
		DeviceMain.DrawPrimitives((PrimitiveType)5, 0, 4);
	}
}
