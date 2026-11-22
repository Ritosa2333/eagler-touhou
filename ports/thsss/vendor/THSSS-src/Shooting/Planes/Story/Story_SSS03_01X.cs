using System.Drawing;

namespace Shooting.Planes.Story;

internal class Story_SSS03_01X : BaseStory_SSS
{
	public Story_SSS03_01X(StageDataPackage StageData)
		: base(StageData)
	{
		CharR = new CharacterR_Touhou(StageData, "FaceSeiryuu_no")
		{
			TxtureObject2 = TextureObjectDictionary[" "],
			OriginalPosition = new PointF(BoundRect.Width - 60, 380f)
		};
		CharL = new CharacterL_Touhou(StageData, "Myplane")
		{
			TxtureObject2 = TextureObjectDictionary["Myplane"]
		};
		SBox = new StoryBox(StageData);
		CharN = new CharacterName(StageData, "ename_Seiryuu");
		new StoryEmitterStar(StageData, CharN.Position, 0f, 0.0);
		Conv.Add(new Conversation("。。。", CharLAcitve: false, CharRAcitve: true, "Myplane", "FaceSeiryuu_no"));
		Conv.Add(new Conversation("。。。。。。", CharLAcitve: true, CharRAcitve: false, "Myplane", "FaceSeiryuu_no"));
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == Conv.Count)
		{
			Boss.Enabled = true;
			StageData.ChangeBGM(".\\BGM\\Boss03.wav", 0, 0, 255, 1286397, 6275430);
			new MusicTitle(StageData, "青柳传说", new Point(BoundRect.Width, BoundRect.Height - 16))
			{
				OriginalPosition = new PointF(BoundRect.Width, BoundRect.Height + 100),
				Scale = 0.5f
			};
			SBox.Dispose();
			Story = null;
			new EmitterBossFire(StageData, Boss.OriginalPosition, Color.FromArgb(40, 255, 20));
			new MagicCircle(StageData, "MagicCircleSix").Scale = 1.5f;
			Background3D.WarpEnabled = true;
		}
	}
}
