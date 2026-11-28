using System;
using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

public class EnvironmentSSS06 : BaseObject, IEnvironment
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

	public EnvironmentSSS06(StageDataPackage StageData)
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
		Velocity = 0.5f;
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
		//IL_01cd: Unknown result type (might be due to invalid IL or missing references)
		//IL_01d2: Unknown result type (might be due to invalid IL or missing references)
		//IL_01e7: Unknown result type (might be due to invalid IL or missing references)
		//IL_01ec: Unknown result type (might be due to invalid IL or missing references)
		//IL_0201: Unknown result type (might be due to invalid IL or missing references)
		//IL_0206: Unknown result type (might be due to invalid IL or missing references)
		//IL_0466: Unknown result type (might be due to invalid IL or missing references)
		//IL_0706: Unknown result type (might be due to invalid IL or missing references)
		//IL_071c: Unknown result type (might be due to invalid IL or missing references)
		//IL_073b: Unknown result type (might be due to invalid IL or missing references)
		Time++;
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
		if (TimeMain == 3)
		{
			BaseParticle3D baseParticle3D = new BaseParticle3D(StageData, "BG01a", new PointF(BoundRect.Width / 2, BoundRect.Height / 2), 0f, Math.PI / 2.0);
			baseParticle3D.TransparentVelocityDictionary.Add(500, -2f);
			Background3D.BackgroundParticleList.Add(baseParticle3D);
			new Background3DObject(StageData, "祭坛", new PointF(0f, 0f), InsertAtFirst: false).Scale = 10f;
			Velocity = 0f;
			CameraPosition = new Vector3(0f, -550f, 280f);
			CameraTarget = new Vector3(0f, 0f, 100f);
			CameraDirection = new Vector3(0f, 0f, 1f);
		}
		if (TimeMain > 0 && TimeMain < 800)
		{
			CameraTarget.Z += 1.5f;
			CameraPosition.Z += 1.5f;
		}
		else if (TimeMain == 800)
		{
			PosY2 -= 4000;
			Background3D.BackgroundList.Clear();
			while ((float)PosY2 < CameraPosition.Y + 6132f)
			{
				new Background3DObject(StageData, "云（亮层）", new PointF(0f, PosY2), InsertAtFirst: true)
				{
					Scale = 3f,
					Mirrored = false,
					OriginalPositionZ = CameraPosition.Z - 200f,
					Angle3DX = -(float)Math.PI,
					TransparentValueF = 0f,
					MaxTransparent = 250,
					TransparentVelocity = 2.5f,
					Velocity = 0f
				};
				Background3DObject bo = new Background3DObject(StageData, "Cloud01", new PointF(0f, PosY2 + 250), InsertAtFirst: false)
				{
					Scale = 3f,
					Mirrored = true,
					OriginalPositionZ = CameraPosition.Z - 200f + 20f,
					Angle3DX = -(float)Math.PI,
					TransparentValueF = 0f,
					MaxTransparent = 50,
					TransparentVelocity = 0.5f,
					Velocity = 0.5f,
					Direction = -Math.PI / 2.0
				};
				bo.CtrlCall = (string x) =>
				{
					if (bo.Time > 100)
					{
						bo.TransparentVelocity = -(float)Math.PI / 5f * (float)Math.Sin((double)((float)(Time - 100) / 300f) * Math.PI * 2.0);
					}
				};
				PosY2 += 1314;
			}
		}
		else if (TimeMain > 800)
		{
			if (TimeMain < 1000)
			{
				CameraPosition.Z++;
			}
			if (TimeMain < 8000 && Time % 10 == 0)
			{
				new Background3DObject(OriginalPosition: new PointF(Ran.Next(-400, 400), CameraPos.Y + (float)Ran.Next(100, 500)), StageData: StageData, modelName: "591-1", InsertAtFirst: false)
				{
					Scale = (float)Ran.Next(80, 100) / 100f,
					ScaleVelocity = -0.004f,
					Velocity = (float)Ran.Next(-10, 10) / 20f,
					DirectionDegree = Ran.Next(360),
					VelocityZ = (float)Ran.Next(6, 15) / 20f,
					LifeTime = 200,
					Active = true,
					Angle3DX = -(float)Math.PI / 2f + StageData.Background3D.Envi.CameraAngle,
					TransparentValueF = 0f,
					TransparentVelocity = 10f,
					OriginalPositionZ = CameraPosition.Z - 200f
				};
			}
			if (8000 < TimeMain && TimeMain < 8200)
			{
				CameraTarget.Z += 1.5f;
			}
		}
		if (TimeMain == 8200)
		{
			BaseParticle3D baseParticle3D2 = new BaseParticle3D(StageData, "夜空（暗）", new PointF(BoundRect.Width / 2, BoundRect.Height / 2), 0f, Math.PI / 2.0);
			baseParticle3D2.TransparentValueF = 0f;
			baseParticle3D2.TransparentVelocity = 3f;
			Background3D.BackgroundParticleList.Add(baseParticle3D2);
		}
		if (TimeMain == 8401 && Background3D.BackgroundParticleList.Count == 0)
		{
			BaseParticle3D item = new BaseParticle3D(StageData, "夜空（暗）", new PointF(BoundRect.Width / 2, BoundRect.Height / 2), 0f, Math.PI / 2.0);
			Background3D.BackgroundParticleList.Add(item);
		}
		for (int num = Background3D.BackgroundList.Count - 1; num >= 0; num--)
		{
			if (Background3D.BackgroundList[num].OriginalPosition.Y < CameraPosition.Y - 1000f)
			{
				Background3D.BackgroundList.RemoveAt(num);
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
		//IL_004a: Unknown result type (might be due to invalid IL or missing references)
		//IL_0059: Unknown result type (might be due to invalid IL or missing references)
		//IL_0068: Unknown result type (might be due to invalid IL or missing references)
		//IL_0077: Unknown result type (might be due to invalid IL or missing references)
		//IL_0086: Unknown result type (might be due to invalid IL or missing references)
		//IL_0095: Unknown result type (might be due to invalid IL or missing references)
		//IL_00a4: Unknown result type (might be due to invalid IL or missing references)
		//IL_00b3: Unknown result type (might be due to invalid IL or missing references)
		//IL_00c1: Unknown result type (might be due to invalid IL or missing references)
		//IL_00c7: Unknown result type (might be due to invalid IL or missing references)
		//IL_00cd: Unknown result type (might be due to invalid IL or missing references)
		//IL_00d2: Unknown result type (might be due to invalid IL or missing references)
		//IL_00d7: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ff: Unknown result type (might be due to invalid IL or missing references)
		//IL_0104: Unknown result type (might be due to invalid IL or missing references)
		//IL_0113: Unknown result type (might be due to invalid IL or missing references)
		//IL_0122: Unknown result type (might be due to invalid IL or missing references)
		//IL_013b: Unknown result type (might be due to invalid IL or missing references)
		//IL_014f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0163: Unknown result type (might be due to invalid IL or missing references)
		//IL_0171: Unknown result type (might be due to invalid IL or missing references)
		//IL_0176: Unknown result type (might be due to invalid IL or missing references)
		//IL_0188: Unknown result type (might be due to invalid IL or missing references)
		//IL_0196: Unknown result type (might be due to invalid IL or missing references)
		DeviceMain.Clear((ClearFlags)3, Color4.op_Implicit(FogColor), 1f, 0);
		SpriteMain.Begin((SpriteFlags)16);
		SpriteMain.End();
		DeviceMain.SetRenderState((RenderState)7, TimeMain < 800);
		DeviceMain.SetRenderState((RenderState)22, true);
		DeviceMain.SetRenderState((RenderState)27, true);
		DeviceMain.SetRenderState<Blend>((RenderState)19, (Blend)5);
		DeviceMain.SetRenderState<Blend>((RenderState)20, (Blend)6);
		DeviceMain.SetTextureStageState(0, (TextureStage)5, (TextureArgument)2);
		DeviceMain.SetTextureStageState(0, (TextureStage)4, (TextureOperation)4);
		DeviceMain.SetTextureStageState(0, (TextureStage)6, (TextureArgument)0);
		DeviceMain.SetTransform((TransformState)2, Matrix.LookAtLH(CameraPosition, CameraTarget, CameraDirection));
		DeviceMain.SetTransform((TransformState)3, Matrix.PerspectiveFovLH((float)Math.PI / 4f, 1f, 1f, FogEnd + 200f));
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
