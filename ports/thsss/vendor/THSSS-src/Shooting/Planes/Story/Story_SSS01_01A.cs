using System.Drawing;

namespace Shooting.Planes.Story;

internal class Story_SSS01_01A : BaseStory_SSS
{
	public Story_SSS01_01A(StageDataPackage StageData)
		: base(StageData)
	{
		LoadConversation2(".\\Story\\1-1A.csv");
	}

	public override void Ctrl()
	{
		base.Ctrl();
		if (Time == NameTime && LastTime < Time)
		{
			LastTime = Time;
			CharN = new CharacterName(StageData, "ename_Ami");
			new StoryEmitterStar(StageData, CharN.Position, 0f, 0.0);
		}
		if (Time == MusicTime && LastTime < Time)
		{
			LastTime = Time;
			StageData.ChangeBGM(".\\BGM\\Boss01.wav", 0, 0, 255, 754110, 3294270);
		}
		if (Time == Conv.Count)
		{
			new MusicTitle(StageData, "喧闹吧！在这不眠之夜", new Point(BoundRect.Width, BoundRect.Height - 16))
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
