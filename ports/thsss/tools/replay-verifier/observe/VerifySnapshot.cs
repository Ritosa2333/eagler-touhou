#if THSSS_OBSERVE || THSSS_DIAGNOSTIC
using System;
using System.Collections;
using System.Collections.Generic;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Reflection;
using System.Text;

namespace THSSS.Observe
{
		public static class RngTape
		{
			public static bool Armed;
			static readonly List<int> frame = new List<int>();
			static int[] playback;
			static int cursor;
			static bool loaded;

			public static void Add(int op, int a, int b, int result)
			{
				if (!Armed)
				{
					return;
				}
				frame.Add(op);
				frame.Add(a);
				frame.Add(b);
				frame.Add(result);
			}

			public static void AppendDraws(StringBuilder json)
			{
				json.Append('[');
				for (int i = 0; i < frame.Count; i++)
				{
					if (i != 0)
					{
						json.Append(',');
					}
					json.Append(frame[i].ToString(CultureInfo.InvariantCulture));
				}
				json.Append(']');
				frame.Clear();
			}

			public static void Load(string path)
			{
				if (loaded || string.IsNullOrEmpty(path) || !File.Exists(path))
				{
					return;
				}
				string[] lines = File.ReadAllLines(path);
				var values = new List<int>(lines.Length * 4);
				for (int i = 0; i < lines.Length; i++)
				{
					if (lines[i].Length == 0)
					{
						continue;
					}
					string[] parts = lines[i].Split(' ');
					if (parts.Length != 4)
					{
						continue;
					}
					values.Add(int.Parse(parts[0], CultureInfo.InvariantCulture));
					values.Add(int.Parse(parts[1], CultureInfo.InvariantCulture));
					values.Add(int.Parse(parts[2], CultureInfo.InvariantCulture));
					values.Add(int.Parse(parts[3], CultureInfo.InvariantCulture));
				}
				playback = values.ToArray();
				cursor = 0;
				loaded = true;
				Armed = true;
				frame.Clear();
			}

			public static bool TryInt(int op, int a, int b, out int result)
			{
				result = 0;
				if (!Armed || playback == null || cursor + 4 > playback.Length)
				{
					return false;
				}
				if (playback[cursor] != op || playback[cursor + 1] != a || playback[cursor + 2] != b)
				{
					return false;
				}
				result = playback[cursor + 3];
				cursor += 4;
				Add(op, a, b, result);
				return true;
			}

			public static bool TryDouble(out double value)
			{
				value = 0;
				if (!Armed || playback == null || cursor + 4 > playback.Length || playback[cursor] != 3)
				{
					return false;
				}
				int low = playback[cursor + 1];
				int high = playback[cursor + 2];
				int result = playback[cursor + 3];
				cursor += 4;
				long bits = (long)(uint)low | ((long)high << 32);
				value = BitConverter.Int64BitsToDouble(bits);
				Add(3, low, high, result);
				return true;
			}
		}

