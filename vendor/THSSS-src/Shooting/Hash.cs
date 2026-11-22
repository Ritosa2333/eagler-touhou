using System.Collections;

namespace Shooting;

public class Hash
{
	public static Hashtable conditions = new Hashtable();

	public static Hashtable results = new Hashtable();

	public static Hashtable type = new Hashtable();

	public static Hashtable conditions2 = new Hashtable();

	public static Hashtable results2 = new Hashtable();

	public static Hashtable results3 = new Hashtable();

	public static Hashtable cconditions = new Hashtable();

	public static Hashtable cresults = new Hashtable();

	public static Hashtable lconditions = new Hashtable();

	public static Hashtable lresults = new Hashtable();

	public static Hashtable lresults2 = new Hashtable();

	public static void Init()
	{
		type.Add("正比", 0);
		type.Add("固定", 1);
		type.Add("正弦", 2);
		conditions.Add("", 0);
		conditions.Add("当前帧", 0);
		conditions.Add("X坐标", 1);
		conditions.Add("Y坐标", 2);
		conditions.Add("半径", 3);
		conditions.Add("半径方向", 4);
		conditions.Add("条数", 5);
		conditions.Add("周期", 6);
		conditions.Add("角度", 7);
		conditions.Add("范围", 8);
		conditions.Add("宽比", 9);
		conditions.Add("高比", 10);
		conditions.Add("不透明度", 11);
		conditions.Add("朝向", 12);
		results.Add("X坐标", 0);
		results.Add("Y坐标", 1);
		results.Add("半径", 2);
		results.Add("半径方向", 3);
		results.Add("条数", 4);
		results.Add("周期", 5);
		results.Add("角度", 6);
		results.Add("范围", 7);
		results.Add("速度", 8);
		results.Add("速度方向", 9);
		results.Add("加速度", 10);
		results.Add("加速度方向", 11);
		results.Add("生命", 12);
		results.Add("类型", 13);
		results.Add("宽比", 14);
		results.Add("高比", 15);
		results.Add("R", 16);
		results.Add("G", 17);
		results.Add("B", 18);
		results.Add("不透明度", 19);
		results.Add("朝向", 20);
		results.Add("子弹速度", 21);
		results.Add("子弹速度方向", 22);
		results.Add("子弹加速度", 23);
		results.Add("子弹加速度方向", 24);
		results.Add("横比", 25);
		results.Add("纵比", 26);
		results.Add("雾化效果", 27);
		results.Add("消除效果", 28);
		results.Add("高光效果", 29);
		results.Add("拖影效果", 30);
		results.Add("出屏即消", 31);
		results.Add("无敌状态", 32);
		conditions2.Add("", 0);
		conditions2.Add("当前帧", 0);
		conditions2.Add("X坐标", 1);
		conditions2.Add("Y坐标", 2);
		results2.Add("生命", 0);
		results2.Add("类型", 1);
		results2.Add("宽比", 2);
		results2.Add("高比", 3);
		results2.Add("R", 4);
		results2.Add("G", 5);
		results2.Add("B", 6);
		results2.Add("不透明度", 7);
		results2.Add("朝向", 8);
		results2.Add("子弹速度", 9);
		results2.Add("子弹速度方向", 10);
		results2.Add("子弹加速度", 11);
		results2.Add("子弹加速度方向", 12);
		results2.Add("横比", 13);
		results2.Add("纵比", 14);
		results2.Add("雾化效果", 15);
		results2.Add("消除效果", 16);
		results2.Add("高光效果", 17);
		results2.Add("拖影效果", 18);
		results2.Add("出屏即消", 19);
		results2.Add("无敌状态", 20);
		cconditions.Add("", 0);
		cconditions.Add("当前帧", 0);
		cconditions.Add("X坐标", 1);
		cconditions.Add("Y坐标", 2);
		cconditions.Add("半宽", 3);
		cconditions.Add("半高", 4);
		cresults.Add("半宽", 0);
		cresults.Add("半高", 1);
		cresults.Add("启用圆形", 2);
		cresults.Add("速度", 3);
		cresults.Add("速度方向", 4);
		cresults.Add("加速度", 5);
		cresults.Add("加速度方向", 6);
		cresults.Add("类型", 7);
		cresults.Add("ID", 8);
		cresults.Add("X坐标", 9);
		cresults.Add("Y坐标", 10);
	}
}
