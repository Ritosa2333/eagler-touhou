using System;
using System.Collections.Generic;
using System.IO;
using System.Security.Cryptography;
using System.Windows.Forms;
using SlimDX.Audio;
using SlimDX.DirectInput;
using SlimDX.Gl;
using Shooting;

namespace Thsss.Browser
{
	public static partial class GameHost
	{
		private static Game_Main game;

		private static int frame;

		private static bool unlockFps;

		private static bool exited;

		private static bool exitNotified;

		private static readonly Dictionary<string, string> hashes = new Dictionary<string, string>();

		public static async System.Threading.Tasks.Task Start()
		{
			GlHost.Api = new JsGlApi();
			AudioHost.Api = new JsAudioApi();
			System.Drawing.GlyphRaster.Draw = delegate(string text, int width, int height, float sizePx, bool bold, bool italic, int argb, string family)
			{
				return ThsssJs.RasterTextBox(text, width, height, sizePx, bold, italic, argb, family ?? "sans-serif");
			};
			await ContentLoader.LoadAsync();
			ThsssJs.SetStatus("Creating the Direct3D device…");
			game = new Game_Main();
			if (!game.initSuccess)
			{
				throw new InvalidOperationException("Game_Main did not finish device init.");
			}
			FlushSaves();
			try
			{
				unlockFps = new INI_RW(".\\Setting.INI").IniReadValue("Mode", "UnlockFPS") != "0";
			}
			catch (Exception)
			{
				unlockFps = true;
			}
			Game_Main.RefreshUnlocked = unlockFps;
			ThsssJs.SetStatus("Device ready. Click the canvas. Arrows move, Z shoots, X bombs, Shift slows, Esc pauses.");
		}

		public static void MarkExited()
		{
			exited = true;
		}

		public static void NotifyClosed()
		{
			exited = true;
			if (exitNotified)
			{
				return;
			}
			exitNotified = true;
			try
			{
				ThsssJs.NotifyExit();
			}
			catch (Exception)
			{
			}
		}

		public static int FpsUnlocked()
		{
			return unlockFps ? 1 : 0;
		}

		public static void SetFpsUnlocked(int value)
		{
			unlockFps = value != 0;
			Game_Main.RefreshUnlocked = unlockFps;
			try
			{
				new INI_RW(".\\Setting.INI").IniWriteValue("Mode", "UnlockFPS", unlockFps ? "1" : "0");
			}
			catch (Exception)
			{
			}
		}

		public static void Frame()
		{
			if (exited || game == null || !game.initSuccess)
			{
				return;
			}
			ContentLoader.Pump(2);
			try
			{
				game.MainProcess();
			}
			catch (Exception ex)
			{
				ThsssJs.SetStatus(ex.ToString());
				Console.WriteLine(ex);
				game = null;
				return;
			}
			frame++;
			if (frame % 30 == 0)
			{
				FlushSaves();
			}
		}

		public static void Simulate()
		{
			if (exited || game == null || !game.initSuccess)
			{
				return;
			}
			ContentLoader.Pump(2);
			try
			{
				game.SimulateOnly();
			}
			catch (Exception ex)
			{
				ThsssJs.SetStatus(ex.ToString());
				Console.WriteLine(ex);
				game = null;
				return;
			}
			frame++;
			if (frame % 30 == 0)
			{
				FlushSaves();
			}
		}

		public static void Draw(float alpha)
		{
			if (exited || game == null || !game.initSuccess)
			{
				return;
			}
			try
			{
				game.DrawOnly(alpha);
			}
			catch (Exception ex)
			{
				ThsssJs.SetStatus(ex.ToString());
				Console.WriteLine(ex);
				game = null;
			}
		}

		public static int TouchSurface()
		{
			if (game == null)
			{
				return 0;
			}
			return game.TouchSurface();
		}

		private static float touchResidualX;

		private static float touchResidualY;

		public static bool TouchInputEnabled { get; private set; }

		public static void SetTouchInputEnabled(bool enabled)
		{
			TouchInputEnabled = enabled;
		}

		public static void StoreTouchResidual(float x, float y)
		{
			touchResidualX = x;
			touchResidualY = y;
		}

		public static float TakeTouchResidualX()
		{
			return touchResidualX;
		}

		public static float TakeTouchResidualY()
		{
			var y = touchResidualY;
			touchResidualX = 0f;
			touchResidualY = 0f;
			return y;
		}

		public static int TouchShift(float dx, float dy, int unlimited)
		{
			if (!TouchInputEnabled || game == null)
			{
				return 0;
			}
			return game.TouchShift(dx, dy, unlimited);
		}

		public static void OnKey(string name, int down)
		{
			if (game == null || game.Form_Main == null || string.IsNullOrEmpty(name))
			{
				return;
			}
			var pressed = down != 0;
			if (pressed)
			{
				game.Form_Main.RaiseKeyDown(Keys.Parse(name));
			}
			else
			{
				game.Form_Main.RaiseKeyUp(Keys.Parse(name));
			}
			Key key;
			if (TryMap(name, out key))
			{
				if (pressed)
				{
					KeyboardStateMap.Down.Add(key);
				}
				else
				{
					KeyboardStateMap.Down.Remove(key);
				}
			}
		}

		private static bool TryMap(string name, out Key key)
		{
			switch (name)
			{
			case "Left": key = Key.LeftArrow; return true;
			case "Right": key = Key.RightArrow; return true;
			case "Up": key = Key.UpArrow; return true;
			case "Down": key = Key.DownArrow; return true;
			case "ShiftKey": key = Key.LeftShift; return true;
			case "ControlKey": key = Key.LeftControl; return true;
			case "Menu": key = Key.LeftAlt; return true;
			case "Return": key = Key.Return; return true;
			case "Escape": key = Key.Escape; return true;
			case "Space": key = Key.Space; return true;
			case "Z": key = Key.Z; return true;
			case "X": key = Key.X; return true;
			case "P": key = Key.P; return true;
			case "R": key = Key.R; return true;
			case "OemMinus": key = Key.Minus; return true;
			case "Oemplus": key = Key.Equals; return true;
			case "F8": key = Key.F8; return true;
			default:
				key = Key.Escape;
				return false;
			}
		}

		private static void FlushSaves()
		{
			Remember("Setting.INI");
			Remember("Score.dat");
			Remember("Log.txt");
			if (Directory.Exists("Replay"))
			{
				foreach (var file in Directory.GetFiles("Replay"))
				{
					Remember(file);
				}
			}
		}

		private static void Remember(string path)
		{
			if (!File.Exists(path))
			{
				return;
			}
			var bytes = File.ReadAllBytes(path);
			string hash;
			using (var sha = SHA256.Create())
			{
				hash = Convert.ToBase64String(sha.ComputeHash(bytes));
			}
			string previous;
			if (hashes.TryGetValue(path, out previous) && previous == hash)
			{
				return;
			}
			hashes[path] = hash;
			ThsssJs.WriteSave(path.Replace('\\', '/'), Convert.ToBase64String(bytes));
		}
	}
}
