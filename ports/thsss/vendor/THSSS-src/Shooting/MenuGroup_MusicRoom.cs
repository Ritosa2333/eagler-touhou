using System.Collections.Generic;
using System.Drawing;
using Shooting.Planes.Story;

namespace Shooting;

internal class MenuGroup_MusicRoom : BaseMenuGroup
{
	private DescriptionBox DescriptionBox;

	public MenuGroup_MusicRoom(StageDataPackage StageData)
		: base(StageData)
	{
		MenuSelectIndex = 0;
		MenuItemList = new List<BaseMenuItem>
		{
			new BaseMenuItem(StageData, "被遗忘的久远星辉"),
			new BaseMenuItem(StageData, "宁静夏夜的微风"),
			new BaseMenuItem(StageData, "喧闹吧！在这不眠之夜"),
			new BaseMenuItem(StageData, "灯火竹林"),
			new BaseMenuItem(StageData, "疾风闪电"),
			new BaseMenuItem(StageData, "木灵们的夏夜祭"),
			new BaseMenuItem(StageData, "青柳传说"),
			new BaseMenuItem(StageData, "万花世界的光与影"),
			new BaseMenuItem(StageData, "镜中的幻象"),
			new BaseMenuItem(StageData, "记忆中遥远的星星"),
			new BaseMenuItem(StageData, "引燃夜空的星火"),
			new BaseMenuItem(StageData, "银河的彼方"),
			new BaseMenuItem(StageData, "？？？？？"),
			new BaseMenuItem(StageData, "闪耀在世界尽头"),
			new BaseMenuItem(StageData, "晚星之梦"),
			new BaseMenuItem(StageData, "璀璨无比的星空"),
			new BaseMenuItem(StageData, "来自仙界的新风"),
			new BaseMenuItem(StageData, "风中花，雪中月")
		};
		int num = 100;
		int num2 = 100;
		foreach (BaseMenuItem menuItem in MenuItemList)
		{
			menuItem.Position = new PointF(num, 90f);
			menuItem.DestPoint = new PointF(num, num2);
			menuItem.MaxVelocity = 100f;
			num2 += 30;
		}
		MenuItemList[MenuSelectIndex].Selected = true;
		MenuTitlePos1 = new PointF(140f, -30f);
		MenuTitlePos2 = new PointF(140f, -150f);
		MenuTilte = new BaseMenuItem(StageData, "MenuTitle_MusicRoom")
		{
			Position = MenuTitlePos2
		};
		TransparentValueF = 0f;
		TransparentVelocity = 5f;
		TxtureObject = TextureObjectDictionary["MenuBackground"];
		Scale = 0.625f;
		OriginalPosition = new PointF(BoundRect.Width / 2, BoundRect.Height / 2);
		AngleDegree = 90.0;
		ColorValue = Color.SlateBlue;
		DescriptionBox = new DescriptionBox(StageData)
		{
			MaxTransparent = 0,
			OffsetX = 50
		};
	}

	public override void SelectItemChanged()
	{
		base.SelectItemChanged();
		int num = 100;
		int num2 = 100;
		int num3 = (MenuSelectIndex - 6) * 30;
		if (num3 < 0)
		{
			num3 = 0;
		}
		foreach (BaseMenuItem menuItem in MenuItemList)
		{
			menuItem.DestPoint = new PointF(num, num2 - num3);
			num2 += 30;
		}
	}

	public override void Ctrl()
	{
		base.Ctrl();
		DescriptionBox.Ctrl();
		foreach (BaseMenuItem menuItem in MenuItemList)
		{
			if (menuItem.DestPoint.Y < 80f || menuItem.DestPoint.Y > 296f)
			{
				menuItem.MaxTransparent -= 15;
			}
			else
			{
				menuItem.MaxTransparent += 15;
			}
		}
	}

