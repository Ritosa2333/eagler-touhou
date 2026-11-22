using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Text.Json;

namespace Thsss.Tools.PrepareGameSources
{
	internal static class Program
	{
		private const string IlspyVersion = "11.2.0.10050";
		private static readonly string[] DecompileFlags = { "-p", "--nested-directories", "--disable-updatecheck" };

		private static int Main(string[] args)
		{
			try
			{
				if (args.Length == 0)
				{
					throw new ArgumentException(Usage());
				}
				var cmd = args[0];
				var options = Options.Parse(args.Skip(1).ToArray());
				switch (cmd)
				{
					case "prepare":
						Prepare(options);
						break;
					case "refresh-patches":
						RefreshPatches(options);
						break;
					case "decompile":
						Decompile(options, force: true);
						break;
					default:
						throw new ArgumentException("Unknown command: " + cmd + "\n" + Usage());
				}
				return 0;
			}
			catch (Exception ex)
			{
				Console.Error.WriteLine(ex.Message);
				return 1;
			}
		}

		private static string Usage()
		{
			return "Usage: PrepareGameSources <prepare|refresh-patches|decompile> [--repo <root>]";
		}

		private static string PristineDir(Options options)
		{
			return Path.Combine(options.Repo, "vendor", "THSSS-src");
		}

		private static void Prepare(Options options)
		{
			var pristine = PristineDir(options);
			if (!Directory.Exists(pristine))
			{
				throw new DirectoryNotFoundException(
					"Missing committed sources at " + pristine + "; run decompile to regenerate from vendor/THSSS.exe.");
			}
			var work = Path.Combine(options.Repo, "THSSS");
			ResetDirectory(work, pristine);
			ApplySeries(options.Repo, work);
			Console.WriteLine("Prepared " + work);
		}

		private static void RefreshPatches(Options options)
		{
			var pristine = PristineDir(options);
			var work = Path.Combine(options.Repo, "THSSS");
			var patchesDir = Path.Combine(options.Repo, "patches");
			var seriesPath = Path.Combine(patchesDir, "series");
			if (!Directory.Exists(pristine))
			{
				throw new DirectoryNotFoundException("Missing pristine sources at " + pristine + "; run decompile first.");
			}
			if (!Directory.Exists(work))
			{
				throw new DirectoryNotFoundException("Missing working tree at " + work + "; run prepare first.");
			}
			if (!File.Exists(seriesPath))
			{
				throw new FileNotFoundException("Missing patches/series");
			}

			var series = ReadSeries(seriesPath);
			var ownership = LoadOwnership(patchesDir, series);
			var baseDir = Path.Combine(options.Repo, ".decompile-patch-base");
			ResetDirectory(baseDir, pristine);

			for (var i = 0; i < series.Count; i++)
			{
				var name = series[i];
				var files = ownership[name];
				if (files.Count == 0)
				{
					throw new InvalidOperationException("Patch " + name + " owns no files; add them to patches/ownership.json");
				}

				var patchPath = Path.Combine(patchesDir, name);
				var sb = new StringBuilder();
				sb.AppendLine("From: PrepareGameSources");
				sb.AppendLine("Subject: " + name);
				sb.AppendLine("---");
				sb.AppendLine();
				foreach (var rel in files.OrderBy(f => f, StringComparer.Ordinal))
				{
					var oldPath = Path.Combine(baseDir, rel);
					var newPath = Path.Combine(work, rel);
					if (!File.Exists(newPath))
					{
						throw new FileNotFoundException("Working tree missing owned file for " + name + ": " + rel);
					}
					var diff = DiffFile(rel, oldPath, newPath);
					if (!string.IsNullOrEmpty(diff))
					{
						sb.Append(diff);
						if (!diff.EndsWith("\n", StringComparison.Ordinal))
						{
							sb.Append('\n');
						}
					}
				}
				File.WriteAllText(patchPath, sb.ToString(), new UTF8Encoding(false));
				Console.WriteLine("Refreshed " + name + " (" + files.Count + " files)");

				// Advance base with this patch so later patches diff against stacked state.
				RunGitApply(options.Repo, baseDir, patchPath);
			}

			if (Directory.Exists(baseDir))
			{
				Directory.Delete(baseDir, true);
			}
		}

