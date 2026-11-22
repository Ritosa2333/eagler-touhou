using System.Collections.Generic;

namespace Shooting;

public class BaseStory : BaseObject
{
	public List<BaseObject> StoryEffectList = new List<BaseObject>();

	private bool Z = true;

	public BaseStory()
	{
	}

	public BaseStory(StageDataPackage StageData)
	{
		base.StageData = StageData;
		Story = this;
		if (Boss != null)
		{
			Boss.Enabled = false;
		}
	}

	public override void Ctrl()
	{
		if (KClass.Key_Z || KClass.Key_Ctrl)
		{
			if (!Z)
			{
				Time++;
				StageData.SoundPlay("se_plst00.wav");
			}
			if (!KClass.Key_Ctrl)
			{
				Z = true;
			}
		}
		else
		{
			Z = false;
		}
	}
}
