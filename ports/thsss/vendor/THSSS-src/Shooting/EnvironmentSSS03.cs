using System;
using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

public class EnvironmentSSS03 : BaseObject, IEnvironment
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

	public EnvironmentSSS03(StageDataPackage StageData)
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
		//IL_0372: Unknown result type (might be due to invalid IL or missing references)
		//IL_0388: Unknown result type (might be due to invalid IL or missing references)
		//IL_03a7: Unknown result type (might be due to invalid IL or missing references)
		Time++;
		Velocity += Accelerate;
		if (Velocity < 0.5f)
		{
			Velocity = 0.5f;
		}
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
		CameraPosition.Y += Velocity;
		CameraTarget.Y += Velocity;
		CameraPosition.X = 40f * (float)Math.Sin((float)Time / 360f);
		CameraTarget.X = 40f * (float)Math.Sin((float)(Time - 60) / 360f);
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
