using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Thsss.Browser.TransformSources
{
	internal static class Program
	{
		private static int Main(string[] args)
		{
			try
			{
				var options = Options.Parse(args);
				var count = Transform(options);
				Console.WriteLine("Transformed " + count + " game sources");
				return 0;
			}
			catch (Exception ex)
			{
				Console.Error.WriteLine(ex.Message);
				return 1;
			}
		}

		private static int Transform(Options options)
		{
			var source = Path.GetFullPath(options.Source);
			var dest = Path.GetFullPath(options.Dest);
			if (!Directory.Exists(source))
			{
				throw new DirectoryNotFoundException("Game source directory was not found: " + source);
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

			var count = 0;
			foreach (var file in Directory.EnumerateFiles(source, "*.cs", SearchOption.AllDirectories))
			{
				if (IsExcluded(source, file))
				{
					continue;
				}
				var rel = Path.GetRelativePath(source, file);
				var outPath = Path.Combine(dest, rel);
				var dir = Path.GetDirectoryName(outPath);
				if (!string.IsNullOrEmpty(dir))
				{
					Directory.CreateDirectory(dir);
				}
				var raw = CollapseBackslashes(File.ReadAllBytes(file));
				var hasBom = raw.Length >= 3 && raw[0] == 0xEF && raw[1] == 0xBB && raw[2] == 0xBF;
				var text = Encoding.UTF8.GetString(raw);
				text = text.Replace("System.Text.Encoding.Default", "Shooting.Ansi.GBK");
				text = text.Replace("Encoding.Default", "Shooting.Ansi.GBK");
				text = text.Replace(".ForEach(", ".ForEachNet48(");
				// ILSpy emits operator method calls; rewrite to normal C#.
				text = StripOpImplicitCalls(text, "Color4");
				text = StripOpImplicitCalls(text, "EffectHandle");
				text = text.Replace("((Voice)sourceVoice).", "sourceVoice.");
				text = text.Replace("((Voice)sourceVoice)", "sourceVoice");
				text = text.Replace("((Device)keyboard).", "keyboard.");
				text = text.Replace("((Device)keyboard)", "keyboard");
				text = text.Replace("((Device)joystick).", "joystick.");
				text = text.Replace("((Device)joystick)", "joystick");
				text = text.Replace("((BaseMesh)mesh).", "mesh.");
				text = text.Replace("((BaseMesh)mesh)", "mesh");
				text = text.Replace("((ComObject)mesh).Dispose()", "mesh.Dispose()");
				text = text.Replace("VertexFormat & 0x10", "VertexFormat & (VertexFormat)0x10");
				text = text.Replace("VertexFormat | 0x10", "VertexFormat | (VertexFormat)0x10");
				text = text.Replace("((ContainerControl)this).", "this.");
				text = text.Replace("((ContainerControl)this)", "this");
				text = text.Replace("((Control)Form_Main.panel1).Handle", "Form_Main.panel1.Handle");
				text = text.Replace("((Control)Form_Main).Handle", "Form_Main.Handle");
				text = text.Replace("((BaseEffect)StageData.EffectMain).", "StageData.EffectMain.");
				text = text.Replace("((BaseEffect)GlobalData.EffectMain).", "GlobalData.EffectMain.");
				text = text.Replace("((BaseEffect)GlobalData.EffectMain)", "GlobalData.EffectMain");
				text = text.Replace("((SlimDXException)ex2).", "ex2.");
				text = text.Replace("((SlimDXException)ex2)", "ex2");
				text = text.Replace("new Font(DeviceMain,", "new SlimDX.Direct3D9.Font(DeviceMain,");
				text = text.Replace("new Font(StageData.DeviceMain,", "new SlimDX.Direct3D9.Font(StageData.DeviceMain,");
				text = text.Replace("new Font(StageData.GlobalData.FontType,", "new System.Drawing.Font(StageData.GlobalData.FontType,");
				text = text.Replace("new Font(\"Arial\",", "new System.Drawing.Font(\"Arial\",");
				text = text.Replace("Font val = new System.Drawing.Font", "System.Drawing.Font val = new System.Drawing.Font");
				text = text.Replace("private Font sysfont", "private System.Drawing.Font sysfont");
				text = text.Replace("private Font DXfont", "private SlimDX.Direct3D9.Font DXfont");
				text = text.Replace("((Form)Form_Main).Close()", "Form_Main.Close()");
				text = text.Replace("((Form)Form_Main).", "Form_Main.");
				text = text.Replace("((ComObject)sprite).Dispose()", "sprite.Dispose()");
				text = text.Replace("((ComObject)keyboard).Dispose()", "keyboard.Dispose()");
				text = text.Replace("((ComObject)joystick).Dispose()", "joystick.Dispose()");
				text = text.Replace("((ComObject)DXFont).Dispose()", "DXFont.Dispose()");
				text = text.Replace("((Form)this).Dispose(disposing)", "base.Dispose(disposing)");
				text = text.Replace("((Control)this).ImeMode", "this.ImeMode");
				text = text.Replace("(Nullable)", "");
				text = text.Replace("(object?)", "(object)");
				text = text.Replace("\n\t\t\tnum = num;\n", "\n");
				text = text.Replace("\n\t\t\tnum = num;\r\n", "\n");
				var utf8 = new UTF8Encoding(hasBom);
				File.WriteAllBytes(outPath, utf8.GetBytes(text));
				count++;
			}
			return count;
		}

		/// <summary>
		/// Replace Type.op_Implicit(expr) with (expr).
		/// </summary>
		private static string StripOpImplicitCalls(string text, string typeName)
		{
			var token = typeName + ".op_Implicit(";
			var sb = new StringBuilder(text.Length);
			var i = 0;
			while (i < text.Length)
			{
				var idx = text.IndexOf(token, i, StringComparison.Ordinal);
				if (idx < 0)
				{
					sb.Append(text, i, text.Length - i);
					break;
				}
				sb.Append(text, i, idx - i);
				var start = idx + token.Length;
				var depth = 1;
				var j = start;
				while (j < text.Length && depth > 0)
				{
					var c = text[j];
					if (c == '(')
					{
						depth++;
					}
					else if (c == ')')
					{
						depth--;
					}
					j++;
				}
				if (depth != 0)
				{
					sb.Append(text, idx, text.Length - idx);
					break;
				}
				sb.Append('(');
				sb.Append(text, start, j - start - 1);
				sb.Append(')');
				i = j;
			}
			return sb.ToString();
		}

		private static bool IsExcluded(string source, string file)
		{
			var name = Path.GetFileName(file);
			if (name.Equals("Program.cs", StringComparison.OrdinalIgnoreCase)
				|| name.Equals("AssemblyInfo.cs", StringComparison.OrdinalIgnoreCase)
				|| name.Equals("Form_Test.cs", StringComparison.OrdinalIgnoreCase))
			{
				return true;
			}
			var rel = Path.GetRelativePath(source, file);
			var parts = rel.Split(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar);
			foreach (var part in parts)
			{
				if (part.Equals("bin", StringComparison.OrdinalIgnoreCase) || part.Equals("obj", StringComparison.OrdinalIgnoreCase))
				{
					return true;
				}
			}
			return false;
		}

		private static byte[] CollapseBackslashes(byte[] bytes)
		{
			var list = new List<byte>(bytes.Length);
			for (var i = 0; i < bytes.Length; i++)
			{
				if (i + 1 < bytes.Length && bytes[i] == 0x5C && bytes[i + 1] == 0x5C)
				{
					list.Add(0x2F);
					i++;
				}
				else
				{
					list.Add(bytes[i]);
				}
			}
			return list.ToArray();
		}

		private sealed class Options
		{
			public string Source;

			public string Dest;

			public static Options Parse(string[] args)
			{
				var options = new Options();
				for (var i = 0; i < args.Length; i++)
				{
					var arg = args[i];
					if (arg == "--source" && i + 1 < args.Length)
					{
						options.Source = args[++i];
						continue;
					}
					if (arg == "--dest" && i + 1 < args.Length)
					{
						options.Dest = args[++i];
						continue;
					}
					throw new ArgumentException("Unknown argument: " + arg);
				}
				if (string.IsNullOrWhiteSpace(options.Source) || string.IsNullOrWhiteSpace(options.Dest))
				{
					throw new ArgumentException("Usage: TransformSources --source <THSSS> --dest <game-src>");
				}
				return options;
			}
		}
	}
}
