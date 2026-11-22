using System;
using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

public class EnvironmentSSS04 : BaseObject, IEnvironment
{
	private Light light;

	private Color LightColor;

	private Color FogColor;

	private float FogStart;

	private float FogEnd;

	public Vector3 CameraPosition;

	public Vector3 CameraTarget;

	public Vector3 CameraDirection;

	private int PosY = -1000;

	private int PosY2 = -1000;

	private int PosY3 = -1000;

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

	public EnvironmentSSS04(StageDataPackage StageData)
	{
		//IL_0055: Unknown result type (might be due to invalid IL or missing references)
		//IL_005a: Unknown result type (might be due to invalid IL or missing references)
		//IL_006f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0074: Unknown result type (might be due to invalid IL or missing references)
		//IL_0089: Unknown result type (might be due to invalid IL or missing references)
		//IL_008e: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ba: Unknown result type (might be due to invalid IL or missing references)
		base.StageData = StageData;
		DeviceMain = StageData.DeviceMain;
		Velocity = 1f;
		CameraPosition = new Vector3(0f, 0f, 170f);
		CameraTarget = new Vector3(0f, 200f, 170f);
		CameraDirection = new Vector3(0f, 0f, 1f);
		FogStart = 0f;
		FogEnd = 200f;
		LightColor = Color.White;
		light = default;
		light.Type = (LightType)3;
	}

	public override void Ctrl()
	{
		//IL_0410: Unknown result type (might be due to invalid IL or missing references)
		//IL_0415: Unknown result type (might be due to invalid IL or missing references)
		//IL_042a: Unknown result type (might be due to invalid IL or missing references)
		//IL_042f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0444: Unknown result type (might be due to invalid IL or missing references)
		//IL_0449: Unknown result type (might be due to invalid IL or missing references)
		//IL_0703: Unknown result type (might be due to invalid IL or missing references)
		//IL_0719: Unknown result type (might be due to invalid IL or missing references)
		//IL_0738: Unknown result type (might be due to invalid IL or missing references)
		//IL_05da: Unknown result type (might be due to invalid IL or missing references)
		//IL_05df: Unknown result type (might be due to invalid IL or missing references)
		//IL_05f4: Unknown result type (might be due to invalid IL or missing references)
		//IL_05f9: Unknown result type (might be due to invalid IL or missing references)
		//IL_060e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0613: Unknown result type (might be due to invalid IL or missing references)
		Time++;
		if (TimeMain < 400)
		{
			if (Time > 50)
			{
				FogStart++;
				if (FogStart > 10f)
				{
					FogStart = 10f;
				}
				FogEnd += 20f;
				if (FogEnd > 900f)
				{
					FogEnd = 900f;
				}
			}
			Velocity += Accelerate;
			if (Velocity < 0.5f)
			{
				Velocity = 0.5f;
			}
			CameraPosition.Y += Velocity;
			CameraTarget.Y += Velocity;
			CameraPosition.X = 40f * (float)Math.Sin((float)Time / 360f);
			CameraTarget.X = 40f * (float)Math.Sin((float)(Time - 60) / 360f);
			while ((float)PosY3 < CameraPosition.Y + 4200f)
			{
				new Background3DObject(StageData, "柳树", new PointF(300f, PosY3), InsertAtFirst: true)
				{
					Angle3DX = -(float)Math.PI / 2f,
					Height = 400f
				};
				new Background3DObject(StageData, "柳树", new PointF(-300f, PosY3), InsertAtFirst: true)
				{
					Angle3DX = -(float)Math.PI / 2f,
					Height = 400f,
					Mirrored = true
				};
				new Background3DObject(StageData, "柳树", new PointF(300f, PosY3 + 200), InsertAtFirst: true)
				{
					Angle3DX = -(float)Math.PI / 2f,
					Height = 400f,
					Mirrored = true
				};
				new Background3DObject(StageData, "柳树", new PointF(-300f, PosY3 + 200), InsertAtFirst: true)
				{
					Angle3DX = -(float)Math.PI / 2f,
					Height = 400f
				};
				PosY3 += 400;
			}
			while ((float)PosY < CameraPosition.Y + 3072f)
			{
				new Background3DObject(StageData, "地砖", new PointF(0f, PosY), InsertAtFirst: true)
				{
					Angle3DX = -(float)Math.PI,
					Scale = 1f
				};
				PosY += 256;
			}
			while ((float)PosY2 < CameraPosition.Y + 1950f)
			{
				new Background3DObject(StageData, "草地", new PointF(0f, PosY2), InsertAtFirst: true)
				{
					Angle3DX = -(float)Math.PI,
					Scale = 5f
				};
				PosY2 += 1625;
			}
			for (int num = Background3D.BackgroundList.Count - 1; num >= 0; num--)
			{
				if (Background3D.BackgroundList[num].OriginalPosition.Y < CameraPosition.Y - 1000f)
				{
					Background3D.BackgroundList.RemoveAt(num);
				}
			}
		}
		else if (TimeMain > 4000 && CameraTarget.Y < 500f)
		{
			CameraTarget.Y++;
		}
		if (TimeMain == 150)
		{
			new BackgroundTransitionOut(StageData, 150).LifeTime = 400;
		}
		else if (TimeMain == 550)
		{
			new BackgroundTransitionIn(StageData)
			{
				Delay = 0,
				LifeTime = 300
			};
			CameraPosition = new Vector3(0f, 0f, 250f);
			CameraTarget = new Vector3(0f, 0f, 0f);
			CameraDirection = new Vector3(0f, 1f, 1f);
			Background3D.BackgroundList.Clear();
			new Background3DObject(StageData, "万花镜素材", new PointF(0f, 0f), InsertAtFirst: false).AngularVelocityDegree = 0.1f;
			new Background3DObject(StageData, "黑暗滤镜", new PointF(0f, 0f), InsertAtFirst: false)
			{
				Height = 10f,
				Scale = 1f,
				TransparentVelocityDictionary = { { 3350, -5f } }
			};
			FogStart = 0f;
			FogEnd = 2000000f;
		}
		else if (TimeMain == 4300)
		{
			BaseParticle3D bk = new BaseParticle3D(StageData, "极光素材", new PointF(BoundRect.Width / 2, 125f), 0f, 0.0)
			{
				TransparentValueF = 128f,
				Active = true,
				AngleDegree = 0.0,
				ColorValue = Color.Black
			};
			bk.CtrlCall = (string x) =>
			{
				if (bk.Time * 2 < 255)
				{
					bk.ColorValue = Color.FromArgb(bk.Time * 2, bk.Time * 2, bk.Time * 2);
				}
				bk.TransparentValueF = 128f + 48f * (float)Math.Sin((double)((float)bk.Time / 200f) * Math.PI * 2.0);
			};
			Background3D.BackgroundParticleList.Add(bk);
		}
		if (TimeMain == 8701 && Background3D.BackgroundParticleList.Count == 0)
		{
			CameraPosition = new Vector3(0f, 0f, 250f);
			CameraTarget = new Vector3(0f, 500f, 0f);
			CameraDirection = new Vector3(0f, 1f, 1f);
			new Background3DObject(StageData, "万花镜素材", new PointF(0f, 0f), InsertAtFirst: false).AngularVelocityDegree = 0.1f;
			FogStart = 0f;
			FogEnd = 2000000f;
			BaseParticle3D bk2 = new BaseParticle3D(StageData, "极光素材", new PointF(BoundRect.Width / 2, 125f), 0f, 0.0)
			{
				TransparentValueF = 128f,
				Active = true,
				AngleDegree = 0.0,
				ColorValue = Color.Black
			};
			bk2.CtrlCall = (string x) =>
			{
				if (bk2.Time * 2 < 255)
				{
					bk2.ColorValue = Color.FromArgb(bk2.Time * 2, bk2.Time * 2, bk2.Time * 2);
				}
				bk2.TransparentValueF = 128f + 48f * (float)Math.Sin((double)((float)bk2.Time / 200f) * Math.PI * 2.0);
			};
			Background3D.BackgroundParticleList.Add(bk2);
		}
		light.Diffuse = Color4.op_Implicit(LightColor);
		light.Ambient = Color4.op_Implicit(LightColor);
		light.Direction = new Vector3(0f, 0f, -1f);
	}

