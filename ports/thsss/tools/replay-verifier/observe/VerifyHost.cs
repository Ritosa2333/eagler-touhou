using System;
using System.Collections;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Reflection;

namespace THSSS.Observe
{
	public static class VerifyHost
	{
		static bool anchored;
		static bool opened;
		static bool finished;
		static bool noted;
		static bool listen;
		static int samples;
		static StreamWriter writer;
		static StreamWriter rngWriter;

		public static void BeginFrame(object game)
		{
			if (opened)
			{
				listen = true;
				RngTape.Armed = true;
			}
		}

		public static void NoteRng(int result, int op, int a, int b)
		{
			if (!listen)
			{
				return;
			}
			RngTape.Armed = true;
			RngTape.Add(op, a, b, result);
			WriteRng(op.ToString(CultureInfo.InvariantCulture) + " " + a.ToString(CultureInfo.InvariantCulture) + " " + b.ToString(CultureInfo.InvariantCulture) + " " + result.ToString(CultureInfo.InvariantCulture));
		}

		public static void NoteRngDouble(double value)
		{
			if (!listen)
			{
				return;
			}
			long bits = BitConverter.DoubleToInt64Bits(value);
			int low = (int)bits;
			int high = (int)(bits >> 32);
			NoteRng(0, 3, low, high);
		}

		public static void AfterUpdate(object game)
		{
			if (finished || game == null)
			{
				return;
			}
			try
			{
				if (!noted)
				{
					noted = true;
					Note("hook " + VerifySnapshot.StageName(game));
				}
				if (!anchored)
				{
					if (!VerifySnapshot.IsMainMenu(game))
					{
						return;
					}
					anchored = true;
				}
				if (!opened)
				{
					OpenReplay(game);
					opened = true;
					return;
				}
				if (VerifySnapshot.IsGameOver(game))
				{
					Finish("replay-complete", true);
					return;
				}
				string line = VerifySnapshot.Build(game);
				listen = false;
				if (rngWriter != null)
				{
					rngWriter.Flush();
				}
				if (line == null)
				{
					return;
				}
				Write(line);
				samples++;
				int max = MaxFrames();
				if (max > 0 && samples >= max)
				{
					Finish("frame-budget", false);
				}
			}
			catch (Exception ex)
			{
				try
				{
					Write("{\"record\":false,\"error\":\"" + ex.GetType().Name + ": " + ex.Message.Replace("\\", "\\\\").Replace("\"", "\\\"") + "\"}");
				}
				catch
				{
				}
				Finish("capture-error", false);
			}
		}

		static void OpenReplay(object game)
		{
			listen = true;
			RngTape.Armed = true;
			string path = Setting("REPLAY");
			if (string.IsNullOrEmpty(path))
			{
				throw new InvalidOperationException("THSSS_VERIFY_REPLAY is required");
			}
			int stageId = 1;
			string stageText = Setting("STAGE");
			if (!string.IsNullOrEmpty(stageText))
			{
				stageId = int.Parse(stageText, CultureInfo.InvariantCulture);
			}
			object present = Member(game, "PresentState");
			object global = Member(present, "GlobalData");
			Assembly assembly = game.GetType().Assembly;
			object replay = Member(global, "Rep");
			if (replay == null)
			{
				replay = Activator.CreateInstance(assembly.GetType("Shooting.Replay"));
				VerifySnapshot.Set(global, "Rep", replay);
			}
			else
			{
				MethodInfo close = replay.GetType().GetMethod("CloseRpy");
				if (close != null)
				{
					close.Invoke(replay, null);
				}
			}
			replay.GetType().GetMethod("LoadRpy").Invoke(replay, new object[] { path });
			object info = assembly.GetType("Shooting.Replay").GetMethod("ReadTitle").Invoke(null, new object[] { path });
			string start = Convert.ToString(Member(info, "StartStage"), CultureInfo.InvariantCulture);
			string next = NextState(start, stageId);
			int planeIndex = PlaneIndex(start, stageId);
			object package = Activator.CreateInstance(assembly.GetType("Shooting.StageDataPackage"), global);
			VerifySnapshot.Set(package, "OnReplay", true);
			VerifySnapshot.Set(package, "Difficulty", Member(info, "Rank"));
			VerifySnapshot.Set(package, "RepInfo", info);
			IList planes = (IList)Member(info, "MyPlaneData");
			object sample = planeIndex >= 0 && planeIndex < planes.Count ? planes[planeIndex] : null;
			Point point = sample == null ? new Point(192, 398) : new Point(Convert.ToInt32(Member(sample, "PosX"), CultureInfo.InvariantCulture), Convert.ToInt32(Member(sample, "PosY"), CultureInfo.InvariantCulture));
			object plane = CreatePlane(assembly, info, package, point);
			if (sample != null)
			{
				CopyPlane(plane, sample);
				VerifySnapshot.Set(replay, "DataPosition", Convert.ToInt64(Member(sample, "DataPosition"), CultureInfo.InvariantCulture));
			}
			VerifySnapshot.Set(package, "MyPlane", plane);
			object switchData = Activator.CreateInstance(assembly.GetType("Shooting.StateSwitchDataPackage"));
			VerifySnapshot.Set(switchData, "NextState", next);
			VerifySnapshot.Set(switchData, "NeedInit", true);
			VerifySnapshot.Set(switchData, "SDPswitch", package);
			VerifySnapshot.Set(present, "StateSwitchData", switchData);
			listen = false;
		}

