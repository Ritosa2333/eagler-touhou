using System;

namespace Thsss.Browser
{
	public static class PracticeSession
	{
		public static bool Enabled;

		public static bool Invulnerable;

		public static bool DisableBomb;

		public static bool IgnoreFocusLoss;

		public static int Life = -1;

		public static int Spell = -1;

		public static int Power = -1;

		public static string Warp = "";

		public static bool ResourcesPending;

		public static void Configure(int enabled, int invulnerable, int disableBomb, int ignoreFocusLoss, int life, int spell, int power)
		{
			Enabled = enabled != 0;
			Invulnerable = Enabled && invulnerable != 0;
			DisableBomb = Enabled && disableBomb != 0;
			IgnoreFocusLoss = Enabled && ignoreFocusLoss != 0;
			Life = Enabled ? Clamp(life, 0, 8) : -1;
			Spell = Enabled ? Clamp(spell, 0, 8) : -1;
			Power = Enabled ? Clamp(power, 0, 400) : -1;
			if (!Enabled)
			{
				Warp = "";
				ResourcesPending = false;
			}
		}

		public static void RequestWarp(string stage)
		{
			if (!Enabled || string.IsNullOrEmpty(stage)) return;
			Warp = stage;
		}

		public static void RequestResources()
		{
			if (!Enabled) return;
			ResourcesPending = true;
		}

		public static string TakeWarp()
		{
			var stage = Warp;
			Warp = "";
			return stage ?? "";
		}

		public static string Trailer()
		{
			if (!Enabled) return "";
			return "Prac=1," + Bit(Invulnerable) + "," + Bit(DisableBomb) + "," + Bit(IgnoreFocusLoss)
				+ "," + Life + "," + Spell + "," + Power;
		}

		public static void RestoreTrailer(string line)
		{
			if (string.IsNullOrEmpty(line) || !line.StartsWith("Prac=1,", StringComparison.Ordinal)) return;
			var parts = line.Substring(7).Split(',');
			if (parts.Length < 6) return;
			Enabled = true;
			Invulnerable = parts[0] == "1";
			DisableBomb = parts[1] == "1";
			IgnoreFocusLoss = parts[2] == "1";
			Life = Parse(parts[3], 0, 8);
			Spell = Parse(parts[4], 0, 8);
			Power = Parse(parts[5], 0, 400);
			ResourcesPending = true;
		}

		private static int Bit(bool value)
		{
			return value ? 1 : 0;
		}

		private static int Clamp(int value, int min, int max)
		{
			if (value < min) return min;
			if (value > max) return max;
			return value;
		}

		private static int Parse(string text, int min, int max)
		{
			int value;
			if (!int.TryParse(text, out value)) return min;
			return Clamp(value, min, max);
		}
	}
}
