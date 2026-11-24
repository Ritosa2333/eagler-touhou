using System;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Text;

namespace Shooting
{
	public partial class Game_Main
	{
		public bool WorkspaceActive { get; private set; }

		public int WorkspaceFrame { get; private set; }

		public int WorkspaceTotal { get; private set; }

		public int WorkspaceStage { get; private set; }

		public int WorkspaceStartStage { get; private set; }

		public bool WorkspaceEnded { get; private set; }

		public bool WorkspaceMenuReady()
		{
			return initSuccess && !browserAwaitLoad && !InitFlag && PresentState != null
				&& GameStatesDictionary != null && GameStatesDictionary.ContainsKey("St1");
		}

		public void WorkspaceAudio(bool hold)
		{
			if (GlobalData == null)
			{
				return;
			}
			if (hold)
			{
				GlobalData.BGM_Pause();
			}
			else
			{
				GlobalData.BGM_Resume();
			}
		}

		public static string DescribeReplay(string path)
		{
			var native = ExistingReplayPath(path);
			if (native.Length == 0)
			{
				return "";
			}
			var info = Replay.ReadTitle(native);
			if (!Playable(info))
			{
				return "";
			}
			var stages = new StringBuilder();
			var any = false;
			AppendStages(stages, info, ref any);
			if (!any)
			{
				return "";
			}
			var shot = info.WeaponType == "A" ? 0 : info.WeaponType == "B" ? 1 : -1;
			var json = new StringBuilder();
			json.Append("{\"name\":\"").Append(JsonString(info.PlayerName));
			json.Append("\",\"date\":\"").Append(JsonString(info.Date));
			json.Append("\",\"character\":").Append(CharacterId(info.MyPlaneName).ToString(CultureInfo.InvariantCulture));
			json.Append(",\"difficulty\":").Append(((int)info.Rank).ToString(CultureInfo.InvariantCulture));
			if (shot >= 0)
			{
				json.Append(",\"shot\":").Append(shot.ToString(CultureInfo.InvariantCulture));
			}
			json.Append(",\"stages\":[").Append(stages).Append("]}");
			return json.ToString();
		}

		public bool OpenWorkspaceReplay(string path, int stageId)
		{
			var native = ExistingReplayPath(path);
			if (native.Length == 0 || PresentState == null || GlobalData == null || GameStatesDictionary == null)
			{
				return false;
			}
			var info = Replay.ReadTitle(native);
			string state;
			int planeIndex;
			if (!TryResolve(info, stageId, out state, out planeIndex) || !GameStatesDictionary.ContainsKey(state))
			{
				return false;
			}
			if (GlobalData.Rep == null)
			{
				GlobalData.Rep = new Replay();
			}
			else
			{
				GlobalData.Rep.CloseRpy();
			}
			GlobalData.Rep.LoadRpy(native);
			var sample = info.MyPlaneData[planeIndex];
			var point = new Point((int)sample.PosX, (int)sample.PosY);
			var package = new StageDataPackage(GlobalData)
			{
				OnReplay = true,
				Difficulty = info.Rank,
				RepInfo = info
			};
			var plane = CreatePlane(package, info, point);
			plane.Life = sample.Life;
			plane.Spell = sample.Spell;
			plane.Power = sample.Power;
			plane.Score = sample.Score;
			plane.OriginalPosition = new PointF(sample.PosX, sample.PosY);
			plane.Graze = sample.Graze;
			plane.LifeUpCount = sample.LifeUpCount;
			plane.LifeChip = sample.LifeChip;
			plane.SpellChip = sample.SpellChip;
			plane.StarPoint = sample.StarPoint;
			plane.HighItemScore = sample.HighItemScore;
			plane.Rate = sample.Rate;
			plane.LastColor = sample.LastColor;
			package.MyPlane = plane;
			GlobalData.Rep.DataPosition = sample.DataPosition;
			PresentState.StateSwitchData = new StateSwitchDataPackage
			{
				NextState = state,
				NeedInit = true,
				SDPswitch = package
			};
			WorkspaceActive = true;
			WorkspaceFrame = 0;
			WorkspaceEnded = false;
			WorkspaceStage = stageId;
			WorkspaceStartStage = stageId;
			WorkspaceTotal = CountLogicFrames(native, sample.DataPosition, Replay.IsTouchReplayPath(native));
			return true;
		}

