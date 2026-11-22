using System;
using System.Collections.Generic;
using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

public class BackgroundManager3D
{
	private Light light;

	public bool Enabled3D { get; set; }

	public bool WarpEnabled { get; set; }

	public int WarpColorKey { get; set; }

	public float WarpRadius { get; set; }

	public List<BaseObject> BackgroundList { get; set; }

	public List<BaseParticle3D> BackgroundParticleList { get; set; }

	private VertexDeclaration vertexDecl { get; set; }

	public StageDataPackage StageData { get; private set; }

	public Device DeviceMain => StageData.DeviceMain;

	public IEnvironment Envi { get; set; }

	public BackgroundManager3D(StageDataPackage StageData)
	{
		//IL_0036: Unknown result type (might be due to invalid IL or missing references)
		//IL_0040: Expected Obj, but got Unknown
		BackgroundList = new List<BaseObject>();
		BackgroundParticleList = new List<BaseParticle3D>();
		this.StageData = StageData;
		Enabled3D = true;
		vertexDecl = new VertexDeclaration(DeviceMain, TexturedVertex.VertexElements);
		SetupLight();
	}

	private void SetupLight()
	{
		//IL_0006: Unknown result type (might be due to invalid IL or missing references)
		//IL_0023: Unknown result type (might be due to invalid IL or missing references)
		//IL_0038: Unknown result type (might be due to invalid IL or missing references)
		//IL_0057: Unknown result type (might be due to invalid IL or missing references)
		light = default;
		light.Type = (LightType)3;
		light.Diffuse = Color4.op_Implicit(Color.White);
		light.Ambient = Color4.op_Implicit(Color.White);
		light.Direction = new Vector3(0f, 0f, -1f);
	}

	public void Ctrl()
	{
		if (Envi != null)
		{
			Envi.Ctrl();
		}
		BackgroundList.ForEach((BaseObject x) =>
		{
			if (x.OutBoundary())
			{
				x.Dispose();
				BackgroundList.Remove(x);
			}
		});
		BackgroundList.ForEach((BaseObject x) =>
		{
			x.Ctrl();
		});
		BackgroundParticleList.ForEach((BaseParticle3D x) =>
		{
			if (x.OutBoundary())
			{
				x.Dispose();
				BackgroundParticleList.Remove(x);
			}
		});
		BackgroundParticleList.ForEach((BaseParticle3D x) =>
		{
			x.Ctrl();
		});
		BackgroundParticleList.Sort(new CompareDepth());
		if (WarpEnabled)
		{
			if (WarpRadius < 128f)
			{
				WarpRadius += 4f;
			}
		}
		else if (WarpRadius > 0f)
		{
			WarpRadius -= 4f;
		}
	}

