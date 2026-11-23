using System;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace Thsss.Browser
{
	public static class WasmExports
	{
		private static Task boot;

		[UnmanagedCallersOnly(EntryPoint = "thsss_start")]
		public static void Start()
		{
			if (boot != null)
			{
				return;
			}
			try
			{
				boot = Run();
			}
			catch (Exception ex)
			{
				boot = Task.FromException(ex);
				Report(ex);
			}
		}

		[UnmanagedCallersOnly(EntryPoint = "thsss_ready")]
		public static int Ready()
		{
			if (boot == null)
			{
				return 0;
			}
			if (boot.IsFaulted || boot.IsCanceled)
			{
				return -1;
			}
			return boot.IsCompletedSuccessfully ? 1 : 0;
		}

		[UnmanagedCallersOnly(EntryPoint = "thsss_frame")]
		public static void Frame()
		{
			try
			{
				GameHost.Frame();
			}
			catch (Exception ex)
			{
				Report(ex);
			}
		}

		[UnmanagedCallersOnly(EntryPoint = "thsss_simulate")]
		public static void Simulate()
		{
			try
			{
				GameHost.Simulate();
			}
			catch (Exception ex)
			{
				Report(ex);
			}
		}

		[UnmanagedCallersOnly(EntryPoint = "thsss_draw")]
		public static void Draw(float alpha)
		{
			try
			{
				GameHost.Draw(alpha);
			}
			catch (Exception ex)
			{
				Report(ex);
			}
		}

		[UnmanagedCallersOnly(EntryPoint = "thsss_input_surface")]
		public static int InputSurface()
		{
			try
			{
				return GameHost.TouchSurface();
			}
			catch (Exception ex)
			{
				Report(ex);
				return 0;
			}
		}

		[UnmanagedCallersOnly(EntryPoint = "thsss_touch_residual_x")]
		public static float TouchResidualX()
		{
			try
			{
				return GameHost.TakeTouchResidualX();
			}
			catch (Exception ex)
			{
				Report(ex);
				return 0f;
			}
		}

		[UnmanagedCallersOnly(EntryPoint = "thsss_touch_residual_y")]
		public static float TouchResidualY()
		{
			try
			{
				return GameHost.TakeTouchResidualY();
			}
			catch (Exception ex)
			{
				Report(ex);
				return 0f;
			}
		}

		[UnmanagedCallersOnly(EntryPoint = "thsss_practice_configure")]
		public static void PracticeConfigure(int enabled, int invulnerable, int disableBomb, int ignoreFocusLoss, int life, int spell, int power)
		{
			try
			{
				PracticeSession.Configure(enabled, invulnerable, disableBomb, ignoreFocusLoss, life, spell, power);
			}
			catch (Exception ex)
			{
				Report(ex);
			}
		}

		[UnmanagedCallersOnly(EntryPoint = "thsss_practice_warp")]
		public static unsafe void PracticeWarp(byte* stage)
		{
			try
			{
				PracticeSession.RequestWarp(WasmText.Read(stage));
			}
			catch (Exception ex)
			{
				Report(ex);
			}
		}

		[UnmanagedCallersOnly(EntryPoint = "thsss_practice_resources")]
		public static void PracticeResources()
		{
			try
			{
				PracticeSession.RequestResources();
			}
			catch (Exception ex)
			{
				Report(ex);
			}
		}

		[UnmanagedCallersOnly(EntryPoint = "thsss_set_touch_input_enabled")]
		public static void SetTouchInputEnabled(int enabled)
		{
			try
			{
				GameHost.SetTouchInputEnabled(enabled != 0);
			}
			catch (Exception ex)
			{
				Report(ex);
			}
		}

		[UnmanagedCallersOnly(EntryPoint = "thsss_touch_shift")]
		public static int TouchShift(float dx, float dy, int unlimited)
		{
			try
			{
				return GameHost.TouchShift(dx, dy, unlimited);
			}
			catch (Exception ex)
			{
				Report(ex);
				return 0;
			}
		}

		[UnmanagedCallersOnly(EntryPoint = "thsss_on_key")]
		public static unsafe void OnKey(byte* name, int down)
		{
			try
			{
				GameHost.OnKey(WasmText.Read(name), down);
			}
			catch (Exception ex)
			{
				Report(ex);
			}
		}

		[UnmanagedCallersOnly(EntryPoint = "thsss_fps_unlocked")]
		public static int FpsUnlocked()
		{
			try
			{
				return GameHost.FpsUnlocked();
			}
			catch (Exception ex)
			{
				Report(ex);
				return 0;
			}
		}

		[UnmanagedCallersOnly(EntryPoint = "thsss_free")]
		public static unsafe void Free(byte* ptr)
		{
			NativeMemory.Free(ptr);
		}

		[UnmanagedCallersOnly(EntryPoint = "thsss_set_fps_unlocked")]
		public static void SetFpsUnlocked(int value)
		{
			try
			{
				GameHost.SetFpsUnlocked(value);
			}
			catch (Exception ex)
			{
				Report(ex);
			}
		}

		private static async Task Run()
		{
			try
			{
				await GameHost.Start();
			}
			catch (Exception ex)
			{
				Report(ex);
				throw;
			}
		}

		private static void Report(Exception ex)
		{
			Console.WriteLine(ex);
			try
			{
				ThsssJs.SetStatus(ex.ToString());
			}
			catch (Exception)
			{
			}
		}
	}
}