		public string WorkspaceStateJson()
		{
			return "{\"frame\":" + WorkspaceFrame.ToString(CultureInfo.InvariantCulture)
				+ ",\"total\":" + WorkspaceTotal.ToString(CultureInfo.InvariantCulture)
				+ ",\"ended\":" + (WorkspaceEnded ? "true" : "false")
				+ ",\"stage\":" + WorkspaceStage.ToString(CultureInfo.InvariantCulture)
				+ ",\"startStage\":" + WorkspaceStartStage.ToString(CultureInfo.InvariantCulture) + "}";
		}

		partial void NoteWorkspaceFrame()
		{
			if (!WorkspaceActive || WorkspaceEnded || PresentState == null)
			{
				return;
			}
			WorkspaceFrame++;
			int id;
			if (TryStageId(PresentState.StageName, out id))
			{
				WorkspaceStage = id;
			}
			var next = PresentState.StateSwitchData != null ? PresentState.StateSwitchData.NextState : null;
			if (next == "GameOverMenu" || PresentState.StageName == "GameOverMenu")
			{
				WorkspaceEnded = true;
			}
		}

		private static bool Playable(ReplayInfo info)
		{
			return info != null && info.Version == "ver 1.00" && info.MyPlaneData != null && info.MyPlaneData.Count > 0
				&& !string.IsNullOrEmpty(info.StartStage);
		}

		private static bool TryResolve(ReplayInfo info, int stageId, out string state, out int planeIndex)
		{
			state = null;
			planeIndex = 0;
			if (!Playable(info))
			{
				return false;
			}
			if (info.StartStage == "StEx")
			{
				if (stageId != 7 || info.MyPlaneData.Count < 2)
				{
					return false;
				}
				state = "StEx";
				planeIndex = 0;
				return true;
			}
			int startNum;
			string prefix;
			if (!TryStart(info.StartStage, out prefix, out startNum))
			{
				return false;
			}
			for (var j = 0; j < info.MyPlaneData.Count - 1; j++)
			{
				var menu = startNum - 1 + j;
				if (menu < 0 || menu >= 6 || menu + 1 != stageId)
				{
					continue;
				}
				planeIndex = stageId - startNum;
				if (planeIndex < 0 || planeIndex >= info.MyPlaneData.Count)
				{
					return false;
				}
				state = prefix + stageId.ToString(CultureInfo.InvariantCulture);
				return true;
			}
			return false;
		}

		private static void AppendStages(StringBuilder stages, ReplayInfo info, ref bool any)
		{
			if (info.StartStage == "StEx")
			{
				if (info.MyPlaneData.Count > 1)
				{
					AppendStage(stages, ref any, 7, "StEx", info.MyPlaneData[1].Score);
				}
				return;
			}
			int startNum;
			string prefix;
			if (!TryStart(info.StartStage, out prefix, out startNum))
			{
				return;
			}
			for (var j = 0; j < info.MyPlaneData.Count - 1; j++)
			{
				var menu = startNum - 1 + j;
				if (menu < 0 || menu >= 6)
				{
					continue;
				}
				AppendStage(stages, ref any, menu + 1, prefix + (menu + 1).ToString(CultureInfo.InvariantCulture), info.MyPlaneData[j + 1].Score);
			}
		}

		private static void AppendStage(StringBuilder stages, ref bool any, int id, string label, long score)
		{
			if (any)
			{
				stages.Append(',');
			}
			any = true;
			stages.Append("{\"id\":").Append(id.ToString(CultureInfo.InvariantCulture));
			stages.Append(",\"label\":\"").Append(JsonString(label));
			stages.Append("\",\"score\":");
			if (score >= 0)
			{
				stages.Append(score.ToString(CultureInfo.InvariantCulture));
			}
			else
			{
				stages.Append("null");
			}
			stages.Append('}');
		}