	private void SetParticleRenderPara()
	{
		//IL_0017: Unknown result type (might be due to invalid IL or missing references)
		//IL_001c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0027: Unknown result type (might be due to invalid IL or missing references)
		//IL_003b: Unknown result type (might be due to invalid IL or missing references)
		//IL_004f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0054: Unknown result type (might be due to invalid IL or missing references)
		//IL_0059: Unknown result type (might be due to invalid IL or missing references)
		//IL_0071: Unknown result type (might be due to invalid IL or missing references)
		//IL_0076: Unknown result type (might be due to invalid IL or missing references)
		//IL_0085: Unknown result type (might be due to invalid IL or missing references)
		//IL_008a: Unknown result type (might be due to invalid IL or missing references)
		//IL_009e: Unknown result type (might be due to invalid IL or missing references)
		//IL_00a3: Unknown result type (might be due to invalid IL or missing references)
		//IL_00b1: Unknown result type (might be due to invalid IL or missing references)
		//IL_00c0: Unknown result type (might be due to invalid IL or missing references)
		//IL_00cf: Unknown result type (might be due to invalid IL or missing references)
		//IL_00de: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ed: Unknown result type (might be due to invalid IL or missing references)
		//IL_00fc: Unknown result type (might be due to invalid IL or missing references)
		//IL_010b: Unknown result type (might be due to invalid IL or missing references)
		//IL_011a: Unknown result type (might be due to invalid IL or missing references)
		//IL_0128: Unknown result type (might be due to invalid IL or missing references)
		//IL_012d: Unknown result type (might be due to invalid IL or missing references)
		//IL_013f: Unknown result type (might be due to invalid IL or missing references)
		//IL_014d: Unknown result type (might be due to invalid IL or missing references)
		//IL_016a: Unknown result type (might be due to invalid IL or missing references)
		Device deviceMain = DeviceMain;
		Viewport viewport = DeviceMain.Viewport;
		deviceMain.SetTransform((TransformState)2, Matrix.LookAtLH(new Vector3(0f, 0f, (float)(viewport.Height / 2)), new Vector3(0f, 0f, 0f), new Vector3(0f, -1f, 0f)));
		Device deviceMain2 = DeviceMain;
		viewport = DeviceMain.Viewport;
		float num = viewport.Width;
		viewport = DeviceMain.Viewport;
		deviceMain2.SetTransform((TransformState)3, Matrix.PerspectiveFovLH((float)Math.PI / 2f, num / (float)viewport.Height, 1f, 1000f));
		DeviceMain.SetRenderState((RenderState)7, false);
		DeviceMain.SetRenderState((RenderState)22, true);
		DeviceMain.SetRenderState((RenderState)27, true);
		DeviceMain.SetRenderState<Blend>((RenderState)19, (Blend)5);
		DeviceMain.SetRenderState((RenderState)28, false);
		DeviceMain.SetTextureStageState(0, (TextureStage)5, (TextureArgument)2);
		DeviceMain.SetTextureStageState(0, (TextureStage)4, (TextureOperation)4);
		DeviceMain.SetTextureStageState(0, (TextureStage)6, (TextureArgument)0);
		DeviceMain.SetLight(0, light);
		DeviceMain.SetRenderState((RenderState)137, true);
		DeviceMain.EnableLight(0, true);
		DeviceMain.VertexDeclaration = vertexDecl;
		DeviceMain.VertexFormat = TexturedVertex.Format;
	}

