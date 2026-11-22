using System.Collections.Generic;

namespace Shooting;

public class BackgroundManager
{
	private int light;

	public int Light
	{
		get
		{
			return light;
		}
		set
		{
			if (value < 0)
			{
				light = 0;
			}
			else if (value > 255)
			{
				light = 255;
			}
			else
			{
				light = value;
			}
		}
	}

	public List<BaseObject> BackgroundList { get; set; }

	public BackgroundManager()
	{
		Light = 255;
		BackgroundList = new List<BaseObject>();
	}

	public void Ctrl()
	{
		BackgroundList.ForEach((BaseObject x) =>
		{
			if (x.OutBoundary())
			{
				BackgroundList.Remove(x);
			}
		});
		BackgroundList.ForEach((BaseObject x) =>
		{
			x.Ctrl();
		});
	}

	public void Show(bool Active)
	{
		if (Active)
		{
			BackgroundList.ForEach((BaseObject x) =>
			{
				if (x.Active)
				{
					x.Show();
				}
			});
			return;
		}
		BackgroundList.ForEach((BaseObject x) =>
		{
			if (!x.Active)
			{
				x.Show();
			}
		});
	}

	public void Clear()
	{
		BackgroundList.Clear();
	}
}