		private static bool TryStart(string startStage, out string prefix, out int startNum)
		{
			prefix = startStage != null && startStage.Contains("St") ? "St" : "Bs";
			var digits = (startStage ?? "").Replace("St", "").Replace("Bs", "");
			return int.TryParse(digits, NumberStyles.Integer, CultureInfo.InvariantCulture, out startNum) && startNum >= 1 && startNum <= 6;
		}

		private static bool TryStageId(string name, out int id)
		{
			id = 0;
			if (name == "StEx")
			{
				id = 7;
				return true;
			}
			if (string.IsNullOrEmpty(name) || name.Length < 3)
			{
				return false;
			}
			if (!name.StartsWith("St", StringComparison.Ordinal) && !name.StartsWith("Bs", StringComparison.Ordinal))
			{
				return false;
			}
			return int.TryParse(name.Substring(2), NumberStyles.Integer, CultureInfo.InvariantCulture, out id);
		}

		private static int CharacterId(string name)
		{
			switch (name)
			{
			case "Aya": return 0;
			case "Plane": return 1;
			case "Reimu": return 2;
			case "Sanae": return 3;
			case "Marisa": return 4;
			case "Koishi": return 5;
			default: return 0;
			}
		}

		private static BaseMyPlane CreatePlane(StageDataPackage package, ReplayInfo info, Point point)
		{
			switch (info.MyPlaneName)
			{
			case "Aya":
				return info.WeaponType == "B" ? new MyPlane_AyaB(package, point) : new MyPlane_Aya(package, point);
			case "Plane":
				return info.WeaponType == "B" ? new MyPlane_PlaneB(package, point) : new BaseMyPlane(package, point);
			case "Reimu":
				return new MyPlane_Reimu(package, point);
			case "Sanae":
				return new MyPlane_Sanae(package, point);
			case "Marisa":
				return new MyPlane_Marisa(package, point);
			case "Koishi":
				return new MyPlane_Koishi(package, point);
			default:
				return new BaseMyPlane(package, point);
			}
		}

		private static string ExistingReplayPath(string path)
		{
			var text = (path ?? "").Replace('\\', '/').TrimStart('/');
			if (text.Length == 0 || text.Contains(".."))
			{
				return "";
			}
			var slash = text.IndexOf('/');
			if (slash >= 0 && text.Substring(0, slash).Equals("replay", StringComparison.OrdinalIgnoreCase))
			{
				text = "Replay/" + text.Substring(slash + 1);
			}
			if (File.Exists(text))
			{
				return text;
			}
			var dotted = ".\\" + text.Replace('/', '\\');
			return File.Exists(dotted) ? dotted : "";
		}

		private static int CountLogicFrames(string path, long start, bool touch)
		{
			var bytes = File.ReadAllBytes(path);
			var end = bytes.Length;
			for (var i = 0; i + 6 < bytes.Length; i++)
			{
				if (bytes[i] == 0xF0 && bytes[i + 1] == 0x0F && bytes[i + 2] == 0x0D && bytes[i + 3] == 0x0A
					&& bytes[i + 4] == (byte)'R' && bytes[i + 5] == (byte)'e')
				{
					end = i;
					break;
				}
			}
			var pos = start < 0 ? 0 : start > end ? end : (int)start;
			var frames = 0;
			while (pos + 1 < end)
			{
				var key = bytes[pos] + (bytes[pos + 1] << 8);
				pos += 2;
				if (key == 57358)
				{
					continue;
				}
				frames++;
				if (key == 4080)
				{
					break;
				}
				if (touch)
				{
					if (pos + 4 > end)
					{
						break;
					}
					pos += 4;
				}
			}
			return frames > 0 ? frames : 1;
		}

		private static string JsonString(string value)
		{
			if (string.IsNullOrEmpty(value))
			{
				return "";
			}
			return value.Replace("\\", "\\\\").Replace("\"", "\\\"").Replace("\r", "").Replace("\n", "");
		}
	}
}
