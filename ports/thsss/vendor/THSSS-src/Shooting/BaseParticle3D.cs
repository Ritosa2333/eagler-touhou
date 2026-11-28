using System;
using System.Drawing;
using System.Runtime.CompilerServices;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

public class BaseParticle3D : BaseObject
{
	public IModel model { get; set; }

	public Device DeviceMain => StageData.DeviceMain;

	public float Depth { get; set; }

	public float MaxDepth { get; set; }

	public double Direction3D { get; set; }

	public Vector3 RotatingAxis
	{
		[CompilerGenerated]
		get
		{
			//IL_0001: Unknown result type (might be due to invalid IL or missing references)
			return field;
		}
		[CompilerGenerated]
		set
		{
			//IL_0001: Unknown result type (might be due to invalid IL or missing references)
			//IL_0002: Unknown result type (might be due to invalid IL or missing references)
			field = value;
		}
	}

	public float Angle3D { get; set; }

	public float AngularVelocity3D { get; set; }

	public BaseParticle3D(StageDataPackage StageData, string modelName, PointF OriginalPosition, float Velocity, double Direction)
		: base(StageData, modelName, OriginalPosition, Velocity, Direction)
	{
		base.OriginalPosition = OriginalPosition;
		model = StageData.ModelDictionary[modelName];
		MaxDepth = 80f;
	}

	public BaseParticle3D()
	{
	}

	public override void Move()
	{
		if (DestPoint != new PointF(0f, 0f))
		{
			_ = DestPoint;
			MovePoint();
		}
		float num = Velocity * (float)Math.Cos(Direction3D);
		Position = new PointF((float)((double)Position.X + (double)num * Math.Cos(Direction)), (float)((double)Position.Y + (double)num * Math.Sin(Direction)));
		Depth += Velocity * (float)Math.Sin(Direction3D);
		if (Depth > MaxDepth)
		{
			Depth = MaxDepth;
		}
	}

	public override void Ctrl()
	{
		base.Ctrl();
		Angle3D += AngularVelocity3D;
		if (Velocity < 0f)
		{
			Velocity = 0f;
		}
	}

	public virtual void SetupMatrices()
	{
		//IL_0000: Unknown result type (might be due to invalid IL or missing references)
		//IL_0005: Unknown result type (might be due to invalid IL or missing references)
		//IL_001f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0032: Unknown result type (might be due to invalid IL or missing references)
		//IL_0037: Unknown result type (might be due to invalid IL or missing references)
		//IL_003c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0041: Unknown result type (might be due to invalid IL or missing references)
		//IL_0042: Unknown result type (might be due to invalid IL or missing references)
		//IL_004a: Unknown result type (might be due to invalid IL or missing references)
		//IL_004f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0054: Unknown result type (might be due to invalid IL or missing references)
		//IL_0055: Unknown result type (might be due to invalid IL or missing references)
		//IL_0057: Unknown result type (might be due to invalid IL or missing references)
		//IL_000e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0014: Unknown result type (might be due to invalid IL or missing references)
		//IL_0019: Unknown result type (might be due to invalid IL or missing references)
		//IL_001e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0073: Unknown result type (might be due to invalid IL or missing references)
		//IL_0078: Unknown result type (might be due to invalid IL or missing references)
		//IL_007d: Unknown result type (might be due to invalid IL or missing references)
		//IL_007e: Unknown result type (might be due to invalid IL or missing references)
		//IL_00c9: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ce: Unknown result type (might be due to invalid IL or missing references)
		//IL_00d3: Unknown result type (might be due to invalid IL or missing references)
		//IL_00df: Unknown result type (might be due to invalid IL or missing references)
		//IL_00e0: Unknown result type (might be due to invalid IL or missing references)
		Matrix val = Matrix.Identity;
		if (Mirrored)
		{
			val *= Matrix.RotationY((float)Math.PI);
		}
		val *= Matrix.Scaling(new Vector3(Scale, Scale, Scale));
		val *= Matrix.RotationZ((float)Angle);
		val *= Matrix.RotationAxis(RotatingAxis, Mirrored ? (0f - Angle3D) : Angle3D);
		val *= Matrix.Translation(OriginalPosition.X - (float)(StageData.BoundRect.Width / 2), OriginalPosition.Y - (float)(StageData.BoundRect.Height / 2), Depth);
		DeviceMain.SetTransform((TransformState)256, val);
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
		model.Draw();
	}

	public override void Dispose()
	{
	}
}
