using System;
using System.Drawing;

namespace Shooting;

internal class BackgroundGroupSSS01
{
	public BackgroundGroupSSS01(StageDataPackage StageData)
	{
		new BackgroundTouhouStage02_3D(StageData).TxtureObject = StageData.TextureObjectDictionary["BG01b"];
		for (int i = 0; i < 20; i++)
		{
			new BackgroundTree(StageData, "Tree", new PointF(290f, 4200 - i * 200 + 100), InsertAtFirst: false).Mirrored = true;
			new BackgroundTree(StageData, "Tree", new PointF(290f, 4200 - i * 200), InsertAtFirst: false);
			new BackgroundTree(StageData, "Tree", new PointF(-400f, 4200 - i * 200 + 162), InsertAtFirst: false).Mirrored = true;
			new BackgroundTree(StageData, "Tree", new PointF(-400f, 4200 - i * 200 + 62), InsertAtFirst: false);
		}
		for (int j = 0; j < 10; j++)
		{
			new Background3DObject(StageData, "Cloud02", new PointF(0f, j * 500), InsertAtFirst: false)
			{
				Scale = 2f,
				Mirrored = false,
				OriginalPositionZ = -10f,
				Angle3DX = -(float)Math.PI,
				TransparentValueF = 50f,
				Velocity = 0.5f
			};
			new Background3DObject(StageData, "Cloud02", new PointF(-1024f, j * 500), InsertAtFirst: false)
			{
				Scale = 2f,
				Mirrored = false,
				OriginalPositionZ = -10f,
				Angle3DX = -(float)Math.PI,
				TransparentValueF = 50f,
				Velocity = 0.5f
			};
			new Background3DObject(StageData, "Cloud02", new PointF(-2048f, j * 500), InsertAtFirst: false)
			{
				Scale = 2f,
				Mirrored = false,
				OriginalPositionZ = -10f,
				Angle3DX = -(float)Math.PI,
				TransparentValueF = 50f,
				Velocity = 0.5f
			};
			new Background3DObject(StageData, "Cloud01", new PointF(0f, j * 500 + 250), InsertAtFirst: false)
			{
				Scale = 2f,
				Mirrored = true,
				OriginalPositionZ = -20f,
				Angle3DX = -(float)Math.PI,
				TransparentValueF = 50f,
				Velocity = -0.5f
			};
			new Background3DObject(StageData, "Cloud01", new PointF(1024f, j * 500 + 250), InsertAtFirst: false)
			{
				Scale = 2f,
				Mirrored = true,
				OriginalPositionZ = -20f,
				Angle3DX = -(float)Math.PI,
				TransparentValueF = 50f,
				Velocity = -0.5f
			};
			new Background3DObject(StageData, "Cloud01", new PointF(2048f, j * 500 + 250), InsertAtFirst: false)
			{
				Scale = 2f,
				Mirrored = true,
				OriginalPositionZ = -20f,
				Angle3DX = -(float)Math.PI,
				TransparentValueF = 50f,
				Velocity = -0.5f
			};
		}
	}
}
