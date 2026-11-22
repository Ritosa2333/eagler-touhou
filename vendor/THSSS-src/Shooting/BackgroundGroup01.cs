using System.Drawing;

namespace Shooting;

internal class BackgroundGroup01
{
	public BackgroundGroup01(StageDataPackage StageData)
	{
		new BackgroundTouhouStage01B_3D(StageData, "stage01a1").OriginalPosition = new PointF(0f, 0f);
		new BackgroundTouhouStage01B_3D(StageData, "stage01a1").OriginalPosition = new PointF(0f, 1000f);
		new BackgroundTouhouStage01B_3D(StageData, "stage01a1").OriginalPosition = new PointF(0f, 2000f);
		new BackgroundTouhouStage01B_3D(StageData, "stage01a1").OriginalPosition = new PointF(0f, 3000f);
		new BackgroundTouhouStage01C_3D(StageData, "world02c")
		{
			OriginalPosition = new PointF(0f, 0f),
			TransparentValueF = 128f,
			Time = 300,
			Height = 50
		};
		new BackgroundTouhouStage01C_3D(StageData, "world02c")
		{
			OriginalPosition = new PointF(0f, 1000f),
			TransparentValueF = 128f,
			Time = 300,
			Height = 50
		};
		new BackgroundTouhouStage01C_3D(StageData, "world02c")
		{
			OriginalPosition = new PointF(0f, 2000f),
			TransparentValueF = 128f,
			Time = 300,
			Height = 50
		};
		new BackgroundTouhouStage01C_3D(StageData, "world02c")
		{
			OriginalPosition = new PointF(0f, 3000f),
			TransparentValueF = 128f,
			Time = 300,
			Height = 50
		};
		new BackgroundTouhouStage01C_3D(StageData, "world02c2")
		{
			OriginalPosition = new PointF(0f, 0f),
			TransparentValueF = 128f,
			DirectionDegree = 180.0,
			Height = 100
		};
		new BackgroundTouhouStage01C_3D(StageData, "world02c2")
		{
			OriginalPosition = new PointF(0f, 1000f),
			TransparentValueF = 128f,
			DirectionDegree = 180.0,
			Height = 100
		};
		new BackgroundTouhouStage01C_3D(StageData, "world02c2")
		{
			OriginalPosition = new PointF(0f, 2000f),
			TransparentValueF = 128f,
			DirectionDegree = 180.0,
			Height = 100
		};
		new BackgroundTouhouStage01C_3D(StageData, "world02c2")
		{
			OriginalPosition = new PointF(0f, 3000f),
			TransparentValueF = 128f,
			DirectionDegree = 180.0,
			Height = 100
		};
	}
}
