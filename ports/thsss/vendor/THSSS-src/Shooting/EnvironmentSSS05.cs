using System;
using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

public class EnvironmentSSS05 : BaseObject, IEnvironment
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

	public EnvironmentSSS05(StageDataPackage StageData)
	{
		//IL_0034: Unknown result type (might be due to invalid IL or missing references)
		//IL_0039: Unknown result type (might be due to invalid IL or missing references)
		//IL_004e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0053: Unknown result type (might be due to invalid IL or missing references)
		//IL_0068: Unknown result type (might be due to invalid IL or missing references)
		//IL_006d: Unknown result type (might be due to invalid IL or missing references)
		//IL_008e: Unknown result type (might be due to invalid IL or missing references)
		base.StageData = StageData;
		DeviceMain = StageData.DeviceMain;
		Velocity = 1f;
		CameraPosition = new Vector3(400f, -460f, 210f);
		CameraTarget = new Vector3(400f, 0f, 0f);
		CameraDirection = new Vector3(0f, 0f, 1f);
		FogStart = 0f;
		FogEnd = 0f;
		light = default;
		light.Type = (LightType)3;
		new Background3DObject(StageData, "Stair", new PointF(0f, 0f), InsertAtFirst: false);
	}

	public override void Ctrl()
	{
		//IL_0507: Unknown result type (might be due to invalid IL or missing references)
		//IL_050c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0521: Unknown result type (might be due to invalid IL or missing references)
		//IL_0526: Unknown result type (might be due to invalid IL or missing references)
		//IL_053b: Unknown result type (might be due to invalid IL or missing references)
		//IL_0540: Unknown result type (might be due to invalid IL or missing references)
		//IL_0626: Unknown result type (might be due to invalid IL or missing references)
		//IL_063c: Unknown result type (might be due to invalid IL or missing references)
		//IL_065b: Unknown result type (might be due to invalid IL or missing references)
		Time++;
		Velocity += Accelerate;
		CameraPosition.Y += Velocity;
		CameraTarget.Y += Velocity;
		CameraPosition.Z += Velocity;
		CameraTarget.Z += Velocity;
		if (CameraPosition.Y > -100f)
		{
			CameraPosition.Y -= 150f;
			CameraTarget.Y -= 150f;
			CameraPosition.Z -= 150f;
			CameraTarget.Z -= 150f;
		}
		CameraPosition.X += 1f / 18f * (float)Math.Cos((float)Time / 180f);
		CameraTarget.X += 1f / 18f * (float)Math.Cos((float)Time / 180f);
		if (Time > 100 && Time < 5050)
		{
			FogEnd++;
			if (FogEnd > 700f)
			{
				FogEnd = 700f;
			}
		}
		if (TimeMain > 400 && TimeMain < 700)
		{
			CameraPosition.X--;
			CameraTarget.X--;
		}
		else if (TimeMain >= 700 && TimeMain < 800)
		{
			CameraPosition.X -= 0.5f;
			CameraTarget.X -= 0.5f;
		}
		else if (TimeMain >= 800 && TimeMain < 850)
		{
			CameraPosition.X -= 0.2f;
			CameraTarget.X -= 0.2f;
		}
		if (TimeMain > 200 && TimeMain < 300)
		{
			CameraPosition.Y += 0.5f;
			CameraPosition.Z -= 0.5f;
		}
		else if (TimeMain > 1100 && TimeMain < 1400)
		{
			CameraPosition.X += 0.1f;
			CameraTarget.X += 0.13f;
			CameraPosition.Z -= 0.1f;
			if (CameraDirection.X < 0.03f)
			{
				CameraDirection.X += 0.0001f;
			}
		}
		else if (TimeMain > 1800 && TimeMain < 2100)
		{
			if (CameraPosition.X > 30f)
			{
				CameraPosition.X -= 0.35f;
			}
			if (CameraTarget.X < 200f)
			{
				CameraTarget.X += 0.25f;
			}
			CameraPosition.Z -= 0.3f;
			CameraDirection.X -= 0.0001f;
		}
		if (TimeMain > 8200 && TimeMain < 8250)
		{
			BaseParticle3D baseParticle3D = new BaseParticle3D(StageData, "云（亮层）", new PointF(BoundRect.Width / 2 + Ran.Next(-20, 20), 240 + Ran.Next(-20, 20)), 0f, 0.0)
			{
				TransparentValueF = 0f,
				TransparentVelocity = 0.2f,
				Scale = 2f,
				ColorValue = Color.Gray
			};
			baseParticle3D.TransparentVelocityDictionary.Add(300, -0.2f);
			baseParticle3D.AngularVelocityDegree = (float)Ran.Next(-50, 50) / 20f;
			baseParticle3D.Depth = (float)(TimeMain - 8200) / 10f;
			baseParticle3D.LifeTime = 1500;
			Background3D.BackgroundParticleList.Add(baseParticle3D);
		}
		if (TimeMain == 8601)
		{
			Background3D.BackgroundList.Clear();
			new Background3DObject(StageData, "祭坛", new PointF(0f, 0f), InsertAtFirst: false).Scale = 10f;
			Velocity = 0f;
			CameraPosition = new Vector3(0f, -480f, 480f);
			CameraTarget = new Vector3(0f, 0f, 300f);
			CameraDirection = new Vector3(0f, 0f, 1f);
			FogEnd = 700f;
			FogStart = 200f;
		}
		if (TimeMain > 8600)
		{
			CameraPosition.X += 0.1f * (float)Math.Cos((float)Time / 180f);
			CameraTarget.X += 1f / 12f * (float)Math.Cos((float)Time / 180f);
			CameraPosition.Y -= 1f / 17f * (float)Math.Cos((float)Time / 180f);
			CameraTarget.Y -= 1f / 13f * (float)Math.Cos((float)Time / 180f);
		}
		LightColor = Color.White;
		light.Diffuse = Color4.op_Implicit(LightColor);
		light.Ambient = Color4.op_Implicit(LightColor);
		light.Direction = new Vector3(0f, 0f, -1f);
	}

	public void SetRenderPara()
	{
		//IL_000d: Unknown result type (might be due to invalid IL or missing references)
		//IL_0018: Unknown result type (might be due to invalid IL or missing references)
		//IL_01d2: Unknown result type (might be due to invalid IL or missing references)
		//IL_01e1: Unknown result type (might be due to invalid IL or missing references)
		//IL_01f0: Unknown result type (might be due to invalid IL or missing references)
		//IL_01ff: Unknown result type (might be due to invalid IL or missing references)
		//IL_020e: Unknown result type (might be due to invalid IL or missing references)
		//IL_021d: Unknown result type (might be due to invalid IL or missing references)
		//IL_022c: Unknown result type (might be due to invalid IL or missing references)
		//IL_023b: Unknown result type (might be due to invalid IL or missing references)
		//IL_024a: Unknown result type (might be due to invalid IL or missing references)
		//IL_0258: Unknown result type (might be due to invalid IL or missing references)
		//IL_025e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0264: Unknown result type (might be due to invalid IL or missing references)
		//IL_0269: Unknown result type (might be due to invalid IL or missing references)
		//IL_026e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0296: Unknown result type (might be due to invalid IL or missing references)
		//IL_029b: Unknown result type (might be due to invalid IL or missing references)
		//IL_02aa: Unknown result type (might be due to invalid IL or missing references)
		//IL_02b9: Unknown result type (might be due to invalid IL or missing references)
		//IL_02d2: Unknown result type (might be due to invalid IL or missing references)
		//IL_02e6: Unknown result type (might be due to invalid IL or missing references)
		//IL_02fa: Unknown result type (might be due to invalid IL or missing references)
		//IL_0308: Unknown result type (might be due to invalid IL or missing references)
		//IL_030d: Unknown result type (might be due to invalid IL or missing references)
		//IL_031f: Unknown result type (might be due to invalid IL or missing references)
		//IL_032d: Unknown result type (might be due to invalid IL or missing references)
		DeviceMain.Clear((ClearFlags)3, Color4.op_Implicit(FogColor), 1f, 0);
		SpriteMain.Begin((SpriteFlags)16);
		SpriteMain.Draw2D(StageData.TextureObjectDictionary["夜空底色"], 1f, 0f, new PointF(BoundRect.Left + 205, BoundRect.Top + 240), Color.White);
		SpriteMain.Draw2D(StageData.TextureObjectDictionary["星星1"], 1f, 0f, new PointF(BoundRect.Left + 205, BoundRect.Top + 240), Color.FromArgb((int)(128.0 + 127.0 * Math.Sin((double)((float)Time / 200f * 2f) * Math.PI)), Color.White));
		SpriteMain.Draw2D(StageData.TextureObjectDictionary["星星2"], 1f, 0f, new PointF(BoundRect.Left + 205, BoundRect.Top + 240), Color.FromArgb((int)(128.0 - 127.0 * Math.Sin((double)((float)Time / 200f * 2f) * Math.PI)), Color.White));
		SpriteMain.End();
		DeviceMain.SetRenderState((RenderState)7, true);
		DeviceMain.SetRenderState((RenderState)14, true);
		DeviceMain.SetRenderState((RenderState)22, true);
		DeviceMain.SetRenderState((RenderState)27, true);
		DeviceMain.SetRenderState<Blend>((RenderState)19, (Blend)5);
		DeviceMain.SetRenderState<Blend>((RenderState)20, (Blend)6);
		DeviceMain.SetTextureStageState(0, (TextureStage)5, (TextureArgument)2);
		DeviceMain.SetTextureStageState(0, (TextureStage)4, (TextureOperation)4);
		DeviceMain.SetTextureStageState(0, (TextureStage)6, (TextureArgument)0);
		DeviceMain.SetTransform((TransformState)2, Matrix.LookAtLH(CameraPosition, CameraTarget, CameraDirection));
		DeviceMain.SetTransform((TransformState)3, Matrix.PerspectiveFovLH((float)Math.PI / 4f, 1f, 1f, FogEnd + 400f));
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
