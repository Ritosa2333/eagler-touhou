using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;

namespace Shooting;

public class CS_Data
{
	private string[] MBGtext;

	public List<BaseEmitter_CS> EmitterList { get; private set; }

	public int TimeTotal { get; private set; }

	public CS_Data()
	{
	}

	public CS_Data(string FileName)
	{
		MBGtext = File.ReadAllLines(FileName, Encoding.UTF8);
		EmitterList = new List<BaseEmitter_CS>();
	}

	public void String2Data(StageDataPackage StageData)
	{
		//IL_0032: Unknown result type (might be due to invalid IL or missing references)
		EmitterList.Clear();
		int num = 0;
		if (MBGtext[num++] != "Crazy Storm Data 1.01")
		{
			MessageBox.Show("当前暂不支持除1.01以外的数据格式", "错误", (MessageBoxButtons)0, (MessageBoxIcon)16);
			return;
		}
		string text = MBGtext[num++];
		if (text.Contains("Types"))
		{
			int num2 = int.Parse(text.Split(new char[1] { ' ' })[0]);
			for (int i = 0; i < num2; i++)
			{
				MBGtext[num++].Split(new char[1] { '_' });
			}
			text = MBGtext[num++];
		}
		if (text.Contains("GlobalEvents"))
		{
			int num3 = int.Parse(text.Split(new char[1] { ' ' })[0]);
			for (int j = 0; j < num3; j++)
			{
				_ = MBGtext[num++];
			}
			text = MBGtext[num++];
		}
		if (text.Contains("Sounds"))
		{
			int num4 = int.Parse(text.Split(new char[1] { ' ' })[0]);
			for (int k = 0; k < num4; k++)
			{
				_ = MBGtext[num++];
			}
			text = MBGtext[num++];
		}
		text = MBGtext[num++];
		TimeTotal = int.Parse(text.Split(new char[1] { ':' })[1]);
		for (int l = 0; l < 4; l++)
		{
			text = MBGtext[num++];
			if (!(text.Split(new char[1] { ':' })[1].Split(new char[1] { ',' })[0] != "empty"))
			{
				continue;
			}
			int num5 = int.Parse(text.Split(new char[1] { ':' })[1].Split(new char[1] { ',' })[3]);
			for (int m = 0; m < num5; m++)
			{
				string[] array = MBGtext[num++].Split(new char[1] { ',' });
				EmitterMode emitterMode = EmitterMode.Bullet;
				if (array.Length > 91)
				{
					if (bool.Parse(array[81]))
					{
						emitterMode = EmitterMode.Enemy;
					}
					else if (int.Parse(array[91]) > 0)
					{
						emitterMode = EmitterMode.Effect;
					}
				}
				BaseEmitter_CS baseEmitter_CS = new BaseEmitter_CS(StageData, this, emitterMode);
				if (baseEmitter_CS.EmitterMode == EmitterMode.Enemy)
				{
					EnemyFactory enemyFactory = new EnemyFactory(array[82]);
					enemyFactory.HealthPoint = int.Parse(array[83]);
					enemyFactory.RedCount = int.Parse(array[84]);
					enemyFactory.BlueCount = int.Parse(array[85]);
					enemyFactory.ColorType = byte.Parse(array[86]);
					enemyFactory.BackFire = bool.Parse(array[87]);
					enemyFactory.ClearRadius = int.Parse(array[88]);
					enemyFactory.GreenCount = int.Parse(array[89]);
					enemyFactory.StarFall = array[90] != "0";
					baseEmitter_CS.SubBullet = enemyFactory.GenerateEnemy(StageData);
				}
				baseEmitter_CS.ID = int.Parse(array[0]);
				baseEmitter_CS.LayerID = int.Parse(array[1]);
				baseEmitter_CS.BindingState = bool.Parse(array[2]);
				baseEmitter_CS.BindingID = int.Parse(array[3]);
				baseEmitter_CS.BindWithDirection = bool.Parse(array[4]);
				baseEmitter_CS.CS_Position = new PointF(float.Parse(array[6]), float.Parse(array[7]));
				baseEmitter_CS.StartTime = int.Parse(array[8]);
				baseEmitter_CS.Duration = int.Parse(array[9]);
				baseEmitter_CS.EmitPoint = new PointF(float.Parse(array[10]), float.Parse(array[11]));
				baseEmitter_CS.EmitRadius = int.Parse(array[12]);
				baseEmitter_CS.RadiusDirection = float.Parse(array[13]);
				string text2 = array[14].Replace("{", "").Replace("}", "");
				baseEmitter_CS.RadiusDirectionPoint = new PointF(float.Parse(text2.Split(new char[1] { ' ' })[0].Split(new char[1] { ':' })[1]), float.Parse(text2.Split(new char[1] { ' ' })[1].Split(new char[1] { ':' })[1]));
				baseEmitter_CS.Way = int.Parse(array[15]);
				baseEmitter_CS.Circle = int.Parse(array[16]);
				baseEmitter_CS.EmitDirection = float.Parse(array[17]);
				text2 = array[18].Replace("{", "").Replace("}", "");
				baseEmitter_CS.EmitDirectionPoint = new PointF(float.Parse(text2.Split(new char[1] { ' ' })[0].Split(new char[1] { ':' })[1]), float.Parse(text2.Split(new char[1] { ' ' })[1].Split(new char[1] { ':' })[1]));
				baseEmitter_CS.Range = int.Parse(array[19]);
				baseEmitter_CS.Velocity = float.Parse(array[20]);
				baseEmitter_CS.DirectionDegree = float.Parse(array[21]);
				text2 = array[22].Replace("{", "").Replace("}", "");
				baseEmitter_CS.DestPoint = new PointF(float.Parse(text2.Split(new char[1] { ' ' })[0].Split(new char[1] { ':' })[1]), float.Parse(text2.Split(new char[1] { ' ' })[1].Split(new char[1] { ':' })[1]));
				baseEmitter_CS.AccelerateCS = float.Parse(array[23]);
				baseEmitter_CS.AccDirection = float.Parse(array[24]);
				baseEmitter_CS.SubBullet.LifeTime = int.Parse(array[26]);
				baseEmitter_CS.SubBullet.Type = int.Parse(array[27]);
				baseEmitter_CS.SubBullet.ScaleWidth = float.Parse(array[28]);
				baseEmitter_CS.SubBullet.ScaleLength = float.Parse(array[29]);
				baseEmitter_CS.SubBullet.ColorValue = Color.FromArgb(int.Parse(array[30]), int.Parse(array[31]), int.Parse(array[32]));
				baseEmitter_CS.SubBullet.TransparentValueF = int.Parse(array[33]) * 255 / 100;
				baseEmitter_CS.SubBullet.AngleDegree = float.Parse(array[34]) + 180f;
				baseEmitter_CS.SubBullet.AngleWithDirection = bool.Parse(array[36]);
				baseEmitter_CS.SubBullet.Velocity = float.Parse(array[37]);
				baseEmitter_CS.SubBullet.DirectionDegree = float.Parse(array[38]);
				baseEmitter_CS.SubBullet.DestPoint = new PointF(float.Parse(text2.Split(new char[1] { ' ' })[0].Split(new char[1] { ':' })[1]), float.Parse(text2.Split(new char[1] { ' ' })[1].Split(new char[1] { ':' })[1]));
				baseEmitter_CS.SubBullet.AccelerateCS = float.Parse(array[40]);
				baseEmitter_CS.SubBullet.AccDirection = float.Parse(array[41]);
				baseEmitter_CS.SubBullet.ScaleX = float.Parse(array[43]);
				baseEmitter_CS.SubBullet.ScaleY = float.Parse(array[44]);
				baseEmitter_CS.SubBullet.BeginningEffect = bool.Parse(array[45]);
				baseEmitter_CS.SubBullet.EndingEffect = bool.Parse(array[46]);
				baseEmitter_CS.SubBullet.Active = bool.Parse(array[47]);
				baseEmitter_CS.SubBullet.Ghosting = bool.Parse(array[48]);
				baseEmitter_CS.SubBullet.OutBound = bool.Parse(array[49]);
				baseEmitter_CS.SubBullet.UnRemoveable = bool.Parse(array[50]);
				text2 = array[51];
				for (int num6 = 0; num6 < text2.Split(new char[1] { '&' }).Length - 1; num6++)
				{
					string text3 = text2.Split(new char[1] { '&' })[num6];
					EventGroup eventGroup = new EventGroup();
					eventGroup.index = num6;
					eventGroup.tag = text3.Split(new char[1] { '|' })[0];
					eventGroup.t = int.Parse(text3.Split(new char[1] { '|' })[1]);
					eventGroup.addtime = int.Parse(text3.Split(new char[1] { '|' })[2]);
					for (int num7 = 0; num7 < text3.Split(new char[1] { '|' })[3].Split(new char[1] { ';' }).Length - 1; num7++)
					{
						Event obj = new Event();
						obj.EventString = text3.Split(new char[1] { '|' })[3].Split(new char[1] { ';' })[num7];
						obj.String2EmitterEvent();
						eventGroup.EventList.Add(obj);
					}
					baseEmitter_CS.EventGroupList.Add(eventGroup);
				}
				text2 = array[52];
				for (int num8 = 0; num8 < text2.Split(new char[1] { '&' }).Length - 1; num8++)
				{
					string text4 = text2.Split(new char[1] { '&' })[num8];
					EventGroup eventGroup2 = new EventGroup();
					eventGroup2.index = num8;
					eventGroup2.tag = text4.Split(new char[1] { '|' })[0];
					eventGroup2.t = int.Parse(text4.Split(new char[1] { '|' })[1]);
					eventGroup2.addtime = int.Parse(text4.Split(new char[1] { '|' })[2]);
					for (int num9 = 0; num9 < text4.Split(new char[1] { '|' })[3].Split(new char[1] { ';' }).Length - 1; num9++)
					{
						Event obj2 = new Event();
						obj2.EventString = text4.Split(new char[1] { '|' })[3].Split(new char[1] { ';' })[num9];
						obj2.String2BulletEvent();
						eventGroup2.EventList.Add(obj2);
					}
					baseEmitter_CS.SubBullet.EventGroupList.Add(eventGroup2);
				}
				baseEmitter_CS.RanX = float.Parse(array[53]);
				baseEmitter_CS.RanY = float.Parse(array[54]);
				baseEmitter_CS.RanRadius = int.Parse(array[55]);
				baseEmitter_CS.RanRadiusDirection = float.Parse(array[56]);
				baseEmitter_CS.RanWay = int.Parse(array[57]);
				baseEmitter_CS.RanCircle = int.Parse(array[58]);
				baseEmitter_CS.RanEmitDirection = float.Parse(array[59]);
				baseEmitter_CS.RanRange = int.Parse(array[60]);
				baseEmitter_CS.RanVelocity = float.Parse(array[61]);
				baseEmitter_CS.RanDirection = float.Parse(array[62]);
				baseEmitter_CS.RanAccelerate = float.Parse(array[63]);
				baseEmitter_CS.RanAccDirection = float.Parse(array[64]);
				baseEmitter_CS.SubBullet.RanAngle = float.Parse(array[65]);
				baseEmitter_CS.SubBullet.RanVelocity = float.Parse(array[66]);
				baseEmitter_CS.SubBullet.RanDirection = float.Parse(array[67]);
				baseEmitter_CS.SubBullet.RanAccelerate = float.Parse(array[68]);
				baseEmitter_CS.SubBullet.RanAccDirection = float.Parse(array[69]);
				if (array.Length > 72)
				{
					baseEmitter_CS.SubBullet.Cover = bool.Parse(array[70]);
					baseEmitter_CS.SubBullet.Rebound = bool.Parse(array[71]);
					baseEmitter_CS.SubBullet.Force = bool.Parse(array[72]);
				}
				if (array.Length > 73)
				{
					baseEmitter_CS.DeepBinding = bool.Parse(array[73]);
				}
				if (array.Length > 79)
				{
					baseEmitter_CS.MotionBinding = bool.Parse(array[74]);
					if (baseEmitter_CS.SubBullet is BaseBullet_Touhou)
					{
						((BaseBullet_Touhou)baseEmitter_CS.SubBullet).Reflect = byte.Parse(array[75]);
						((BaseBullet_Touhou)baseEmitter_CS.SubBullet).Layer = int.Parse(array[78]);
					}
					baseEmitter_CS.SoundName = array[76];
					baseEmitter_CS.SpecifySE = bool.Parse(array[77]);
					if (int.Parse(array[79]) != 0)
					{
						baseEmitter_CS.SubBullet.Region = int.Parse(array[79]);
						((BaseBullet_Touhou)baseEmitter_CS.SubBullet).SizeValue = baseEmitter_CS.SubBullet.Region * 2;
					}
				}
				if (array.Length > 95)
				{
					baseEmitter_CS.EffectType = int.Parse(array[91]);
					if (array[92] != null && StageData.TextureObjectDictionary.ContainsKey(array[92]))
					{
						baseEmitter_CS.SubBullet.TxtureObject = StageData.TextureObjectDictionary[array[92]];
					}
					baseEmitter_CS.Count = int.Parse(array[94]);
					baseEmitter_CS.Count = ((baseEmitter_CS.Count < 1) ? 1 : baseEmitter_CS.Count);
					baseEmitter_CS.DeltaV = float.Parse(array[95]);
				}
				if (array.Length > 110)
				{
					baseEmitter_CS.RDirectionWithDirection = !(array[96] == "0");
				}
				EmitterList.Add(baseEmitter_CS);
			}
			if (text.Split(new char[1] { ':' })[1].Split(new char[1] { ',' }).Length < 7)
			{
				continue;
			}
			num5 = int.Parse(text.Split(new char[1] { ':' })[1].Split(new char[1] { ',' })[4]);
			for (int n = 0; n < num5; n++)
			{
				string text5 = MBGtext[num++];
				string[] array2 = text5.Split(new char[1] { ',' });
				BaseEmitter_CS baseEmitter_CS2;
				if (bool.Parse(array2[29]))
				{
					baseEmitter_CS2 = new BaseEmitter_CS(StageData, this, EmitterMode.RadialLaser);
				}
				else if (array2.Length <= 61)
				{
					baseEmitter_CS2 = new BaseEmitter_CS(StageData, this, EmitterMode.StraightLaser);
				}
				else
				{
					baseEmitter_CS2 = ((!bool.Parse(array2[61])) ? new BaseEmitter_CS(StageData, this, EmitterMode.StraightLaser) : new BaseEmitter_CS(StageData, this, EmitterMode.BendLaser));
				}
				baseEmitter_CS2.ID = int.Parse(array2[0]);
				baseEmitter_CS2.LayerID = int.Parse(array2[1]);
				baseEmitter_CS2.BindingState = bool.Parse(array2[2]);
				baseEmitter_CS2.BindingID = int.Parse(array2[3]);
				baseEmitter_CS2.BindWithDirection = bool.Parse(array2[4]);
				baseEmitter_CS2.CS_Position = new PointF(float.Parse(array2[6]), float.Parse(array2[7]));
				baseEmitter_CS2.StartTime = int.Parse(array2[8]);
				baseEmitter_CS2.Duration = int.Parse(array2[9]);
				baseEmitter_CS2.EmitRadius = int.Parse(array2[10]);
				baseEmitter_CS2.RadiusDirection = float.Parse(array2[11]);
				baseEmitter_CS2.Way = int.Parse(array2[13]);
				baseEmitter_CS2.Circle = int.Parse(array2[14]);
				baseEmitter_CS2.EmitDirection = float.Parse(array2[15]);
				baseEmitter_CS2.Range = int.Parse(array2[17]);
				baseEmitter_CS2.Velocity = float.Parse(array2[18]);
				baseEmitter_CS2.DirectionDegree = float.Parse(array2[19]);
				baseEmitter_CS2.AccelerateCS = float.Parse(array2[21]);
				baseEmitter_CS2.AccDirection = float.Parse(array2[22]);
				baseEmitter_CS2.SubBullet.LifeTime = int.Parse(array2[24]);
				baseEmitter_CS2.SubBullet.Type = int.Parse(array2[25]);
				if (baseEmitter_CS2.EmitterMode == EmitterMode.StraightLaser || baseEmitter_CS2.EmitterMode == EmitterMode.BendLaser)
				{
					baseEmitter_CS2.SubBullet.ScaleLength = float.Parse(array2[26]);
					baseEmitter_CS2.SubBullet.GhostingCount = int.Parse(array2[27]);
				}
				else
				{
					baseEmitter_CS2.SubBullet.MaxScaleW = (float)int.Parse(array2[27]) / 256f;
					baseEmitter_CS2.SubBullet.ScaleWidth = baseEmitter_CS2.SubBullet.MaxScaleW;
					baseEmitter_CS2.SubBullet.MaxScaleL = 1f;
					baseEmitter_CS2.SubBullet.ScaleLength = 0.2f;
				}
				baseEmitter_CS2.SubBullet.TransparentValueF = int.Parse(array2[28]) * 255 / 100;
				baseEmitter_CS2.SubBullet.Velocity = float.Parse(array2[30]);
				baseEmitter_CS2.SubBullet.DirectionDegree = float.Parse(array2[31]);
				baseEmitter_CS2.SubBullet.AccelerateCS = float.Parse(array2[33]);
				baseEmitter_CS2.SubBullet.AccDirection = float.Parse(array2[34]);
				baseEmitter_CS2.SubBullet.Active = bool.Parse(array2[38]);
				baseEmitter_CS2.SubBullet.UnRemoveable = bool.Parse(array2[40]);
				string text6 = array2[42];
				for (int num10 = 0; num10 < text6.Split(new char[1] { '&' }).Length - 1; num10++)
				{
					string text7 = text6.Split(new char[1] { '&' })[num10];
					EventGroup eventGroup3 = new EventGroup();
					eventGroup3.index = num10;
					eventGroup3.tag = text7.Split(new char[1] { '|' })[0];
					eventGroup3.t = int.Parse(text7.Split(new char[1] { '|' })[1]);
					eventGroup3.addtime = int.Parse(text7.Split(new char[1] { '|' })[2]);
					for (int num11 = 0; num11 < text7.Split(new char[1] { '|' })[3].Split(new char[1] { ';' }).Length - 1; num11++)
					{
						Event obj3 = new Event();
						obj3.EventString = text7.Split(new char[1] { '|' })[3].Split(new char[1] { ';' })[num11];
						obj3.String2EmitterEvent();
						eventGroup3.EventList.Add(obj3);
					}
					baseEmitter_CS2.EventGroupList.Add(eventGroup3);
				}
				text6 = array2[43];
				for (int num12 = 0; num12 < text6.Split(new char[1] { '&' }).Length - 1; num12++)
				{
					string text8 = text6.Split(new char[1] { '&' })[num12];
					EventGroup eventGroup4 = new EventGroup();
					eventGroup4.index = num12;
					eventGroup4.tag = text8.Split(new char[1] { '|' })[0];
					eventGroup4.t = int.Parse(text8.Split(new char[1] { '|' })[1]);
					eventGroup4.addtime = int.Parse(text8.Split(new char[1] { '|' })[2]);
					for (int num13 = 0; num13 < text8.Split(new char[1] { '|' })[3].Split(new char[1] { ';' }).Length - 1; num13++)
					{
						Event obj4 = new Event();
						obj4.EventString = text8.Split(new char[1] { '|' })[3].Split(new char[1] { ';' })[num13];
						obj4.String2BulletEvent();
						eventGroup4.EventList.Add(obj4);
					}
					baseEmitter_CS2.SubBullet.EventGroupList.Add(eventGroup4);
				}
				baseEmitter_CS2.RanRadius = int.Parse(array2[44]);
				baseEmitter_CS2.RanRadiusDirection = float.Parse(array2[45]);
				baseEmitter_CS2.RanWay = int.Parse(array2[46]);
				baseEmitter_CS2.RanCircle = int.Parse(array2[47]);
				baseEmitter_CS2.RanEmitDirection = float.Parse(array2[48]);
				baseEmitter_CS2.RanRange = int.Parse(array2[49]);
				baseEmitter_CS2.RanVelocity = float.Parse(array2[50]);
				baseEmitter_CS2.RanDirection = float.Parse(array2[51]);
				baseEmitter_CS2.RanAccelerate = float.Parse(array2[52]);
				baseEmitter_CS2.RanAccDirection = float.Parse(array2[53]);
				baseEmitter_CS2.SubBullet.RanVelocity = float.Parse(array2[54]);
				baseEmitter_CS2.SubBullet.RanDirection = float.Parse(array2[55]);
				baseEmitter_CS2.SubBullet.RanAccelerate = float.Parse(array2[56]);
				baseEmitter_CS2.SubBullet.RanAccDirection = float.Parse(array2[57]);
				if (array2.Length > 58)
				{
					baseEmitter_CS2.DeepBinding = bool.Parse(array2[58]);
				}
				if (array2.Length > 62)
				{
					if (baseEmitter_CS2.SubBullet is BaseBullet_Touhou)
					{
						((BaseBullet_Touhou)baseEmitter_CS2.SubBullet).Reflect = byte.Parse(text5.Split(new char[1] { ',' })[59]);
					}
					baseEmitter_CS2.MotionBinding = bool.Parse(text5.Split(new char[1] { ',' })[60]);
					if (baseEmitter_CS2.EmitterMode == EmitterMode.StraightLaser)
					{
						((BaseStraightLaser_CS)baseEmitter_CS2.SubBullet).LaserHead = bool.Parse(text5.Split(new char[1] { ',' })[62]);
					}
				}
				EmitterList.Add(baseEmitter_CS2);
			}
			num5 = int.Parse(text.Split(new char[1] { ':' })[1].Split(new char[1] { ',' })[5]);
			for (int num14 = 0; num14 < num5; num14++)
			{
				string[] array3 = MBGtext[num++].Split(new char[1] { ',' });
				BaseCover_CS baseCover_CS = new BaseCover_CS(StageData);
				baseCover_CS.MaxScale = 1000f;
				baseCover_CS.ID = int.Parse(array3[0]);
				baseCover_CS.LayerID = int.Parse(array3[1]);
				baseCover_CS.CS_Position = new PointF(float.Parse(array3[2]), float.Parse(array3[3]));
				baseCover_CS.StartTime = int.Parse(array3[4]);
				baseCover_CS.Duration = int.Parse(array3[5]);
				baseCover_CS.ScaleWidth = int.Parse(array3[6]);
				baseCover_CS.ScaleLength = int.Parse(array3[7]);
				baseCover_CS.Roundness = bool.Parse(array3[8]);
				baseCover_CS.Type = int.Parse(array3[9]);
				baseCover_CS.CtrlID = int.Parse(array3[10]);
				baseCover_CS.Velocity = float.Parse(array3[11]);
				baseCover_CS.DirectionDegree = float.Parse(array3[12]);
				baseCover_CS.AccelerateCS = float.Parse(array3[14]);
				baseCover_CS.AccDirection = float.Parse(array3[15]);
				string text9 = array3[17];
				for (int num15 = 0; num15 < text9.Split(new char[1] { '&' }).Length - 1; num15++)
				{
					string text10 = text9.Split(new char[1] { '&' })[num15];
					EventGroup eventGroup5 = new EventGroup();
					eventGroup5.index = num15;
					eventGroup5.tag = text10.Split(new char[1] { '|' })[0];
					eventGroup5.t = int.Parse(text10.Split(new char[1] { '|' })[1]);
					eventGroup5.addtime = int.Parse(text10.Split(new char[1] { '|' })[2]);
					for (int num16 = 0; num16 < text10.Split(new char[1] { '|' })[3].Split(new char[1] { ';' }).Length - 1; num16++)
					{
						Event obj5 = new Event();
						obj5.EventString = text10.Split(new char[1] { '|' })[3].Split(new char[1] { ';' })[num16];
						obj5.String2EmitterEvent();
						eventGroup5.EventList.Add(obj5);
					}
					baseCover_CS.EventGroupList.Add(eventGroup5);
				}
				text9 = array3[18];
				for (int num17 = 0; num17 < text9.Split(new char[1] { '&' }).Length - 1; num17++)
				{
					string text11 = text9.Split(new char[1] { '&' })[num17];
					EventGroup eventGroup6 = new EventGroup();
					eventGroup6.index = num17;
					eventGroup6.tag = text11.Split(new char[1] { '|' })[0];
					eventGroup6.t = int.Parse(text11.Split(new char[1] { '|' })[1]);
					eventGroup6.addtime = int.Parse(text11.Split(new char[1] { '|' })[2]);
					for (int num18 = 0; num18 < text11.Split(new char[1] { '|' })[3].Split(new char[1] { ';' }).Length - 1; num18++)
					{
						Event obj6 = new Event();
						obj6.EventString = text11.Split(new char[1] { '|' })[3].Split(new char[1] { ';' })[num18];
						obj6.String2BulletEvent();
						eventGroup6.EventList.Add(obj6);
					}
					baseCover_CS.SubEventGroupList.Add(eventGroup6);
				}
				baseCover_CS.RanVelocity = float.Parse(array3[19]);
				baseCover_CS.RanDirection = float.Parse(array3[20]);
				baseCover_CS.RanAccelerate = float.Parse(array3[21]);
				baseCover_CS.RanAccDirection = float.Parse(array3[22]);
				if (array3.Length >= 24)
				{
					baseCover_CS.BindingID = int.Parse(array3[23]);
				}
				if (array3.Length >= 25 && array3[24] != "")
				{
					baseCover_CS.DeepBinding = bool.Parse(array3[24]);
				}
				EmitterList.Add(baseCover_CS);
			}
			num5 = int.Parse(text.Split(new char[1] { ':' })[1].Split(new char[1] { ',' })[6]);
			for (int num19 = 0; num19 < num5; num19++)
			{
				_ = MBGtext[num++];
			}
			num5 = int.Parse(text.Split(new char[1] { ':' })[1].Split(new char[1] { ',' })[7]);
			for (int num20 = 0; num20 < num5; num20++)
			{
				_ = MBGtext[num++];
			}
		}
	}

	private void AddNode(int id, TreeNode treeNode)
	{
		EmitterList.ForEach((BaseEmitter_CS b) =>
		{
			//IL_0044: Unknown result type (might be due to invalid IL or missing references)
			//IL_004a: Expected Obj, but got Unknown
			if (b.BindingID == id)
			{
				string text = ((!(b is BaseCover_CS)) ? b.EmitterMode.ToString() : "Cover");
				TreeNode val = new TreeNode(b.ID.ToString());
				val.Name = b.ID.ToString();
				val.Text = "ID：" + b.ID + "  " + text;
				treeNode.Nodes.Add(val);
				AddNode(b.ID, val);
			}
		});
	}
}
