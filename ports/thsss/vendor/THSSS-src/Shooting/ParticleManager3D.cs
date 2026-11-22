using System;
using System.Collections.Generic;
using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

public class ParticleManager3D
{
	private Light light;

	public List<BaseParticle3D> ParticleList { get; set; }

	public VertexDeclaration vertexDecl { get; set; }

	public StageDataPackage StageData { get; private set; }

	public Device DeviceMain => StageData.DeviceMain;

	public ParticleManager3D(StageDataPackage StageData)
	{
		//IL_0024: Unknown result type (might be due to invalid IL or missing references)
		//IL_002e: Expected Obj, but got Unknown
		ParticleList = new List<BaseParticle3D>();
		this.StageData = StageData;
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
		ParticleList.ForEach((BaseParticle3D x) =>
		{
			if (x.OutBoundary())
			{
				x.Dispose();
				ParticleList.Remove(x);
			}
		});
		ParticleList.ForEach((BaseParticle3D x) =>
		{
			x.Ctrl();
		});
		ParticleList.Sort(new CompareDepth());
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
		//IL_002f: Unknown result type (might be due to invalid IL or missing references)
		//IL_009d: Unknown result type (might be due to invalid IL or missing references)
		//IL_00dd: Unknown result type (might be due to invalid IL or missing references)
		//IL_00fe: Unknown result type (might be due to invalid IL or missing references)
		//IL_010d: Unknown result type (might be due to invalid IL or missing references)
		StageData.ScreenTexMan.Begin();
		Viewport viewport = StageData.DeviceMain.Viewport;
		Device deviceMain = DeviceMain;
		Viewport viewport2 = default;
		viewport2.X = StageData.BoundRect.Left - 10;
		viewport2.Y = StageData.BoundRect.Top - 10;
		viewport2.Width = StageData.BoundRect.Width + 20;
		viewport2.Height = StageData.BoundRect.Height + 20;
		deviceMain.Viewport = viewport2;
		SetParticleRenderPara();
		string ModelName = null;
		ParticleList.ForEach((BaseParticle3D x) =>
		{
			if (x.model != null)
			{
				if (ModelName != x.model.Name)
				{
					x.model.SetModel();
				}
				ModelName = x.model.Name;
			}
			else
			{
				ModelName = null;
			}
			x.Show();
		});
		StageData.ScreenTexMan.End();
		DeviceMain.Viewport = viewport;
		StageData.SpriteMain.Begin((SpriteFlags)16);
		DeviceMain.SetRenderState((RenderState)19, 5);
		DeviceMain.SetRenderState((RenderState)20, 2);
		StageData.SpriteMain.Draw2D(StageData.ScreenTexMan.RenderTexture, new Point(0, 0), 0f, new PointF(0f, 0f), Color.White);
		StageData.SpriteMain.End();
	}

	public void Dispose()
	{
		ParticleList.ForEach((BaseParticle3D x) =>
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
		ParticleList.Clear();
	}
}