		private static string DiffFile(string rel, string oldPath, string newPath)
		{
			var normalized = rel.Replace('\\', '/');
			if (!File.Exists(oldPath))
			{
				// New file
				var text = File.ReadAllText(newPath).Replace("\r\n", "\n");
				if (!text.EndsWith("\n", StringComparison.Ordinal))
				{
					text += "\n";
				}
				var lines = text.Split('\n');
				var sb = new StringBuilder();
				sb.Append("diff --git a/").Append(normalized).Append(" b/").Append(normalized).Append('\n');
				sb.Append("new file mode 100644\n");
				sb.Append("--- /dev/null\n");
				sb.Append("+++ b/").Append(normalized).Append('\n');
				sb.Append("@@ -0,0 +1,").Append(Math.Max(lines.Length - 1, 0)).Append(" @@\n");
				for (var i = 0; i < lines.Length - 1; i++)
				{
					sb.Append('+').Append(lines[i]).Append('\n');
				}
				return sb.ToString();
			}

			var psi = new ProcessStartInfo
			{
				FileName = "git",
				RedirectStandardOutput = true,
				RedirectStandardError = true,
				UseShellExecute = false,
			};
			psi.ArgumentList.Add("diff");
			psi.ArgumentList.Add("--no-index");
			psi.ArgumentList.Add("--");
			psi.ArgumentList.Add(oldPath);
			psi.ArgumentList.Add(newPath);
			using (var p = Process.Start(psi))
			{
				var output = p.StandardOutput.ReadToEnd();
				p.WaitForExit();
				if (p.ExitCode != 0 && p.ExitCode != 1)
				{
					throw new InvalidOperationException("git diff failed for " + rel + ": " + p.StandardError.ReadToEnd());
				}
				if (string.IsNullOrWhiteSpace(output))
				{
					return "";
				}
				var rewritten = new StringBuilder();
				foreach (var line in output.Replace("\r\n", "\n").Split('\n'))
				{
					if (line.StartsWith("diff --git ", StringComparison.Ordinal))
					{
						rewritten.Append("diff --git a/").Append(normalized).Append(" b/").Append(normalized).Append('\n');
					}
					else if (line.StartsWith("--- ", StringComparison.Ordinal))
					{
						rewritten.Append("--- a/").Append(normalized).Append('\n');
					}
					else if (line.StartsWith("+++ ", StringComparison.Ordinal))
					{
						rewritten.Append("+++ b/").Append(normalized).Append('\n');
					}
					else if (line.Length > 0 || rewritten.Length > 0)
					{
						rewritten.Append(line).Append('\n');
					}
				}
				return rewritten.ToString();
			}
		}

		private static void Decompile(Options options, bool force)
		{
			var exe = Path.Combine(options.Repo, "vendor", "THSSS.exe");
			var dest = PristineDir(options);
			var stampPath = Path.Combine(dest, ".stamp.json");
			if (!File.Exists(exe))
			{
				throw new FileNotFoundException("Missing vendor/THSSS.exe");
			}
			var sha = Sha256(exe);
			if (!force && File.Exists(stampPath) && Directory.Exists(dest))
			{
				try
				{
					var existing = JsonSerializer.Deserialize<Stamp>(File.ReadAllText(stampPath));
					if (existing != null
						&& existing.exeSha256 == sha
						&& existing.ilspycmd == IlspyVersion
						&& existing.flags != null
						&& existing.flags.SequenceEqual(DecompileFlags))
					{
						Console.WriteLine("Source stamp matches; keeping " + dest);
						return;
					}
				}
				catch
				{
					// fall through
				}
			}

			if (Directory.Exists(dest))
			{
				Directory.Delete(dest, true);
			}
			Directory.CreateDirectory(dest);

			var psi = new ProcessStartInfo
			{
				FileName = "dotnet",
				WorkingDirectory = options.Repo,
				RedirectStandardOutput = true,
				RedirectStandardError = true,
				UseShellExecute = false,
			};
			psi.ArgumentList.Add("tool");
			psi.ArgumentList.Add("run");
			psi.ArgumentList.Add("ilspycmd");
			psi.ArgumentList.Add("--");
			psi.ArgumentList.Add(exe);
			foreach (var flag in DecompileFlags)
			{
				psi.ArgumentList.Add(flag);
			}
			psi.ArgumentList.Add("-o");
			psi.ArgumentList.Add(dest);

			Console.WriteLine("Decompiling " + exe + " -> " + dest);
			using (var p = Process.Start(psi))
			{
				var stdout = p.StandardOutput.ReadToEnd();
				var stderr = p.StandardError.ReadToEnd();
				p.WaitForExit();
				if (p.ExitCode != 0)
				{
					throw new InvalidOperationException("ilspycmd failed (" + p.ExitCode + "): " + stderr + stdout);
				}
				if (!string.IsNullOrWhiteSpace(stdout))
				{
					Console.WriteLine(stdout.TrimEnd());
				}
			}

			var stamp = new Stamp
			{
				exeSha256 = sha,
				ilspycmd = IlspyVersion,
				flags = DecompileFlags.ToArray(),
			};
			File.WriteAllText(stampPath, JsonSerializer.Serialize(stamp, new JsonSerializerOptions { WriteIndented = true }) + "\n");
		}

