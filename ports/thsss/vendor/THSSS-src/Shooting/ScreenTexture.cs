using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

public class ScreenTexture
{
	public Texture renderTexture { get; private set; }

	private Surface oldRenderSurface { get; set; }

	private Surface oldDepthStencilSurface { get; set; }

	private Surface renderSurface { get; set; }

	private Surface renderDepthStencilSurface { get; set; }

	private Device DeviceMain { get; set; }

	private int WindowWidth { get; set; }

	private int WindowHeight { get; set; }

	public ScreenTexture(Device DeviceMain, int WindowWidth, int WindowHeight)
	{
		this.DeviceMain = DeviceMain;
		this.WindowWidth = WindowWidth;
		this.WindowHeight = WindowHeight;
		while (!SetupRenderTexture())
		{
		}
	}

	private bool SetupRenderTexture()
	{
		//IL_0018: Unknown result type (might be due to invalid IL or missing references)
		//IL_0022: Expected Obj, but got Unknown
		try
		{
			renderTexture = new Texture(DeviceMain, WindowWidth, WindowHeight, 1, (Usage)1, (Format)22, (Pool)0);
			renderSurface = renderTexture.GetSurfaceLevel(0);
			renderDepthStencilSurface = Surface.CreateDepthStencil(DeviceMain, WindowWidth, WindowHeight, (Format)80, (MultisampleType)0, 0, true);
			return true;
		}
		catch
		{
			return false;
		}
	}

	public void Begin()
	{
		//IL_0030: Unknown result type (might be due to invalid IL or missing references)
		//IL_0062: Unknown result type (might be due to invalid IL or missing references)
		//IL_006d: Unknown result type (might be due to invalid IL or missing references)
		oldRenderSurface = DeviceMain.GetRenderTarget(0);
		oldDepthStencilSurface = DeviceMain.DepthStencilSurface;
		DeviceMain.SetRenderTarget(0, renderSurface);
		DeviceMain.DepthStencilSurface = renderDepthStencilSurface;
		DeviceMain.Clear((ClearFlags)3, new Color4(0f, 0f, 0f, 0f), 1f, 0);
	}

	public void End()
	{
		//IL_000d: Unknown result type (might be due to invalid IL or missing references)
		DeviceMain.SetRenderTarget(0, oldRenderSurface);
		DeviceMain.DepthStencilSurface = oldDepthStencilSurface;
	}

	public void Reset()
	{
		SetupRenderTexture();
	}

	public void Dispose()
	{
		if (renderSurface != null)
		{
			((ComObject)renderSurface).Dispose();
		}
		if (renderDepthStencilSurface != null)
		{
			((ComObject)renderDepthStencilSurface).Dispose();
		}
		if (renderTexture != null)
		{
			((ComObject)renderTexture).Dispose();
		}
		if (oldDepthStencilSurface != null)
		{
			((ComObject)oldDepthStencilSurface).Dispose();
		}
		if (oldRenderSurface != null)
		{
			((ComObject)oldRenderSurface).Dispose();
		}
	}
}
