using System;

namespace Shooting;

[Serializable]
public class Event
{
	public string EventString;

	public float rand;

	public int special;

	public int special2;

	public string condition;

	public string condition2;

	public string result;

	public int contype;

	public int contype2;

	public string opreator;

	public string opreator2;

	public string collector;

	public int change;

	public int changetype;

	public int changevalue;

	public int changename;

	public float res;

	public int times;

	public int time;

	public bool noloop;

	public void String2EmitterEvent()
	{
		string eventString = EventString;
		string text = eventString.Split(new char[1] { '：' })[0];
		string text2 = "";
		string key = "";
		string text3 = "";
		string text4 = "";
		string text5 = "";
		string text6 = eventString.Split(new char[1] { '：' })[1];
		float num = 0f;
		int num2 = 0;
		if (eventString.Contains("且"))
		{
			text5 = "且";
			text2 = text.Split("且".ToCharArray())[1];
			text = text.Split("且".ToCharArray())[0];
		}
		else if (eventString.Contains("或"))
		{
			text5 = "或";
			text2 = text.Split("或".ToCharArray())[1];
			text = text.Split("或".ToCharArray())[0];
		}
		if (text.Contains(">"))
		{
			key = text.Split(new char[1] { '>' })[0];
			text3 = ">";
			text = text.Split(new char[1] { '>' })[1];
		}
		else if (text.Contains("="))
		{
			key = text.Split(new char[1] { '=' })[0];
			text3 = "=";
			text = text.Split(new char[1] { '=' })[1];
		}
		else if (text.Contains("<"))
		{
			key = text.Split(new char[1] { '<' })[0];
			text3 = "<";
			text = text.Split(new char[1] { '<' })[1];
		}
		if (text2.Contains(">"))
		{
			_ = text2.Split(new char[1] { '>' })[0];
			text4 = ">";
			text2 = text2.Split(new char[1] { '>' })[1];
		}
		else if (text2.Contains("="))
		{
			_ = text2.Split(new char[1] { '=' })[0];
			text4 = "=";
			text2 = text2.Split(new char[1] { '=' })[1];
		}
		else if (text2.Contains("<"))
		{
			_ = text2.Split(new char[1] { '<' })[0];
			text4 = "<";
			text2 = text2.Split(new char[1] { '<' })[1];
		}
		if (eventString.Contains("变化到"))
		{
			int num3 = 0;
			string[] array = text6.Split("变".ToCharArray())[1].Split("，".ToCharArray());
			int num4 = (int)Hash.results[text6.Split("变".ToCharArray())[0]];
			string key2 = text6.Split("变".ToCharArray())[0];
			if (array[0].Replace("化到", "").Contains("+"))
			{
				if (array[0].Replace("化到", "").Split(new char[1] { '+' })[0] == "自身")
				{
					num2 = 3;
				}
				else if (array[0].Replace("化到", "").Split(new char[1] { '+' })[0] == "自机")
				{
					num2 = 4;
				}
				else
				{
					num = float.Parse(array[0].Replace("化到", "").Split(new char[1] { '+' })[0]);
				}
			}
			else if (array[0].Replace("化到", "") == "自身")
			{
				num2 = 3;
			}
			else if (array[0].Replace("化到", "") == "自机")
			{
				num2 = 4;
			}
			else
			{
				num = float.Parse(array[0].Replace("化到", ""));
			}
			string key3 = array[1];
			int num5 = int.Parse(array[2].Split("帧".ToCharArray())[0]);
			condition = text;
			result = text6;
			condition2 = text2;
			contype = (int)Hash.conditions[key];
			contype2 = (int)Hash.conditions[key];
			opreator = text3;
			opreator2 = text4;
			collector = text5;
			change = num3;
			changetype = (int)Hash.type[key3];
			changevalue = num4;
			changename = (int)Hash.results[key2];
			res = num;
			special = num2;
			if (array[0].Replace("化到", "").Contains("+"))
			{
				rand = float.Parse(array[0].Replace("化到", "").Split(new char[1] { '+' })[1]);
			}
			times = num5;
			if (array[2].Contains("("))
			{
				time = int.Parse(array[2].Split(new char[1] { '(' })[1].Split(new char[1] { ')' })[0]);
			}
		}
		else if (eventString.Contains("增加"))
		{
			int num6 = 1;
			string[] array2 = text6.Split("增".ToCharArray())[1].Split("，".ToCharArray());
			array2[0] = array2[0].Replace("加", "");
			int num7 = (int)Hash.results[text6.Split("增".ToCharArray())[0]];
			string key4 = text6.Split("增".ToCharArray())[0];
			if (array2[0].Contains("+"))
			{
				if (array2[0].Split(new char[1] { '+' })[0] == "自身")
				{
					num2 = 3;
				}
				else if (array2[0].Split(new char[1] { '+' })[0] == "自机")
				{
					num2 = 4;
				}
				else
				{
					num = float.Parse(array2[0].Split(new char[1] { '+' })[0]);
				}
			}
			else if (array2[0] == "自身")
			{
				num2 = 3;
			}
			else if (array2[0] == "自机")
			{
				num2 = 4;
			}
			else
			{
				num = float.Parse(array2[0]);
			}
			string key5 = array2[1];
			int num8 = int.Parse(array2[2].Split("帧".ToCharArray())[0]);
			condition = text;
			result = text6;
			condition2 = text2;
			contype = (int)Hash.conditions[key];
			contype2 = (int)Hash.conditions[key];
			opreator = text3;
			opreator2 = text4;
			collector = text5;
			change = num6;
			changetype = (int)Hash.type[key5];
			changevalue = num7;
			changename = (int)Hash.results[key4];
			res = num;
			special = num2;
			if (array2[0].Contains("+"))
			{
				rand = float.Parse(array2[0].Split(new char[1] { '+' })[1]);
			}
			times = num8;
			if (array2[2].Contains("("))
			{
				time = int.Parse(array2[2].Split(new char[1] { '(' })[1].Split(new char[1] { ')' })[0]);
			}
		}
		else if (eventString.Contains("减少"))
		{
			int num9 = 2;
			string[] array3 = text6.Split("减少".ToCharArray())[2].Split("，".ToCharArray());
			int num10 = (int)Hash.results[text6.Split("减少".ToCharArray())[0]];
			string key6 = text6.Split("减少".ToCharArray())[0];
			if (array3[0].Contains("+"))
			{
				if (array3[0].Split(new char[1] { '+' })[0] == "自身")
				{
					num2 = 3;
				}
				else if (array3[0].Split(new char[1] { '+' })[0] == "自机")
				{
					num2 = 4;
				}
				else
				{
					num = float.Parse(array3[0].Split(new char[1] { '+' })[0]);
				}
			}
			else if (array3[0] == "自身")
			{
				num2 = 3;
			}
			else if (array3[0] == "自机")
			{
				num2 = 4;
			}
			else
			{
				num = float.Parse(array3[0]);
			}
			string key7 = array3[1];
			int num11 = int.Parse(array3[2].Split("帧".ToCharArray())[0]);
			condition = text;
			result = text6;
			condition2 = text2;
			contype = (int)Hash.conditions[key];
			contype2 = (int)Hash.conditions[key];
			opreator = text3;
			opreator2 = text4;
			collector = text5;
			change = num9;
			changetype = (int)Hash.type[key7];
			changevalue = num10;
			changename = (int)Hash.results[key6];
			res = num;
			special = num2;
			if (array3[0].Contains("+"))
			{
				rand = float.Parse(array3[0].Split(new char[1] { '+' })[1]);
			}
			times = num11;
			if (array3[2].Contains("("))
			{
				time = int.Parse(array3[2].Split(new char[1] { '(' })[1].Split(new char[1] { ')' })[0]);
			}
		}
		else if (eventString.Contains("恢复"))
		{
			special = 1;
			opreator = text3;
			opreator2 = text4;
			condition = text;
			condition2 = text2;
			contype = (int)Hash.conditions[key];
			contype2 = (int)Hash.conditions[key];
			collector = text5;
		}
		else if (eventString.Contains("发射"))
		{
			special = 2;
			opreator = text3;
			opreator2 = text4;
			condition = text;
			condition2 = text2;
			contype = (int)Hash.conditions[key];
			contype2 = (int)Hash.conditions[key];
			collector = text5;
		}
	}

