using System;
using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

internal class Background3DObject : BaseObject
{
	public IModel model { get; set; }

	public float Angle3DX { get; set; }

	public float Angle3DY { get; set; }

	public float Height { get; set; }

	public float OriginalPositionZ { get; set; }

	public float VelocityZ { get; set; }

	public Device DeviceMain => StageData.DeviceMain;

	public Background3DObject(StageDataPackage StageData, string modelName, PointF OriginalPosition, bool InsertAtFirst)
		: base(StageData, null, new PointF(0f, 0f), 0f, 0.0)
	{
		base.OriginalPosition = OriginalPosition;
		if (modelName != null)
		{
			model = StageData.ModelDictionary[modelName];
		}
		if (InsertAtFirst)
		{
			Background3D.BackgroundList.Insert(0, this);
		}
		else
		{
			Background3D.BackgroundList.Add(this);
		}
	}

	public override void Ctrl()
	{
		base.Ctrl();
		OriginalPositionZ += VelocityZ;
	}

	public override bool OutBoundary()
	{
		if (Time > LifeTime && LifeTime > 0)
		{
			return true;
		}
		return false;
	}

	private void SetupMatrices()
	{
		//IL_0000: Unknown result type (might be due to invalid IL or missing references)
		//IL_0005: Unknown result type (might be due to invalid IL or missing references)
		//IL_0006: Unknown result type (might be due to invalid IL or missing references)
		//IL_000e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0013: Unknown result type (might be due to invalid IL or missing references)
		//IL_0018: Unknown result type (might be due to invalid IL or missing references)
		//IL_0019: Unknown result type (might be due to invalid IL or missing references)
		//IL_002c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0031: Unknown result type (might be due to invalid IL or missing references)
		//IL_0036: Unknown result type (might be due to invalid IL or missing references)
		//IL_003b: Unknown result type (might be due to invalid IL or missing references)
		//IL_0055: Unknown result type (might be due to invalid IL or missing references)
		//IL_005c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0061: Unknown result type (might be due to invalid IL or missing references)
		//IL_0066: Unknown result type (might be due to invalid IL or missing references)
		//IL_0067: Unknown result type (might be due to invalid IL or missing references)
		//IL_006e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0073: Unknown result type (might be due to invalid IL or missing references)
		//IL_0078: Unknown result type (might be due to invalid IL or missing references)
		//IL_0079: Unknown result type (might be due to invalid IL or missing references)
		//IL_00a3: Unknown result type (might be due to invalid IL or missing references)
		//IL_00a8: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ad: Unknown result type (might be due to invalid IL or missing references)
		//IL_00b9: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ba: Unknown result type (might be due to invalid IL or missing references)
		//IL_0044: Unknown result type (might be due to invalid IL or missing references)
		//IL_004a: Unknown result type (might be due to invalid IL or missing references)
		//IL_004f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0054: Unknown result type (might be due to invalid IL or missing references)
		Matrix identity = Matrix.Identity;
		identity *= Matrix.RotationZ((float)Angle);
		identity *= Matrix.Scaling(new Vector3(Scale, Scale, Scale));
		if (Mirrored)
		{
			identity *= Matrix.RotationY((float)Math.PI);
		}
		identity *= Matrix.RotationY(Angle3DY);
		identity *= Matrix.RotationX(Angle3DX);
		identity *= Matrix.Translation(OriginalPosition.X, OriginalPosition.Y, OriginalPositionZ + Height);
		DeviceMain.SetTransform((TransformState)256, identity);
	}

	public override void Show()
	{
		//IL_004a: Unknown result type (might be due to invalid IL or missing references)
		//IL_0039: Unknown result type (might be due to invalid IL or missing references)
		SetupMatrices();
		model.TransparentValue = TransparentValue;
		model.ColorValue = ColorValue;
		if (Active)
		{
			DeviceMain.SetRenderState<Blend>((RenderState)20, (Blend)2);
		}
		else
		{
			DeviceMain.SetRenderState<Blend>((RenderState)20, (Blend)6);
		}
		model.SetModel();
		model.Draw();
	}

	public override void Dispose()
	{
	}
}
