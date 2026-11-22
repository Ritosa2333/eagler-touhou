using System.Drawing;

namespace Shooting.Planes.Story;

internal class Story_SSS03_01D : BaseStory_SSS
{
	public Story_SSS03_01D(StageDataPackage StageData)
		: base(StageData)
	{
		LoadConversation(".\\Story\\3-1D.csv");
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == NameTime && LastTime < Time)
		{
			CharN = new CharacterName(StageData, "ename_Seiryuu");
			new StoryEmitterStar(StageData, CharN.Position, 0f, 0.0);
			LastTime = Time;
		}
		else if (Time == MusicTime && LastTime < Time)
		{
			StageData.ChangeBGM(".\\BGM\\Boss03.wav", 0, 0, 255, 1286397, 6275430);
			LastTime = Time;
		}
		else if (Time == Conv.Count)
		{
			new MusicTitle(StageData, "青柳传说", new Point(BoundRect.Width, BoundRect.Height - 16))
			{
				OriginalPosition = new PointF(BoundRect.Width, BoundRect.Height + 100),
				Scale = 0.5f
			};
			Boss.Enabled = true;
			SBox.Dispose();
			Story = null;
			new EmitterBossFire(StageData, Boss.OriginalPosition, Color.FromArgb(40, 255, 20));
			new MagicCircle(StageData, "MagicCircleSix").Scale = 1.5f;
			Background3D.WarpEnabled = true;
		}
	}
}