		public static class VerifySnapshot
		{
		public static string Build(object game)
		{
			if (game == null)
			{
				return null;
			}
			object present = Get(game, "PresentState");
			if (present == null)
			{
				return null;
			}
			string stageName = AsString(Get(present, "StageName"));
			object stage = Get(present, "StageData");
			bool onReplay = stage != null && AsBool(Get(stage, "OnReplay"));
			object next = stage == null ? null : Get(stage, "StateSwitchData");
			bool leaving = next != null && AsString(Get(next, "NextState")) == "GameOverMenu";
			if (!onReplay || !IsReplayStage(stageName) || leaving)
			{
				return null;
			}
			var json = new StringBuilder(4096);
			json.Append("{\"record\":true,\"ended\":false");
			AppendProgress(json, game, stageName);
			json.Append(",\"stageName\":\"").Append(Esc(stageName)).Append('"');
			json.Append(",\"stageId\":").Append(StageId(stageName).ToString(CultureInfo.InvariantCulture));
			int held = Held(stage);
			json.Append(",\"appliedInput\":").Append(held.ToString(CultureInfo.InvariantCulture));
			json.Append(",\"clocks\":");
			AppendClocks(json, stage, stageName);
			json.Append(",\"scalars\":{\"stageId\":").Append(StageId(stageName).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"difficulty\":").Append(EnumInt(Get(stage, "Difficulty")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"stageName\":\"").Append(Esc(stageName)).Append("\"}");
			json.Append(",\"categories\":{");
			json.Append("\"clocks\":");
			AppendClocks(json, stage, stageName);
			json.Append(",\"input\":{\"held\":").Append(held.ToString(CultureInfo.InvariantCulture)).Append('}');
			json.Append(",\"rng\":");
			AppendRng(json, Get(stage, "Ran"));
			json.Append(",\"player\":");
			AppendPlayer(json, Get(stage, "MyPlane"));
			json.Append(",\"economy\":");
			AppendEconomy(json, stage);
			json.Append(",\"boss\":");
			AppendBoss(json, Get(stage, "Boss"));
			json.Append(",\"entities\":");
			AppendEntities(json, stage);
			json.Append(",\"draw\":");
			AppendDraw(json, stage);
			json.Append("}}");
			return json.ToString();
		}

		public static string StageName(object game)
		{
			object present = Get(game, "PresentState");
			string name = present == null ? "" : AsString(Get(present, "StageName"));
			return name.Length == 0 ? StateType(game) : name;
		}

		static string StateType(object game)
		{
			object present = game == null ? null : Get(game, "PresentState");
			return present == null ? "" : present.GetType().Name;
		}

		public static bool IsMainMenu(object game)
		{
			return StateType(game) == "GameState_MainMenu";
		}

		public static bool IsGameOver(object game)
		{
			if (StateType(game) == "GameState_GameOverMenu")
			{
				return true;
			}
			object present = Get(game, "PresentState");
			if (present == null)
			{
				return false;
			}
			object stage = Get(present, "StageData");
			object next = stage == null ? null : Get(stage, "StateSwitchData");
			return next != null && AsString(Get(next, "NextState")) == "GameOverMenu";
		}

		static void AppendProgress(StringBuilder json, object game, string stageName)
		{
			json.Append(",\"frame\":").Append(AsInt(Get(game, "WorkspaceFrame")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"total\":").Append(AsInt(Get(game, "WorkspaceTotal")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"stage\":").Append(StageId(stageName).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"startStage\":").Append(AsInt(Get(game, "WorkspaceStartStage")).ToString(CultureInfo.InvariantCulture));
		}

		static void AppendClocks(StringBuilder json, object stage, string stageName)
		{
			json.Append("{\"timeMain\":").Append(AsInt(Get(stage, "TimeMain")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"timeTotal\":").Append(AsLongString(Get(stage, "TimeTotal")));
			json.Append(",\"realTimeTotal\":").Append(AsLongString(Get(stage, "RealTimeTotal")));
			json.Append(",\"stageId\":").Append(StageId(stageName).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"difficulty\":").Append(EnumInt(Get(stage, "Difficulty")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"stageName\":\"").Append(Esc(stageName)).Append("\"}");
		}

		static void AppendRng(StringBuilder json, object ran)
		{
			json.Append("{\"ranValue\":").Append(AsInt(Get(ran, "ranValue")).ToString(CultureInfo.InvariantCulture));
			if (RngTape.Armed)
			{
				json.Append(",\"shape\":\"tape\",\"draws\":");
				RngTape.AppendDraws(json);
				json.Append('}');
				return;
			}
			if (ran == null)
			{
				json.Append(",\"shape\":\"missing\"}");
				return;
			}
			Type random = typeof(Random);
			FieldInfo seed = FindField(random, "SeedArray");
			if (seed != null)
			{
				json.Append(",\"shape\":\"framework\"");
				json.Append(",\"inext\":").Append(AsInt(FindField(random, "inext").GetValue(ran)).ToString(CultureInfo.InvariantCulture));
				json.Append(",\"inextp\":").Append(AsInt(FindField(random, "inextp").GetValue(ran)).ToString(CultureInfo.InvariantCulture));
				var values = (int[])seed.GetValue(ran);
				json.Append(",\"seed\":[");
				for (int i = 0; i < values.Length; i++)
				{
					if (i != 0)
					{
						json.Append(',');
					}
					json.Append(values[i].ToString(CultureInfo.InvariantCulture));
				}
				json.Append("]}");
				return;
			}
			json.Append(",\"shape\":\"core\"");
			FieldInfo implField = FindField(ran.GetType(), "_impl");
			object impl = implField == null ? null : Value(implField, ran);
			json.Append(",\"core\":{");
			AppendCore(json, impl ?? ran);
			json.Append("}}");
		}

		static void AppendCore(StringBuilder json, object impl)
		{
			bool any = false;
			if (impl == null)
			{
				return;
			}
			foreach (FieldInfo field in impl.GetType().GetFields(BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic))
			{
				if (field.FieldType != typeof(ulong) && field.FieldType != typeof(long) && field.FieldType != typeof(uint))
				{
					continue;
				}
				object raw = Value(field, impl);
				if (raw == null)
				{
					continue;
				}
				if (any)
				{
					json.Append(',');
				}
				any = true;
				json.Append('"').Append(Esc(field.Name)).Append("\":\"");
				json.Append(Convert.ToUInt64(raw).ToString("x16", CultureInfo.InvariantCulture));
				json.Append('"');
			}
		}

		static void AppendPlayer(StringBuilder json, object plane)
		{
			if (plane == null)
			{
				json.Append("null");
				return;
			}
			int px;
			int py;
			ReadPoint(Get(plane, "OriginalPosition"), out px, out py);
			json.Append("{\"x\":").Append(px.ToString(CultureInfo.InvariantCulture));
			json.Append(",\"y\":").Append(py.ToString(CultureInfo.InvariantCulture));
			json.Append(",\"life\":").Append(AsInt(Get(plane, "Life")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"lifeChip\":").Append(AsInt(Get(plane, "LifeChip")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"spell\":").Append(AsInt(Get(plane, "Spell")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"spellChip\":").Append(AsInt(Get(plane, "SpellChip")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"power\":").Append(AsInt(Get(plane, "Power")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"score\":").Append(AsLongString(Get(plane, "Score")));
			json.Append(",\"hiScore\":").Append(AsLongString(Get(plane, "HiScore")));
			json.Append(",\"graze\":").Append(AsInt(Get(plane, "Graze")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"enchant\":").Append(EnumInt(Get(plane, "EnchantmentState")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"enchantTime\":").Append(AsInt(Get(plane, "EnchantmentTime")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"lastColor\":").Append(EnumInt(Get(plane, "LastColor")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"lifeUpCount\":").Append(AsInt(Get(plane, "LifeUpCount")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"starPoint\":").Append(AsInt(Get(plane, "StarPoint")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"highItemScore\":").Append(AsInt(Get(plane, "HighItemScore")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"rate\":").Append(F32(Get(plane, "Rate")).ToString(CultureInfo.InvariantCulture));
			json.Append('}');
		}

		static void AppendEconomy(StringBuilder json, object stage)
		{
			json.Append("{\"continueTimes\":").Append(AsInt(Get(stage, "ContinueTimes")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"slowMode\":").Append(AsInt(Get(stage, "SlowMode")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"clearCheck\":").Append(AsBool(Get(stage, "C_Check")) ? "1" : "0");
			json.Append('}');
		}

		static void AppendBoss(StringBuilder json, object boss)
		{
			if (boss == null)
			{
				json.Append("{\"present\":0}");
				return;
			}
			int bx;
			int by;
			ReadPoint(Get(boss, "OriginalPosition"), out bx, out by);
			json.Append("{\"present\":1,\"type\":\"").Append(Esc(boss.GetType().Name)).Append('"');
			json.Append(",\"x\":").Append(bx.ToString(CultureInfo.InvariantCulture));
			json.Append(",\"y\":").Append(by.ToString(CultureInfo.InvariantCulture));
			json.Append(",\"hp\":").Append(F32(Get(boss, "HealthPoint")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"life\":").Append(AsInt(Get(boss, "Life")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"time\":").Append(AsInt(Get(boss, "Time")).ToString(CultureInfo.InvariantCulture));
			json.Append('}');
		}

		static void AppendEntities(StringBuilder json, object stage)
		{
			json.Append('{');
			AppendList(json, "bullets", Get(stage, "BulletList"));
			json.Append(',');
			AppendList(json, "myBullets", Get(stage, "MyBulletList"));
			json.Append(',');
			AppendList(json, "enemies", Get(stage, "EnemyPlaneList"));
			json.Append(',');
			AppendList(json, "items", Get(stage, "ItemList"));
			json.Append(',');
			AppendList(json, "spells", Get(stage, "SpellList"));
			json.Append('}');
		}

		static void AppendList(StringBuilder json, string name, object list)
		{
			json.Append('"').Append(name).Append("\":[");
			IEnumerable items = list as IEnumerable;
			if (items != null)
			{
				bool any = false;
				foreach (object item in items)
				{
					if (item == null)
					{
						continue;
					}
					if (any)
					{
						json.Append(',');
					}
					any = true;
					int ex;
					int ey;
					ReadPoint(Get(item, "OriginalPosition"), out ex, out ey);
					json.Append("{\"t\":\"").Append(Esc(item.GetType().Name)).Append('"');
					json.Append(",\"x\":").Append(ex.ToString(CultureInfo.InvariantCulture));
					json.Append(",\"y\":").Append(ey.ToString(CultureInfo.InvariantCulture));
					json.Append(",\"hp\":").Append(F32(Get(item, "HealthPoint")).ToString(CultureInfo.InvariantCulture));
					json.Append(",\"time\":").Append(AsInt(Get(item, "Time")).ToString(CultureInfo.InvariantCulture));
					json.Append(",\"life\":").Append(AsInt(Get(item, "LifeTime")).ToString(CultureInfo.InvariantCulture));
					json.Append(",\"region\":").Append(AsInt(Get(item, "Region")).ToString(CultureInfo.InvariantCulture));
					json.Append('}');
				}
			}
			json.Append(']');
		}

		static void AppendDraw(StringBuilder json, object stage)
		{
			json.Append("{\"effects\":").Append(Count(Get(stage, "EffectList")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"interfaces\":").Append(Count(Get(stage, "InterfaceList")).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"background\":").Append(Get(stage, "Background") != null ? "1" : "0");
			json.Append(",\"background2\":").Append(Get(stage, "Background2") != null ? "1" : "0");
			json.Append(",\"background3d\":").Append(Get(stage, "Background3D") != null ? "1" : "0");
			json.Append(",\"particles\":").Append(Get(stage, "Particle3D") != null ? "1" : "0");
			json.Append('}');
		}

		static int Held(object stage)
		{
			object global = Get(stage, "GlobalData");
			object keys = global == null ? null : Get(global, "KClass");
			if (keys == null)
			{
				return 0;
			}
			MethodInfo method = keys.GetType().GetMethod("Key2Hex", BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
			return method == null ? 0 : AsInt(method.Invoke(keys, null));
		}

		public static bool IsReplayStage(string name)
		{
			if (string.IsNullOrEmpty(name) || name == "MainMenu" || name == "GameOverMenu")
			{
				return false;
			}
			if (name == "StEx")
			{
				return true;
			}
			if (name.Length < 3)
			{
				return false;
			}
			if (!name.StartsWith("St", StringComparison.Ordinal) && !name.StartsWith("Bs", StringComparison.Ordinal))
			{
				return false;
			}
			int stageNumber;
			return int.TryParse(name.Substring(2), NumberStyles.Integer, CultureInfo.InvariantCulture, out stageNumber);
		}

		public static int StageId(string name)
		{
			if (name == "StEx")
			{
				return 7;
			}
			int stageNumber;
			if (!string.IsNullOrEmpty(name) && name.Length >= 3 && int.TryParse(name.Substring(2), NumberStyles.Integer, CultureInfo.InvariantCulture, out stageNumber))
			{
				return stageNumber;
			}
			return 0;
		}

		static int Count(object list)
		{
			ICollection collection = list as ICollection;
			return collection == null ? 0 : collection.Count;
		}

		static object Get(object target, string name)
		{
			if (target == null || name == null)
			{
				return null;
			}
			for (Type type = target.GetType(); type != null; type = type.BaseType)
			{
				PropertyInfo property = type.GetProperty(name, BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
				if (property != null)
				{
					try
					{
						return property.GetValue(target, null);
					}
					catch
					{
					}
				}
				FieldInfo field = type.GetField(name, BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
				if (field == null && name.Length > 0)
				{
					string lower = char.ToLowerInvariant(name[0]) + name.Substring(1);
					field = type.GetField(lower, BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
					if (field == null)
					{
						field = type.GetField("<" + name + ">k__BackingField", BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
					}
				}
				if (field != null)
				{
					try
					{
						return field.GetValue(target);
					}
					catch
					{
					}
				}
			}
			return null;
		}

		public static void Set(object target, string name, object value)
		{
			for (Type type = target.GetType(); type != null; type = type.BaseType)
			{
				PropertyInfo property = type.GetProperty(name, BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
				if (property != null && property.CanWrite)
				{
					property.SetValue(target, value, null);
					return;
				}
				FieldInfo field = type.GetField(name, BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
				if (field != null)
				{
					field.SetValue(target, value);
					return;
				}
			}
			throw new MissingMemberException(target.GetType().FullName, name);
		}

		static object Value(FieldInfo field, object target)
		{
			try
			{
				return field.GetValue(target);
			}
			catch
			{
				return null;
			}
		}

		static FieldInfo FindField(Type type, string name)
		{
			for (Type current = type; current != null; current = current.BaseType)
			{
				FieldInfo field = current.GetField(name, BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
				if (field != null)
				{
					return field;
				}
			}
			return null;
		}

		static void ReadPoint(object pos, out int x, out int y)
		{
			if (pos is PointF)
			{
				PointF point = (PointF)pos;
				x = F32(point.X);
				y = F32(point.Y);
				return;
			}
			x = F32(Get(pos, "X"));
			y = F32(Get(pos, "Y"));
		}

		static int F32(object value)
		{
			if (value == null)
			{
				return 0;
			}
			float number = Convert.ToSingle(value, CultureInfo.InvariantCulture);
			return BitConverter.ToInt32(BitConverter.GetBytes(number), 0);
		}

		static int EnumInt(object value)
		{
			return value == null || !value.GetType().IsEnum ? AsInt(value) : Convert.ToInt32(value, CultureInfo.InvariantCulture);
		}

		static int AsInt(object value)
		{
			if (value == null)
			{
				return 0;
			}
			return value.GetType().IsEnum ? Convert.ToInt32(value, CultureInfo.InvariantCulture) : Convert.ToInt32(value, CultureInfo.InvariantCulture);
		}

		static bool AsBool(object value)
		{
			return value is bool && (bool)value;
		}

		static string AsString(object value)
		{
			return value == null ? "" : Convert.ToString(value, CultureInfo.InvariantCulture);
		}

		static string AsLongString(object value)
		{
			if (value == null)
			{
				return "\"0\"";
			}
			return "\"" + Convert.ToInt64(value, CultureInfo.InvariantCulture).ToString(CultureInfo.InvariantCulture) + "\"";
		}

		static string Esc(string value)
		{
			if (string.IsNullOrEmpty(value))
			{
				return "";
			}
			return value.Replace("\\", "\\\\").Replace("\"", "\\\"");
		}
	}
}
#endif