		private static void ApplySeries(string repo, string work)
		{
			var seriesPath = Path.Combine(repo, "patches", "series");
			if (!File.Exists(seriesPath))
			{
				Console.WriteLine("No patches/series; leaving pristine working tree.");
				return;
			}
			foreach (var name in ReadSeries(seriesPath))
			{
				var patch = Path.Combine(repo, "patches", name);
				if (!File.Exists(patch))
				{
					throw new FileNotFoundException("Missing patch listed in series: " + name);
				}
				RunGitApply(repo, work, patch);
				Console.WriteLine("Applied " + name);
			}
		}

		private static void RunGitApply(string repo, string workTree, string patchFile)
		{
			var relDir = Path.GetRelativePath(repo, workTree).Replace('\\', '/');
			var psi = new ProcessStartInfo
			{
				FileName = "git",
				WorkingDirectory = repo,
				RedirectStandardOutput = true,
				RedirectStandardError = true,
				UseShellExecute = false,
			};
			psi.ArgumentList.Add("apply");
			psi.ArgumentList.Add("--unsafe-paths");
			psi.ArgumentList.Add("--directory=" + relDir);
			psi.ArgumentList.Add("--whitespace=nowarn");
			psi.ArgumentList.Add(patchFile);
			using (var p = Process.Start(psi))
			{
				var stdout = p.StandardOutput.ReadToEnd();
				var stderr = p.StandardError.ReadToEnd();
				p.WaitForExit();
				if (p.ExitCode != 0)
				{
					throw new InvalidOperationException("git apply failed for " + patchFile + ":\n" + stderr + stdout);
				}
			}
		}

		private static List<string> ReadSeries(string seriesPath)
		{
			var list = new List<string>();
			foreach (var raw in File.ReadAllLines(seriesPath))
			{
				var line = raw.Trim();
				if (line.Length == 0 || line.StartsWith("#", StringComparison.Ordinal))
				{
					continue;
				}
				list.Add(line);
			}
			return list;
		}

		private static Dictionary<string, List<string>> LoadOwnership(string patchesDir, List<string> series)
		{
			var path = Path.Combine(patchesDir, "ownership.json");
			if (!File.Exists(path))
			{
				throw new FileNotFoundException("Missing patches/ownership.json");
			}
			var doc = JsonSerializer.Deserialize<Dictionary<string, List<string>>>(File.ReadAllText(path));
			if (doc == null)
			{
				throw new InvalidOperationException("Invalid ownership.json");
			}
			var map = new Dictionary<string, List<string>>(StringComparer.Ordinal);
			foreach (var name in series)
			{
				if (!doc.TryGetValue(name, out var files) || files == null)
				{
					throw new InvalidOperationException("ownership.json missing entry for " + name);
				}
				map[name] = files.Select(f => f.Replace('\\', '/')).ToList();
			}
			return map;
		}

		private static void ResetDirectory(string dest, string source)
		{
			if (Directory.Exists(dest))
			{
				Directory.Delete(dest, true);
			}
			CopyDirectory(source, dest);
		}

		private static void CopyDirectory(string source, string dest)
		{
			var srcRoot = Path.GetFullPath(source);
			var dstRoot = Path.GetFullPath(dest);
			Directory.CreateDirectory(dstRoot);
			foreach (var dir in Directory.EnumerateDirectories(srcRoot, "*", SearchOption.AllDirectories))
			{
				var rel = Path.GetRelativePath(srcRoot, dir);
				Directory.CreateDirectory(Path.Combine(dstRoot, rel));
			}
			foreach (var file in Directory.EnumerateFiles(srcRoot, "*", SearchOption.AllDirectories))
			{
				var name = Path.GetFileName(file);
				if (name.Equals(".stamp.json", StringComparison.Ordinal))
				{
					continue;
				}
				var rel = Path.GetRelativePath(srcRoot, file);
				var target = Path.Combine(dstRoot, rel);
				Directory.CreateDirectory(Path.GetDirectoryName(target));
				File.Copy(file, target, true);
			}
		}

		private static string Sha256(string path)
		{
			using (var stream = File.OpenRead(path))
			using (var sha = SHA256.Create())
			{
				return Convert.ToHexString(sha.ComputeHash(stream)).ToLowerInvariant();
			}
		}

		private sealed class Stamp
		{
			public string exeSha256 { get; set; }
			public string ilspycmd { get; set; }
			public string[] flags { get; set; }
		}

		private sealed class Options
		{
			public string Repo;

			public static Options Parse(string[] args)
			{
				var options = new Options();
				for (var i = 0; i < args.Length; i++)
				{
					if (args[i] == "--repo" && i + 1 < args.Length)
					{
						options.Repo = Path.GetFullPath(args[++i]);
						continue;
					}
					throw new ArgumentException("Unknown argument: " + args[i]);
				}
				if (string.IsNullOrWhiteSpace(options.Repo))
				{
					options.Repo = Directory.GetCurrentDirectory();
				}
				return options;
			}
		}
	}
}
