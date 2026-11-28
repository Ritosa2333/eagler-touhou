using System;
using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

internal class BackgroundTree : BaseObject
{
	public IModel model { get; set; }

	public float CameraAngle { get; set; }

	public int Height { get; set; }

	public Device DeviceMain => StageData.DeviceMain;

	public BackgroundTree(StageDataPackage StageData, string modelName, PointF OriginalPosition, bool InsertAtFirst)
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

	public override bool OutBoundary()
	{
		if (Time > LifeTime && LifeTime != 0)
		{
			return true;
		}
		return false;
	}

	private void SetupMatrices()
	{
		//IL_0021: Unknown result type (might be due to invalid IL or missing references)
		//IL_0026: Unknown result type (might be due to invalid IL or missing references)
		//IL_0027: Unknown result type (might be due to invalid IL or missing references)
		//IL_002f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0034: Unknown result type (might be due to invalid IL or missing references)
		//IL_0039: Unknown result type (might be due to invalid IL or missing references)
		//IL_003a: Unknown result type (might be due to invalid IL or missing references)
		//IL_004d: Unknown result type (might be due to invalid IL or missing references)
		//IL_0052: Unknown result type (might be due to invalid IL or missing references)
		//IL_0057: Unknown result type (might be due to invalid IL or missing references)
		//IL_005c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0076: Unknown result type (might be due to invalid IL or missing references)
		//IL_007d: Unknown result type (might be due to invalid IL or missing references)
		//IL_0082: Unknown result type (might be due to invalid IL or missing references)
		//IL_0087: Unknown result type (might be due to invalid IL or missing references)
		//IL_0088: Unknown result type (might be due to invalid IL or missing references)
		//IL_00df: Unknown result type (might be due to invalid IL or missing references)
		//IL_00e4: Unknown result type (might be due to invalid IL or missing references)
		//IL_00e9: Unknown result type (might be due to invalid IL or missing references)
		//IL_00f5: Unknown result type (might be due to invalid IL or missing references)
		//IL_00f6: Unknown result type (might be due to invalid IL or missing references)
		//IL_0065: Unknown result type (might be due to invalid IL or missing references)
		//IL_006b: Unknown result type (might be due to invalid IL or missing references)
		//IL_0070: Unknown result type (might be due to invalid IL or missing references)
		//IL_0075: Unknown result type (might be due to invalid IL or missing references)
		CameraAngle = -(float)Math.PI / 2f + StageData.Background3D.Envi.CameraAngle;
		Matrix identity = Matrix.Identity;
		identity *= Matrix.RotationZ((float)Angle);
		identity *= Matrix.Scaling(new Vector3(Scale, Scale, Scale));
		if (Mirrored)
		{
			identity *= Matrix.RotationY((float)Math.PI);
		}
		identity *= Matrix.RotationX(CameraAngle);
		identity *= Matrix.Translation(OriginalPosition.X, OriginalPosition.Y, (float)(model.TxtureObject.Height / 2 + Height) * Scale * (float)Math.Cos((double)(0f - CameraAngle) - Math.PI / 2.0));
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