	public void SetRenderPara()
	{
		//IL_000d: Unknown result type (might be due to invalid IL or missing references)
		//IL_0018: Unknown result type (might be due to invalid IL or missing references)
		//IL_00a9: Unknown result type (might be due to invalid IL or missing references)
		//IL_00b8: Unknown result type (might be due to invalid IL or missing references)
		//IL_00c7: Unknown result type (might be due to invalid IL or missing references)
		//IL_00d6: Unknown result type (might be due to invalid IL or missing references)
		//IL_00e5: Unknown result type (might be due to invalid IL or missing references)
		//IL_00f4: Unknown result type (might be due to invalid IL or missing references)
		//IL_0103: Unknown result type (might be due to invalid IL or missing references)
		//IL_0112: Unknown result type (might be due to invalid IL or missing references)
		//IL_0120: Unknown result type (might be due to invalid IL or missing references)
		//IL_0126: Unknown result type (might be due to invalid IL or missing references)
		//IL_012c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0131: Unknown result type (might be due to invalid IL or missing references)
		//IL_0136: Unknown result type (might be due to invalid IL or missing references)
		//IL_015e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0163: Unknown result type (might be due to invalid IL or missing references)
		//IL_0172: Unknown result type (might be due to invalid IL or missing references)
		//IL_0181: Unknown result type (might be due to invalid IL or missing references)
		//IL_019a: Unknown result type (might be due to invalid IL or missing references)
		//IL_01ae: Unknown result type (might be due to invalid IL or missing references)
		//IL_01c2: Unknown result type (might be due to invalid IL or missing references)
		//IL_01d0: Unknown result type (might be due to invalid IL or missing references)
		//IL_01d5: Unknown result type (might be due to invalid IL or missing references)
		//IL_01e7: Unknown result type (might be due to invalid IL or missing references)
		//IL_01f5: Unknown result type (might be due to invalid IL or missing references)
		DeviceMain.Clear((ClearFlags)3, Color4.op_Implicit(FogColor), 1f, 0);
		SpriteMain.Begin((SpriteFlags)16);
		if (TimeMain < 400)
		{
			SpriteMain.Draw2D(StageData.TextureObjectDictionary["BG01a"], 1f, 0f, new PointF(BoundRect.Left + 256, BoundRect.Top + 256), Color.White);
		}
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
