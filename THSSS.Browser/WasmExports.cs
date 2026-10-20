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

		[UnmanagedCallersOnly(EntryPoint = "thsss_touch_shift")]
		public static void TouchShift(float dx, float dy)
		{
			try
			{
				GameHost.TouchShift(dx, dy);
			}
			catch (Exception ex)
			{
				Report(ex);
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