		static object CreatePlane(Assembly assembly, object info, object package, Point point)
		{
			string name = Convert.ToString(Member(info, "MyPlaneName"), CultureInfo.InvariantCulture);
			string weapon = Convert.ToString(Member(info, "WeaponType"), CultureInfo.InvariantCulture);
			string typeName = "Shooting.BaseMyPlane";
			if (name == "Aya")
			{
				typeName = weapon == "B" ? "Shooting.MyPlane_AyaB" : "Shooting.MyPlane_Aya";
			}
			else if (name == "Plane" && weapon == "B")
			{
				typeName = "Shooting.MyPlane_PlaneB";
			}
			else if (name == "Reimu")
			{
				typeName = "Shooting.MyPlane_Reimu";
			}
			else if (name == "Sanae")
			{
				typeName = "Shooting.MyPlane_Sanae";
			}
			else if (name == "Marisa")
			{
				typeName = "Shooting.MyPlane_Marisa";
			}
			else if (name == "Koishi")
			{
				typeName = "Shooting.MyPlane_Koishi";
			}
			return Activator.CreateInstance(assembly.GetType(typeName), package, point);
		}

		static void CopyPlane(object plane, object sample)
		{
			string[] names = new string[] { "Life", "Spell", "Power", "Score", "Graze", "LifeUpCount", "LifeChip", "SpellChip", "StarPoint", "HighItemScore", "Rate", "LastColor" };
			for (int i = 0; i < names.Length; i++)
			{
				VerifySnapshot.Set(plane, names[i], Member(sample, names[i]));
			}
			float x = Convert.ToSingle(Member(sample, "PosX"), CultureInfo.InvariantCulture);
			float y = Convert.ToSingle(Member(sample, "PosY"), CultureInfo.InvariantCulture);
			VerifySnapshot.Set(plane, "OriginalPosition", new PointF(x, y));
		}

		static string NextState(string start, int stageId)
		{
			if (start == "StEx")
			{
				return "StEx";
			}
			string prefix = start != null && start.Contains("St") ? "St" : "Bs";
			return prefix + stageId.ToString(CultureInfo.InvariantCulture);
		}

		static int PlaneIndex(string start, int stageId)
		{
			if (start == "StEx")
			{
				return 0;
			}
			int startNum = int.Parse(start.Replace("St", "").Replace("Bs", ""), CultureInfo.InvariantCulture);
			return stageId - startNum;
		}

		static object Member(object target, string name)
		{
			for (Type type = target.GetType(); type != null; type = type.BaseType)
			{
				PropertyInfo property = type.GetProperty(name, BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
				if (property != null)
				{
					return property.GetValue(target, null);
				}
				FieldInfo field = type.GetField(name, BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
				if (field != null)
				{
					return field.GetValue(target);
				}
			}
			throw new MissingMemberException(target.GetType().FullName, name);
		}

		static void Write(string line)
		{
			if (writer == null)
			{
				string path = Setting("OUT");
				if (string.IsNullOrEmpty(path))
				{
					throw new InvalidOperationException("THSSS_VERIFY_OUT is required");
				}
				string dir = Path.GetDirectoryName(path);
				if (!string.IsNullOrEmpty(dir))
				{
					Directory.CreateDirectory(dir);
				}
				writer = new StreamWriter(path, false);
				writer.AutoFlush = true;
			}
			writer.WriteLine(line);
		}

		static void WriteRng(string line)
		{
			if (rngWriter == null)
			{
				string path = Setting("RNG");
				if (string.IsNullOrEmpty(path))
				{
					return;
				}
				string dir = Path.GetDirectoryName(path);
				if (!string.IsNullOrEmpty(dir))
				{
					Directory.CreateDirectory(dir);
				}
				rngWriter = new StreamWriter(path, false);
			}
			rngWriter.WriteLine(line);
		}

		static void Finish(string reason, bool complete)
		{
			if (finished)
			{
				return;
			}
			finished = true;
			Write("{\"record\":false,\"ended\":true,\"complete\":" + (complete ? "true" : "false") + ",\"reason\":\"" + reason + "\",\"samples\":" + samples.ToString(CultureInfo.InvariantCulture) + "}");
			if (writer != null)
			{
				writer.Flush();
				writer.Close();
			}
			if (rngWriter != null)
			{
				rngWriter.Flush();
				rngWriter.Close();
			}
			Environment.Exit(complete ? 0 : 2);
		}

		static string Setting(string name)
		{
			string env = Environment.GetEnvironmentVariable("THSSS_VERIFY_" + name);
			if (!string.IsNullOrEmpty(env))
			{
				return env;
			}
			string path = Path.Combine(AppDir(), "THSSS.verify.cfg");
			if (!File.Exists(path))
			{
				return "";
			}
			string[] lines = File.ReadAllLines(path);
			for (int i = 0; i < lines.Length; i++)
			{
				int eq = lines[i].IndexOf('=');
				if (eq > 0 && lines[i].Substring(0, eq) == name)
				{
					return lines[i].Substring(eq + 1);
				}
			}
			return "";
		}

		static void Note(string text)
		{
			try
			{
				File.AppendAllText(Path.Combine(AppDir(), "THSSS.verify.log"), text + "\r\n");
			}
			catch
			{
			}
		}

		static string AppDir()
		{
			return Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
		}

		static int MaxFrames()
		{
			string text = Setting("MAX");
			int value;
			return !string.IsNullOrEmpty(text) && int.TryParse(text, NumberStyles.Integer, CultureInfo.InvariantCulture, out value) ? value : 0;
		}
	}
}