	public void Show()
	{
		//IL_0021: Unknown result type (might be due to invalid IL or missing references)
		//IL_0026: Unknown result type (might be due to invalid IL or missing references)
		//IL_0060: Unknown result type (might be due to invalid IL or missing references)
		//IL_00e6: Unknown result type (might be due to invalid IL or missing references)
		//IL_015b: Unknown result type (might be due to invalid IL or missing references)
		//IL_01af: Unknown result type (might be due to invalid IL or missing references)
		//IL_01be: Unknown result type (might be due to invalid IL or missing references)
		//IL_040c: Unknown result type (might be due to invalid IL or missing references)
		//IL_041b: Unknown result type (might be due to invalid IL or missing references)
		//IL_0220: Unknown result type (might be due to invalid IL or missing references)
		//IL_022f: Unknown result type (might be due to invalid IL or missing references)
		//IL_02b3: Unknown result type (might be due to invalid IL or missing references)
		//IL_02ec: Unknown result type (might be due to invalid IL or missing references)
		//IL_0313: Unknown result type (might be due to invalid IL or missing references)
		//IL_0340: Unknown result type (might be due to invalid IL or missing references)
		//IL_0361: Unknown result type (might be due to invalid IL or missing references)
		//IL_0373: Unknown result type (might be due to invalid IL or missing references)
		//IL_03c9: Unknown result type (might be due to invalid IL or missing references)
		//IL_03ea: Unknown result type (might be due to invalid IL or missing references)
		StageData.ScreenTexMan.Begin();
		Viewport viewport = StageData.DeviceMain.Viewport;
		if (!StageData.BoundRect.Equals((object?)new Rectangle(0, 0, 640, 480)))
		{
			Device deviceMain = DeviceMain;
			Viewport viewport2 = default;
			viewport2.X = StageData.BoundRect.Left - 10;
			viewport2.Y = StageData.BoundRect.Top - 10;
			viewport2.Width = StageData.BoundRect.Width + 20;
			viewport2.Height = StageData.BoundRect.Height + 20;
			viewport2.MinZ = 0f;
			viewport2.MaxZ = 1f;
			deviceMain.Viewport = viewport2;
		}
		if (Envi != null)
		{
			Envi.SetRenderPara();
		}
		if (Enabled3D)
		{
			BackgroundList.ForEach((BaseObject x) =>
			{
				x.Show();
			});
		}
		SetParticleRenderPara();
		string ModelName = null;
		BackgroundParticleList.ForEach((BaseParticle3D x) =>
		{
			if (x.model != null)
			{
				if (ModelName != x.model.Name)
				{
					x.model.SetModel();
				}
				x.Show();
				ModelName = x.model.Name;
			}
		});
		DeviceMain.Viewport = viewport;
		StageData.SpriteMain.Begin((SpriteFlags)16);
		StageData.Background.Show(Active: false);
		StageData.SpriteMain.End();
		StageData.SpriteMain.Begin((SpriteFlags)16);
		DeviceMain.SetRenderState((RenderState)19, 5);
		DeviceMain.SetRenderState((RenderState)20, 2);
		StageData.Background.Show(Active: true);
		StageData.SpriteMain.End();
		StageData.ScreenTexMan.End();
		if (StageData.Boss != null)
		{
			StageData.SpriteMain.Begin((SpriteFlags)16);
			DeviceMain.SetRenderState((RenderState)19, 5);
			DeviceMain.SetRenderState((RenderState)20, 2);
			StageData.EffectMain = StageData.EffectDictionary["Warp"];
			StageData.EffectMain.Technique = EffectHandle.op_Implicit("technique1");
			StageData.EffectMain.Begin();
			((BaseEffect)StageData.EffectMain).SetValue<float>(EffectHandle.op_Implicit("X"), StageData.Boss.Position.X / 640f);
			((BaseEffect)StageData.EffectMain).SetValue<float>(EffectHandle.op_Implicit("Y"), StageData.Boss.Position.Y / 480f);
			((BaseEffect)StageData.EffectMain).SetValue<float>(EffectHandle.op_Implicit("Radius"), WarpRadius / 640f);
			((BaseEffect)StageData.EffectMain).SetValue<float>(EffectHandle.op_Implicit("Time"), (float)StageData.TimeMain / 20f);
			((BaseEffect)StageData.EffectMain).SetValue<int>(EffectHandle.op_Implicit("ColorKey"), WarpColorKey);
			StageData.EffectMain.BeginPass(0);
			StageData.SpriteMain.Draw2D(StageData.ScreenTexMan.RenderTexture, new Point(0, 0), 0f, new PointF(0f, 0f), Color.White);
			StageData.EffectMain.EndPass();
			StageData.SpriteMain.End();
			StageData.EffectMain.End();
		}
		else
		{
			StageData.SpriteMain.Begin((SpriteFlags)16);
			DeviceMain.SetRenderState((RenderState)19, 5);
			DeviceMain.SetRenderState((RenderState)20, 2);
			StageData.SpriteMain.Draw2D(StageData.ScreenTexMan.RenderTexture, new Point(0, 0), 0f, new PointF(0f, 0f), Color.White);
			StageData.SpriteMain.End();
		}
	}

	public void Dispose()
	{
		BackgroundList.ForEach((BaseObject x) =>
		{
			x.Dispose();
		});
		BackgroundParticleList.ForEach((BaseParticle3D x) =>
		{
			x.Dispose();
		});
		if (vertexDecl != null)
		{
			((ComObject)vertexDecl).Dispose();
		}
	}

	public void Clear()
	{
		BackgroundList.Clear();
		BackgroundParticleList.Clear();
	}
}
