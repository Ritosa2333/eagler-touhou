using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace Thsss.Browser
{
	public static class ContentLoader
	{
		private static readonly HashSet<string> readyFiles = new HashSet<string>(StringComparer.OrdinalIgnoreCase);

		private static readonly object readyLock = new object();

		private static readonly List<ArchiveEntry> pendingEntries = new List<ArchiveEntry>();

		private static Stream pendingStream;

		private static bool backgroundIdle = true;

		public static bool BackgroundIdle
		{
			get
			{
				return backgroundIdle;
			}
		}

		public static void Pump(int budget)
		{
			if (budget < 1) budget = 1;
			lock (readyLock)
			{
				if (backgroundIdle || pendingStream == null) return;
				var wrote = 0;
				while (pendingEntries.Count > 0 && wrote < budget)
				{
					var entry = pendingEntries[0];
					pendingEntries.RemoveAt(0);
					pendingStream.Position = entry.Offset;
					WriteEntry(pendingStream, entry);
					wrote++;
				}
				if (pendingEntries.Count == 0)
				{
					backgroundIdle = true;
					pendingStream.Dispose();
					pendingStream = null;
				}
			}
		}

		public static bool IsFileReady(string path)
		{
			if (string.IsNullOrEmpty(path))
			{
				return false;
			}
			var norm = path.Replace('\\', '/');
			var image = norm.IndexOf("Image/", StringComparison.OrdinalIgnoreCase);
			if (image < 0)
			{
				return true;
			}
			var rel = norm.Substring(image);
			lock (readyLock)
			{
				return readyFiles.Contains(rel);
			}
		}

		public static async Task LoadAsync()
		{
			var root = "/content";
			try
			{
				Directory.CreateDirectory(root);
				Directory.SetCurrentDirectory(root);
			}
			catch (Exception ex)
			{
				Console.WriteLine("Using default directory: " + ex.Message);
			}
			ThsssJs.SetStatus("Fetching content…");
			var hosted = Array.Empty<byte>();
			try
			{
				hosted = ThsssJs.TakeArchive() ?? Array.Empty<byte>();
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
			if (hosted.Length == 0)
			{
				throw new InvalidDataException("Content archive was not provided.");
			}
			var stream = new MemoryStream(hosted, false);
			await UnpackAsync(stream);
			if (backgroundIdle)
			{
				stream.Dispose();
			}
			EnsureDir("Sound");
			EnsureDir("BPic");
			EnsureDir("ScreenShot");
			EnsureDir("BGM");
			EnsureDir("Replay");
			EnsureDir("CS");
			EnsureDir("Image");
			EnsureDir("Model");
			EnsureDir("Story");
			OverlaySaves();
			if (!File.Exists("Setting.INI"))
			{
				File.WriteAllText("Setting.INI", DefaultIni);
			}
			ThsssJs.SetStatus("Content ready.");
		}

		private static async Task UnpackAsync(Stream input)
		{
			var magic = new byte[4];
			await ReadExactAsync(input, magic, 4);
			if (magic[0] != (byte)'T' || magic[1] != (byte)'H' || magic[2] != (byte)'D' || magic[3] != (byte)'A')
			{
				throw new InvalidDataException("Content archive is not a THDAT file.");
			}
			var version = await ReadU16Async(input);
			await ReadU16Async(input);
			if (version != 1)
			{
				throw new InvalidDataException("Unsupported content archive version.");
			}
			var count = await ReadU32Async(input);
			var entries = new List<ArchiveEntry>(count);
			var position = 12;
			for (var i = 0; i < count; i++)
			{
				var offset = await ReadU32Async(input);
				var size = await ReadU32Async(input);
				var nameLen = await ReadU16Async(input);
				var nameBytes = new byte[nameLen];
				await ReadExactAsync(input, nameBytes, nameLen);
				position += 10 + nameLen;
				var name = Encoding.UTF8.GetString(nameBytes).Replace('\\', '/');
				if (!IsSafePath(name))
				{
					throw new InvalidDataException("Content archive path is not allowed.");
				}
				entries.Add(new ArchiveEntry
				{
					Name = name,
					Offset = offset,
					Size = size
				});
			}
			var check = new List<ArchiveEntry>(entries);
			check.Sort((left, right) => left.Offset.CompareTo(right.Offset));
			var cursor = position;
			foreach (var entry in check)
			{
				if (entry.Offset != cursor)
				{
					throw new InvalidDataException("Content archive entry is out of order.");
				}
				cursor += entry.Size;
			}
			ThsssJs.SetStatus("Unpacking content…");
			long end = 0;
			foreach (var entry in entries)
			{
				var entryEnd = entry.Offset + (long)entry.Size;
				if (entryEnd > end) end = entryEnd;
			}
			if (input.Length < end)
			{
				throw new InvalidDataException("Content archive is shorter than its index.");
			}
			entries.Sort(CompareEntry);
			pendingEntries.Clear();
			var written = 0;
			foreach (var entry in entries)
			{
				// Game_Main spins in LoadEffect until the FX files exist, and the
				// splash reads NowLoading images in its constructor. Both happen
				// before the first frame, so those entries cannot wait for Pump.
				if (IsStartupFile(entry.Name))
				{
					input.Position = entry.Offset;
					await WriteEntryAsync(input, entry);
					if (IsImage(entry.Name)) MarkReady(entry.Name);
					if ((++written & 7) == 0) await Task.Yield();
				}
				else
				{
					pendingEntries.Add(entry);
				}
			}
			lock (readyLock)
			{
				if (pendingEntries.Count == 0)
				{
					backgroundIdle = true;
					pendingStream = null;
				}
				else
				{
					backgroundIdle = false;
					pendingStream = input;
				}
			}
		}

		private static int CompareEntry(ArchiveEntry left, ArchiveEntry right)
		{
			var order = Priority(left.Name) - Priority(right.Name);
			if (order != 0) return order;
			return string.CompareOrdinal(left.Name, right.Name);
		}

		private static int Priority(string name)
		{
			if (name.StartsWith("Image/NowLoading/", StringComparison.OrdinalIgnoreCase)) return 0;
			if (IsImage(name)) return 1;
			return 2;
		}

		private static void WriteEntry(Stream input, ArchiveEntry entry)
		{
			var path = entry.Name.Replace('/', Path.DirectorySeparatorChar);
			var dir = Path.GetDirectoryName(path);
			if (!string.IsNullOrEmpty(dir))
			{
				Directory.CreateDirectory(dir);
			}
			var audio = IsRuntimeAudio(entry.Name) && entry.Size > 0 ? new byte[entry.Size] : null;
			using (var output = new FileStream(path, FileMode.Create, FileAccess.Write, FileShare.None))
			{
				var buffer = new byte[81920];
				var left = entry.Size;
				while (left > 0)
				{
					var want = left < buffer.Length ? left : buffer.Length;
					var read = input.Read(buffer, 0, want);
					if (read <= 0)
					{
						throw new EndOfStreamException("Truncated content archive.");
					}
					output.Write(buffer, 0, read);
					if (audio != null)
					{
						Buffer.BlockCopy(buffer, 0, audio, entry.Size - left, read);
					}
					left -= read;
				}
			}
			if (audio != null)
			{
				ThsssJs.PutAudioFile("/" + entry.Name, audio);
			}
		}

		private static Task WriteEntryAsync(Stream input, ArchiveEntry entry)
		{
			WriteEntry(input, entry);
			return Task.CompletedTask;
		}

		private static async Task ReadExactAsync(Stream input, byte[] buffer, int count)
		{
			var offset = 0;
			while (offset < count)
			{
				var read = await input.ReadAsync(buffer, offset, count - offset);
				if (read <= 0)
				{
					throw new EndOfStreamException("Truncated content archive.");
				}
				offset += read;
			}
		}

		private static async Task<int> ReadU16Async(Stream input)
		{
			var buffer = new byte[2];
			await ReadExactAsync(input, buffer, 2);
			return buffer[0] | (buffer[1] << 8);
		}

		private static async Task<int> ReadU32Async(Stream input)
		{
			var buffer = new byte[4];
			await ReadExactAsync(input, buffer, 4);
			return buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
		}

		private static bool IsSafePath(string rel)
		{
			if (string.IsNullOrEmpty(rel) || rel.StartsWith("/", StringComparison.Ordinal) || rel.IndexOf(':') >= 0)
			{
				return false;
			}
			var parts = rel.Split('/');
			foreach (var part in parts)
			{
				if (part.Length == 0 || part == "." || part == "..")
				{
					return false;
				}
			}
			return true;
		}

		private static bool IsImage(string rel)
		{
			return rel.StartsWith("Image/", StringComparison.OrdinalIgnoreCase);
		}

		private static bool IsStartupFile(string rel)
		{
			return IsImage(rel) || rel.StartsWith("FX/", StringComparison.OrdinalIgnoreCase);
		}

		private static bool IsRuntimeAudio(string rel)
		{
			return rel.StartsWith("Sound/", StringComparison.OrdinalIgnoreCase)
				&& (rel.EndsWith(".wav", StringComparison.OrdinalIgnoreCase) || rel.EndsWith(".ogg", StringComparison.OrdinalIgnoreCase));
		}

		private static void MarkReady(string rel)
		{
			lock (readyLock)
			{
				readyFiles.Add(rel);
			}
		}

		private static void EnsureDir(string name)
		{
			if (!Directory.Exists(name))
			{
				Directory.CreateDirectory(name);
			}
		}

		private static void OverlaySaves()
		{
			var json = ThsssJs.ReadSaves();
			if (string.IsNullOrWhiteSpace(json) || json == "{}")
			{
				return;
			}
			var map = JsonSerializer.Deserialize(json, SaveJsonContext.Default.DictionaryStringString);
			if (map == null)
			{
				return;
			}
			foreach (var pair in map)
			{
				if (string.IsNullOrEmpty(pair.Value))
				{
					continue;
				}
				var bytes = Convert.FromBase64String(pair.Value);
				var path = pair.Key.Replace('/', Path.DirectorySeparatorChar);
				var dir = Path.GetDirectoryName(path);
				if (!string.IsNullOrEmpty(dir))
				{
					Directory.CreateDirectory(dir);
				}
				File.WriteAllBytes(path, bytes);
			}
		}

		private sealed class ArchiveEntry
		{
			public string Name;

			public int Offset;

			public int Size;
		}

		private const string DefaultIni = "[DirectX]\r\nFullWindow=0\r\nAntiAlias=0\r\nVerticalSync=0\r\n[DirectInput]\r\nUseDirectInput=0\r\n[Mode]\r\nClear=no\r\nSingleThreaded=1\r\nFixedFPS=0\r\nUnlockFPS=0\r\nDelayTolerance=10\r\nWindowSize=0\r\nPresentInterval=1\r\n[Key]\r\nLeft=Left\r\nRight=Right\r\nUp=Up\r\nDown=Down\r\nSlow=ShiftKey\r\nShoot=Z\r\nBomb=X\r\n[JoyStick]\r\nSkip=0\r\nPause=0\r\nSlow=0\r\nShoot=0\r\nBomb=0\r\nC=0\r\n[Volume]\r\nBGMVolume=80\r\nSEVolume=80\r\n[Game]\r\nFontType=Arial\r\n";
	}
}
