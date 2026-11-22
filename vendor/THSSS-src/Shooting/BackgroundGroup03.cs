using System.Collections.Generic;
using System.Drawing;

namespace Shooting;

internal class BackgroundGroup03
{
	public BackgroundGroup03(StageDataPackage StageData)
	{
		new BackgroundTouhouWater(StageData, "world02")
		{
			OriginalPosition = new PointF(0f, 0f),
			Height = 20,
			V1 = -0.2f
		};
		new BackgroundTouhouWater(StageData, "world02")
		{
			OriginalPosition = new PointF(0f, 1000f),
			Height = 20,
			V1 = -0.2f
		};
		new BackgroundTouhouWater(StageData, "world02")
		{
			OriginalPosition = new PointF(0f, 2000f),
			Height = 20,
			V1 = -0.2f
		};
		new BackgroundTouhouWater(StageData, "world02")
		{
			OriginalPosition = new PointF(0f, 3000f),
			Height = 20,
			V1 = -0.2f
		};
		new BackgroundTouhouWater(StageData, "world02")
		{
			OriginalPosition = new PointF(0f, 4000f),
			Height = 20,
			V1 = -0.2f
		};
		new BackgroundTouhouWater(StageData, "world02")
		{
			ColorValue = Color.BlueViolet,
			OriginalPosition = new PointF(0f, -500f),
			Height = 25,
			TransparentValueF = 150f,
			V1 = 0.2f,
			Time = 100
		};
		new BackgroundTouhouWater(StageData, "world02")
		{
			ColorValue = Color.BlueViolet,
			OriginalPosition = new PointF(0f, 500f),
			Height = 25,
			TransparentValueF = 150f,
			V1 = 0.2f,
			Time = 100
		};
		new BackgroundTouhouWater(StageData, "world02")
		{
			ColorValue = Color.BlueViolet,
			OriginalPosition = new PointF(0f, 1500f),
			Height = 25,
			TransparentValueF = 150f,
			V1 = 0.2f,
			Time = 100
		};
		new BackgroundTouhouWater(StageData, "world02")
		{
			ColorValue = Color.BlueViolet,
			OriginalPosition = new PointF(0f, 2500f),
			Height = 25,
			TransparentValueF = 150f,
			V1 = 0.2f,
			Time = 100
		};
		new BackgroundTouhouWater(StageData, "world02")
		{
			ColorValue = Color.BlueViolet,
			OriginalPosition = new PointF(0f, 3500f),
			Height = 25,
			TransparentValueF = 150f,
			V1 = 0.2f,
			Time = 100
		};
		List<int> list = new List<int>();
		List<int> list2 = new List<int>();
		for (int i = 0; i < 20; i++)
		{
			int item = StageData.Ran.Next(100, 350);
			list.Add(item);
			item = StageData.Ran.Next(100, 350);
			list2.Add(item);
		}
		for (int num = 3; num >= 1; num--)
		{
			for (int j = 0; j < 20; j++)
			{
				new BackgroundTree(StageData, "EquinoxFlower00", new PointF(list[j], 1000 * num - j * 50), InsertAtFirst: false);
				new BackgroundTree(StageData, "EquinoxFlower01", new PointF(list2[j], 1000 * num - j * 50), InsertAtFirst: false);
			}
		}
	}
}