	public void String2BulletEvent()
	{
		string eventString = EventString;
		string text = eventString.Split(new char[1] { '：' })[0];
		string text2 = "";
		string key = "";
		string text3 = "";
		string text4 = "";
		string text5 = "";
		string text6 = eventString.Split(new char[1] { '：' })[1];
		float num = 0f;
		int num2 = 0;
		if (eventString.Contains("且"))
		{
			text5 = "且";
			text2 = text.Split("且".ToCharArray())[1];
			text = text.Split("且".ToCharArray())[0];
		}
		else if (eventString.Contains("或"))
		{
			text5 = "或";
			text2 = text.Split("或".ToCharArray())[1];
			text = text.Split("或".ToCharArray())[0];
		}
		if (text.Contains(">"))
		{
			key = text.Split(new char[1] { '>' })[0];
			text3 = ">";
			text = text.Split(new char[1] { '>' })[1];
		}
		else if (text.Contains("="))
		{
			key = text.Split(new char[1] { '=' })[0];
			text3 = "=";
			text = text.Split(new char[1] { '=' })[1];
		}
		else if (text.Contains("<"))
		{
			key = text.Split(new char[1] { '<' })[0];
			text3 = "<";
			text = text.Split(new char[1] { '<' })[1];
		}
		if (text2.Contains(">"))
		{
			_ = text2.Split(new char[1] { '>' })[0];
			text4 = ">";
			text2 = text2.Split(new char[1] { '>' })[1];
		}
		else if (text2.Contains("="))
		{
			_ = text2.Split(new char[1] { '=' })[0];
			text4 = "=";
			text2 = text2.Split(new char[1] { '=' })[1];
		}
		else if (text2.Contains("<"))
		{
			_ = text2.Split(new char[1] { '<' })[0];
			text4 = "<";
			text2 = text2.Split(new char[1] { '<' })[1];
		}
		if (eventString.Contains("变化到"))
		{
			int num3 = 0;
			string[] array = text6.Split("变".ToCharArray())[1].Split("，".ToCharArray());
			int num4 = (int)Hash.results2[text6.Split("变".ToCharArray())[0]];
			string key2 = text6.Split("变".ToCharArray())[0];
			if (array[0].Replace("化到", "").Contains("+"))
			{
				if (array[0].Replace("化到", "").Split(new char[1] { '+' })[0] == "自身")
				{
					num2 = 3;
				}
				else if (array[0].Replace("化到", "").Split(new char[1] { '+' })[0] == "自机")
				{
					num2 = 4;
				}
				else
				{
					num = float.Parse(array[0].Replace("化到", "").Split(new char[1] { '+' })[0]);
				}
			}
			else if (array[0].Replace("化到", "") == "自身")
			{
				num2 = 3;
			}
			else if (array[0].Replace("化到", "") == "自机")
			{
				num2 = 4;
			}
			else
			{
				num = float.Parse(array[0].Replace("化到", ""));
			}
			string key3 = array[1];
			int num5 = int.Parse(array[2].Split("帧".ToCharArray())[0]);
			condition = text;
			result = text6;
			condition2 = text2;
			contype = (int)Hash.conditions2[key];
			contype2 = (int)Hash.conditions2[key];
			opreator = text3;
			opreator2 = text4;
			collector = text5;
			change = num3;
			changetype = (int)Hash.type[key3];
			changevalue = num4;
			changename = (int)Hash.results2[key2];
			res = num;
			special = num2;
			if (array[0].Replace("化到", "").Contains("+"))
			{
				rand = float.Parse(array[0].Replace("化到", "").Split(new char[1] { '+' })[1]);
			}
			times = num5;
			if (array[2].Contains("("))
			{
				time = int.Parse(array[2].Split(new char[1] { '(' })[1].Split(new char[1] { ')' })[0]);
			}
		}
		else if (eventString.Contains("增加"))
		{
			int num6 = 1;
			string[] array2 = text6.Split("增".ToCharArray())[1].Split("，".ToCharArray());
			array2[0] = array2[0].Replace("加", "");
			int num7 = (int)Hash.results2[text6.Split("增".ToCharArray())[0]];
			string key4 = text6.Split("增".ToCharArray())[0];
			if (array2[0].Contains("+"))
			{
				if (array2[0].Split(new char[1] { '+' })[0] == "自身")
				{
					num2 = 3;
				}
				else if (array2[0].Split(new char[1] { '+' })[0] == "自机")
				{
					num2 = 4;
				}
				else
				{
					num = float.Parse(array2[0].Split(new char[1] { '+' })[0]);
				}
			}
			else if (array2[0] == "自身")
			{
				num2 = 3;
			}
			else if (array2[0] == "自机")
			{
				num2 = 4;
			}
			else
			{
				num = float.Parse(array2[0]);
			}
			string key5 = array2[1];
			int num8 = int.Parse(array2[2].Split("帧".ToCharArray())[0]);
			condition = text;
			result = text6;
			condition2 = text2;
			contype = (int)Hash.conditions2[key];
			contype2 = (int)Hash.conditions2[key];
			opreator = text3;
			opreator2 = text4;
			collector = text5;
			change = num6;
			changetype = (int)Hash.type[key5];
			changevalue = num7;
			changename = (int)Hash.results2[key4];
			res = num;
			special = num2;
			if (array2[0].Contains("+"))
			{
				rand = float.Parse(array2[0].Split(new char[1] { '+' })[1]);
			}
			times = num8;
			if (array2[2].Contains("("))
			{
				time = int.Parse(array2[2].Split(new char[1] { '(' })[1].Split(new char[1] { ')' })[0]);
			}
		}
		else if (eventString.Contains("减少"))
		{
			int num9 = 2;
			string[] array3 = text6.Split("减少".ToCharArray())[2].Split("，".ToCharArray());
			int num10 = (int)Hash.results2[text6.Split("减少".ToCharArray())[0]];
			string key6 = text6.Split("减少".ToCharArray())[0];
			if (array3[0].Contains("+"))
			{
				if (array3[0].Split(new char[1] { '+' })[0] == "自身")
				{
					num2 = 3;
				}
				else if (array3[0].Split(new char[1] { '+' })[0] == "自机")
				{
					num2 = 4;
				}
				else
				{
					num = float.Parse(array3[0].Split(new char[1] { '+' })[0]);
				}
			}
			else if (array3[0] == "自身")
			{
				num2 = 3;
			}
			else if (array3[0] == "自机")
			{
				num2 = 4;
			}
			else
			{
				num = float.Parse(array3[0]);
			}
			string key7 = array3[1];
			int num11 = int.Parse(array3[2].Split("帧".ToCharArray())[0]);
			condition = text;
			result = text6;
			condition2 = text2;
			contype = (int)Hash.conditions2[key];
			contype2 = (int)Hash.conditions2[key];
			opreator = text3;
			opreator2 = text4;
			collector = text5;
			change = num9;
			changetype = (int)Hash.type[key7];
			changevalue = num10;
			changename = (int)Hash.results2[key6];
			res = num;
			special = num2;
			if (array3[0].Contains("+"))
			{
				rand = float.Parse(array3[0].Split(new char[1] { '+' })[1]);
			}
			times = num11;
			if (array3[2].Contains("("))
			{
				time = int.Parse(array3[2].Split(new char[1] { '(' })[1].Split(new char[1] { ')' })[0]);
			}
		}
		else if (eventString.Contains("恢复"))
		{
			special = 1;
			opreator = text3;
			opreator2 = text4;
			condition = text;
			condition2 = text2;
			contype = (int)Hash.conditions2[key];
			contype2 = (int)Hash.conditions2[key];
			collector = text5;
		}
		else if (eventString.Contains("发射"))
		{
			special = 2;
			opreator = text3;
			opreator2 = text4;
			condition = text;
			condition2 = text2;
			contype = (int)Hash.conditions2[key];
			contype2 = (int)Hash.conditions2[key];
			collector = text5;
		}
	}

	public object Clone()
	{
		return MemberwiseClone();
	}
}
