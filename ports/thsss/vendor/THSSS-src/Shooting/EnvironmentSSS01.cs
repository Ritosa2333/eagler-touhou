using System;
using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

public class EnvironmentSSS01 : BaseObject, IEnvironment
{
	private Light light;

	private Color LightColor;

	private Color FogColor;

	private float FogStart;

	private float FogEnd;

	public Vector3 CameraPosition;

	public Vector3 CameraTarget;

	public Vector3 CameraDirection;

	public Device DeviceMain { get; set; }

	public bool OnBossAttack { get; set; }

	public Vector3 CameraPos
	{
		get
		{
			//IL_0001: Unknown result type (might be due to invalid IL or missing references)
			return CameraPosition;
		}
	}

	public float CameraAngle => (float)Math.Atan((CameraTarget.Z - CameraPosition.Z) / (CameraTarget.Y - CameraPosition.Y));

	public EnvironmentSSS01(StageDataPackage StageData)
	{
		//IL_0034: Unknown result type (might be due to invalid IL or missing references)
		//IL_0039: Unknown result type (might be due to invalid IL or missing references)
		//IL_004e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0053: Unknown result type (might be due to invalid IL or missing references)
		//IL_007f: Unknown result type (might be due to invalid IL or missing references)
		base.StageData = StageData;
		DeviceMain = StageData.DeviceMain;
		Velocity = 2f;
		CameraPosition = new Vector3(0f, 2200f, 130f);
		CameraTarget = new Vector3(0f, 2400f, 50f);
		FogStart = 0f;
		FogEnd = 50f;
		LightColor = Color.White;
		light = default;
		light.Type = (LightType)3;
	}

	public override void Ctrl()
	{
		//IL_0087: Unknown result type (might be due to invalid IL or missing references)
		//IL_028f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0294: Unknown result type (might be due to invalid IL or missing references)
		//IL_0412: Unknown result type (might be due to invalid IL or missing references)
		//IL_0428: Unknown result type (might be due to invalid IL or missing references)
		//IL_0447: Unknown result type (might be due to invalid IL or missing references)
		Time++;
		Velocity += Accelerate;
		if (Time % 2048 == 0)
		{
			Background3D.BackgroundList.ForEach((BaseObject x) =>
			{
				if (x is Background3DObject && ((Background3DObject)x).model.Name != "591-1")
				{
					x.OriginalPosition = new PointF(x.OriginalPosition.X - 1024f, x.OriginalPosition.Y);
				}
			});
		}
		if (Time % 30 == 0)
		{
			new Background3DObject(OriginalPosition: new PointF(Ran.Next(-200, 200), CameraPos.Y + (float)Ran.Next(100, 500)), StageData: StageData, modelName: "591-1", InsertAtFirst: false)
			{
				Scale = (float)Ran.Next(40, 60) / 100f,
				ScaleVelocity = -0.004f,
				Velocity = (float)Ran.Next(-10, 10) / 20f,
				DirectionDegree = Ran.Next(360),
				VelocityZ = (float)Ran.Next(6, 15) / 20f,
				LifeTime = 200,
				Active = true,
				Angle3DX = -(float)Math.PI / 2f + StageData.Background3D.Envi.CameraAngle,
				TransparentValueF = 0f,
				TransparentVelocity = 10f
			};
		}
		if (6000 == TimeMain)
		{
			OnBossAttack = true;
		}
		CameraPosition.Y += Velocity;
		CameraTarget.Y += Velocity;
		if (CameraPosition.Y > 3500f)
		{
			CameraPosition.Y -= 2000f;
			CameraTarget.Y -= 2000f;
			Background3D.BackgroundList.ForEach((BaseObject x) =>
			{
				if (x is Background3DObject && ((Background3DObject)x).model.Name == "591-1")
				{
					x.OriginalPosition = new PointF(x.OriginalPosition.X, x.OriginalPosition.Y - 2000f);
				}
			});
		}
		CameraPosition.X = 40f * (float)Math.Sin((float)Time / 180f);
		CameraTarget.X = 40f * (float)Math.Sin((float)Time / 180f);
		CameraDirection = new Vector3(0.02f * (float)Math.Sin((float)Time / 90f), 1f, 0f);
		if (OnBossAttack)
		{
			if (CameraPosition.Z < 430f)
			{
				CameraPosition.Z++;
				CameraTarget.Z += 1.05f;
			}
			Background3D.BackgroundList.ForEach((BaseObject x) =>
			{
				if (x is Background3DObject)
				{
					x.TransparentValueF -= 0.5f;
				}
			});
		}
		if (!OnBossAttack)
		{
			if (Time <= 980)
			{
				FogEnd += 2f;
				if (FogEnd > 400f)
				{
					FogEnd = 400f;
				}
			}
			if (Time > 980)
			{
				FogStart++;
				if (FogStart > 130f)
				{
					FogStart = 130f;
				}
				FogEnd++;
				if (FogEnd > 510f)
				{
					FogEnd = 510f;
				}
			}
		}
		else
		{
			FogStart++;
			if (FogStart > 230f)
			{
				FogStart = 230f;
			}
			FogEnd++;
			if (FogEnd > 1010f)
			{
				FogEnd = 1010f;
			}
		}
		light.Diffuse = Color4.op_Implicit(LightColor);
		light.Ambient = Color4.op_Implicit(LightColor);
		light.Direction = new Vector3(0f, 0f, -1f);
	}

