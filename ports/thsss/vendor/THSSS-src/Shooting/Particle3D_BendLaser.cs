using System;
using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

public class Particle3D_BendLaser : BaseParticle3D
{
	private VertexBuffer vertexBuffer;

	private Material material;

	private int Length;

	private int Width;

	private int ColorType;

	public Particle3D_BendLaser(StageDataPackage StageData, int Length, int Width, int ColorType)
	{
		//IL_0070: Unknown result type (might be due to invalid IL or missing references)
		//IL_007a: Expected Obj, but got Unknown
		//IL_0080: Unknown result type (might be due to invalid IL or missing references)
		StageData.Particle3D.ParticleList.Add(this);
		Active = true;
		base.StageData = StageData;
		this.ColorType = ColorType;
		TxtureObject = TextureObjectDictionary["Laser0_" + ColorType];
		this.Length = Length;
		this.Width = Width;
		vertexBuffer = new VertexBuffer(DeviceMain, 2 * Length * TexturedVertex.SizeBytes, (Usage)8, (VertexFormat)0, (Pool)1);
		material = default;
	}

	public override void Ctrl()
	{
		Time++;
	}

	private void SetupVertex()
	{
		//IL_0194: Unknown result type (might be due to invalid IL or missing references)
		//IL_01ab: Unknown result type (might be due to invalid IL or missing references)
		//IL_0207: Unknown result type (might be due to invalid IL or missing references)
		//IL_0220: Unknown result type (might be due to invalid IL or missing references)
		//IL_0285: Unknown result type (might be due to invalid IL or missing references)
		//IL_02a7: Unknown result type (might be due to invalid IL or missing references)
		//IL_0307: Unknown result type (might be due to invalid IL or missing references)
		//IL_032b: Unknown result type (might be due to invalid IL or missing references)
		//IL_03db: Unknown result type (might be due to invalid IL or missing references)
		Length = GhostingCount;
		int num = GhostingCount - 1;
		while (num > 0 && Math.Sqrt((LastPoints[num].X - LastPoints[num - 1].X) * (LastPoints[num].X - LastPoints[num - 1].X) + (LastPoints[num].Y - LastPoints[num - 1].Y) * (LastPoints[num].Y - LastPoints[num - 1].Y)) < 1.0)
		{
			Length--;
			num--;
		}
		if (Length < 2)
		{
			Length = 2;
		}
		TexturedVertex[] array = new TexturedVertex[2 * Length];
		double num2 = Math.Atan2(LastPoints[0].Y - LastPoints[1].Y, LastPoints[0].X - LastPoints[1].X);
		array[0] = new TexturedVertex(new Vector3(LastPoints[0].X - (float)(Width / 2) * (float)Math.Sin(num2), LastPoints[0].Y + (float)(Width / 2) * (float)Math.Cos(num2), 0f), new Vector2(0f, (float)ColorType / 16f));
		array[1] = new TexturedVertex(new Vector3(LastPoints[0].X + (float)(Width / 2) * (float)Math.Sin(num2), LastPoints[0].Y - (float)(Width / 2) * (float)Math.Cos(num2), 0f), new Vector2(0f, (float)(ColorType + 1) / 16f));
		for (int i = 1; i < Length; i++)
		{
			array[2 * i] = new TexturedVertex(new Vector3(LastPoints[i].X - (float)(Width / 2) * (float)Math.Sin(num2), LastPoints[i].Y + (float)(Width / 2) * (float)Math.Cos(num2), 0f), new Vector2((float)i / ((float)Length - 1f), (float)ColorType / 16f));
			array[2 * i + 1] = new TexturedVertex(new Vector3(LastPoints[i].X + (float)(Width / 2) * (float)Math.Sin(num2), LastPoints[i].Y - (float)(Width / 2) * (float)Math.Cos(num2), 0f), new Vector2((float)i / ((float)Length - 1f), (float)(ColorType + 1) / 16f));
			if (LastPoints[i - 1] != LastPoints[i])
			{
				num2 = Math.Atan2(LastPoints[i - 1].Y - LastPoints[i].Y, LastPoints[i - 1].X - LastPoints[i].X);
			}
		}
		vertexBuffer.Lock(0, 0, (LockFlags)8192).WriteRange<TexturedVertex>(array);
		vertexBuffer.Unlock();
	}

	public override bool OutBoundary()
	{
		return false;
	}

	public override void SetupMatrices()
	{
		//IL_0000: Unknown result type (might be due to invalid IL or missing references)
		//IL_0005: Unknown result type (might be due to invalid IL or missing references)
		//IL_0006: Unknown result type (might be due to invalid IL or missing references)
		//IL_0034: Unknown result type (might be due to invalid IL or missing references)
		//IL_0039: Unknown result type (might be due to invalid IL or missing references)
		//IL_003e: Unknown result type (might be due to invalid IL or missing references)
		//IL_004a: Unknown result type (might be due to invalid IL or missing references)
		//IL_004b: Unknown result type (might be due to invalid IL or missing references)
		Matrix identity = Matrix.Identity;
		identity *= Matrix.Translation((float)(-StageData.BoundRect.Width / 2), (float)(-StageData.BoundRect.Height / 2), 0f);
		DeviceMain.SetTransform((TransformState)256, identity);
	}

	public override void Show()
	{
		//IL_0019: Unknown result type (might be due to invalid IL or missing references)
		//IL_0031: Unknown result type (might be due to invalid IL or missing references)
		//IL_005f: Unknown result type (might be due to invalid IL or missing references)
		//IL_004e: Unknown result type (might be due to invalid IL or missing references)
		//IL_007c: Unknown result type (might be due to invalid IL or missing references)
		//IL_009d: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ae: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ca: Unknown result type (might be due to invalid IL or missing references)
		SetupVertex();
		DeviceMain.SetStreamSource(0, vertexBuffer, 0, TexturedVertex.SizeBytes);
		DeviceMain.SetTexture(0, (BaseTexture)(object)TxtureObject.TXTure);
		SetupMatrices();
		if (Active)
		{
			DeviceMain.SetRenderState<Blend>((RenderState)20, (Blend)2);
		}
		else
		{
			DeviceMain.SetRenderState<Blend>((RenderState)20, (Blend)6);
		}
		material.Diffuse = new Color4(Color.FromArgb(TransparentValue, ColorValue));
		material.Ambient = new Color4(Color.FromArgb(TransparentValue, ColorValue));
		DeviceMain.Material = material;
		DeviceMain.DrawPrimitives((PrimitiveType)5, 0, 2 * Length - 2);
	}

	public override void Dispose()
	{
		((ComObject)vertexBuffer).Dispose();
	}
}