	public override void ProcessKeys()
	{
		base.ProcessKeys();
		if (KClass.Key_Z && LastZ == 0)
		{
			new StageDataPackage(StageData.GlobalData);
			switch (MenuItemList[MenuSelectIndex].Name)
			{
			case "被遗忘的久远星辉":
				DescriptionBox.Text = "东方惯例的标题曲。  ※作曲：文件";
				DescriptionBox.Text += "\n在东方标题曲特有的旋律上进行了改动，";
				DescriptionBox.Text += "\n不过这样一来旋律似乎就不明显了。";
				DescriptionBox.Text += "\n整体风格略为阴暗，似乎和积极向上（？）的主题不符？";
				StageData.ChangeBGM(".\\BGM\\OP.wav", 0, 0, 255, 604611, 6047874);
				break;
			case "宁静夏夜的微风":
				DescriptionBox.Text = "1面的主题曲。      ※作曲：11";
				DescriptionBox.Text += "\n因为是夏夜的微风，所以开头做成了格外静谧的感觉。";
				DescriptionBox.Text += "\n这样的开始之后，才是渐渐地进入了游戏。";
				DescriptionBox.Text += "\n旋律是怎么写出来的，其实我也不知道。仅仅是在脑中";
				DescriptionBox.Text += "\n突然出现了这段后赶紧记录下来罢了，效果却意外的好呢。";
				StageData.ChangeBGM(".\\BGM\\Stage01.wav", 88200, 0, 255, 0, 0);
				break;
			case "喧闹吧！在这不眠之夜":
				DescriptionBox.Text = "娅米的主题曲。      ※作曲：文件";
				DescriptionBox.Text += "\n按照1面的感觉做出的曲子。";
				DescriptionBox.Text += "\n虽然连贯性不错，但是听起来并不像1面Boss？";
				DescriptionBox.Text += "\n妖精好萌。";
				StageData.ChangeBGM(".\\BGM\\Boss01.wav", 44100, 0, 255, 754110, 3294270);
				break;
			case "灯火竹林":
				DescriptionBox.Text = "2面的主题曲。      ※作曲：11";
				DescriptionBox.Text += "\n一首自始至终都很悠闲自由的曲子，尤其是在旋律上";
				DescriptionBox.Text += "\n随心所欲的发挥。在悠闲之余，还延续了前一面的静谧。";
				DescriptionBox.Text += "\n整首曲子莫名其妙的就做成了钢琴曲。";
				DescriptionBox.Text += "\n不过好像怎么样都不会让人联想到竹林啊。";
				StageData.ChangeBGM(".\\BGM\\Stage02.wav", 88200, 0, 255, 1194228, 6866370);
				break;
			case "疾风闪电":
				DescriptionBox.Text = "桃雨 洛熙的主题曲。 ※作曲：文件";
				DescriptionBox.Text += "\n悠闲的2面之后接着就是这样紧迫的曲子，真的没问题吗。";
				DescriptionBox.Text += "\n虽然的确有Boss的风范，但是听起来并不像2面Boss？";
				DescriptionBox.Text += "\n不过弹幕倒是和曲子一样紧迫。";
				StageData.ChangeBGM(".\\BGM\\Boss02.wav", 44100, 0, 255, 327663, 3504627);
				break;
			case "木灵们的夏夜祭":
				DescriptionBox.Text = "3面的主题曲。      ※作曲：11";
				DescriptionBox.Text += "\n进入三面，游戏也要逐渐热闹起来了。";
				DescriptionBox.Text += "\n在旋律上下了不少功夫，试着把钢琴做成了行云流水的样子";
				DescriptionBox.Text += "\n在后面的风琴上，也尽量往旋律优美的方向去走。";
				DescriptionBox.Text += "\n才刚刚到三面而已啊，所以压制了一下过于激烈的感觉。";
				StageData.ChangeBGM(".\\BGM\\Stage03.wav", 88200, 0, 255, 88200, 5606874);
				break;
			case "青柳传说":
				DescriptionBox.Text = "夏 青柳的主题曲。   ※作曲：11";
				DescriptionBox.Text += "\n突出了远离尘世、不沾俗气的纯净风格。气势也做的很大。";
				DescriptionBox.Text += "\n试着使用了古风的元素，不过不是纯粹的古风。";
				DescriptionBox.Text += "\n面对这样一个设定，怎么能不倾注进感情来作曲呢？";
				DescriptionBox.Text += "\n于是，努力把这首曲子编成了更加有故事、有情感的曲子。";
				StageData.ChangeBGM(".\\BGM\\Boss03.wav", 44100, 0, 255, 1585395, 6571341);
				break;
			case "万花世界的光与影":
				StageData.ChangeBGM(".\\BGM\\Stage04.wav", 88200, 0, 255, 575990, 7775976);
				DescriptionBox.Text = "４面的主题曲。      ※作曲：文件";
				DescriptionBox.Text += "\n按照地灵殿的风格做的曲子（并没有说像地灵殿）。";
				DescriptionBox.Text += "\n虽然被多人吐槽说跑调（？）。";
				DescriptionBox.Text += "\n我不写半音了jojo！";
				break;
			case "镜中的幻象":
				StageData.ChangeBGM(".\\BGM\\Boss04.wav", 44100, 0, 255, 463491, 7541982);
				DescriptionBox.Text = "魅明魔 影的主题曲。 ※作曲：11";
				DescriptionBox.Text += "\n要做这首曲子的时候，首先在我脑中出现的是阴沉和激昂的";
				DescriptionBox.Text += "\n两种风格，于是直接混在一起了。";
				DescriptionBox.Text += "\n似乎加一些优美的旋律会被鬼畜旋律衬托得更优美一点？";
				DescriptionBox.Text += "\n三拍子太难了。";
				break;
			case "记忆中遥远的星星":
				StageData.ChangeBGM(".\\BGM\\Stage05.wav", 44100, 0, 255, 66150, 0);
				DescriptionBox.Text = "５面的主题曲。      ※作曲：文件";
				DescriptionBox.Text += "\n有好音源却用辣鸡音源是一种什么样的心态？";
				DescriptionBox.Text += "\n其实最后的贝斯部分是在偷懒。";
				break;
			case "引燃夜空的星火":
				StageData.ChangeBGM(".\\BGM\\Boss05.wav", 44100, 0, 255, 1000056, 6599345);
				DescriptionBox.Text = "桃雨 落薰的主题曲。 ※作曲：文件";
				DescriptionBox.Text += "\n历经三个版本若干小时才弄出来的曲子。";
				DescriptionBox.Text += "\n 然而时间和质量却成反比。";
				DescriptionBox.Text += "\n这一定是杰拉鲁星人的阴谋（确信）。";
				break;
			case "银河的彼方":
				StageData.ChangeBGM(".\\BGM\\Stage06.wav", 88200, 0, 255, 0, 6220746);
				DescriptionBox.Text = "６面的主题曲。      ※作曲：11";
				DescriptionBox.Text += "\n渲染星空的宁静的气氛的一首曲子。";
				DescriptionBox.Text += "\n刚开始是阴沉的曲调，之后瞬间展开的感觉。";
				DescriptionBox.Text += "\n有一段旋律居然用了四个调，真是神奇。";
				DescriptionBox.Text += "\n会让人想起流星雨的吧？激光流星雨什么的……";
				break;
			case "？？？？？":
				StageData.ChangeBGM(".\\BGM\\Boss06.wav", 44100, 0, 255, 1697409, 10789065);
				DescriptionBox.Text = "梦璃夜 天星的主题曲 ※作曲：11";
				DescriptionBox.Text += "\n想要做成有在梦中游荡的感觉的曲子，";
				DescriptionBox.Text += "\n毕竟，星空似乎常常和梦幻联系在一起吧？";
				DescriptionBox.Text += "\n据说是透露着哀思的旋律呢……";
				DescriptionBox.Text += "\n最终Boss的激烈感与星空的静谧与梦幻融合在一起的感觉。";
				break;
			case "闪耀在世界尽头":
				StageData.ChangeBGM(".\\BGM\\Boss06FSC.wav", 88200, 0, 255, 0, 0);
				DescriptionBox.Text = "梦璃夜 天星的FSC曲  ※作曲：11";
				DescriptionBox.Text += "\nFinal SpellCard的激烈的曲子。";
				DescriptionBox.Text += "\n开头之后，是不断的反复和升调。";
				DescriptionBox.Text += "\n啊啊啊，战斗就这样结束了吗？";
				break;
			case "来自仙界的新风":
				StageData.ChangeBGM(".\\BGM\\StageEx.wav", 44100, 0, 255, 1341963, 7621362);
				DescriptionBox.Text = "Extra关卡的主题曲。 ※作曲：11";
				DescriptionBox.Text += "\n与东方传统的Ex曲子相比，是更为轻松的一首曲子，少了一";
				DescriptionBox.Text += "\n些紧迫感。但是旋律也丝毫没有松懈下来，曲速也不慢。";
				DescriptionBox.Text += "\n在主线关卡的激烈战斗之后，偶尔用这首曲子来轻松一下也";
				DescriptionBox.Text += "\n不是坏事吧。但是不知道游戏中的弹幕允不允许呢？";
				break;
			case "风中花，雪中月":
				StageData.ChangeBGM(".\\BGM\\BossEx.wav", 44100, 0, 255, 1125873, 10783332);
				DescriptionBox.Text = "雾隐 梨花的主题曲。 ※作曲：11";
				DescriptionBox.Text += "\n试着做成一首充满着“东之国”的风格的曲子。";
				DescriptionBox.Text += "\n与青柳相似的曲风，但这首要更加欢快一些。想要让曲子更";
				DescriptionBox.Text += "\n能打动人心，但似乎很难感受到来自ExBoss的压力呢。";
				DescriptionBox.Text += "\n一定是书生的错。";
				break;
			case "晚星之梦":
				StageData.ChangeBGM(".\\BGM\\ED.wav", 44100, 0, 0, 0, 0);
				DescriptionBox.Text = "Ending画面的主题曲  ※作曲：11";
				DescriptionBox.Text += "\n东方惯例的ED曲，东方惯例的音色。";
				DescriptionBox.Text += "\n十分安静的曲子。";
				DescriptionBox.Text += "\n然后这就完全不是东方风了吧？";
				break;
			case "璀璨无比的星空":
				StageData.ChangeBGM(".\\BGM\\Staff.wav", 44100, 0, 0, 0, 0);
				DescriptionBox.Text = "Staff画面的主题曲。 ※作曲：文件";
				DescriptionBox.Text += "\n本来是想加上1~6boss全部旋律的solo部分，然而并不能做";
				DescriptionBox.Text += "\n到，惊了。于是最后只加入了1~3boss要素。";
				DescriptionBox.Text += "\n一定是我太菜了，接下来人像/狂人的姿势/余烬会很有用。";
				break;
			default:
				StageData.StateSwitchData = new StateSwitchDataPackage
				{
					NextState = "此功能尚未开通",
					NeedInit = true,
					SDPswitch = StageData
				};
				break;
			}
		}
		if ((!KClass.Key_X && !KClass.Key_ESC) || LastX != 0)
		{
			return;
		}
		OnRemoveMenu = TimeMain + 20;
		TransparentVelocity = -15f;
		DescriptionBox.TransparentVelocity = -15f;
		foreach (BaseMenuItem menuItem in MenuItemList)
		{
			menuItem.DestPoint = new PointF(100f, 60f);
			menuItem.OnRemove = true;
		}
		StageData.SoundPlay("se_cancel00.wav");
	}

	public override void Show()
	{
		base.Show();
		DescriptionBox.Show();
	}
}