	public void SetRenderPara()
	{
		//IL_000d: Unknown result type (might be due to invalid IL or missing references)
		//IL_0018: Unknown result type (might be due to invalid IL or missing references)
		//IL_009c: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ab: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ba: Unknown result type (might be due to invalid IL or missing references)
		//IL_00c9: Unknown result type (might be due to invalid IL or missing references)
		//IL_00d8: Unknown result type (might be due to invalid IL or missing references)
		//IL_00e7: Unknown result type (might be due to invalid IL or missing references)
		//IL_00f6: Unknown result type (might be due to invalid IL or missing references)
		//IL_0105: Unknown result type (might be due to invalid IL or missing references)
		//IL_0113: Unknown result type (might be due to invalid IL or missing references)
		//IL_0119: Unknown result type (might be due to invalid IL or missing references)
		//IL_011f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0124: Unknown result type (might be due to invalid IL or missing references)
		//IL_0129: Unknown result type (might be due to invalid IL or missing references)
		//IL_0151: Unknown result type (might be due to invalid IL or missing references)
		//IL_0156: Unknown result type (might be due to invalid IL or missing references)
		//IL_0165: Unknown result type (might be due to invalid IL or missing references)
		//IL_0174: Unknown result type (might be due to invalid IL or missing references)
		//IL_018d: Unknown result type (might be due to invalid IL or missing references)
		//IL_01a1: Unknown result type (might be due to invalid IL or missing references)
		//IL_01b5: Unknown result type (might be due to invalid IL or missing references)
		//IL_01c3: Unknown result type (might be due to invalid IL or missing references)
		//IL_01c8: Unknown result type (might be due to invalid IL or missing references)
		//IL_01da: Unknown result type (might be due to invalid IL or missing references)
		//IL_01e8: Unknown result type (might be due to invalid IL or missing references)
		DeviceMain.Clear((ClearFlags)1, Color4.op_Implicit(FogColor), 0f, 0);
		SpriteMain.Begin((SpriteFlags)16);
		SpriteMain.Draw2D(StageData.TextureObjectDictionary["BG01a"], 1f, 0f, new PointF(BoundRect.Left + 256, BoundRect.Top + 256), Color.White);
		SpriteMain.End();
		DeviceMain.SetRenderState((RenderState)7, false);
		DeviceMain.SetRenderState((RenderState)22, true);
		DeviceMain.SetRenderState((RenderState)27, true);
		DeviceMain.SetRenderState<Blend>((RenderState)19, (Blend)5);
		DeviceMain.SetRenderState<Blend>((RenderState)20, (Blend)6);
		DeviceMain.SetTextureStageState(0, (TextureStage)5, (TextureArgument)2);
		DeviceMain.SetTextureStageState(0, (TextureStage)4, (TextureOperation)4);
		DeviceMain.SetTextureStageState(0, (TextureStage)6, (TextureArgument)0);
		DeviceMain.SetTransform((TransformState)2, Matrix.LookAtLH(CameraPosition, CameraTarget, CameraDirection));
		DeviceMain.SetTransform((TransformState)3, Matrix.PerspectiveFovLH((float)Math.PI / 2f, 1f, 1f, FogEnd + 200f));
		DeviceMain.SetRenderState((RenderState)28, true);
		DeviceMain.SetRenderState<FogMode>((RenderState)35, (FogMode)3);
		DeviceMain.SetRenderState((RenderState)34, FogColor.ToArgb());
		DeviceMain.SetRenderState((RenderState)36, FogStart);
		DeviceMain.SetRenderState((RenderState)37, FogEnd);
		DeviceMain.SetLight(0, light);
		DeviceMain.SetRenderState((RenderState)137, true);
		DeviceMain.EnableLight(0, true);
	}
}
