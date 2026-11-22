using SlimDX.Direct3D9;

namespace Shooting;

public class ScreenTexManager
{
	public ScreenTexture[] ScreenTex { get; private set; }

	private int ScreenLevel { get; set; }

	public Texture RenderTexture => ScreenTex[ScreenLevel].renderTexture;

	public ScreenTexManager(Device DeviceMain)
	{
		ScreenTex = new ScreenTexture[5];
		for (int i = 0; i < ScreenTex.Length; i++)
		{
			ScreenTex[i] = new ScreenTexture(DeviceMain, 640, 480);
		}
	}

	public void Begin()
	{
		ScreenTex[ScreenLevel++].Begin();
	}

	public void End()
	{
		ScreenTex[--ScreenLevel].End();
	}

	public void Reset()
	{
		for (int i = 0; i < ScreenTex.Length; i++)
		{
			if (ScreenTex[i] != null)
			{
				ScreenTex[i].Reset();
			}
		}
	}

	public void Dispose()
	{
		for (int i = 0; i < ScreenTex.Length; i++)
		{
			if (ScreenTex[i] != null)
			{
				ScreenTex[i].Dispose();
			}
		}
	}
}
