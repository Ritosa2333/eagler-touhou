using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Thsss.Browser.StageContent
{
	internal static class Program
	{
		private static readonly string[] Required = { "Image", "CS", "FX", "Model", "Story", "Sound", "BGM" };

		private static readonly string[] PackRoots = { "Image", "CS", "FX", "Model", "Story", "Sound" };

		private static readonly string[] LooseRoots = { "BGM" };

		private static readonly HashSet<string> AudioExtensions = new HashSet<string>(StringComparer.OrdinalIgnoreCase)
		{
			".wav", ".mp3", ".ogg", ".xwma", ".flac", ".wma", ".mid", ".midi", ".m4a", ".aac"
		};

		private static int Main(string[] args)
		{
			try
			{
				var options = Options.Parse(args);
				Stage(options);
				return 0;
			}
			catch (Exception ex)
			{
				Console.Error.WriteLine(ex.Message);
				return 1;
			}
		}

		private static void Stage(Options options)
		{
			var repo = Path.GetFullPath(options.Repo);
			var dest = Path.GetFullPath(options.Dest);
			var problems = new List<string>();
			foreach (var root in Required)
			{
				var from = Path.Combine(repo, root);
				if (!Directory.Exists(from))
				{
					problems.Add("missing folder " + root);
					continue;
				}
				if (!HasAnyFile(from))
				{
					problems.Add("empty folder " + root);
				}
			}
			if (problems.Count > 0)
			{
				throw new InvalidDataException("Game content check failed: " + string.Join("; ", problems) + ". Restore the gitignored asset folders under Assets, or build with -p:StageGameAssets=false.");
			}

			var dat = Path.Combine(dest, "thsss.dat");
			if (options.SkipCopy && File.Exists(dat))
			{
				Console.WriteLine("Game content validated; staged archive already present.");
				return;
			}

			var entries = new List<Entry>();
			foreach (var root in PackRoots)
			{
				var from = Path.Combine(repo, root);
				if (!Directory.Exists(from))
				{
					continue;
				}
				foreach (var file in Directory.EnumerateFiles(from, "*", SearchOption.AllDirectories))
				{
					var rel = RelativePath(repo, file);
					if (IsLooseAudio(rel))
					{
						continue;
					}
					entries.Add(new Entry
					{
						Rel = rel,
						Full = file,
						Size = new FileInfo(file).Length
					});
				}
			}
			if (entries.Count == 0)
			{
				throw new InvalidDataException("Game content check failed: no files were packed. Restore the gitignored asset folders under Assets, or build with -p:StageGameAssets=false.");
			}

			entries.Sort((left, right) => string.CompareOrdinal(left.Rel, right.Rel));
			var utf8 = new UTF8Encoding(false);
			long indexSize = 0;
			foreach (var entry in entries)
			{
				var nameLen = utf8.GetByteCount(entry.Rel);
				if (nameLen > ushort.MaxValue)
				{
					throw new InvalidDataException("Archive path is too long: " + entry.Rel);
				}
				if (entry.Size > uint.MaxValue)
				{
					throw new InvalidDataException("Archive file is too large: " + entry.Rel);
				}
				indexSize += 10 + nameLen;
			}
			var dataStart = 12 + indexSize;
			var cursor = dataStart;
			foreach (var entry in entries)
			{
				if (cursor > uint.MaxValue)
				{
					throw new InvalidDataException("Archive offset is too large: " + entry.Rel);
				}
				entry.Offset = cursor;
				cursor += entry.Size;
			}

			if (Directory.Exists(dest))
			{
				Directory.Delete(dest, true);
			}
			else if (File.Exists(dest))
			{
				File.Delete(dest);
			}
			Directory.CreateDirectory(dest);

			using (var output = File.Create(dat))
			{
				output.Write(new byte[] { 0x54, 0x48, 0x44, 0x41 }, 0, 4);
				WriteU16(output, 1);
				WriteU16(output, 0);
				WriteU32(output, entries.Count);
				foreach (var entry in entries)
				{
					var nameBytes = utf8.GetBytes(entry.Rel);
					WriteU32(output, entry.Offset);
					WriteU32(output, entry.Size);
					WriteU16(output, nameBytes.Length);
					output.Write(nameBytes, 0, nameBytes.Length);
				}
				if (output.Position != dataStart)
				{
					throw new InvalidDataException("Archive index size mismatch.");
				}
				foreach (var entry in entries)
				{
					using (var input = File.OpenRead(entry.Full))
					{
						input.CopyTo(output);
					}
				}
			}

			foreach (var root in LooseRoots)
			{
				var from = Path.Combine(repo, root);
				foreach (var file in Directory.EnumerateFiles(from, "*", SearchOption.AllDirectories))
				{
					var rel = RelativePath(repo, file);
					var target = Path.Combine(dest, rel.Replace('/', Path.DirectorySeparatorChar));
					var dir = Path.GetDirectoryName(target);
					if (!string.IsNullOrEmpty(dir))
					{
						Directory.CreateDirectory(dir);
					}
					File.Copy(file, target, true);
				}
			}

			using (var check = File.OpenRead(dat))
			{
				var magic = new byte[4];
				if (check.Read(magic, 0, 4) != 4 || magic[0] != 0x54 || magic[1] != 0x48 || magic[2] != 0x44 || magic[3] != 0x41)
				{
					throw new InvalidDataException("Packed archive is missing the THDAT header.");
				}
			}

			Console.WriteLine("Packed " + entries.Count + " files into thsss.dat; left BGM beside the archive");
		}

		private static bool HasAnyFile(string directory)
		{
			foreach (var unused in Directory.EnumerateFiles(directory, "*", SearchOption.AllDirectories))
			{
				return true;
			}
			return false;
		}

		private static string RelativePath(string root, string full)
		{
			var rel = Path.GetRelativePath(root, full).Replace('\\', '/');
			if (rel.Length == 0 || rel == "." || rel.StartsWith("../", StringComparison.Ordinal) || rel.IndexOf("/../", StringComparison.Ordinal) >= 0)
			{
				throw new InvalidDataException("Archive path is not allowed: " + full);
			}
			return rel;
		}

		private static bool IsLooseAudio(string rel)
		{
			foreach (var root in LooseRoots)
			{
				if (rel.StartsWith(root + "/", StringComparison.OrdinalIgnoreCase))
				{
					return true;
				}
			}
			if (rel.StartsWith("Sound/", StringComparison.OrdinalIgnoreCase))
			{
				return false;
			}
			return AudioExtensions.Contains(Path.GetExtension(rel));
		}

		private static void WriteU16(Stream stream, long value)
		{
			stream.WriteByte((byte)(value & 255));
			stream.WriteByte((byte)((value >> 8) & 255));
		}

		private static void WriteU32(Stream stream, long value)
		{
			stream.WriteByte((byte)(value & 255));
			stream.WriteByte((byte)((value >> 8) & 255));
			stream.WriteByte((byte)((value >> 16) & 255));
			stream.WriteByte((byte)((value >> 24) & 255));
		}

		private sealed class Entry
		{
			public string Rel;

			public string Full;

			public long Size;

			public long Offset;
		}

		private sealed class Options
		{
			public string Repo;

			public string Dest;

			public bool SkipCopy;

			public static Options Parse(string[] args)
			{
				var options = new Options();
				for (var i = 0; i < args.Length; i++)
				{
					var arg = args[i];
					if (arg == "--skip-copy")
					{
						options.SkipCopy = true;
						continue;
					}
					if (arg == "--repo" && i + 1 < args.Length)
					{
						options.Repo = args[++i];
						continue;
					}
					if (arg == "--dest" && i + 1 < args.Length)
					{
						options.Dest = args[++i];
						continue;
					}
					throw new ArgumentException("Unknown argument: " + arg);
				}
				if (string.IsNullOrWhiteSpace(options.Repo) || string.IsNullOrWhiteSpace(options.Dest))
				{
					throw new ArgumentException("Usage: StageContent --repo <Assets> --dest <wwwroot/assets> [--skip-copy]");
				}
				return options;
			}
		}
	}
}
