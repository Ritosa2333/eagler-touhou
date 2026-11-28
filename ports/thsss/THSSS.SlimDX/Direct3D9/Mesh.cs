using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Text;

namespace SlimDX.Direct3D9
{
	public class Mesh : BaseMesh
	{
		private sealed class Subset
		{
			public ExtendedMaterial Material;

			public List<float> Verts = new List<float>();

			public VertexBuffer Buffer;

			public int Count;
		}

		private sealed class MatRec
		{
			public ExtendedMaterial Material = new ExtendedMaterial
			{
				MaterialD3D = default(Material),
				TextureFileName = ""
			};
		}

		private enum TokKind
		{
			Ident,
			Number,
			String,
			LBrace,
			RBrace
		}

		private struct Tok
		{
			public TokKind Kind;

			public string Text;

			public double Number;
		}

		private readonly List<Subset> subsets = new List<Subset>();

		private readonly Dictionary<string, MatRec> materials = new Dictionary<string, MatRec>();

		private VertexDeclaration declaration;

		private Device device;

		private string modelDir = "";

		private string[] modelFiles = new string[0];

		private bool hasBound;

		public override VertexFormat VertexFormat { get; set; }

		public Vector3 BoundMin { get; private set; }

		public Vector3 BoundMax { get; private set; }

		public int VertexCount { get; private set; }

		static Mesh()
		{
			Encoding.RegisterProvider(CodePagesEncodingProvider.Instance);
		}

		public static Mesh FromFile(Device device, string fileName, MeshFlags flags)
		{
			var path = ResolvePath(fileName);
			var bytes = File.ReadAllBytes(path);
			var text = Encoding.GetEncoding(936).GetString(bytes);
			if (text.Length < 8 || text[0] != 'x' || text[1] != 'o' || text[2] != 'f' || text.IndexOf("txt", StringComparison.Ordinal) < 0)
			{
				throw new InvalidDataException("Only text .x meshes are supported.");
			}
			var mesh = new Mesh();
			mesh.device = device;
			mesh.modelDir = Path.GetDirectoryName(path) ?? "";
			mesh.modelFiles = ListFileNames(mesh.modelDir);
			mesh.VertexFormat = VertexFormat.Position | VertexFormat.Normal | VertexFormat.Texture1;
			mesh.declaration = new VertexDeclaration(device, new VertexElement[]
			{
				new VertexElement(0, 0, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.Position, 0),
				new VertexElement(0, 12, DeclarationType.Float2, DeclarationMethod.Default, DeclarationUsage.TextureCoordinate, 0),
				VertexElement.VertexDeclarationEnd
			});
			mesh.Parse(text);
			mesh.Seal();
			return mesh;
		}

		public ExtendedMaterial[] GetMaterials()
		{
			if (this.subsets.Count == 0)
			{
				return new ExtendedMaterial[]
				{
					new ExtendedMaterial
					{
						MaterialD3D = default(Material),
						TextureFileName = ""
					}
				};
			}
			var array = new ExtendedMaterial[this.subsets.Count];
			for (var i = 0; i < array.Length; i++)
			{
				array[i] = this.subsets[i].Material;
			}
			return array;
		}

		public override Mesh Clone(Device device, MeshFlags flags, VertexFormat format)
		{
			var mesh = new Mesh();
			mesh.device = device ?? this.device;
			mesh.VertexFormat = format;
			mesh.declaration = this.declaration;
			mesh.subsets.AddRange(this.subsets);
			mesh.BoundMin = this.BoundMin;
			mesh.BoundMax = this.BoundMax;
			mesh.VertexCount = this.VertexCount;
			mesh.hasBound = this.hasBound;
			return mesh;
		}

		public void ComputeNormals()
		{
		}

		public override void DrawSubset(int subset)
		{
			if (subset < 0 || subset >= this.subsets.Count || this.device == null)
			{
				return;
			}
			var item = this.subsets[subset];
			if (item.Buffer == null || item.Count < 3)
			{
				return;
			}
			var previousDecl = this.device.VertexDeclaration;
			var previousFormat = this.device.VertexFormat;
			this.device.VertexDeclaration = this.declaration;
			this.device.VertexFormat = this.VertexFormat;
			this.device.SetStreamSource(0, item.Buffer, 0, 20);
			this.device.DrawPrimitives(PrimitiveType.TriangleList, 0, item.Count / 3);
			this.device.VertexDeclaration = previousDecl;
			this.device.VertexFormat = previousFormat;
		}

		private void Seal()
		{
			var total = 0;
			for (var s = 0; s < this.subsets.Count; s++)
			{
				var subset = this.subsets[s];
				var count = subset.Verts.Count / 5;
				subset.Count = count;
				total += count;
				if (count < 3)
				{
					continue;
				}
				var data = new byte[count * 20];
				for (var i = 0; i < count; i++)
				{
					var src = i * 5;
					var dst = i * 20;
					WriteFloat(data, dst, subset.Verts[src]);
					WriteFloat(data, dst + 4, subset.Verts[src + 1]);
					WriteFloat(data, dst + 8, subset.Verts[src + 2]);
					WriteFloat(data, dst + 12, subset.Verts[src + 3]);
					WriteFloat(data, dst + 16, subset.Verts[src + 4]);
				}
				subset.Buffer = new VertexBuffer(this.device, data.Length, Usage.WriteOnly, VertexFormat.None, Pool.Managed);
				System.Buffer.BlockCopy(data, 0, subset.Buffer.Data, 0, data.Length);
				subset.Verts = null;
			}
			this.VertexCount = total;
			if (!this.hasBound)
			{
				this.BoundMin = Vector3.Zero;
				this.BoundMax = Vector3.Zero;
			}
		}

		private static void WriteFloat(byte[] dest, int offset, float value)
		{
			var bytes = BitConverter.GetBytes(value);
			dest[offset] = bytes[0];
			dest[offset + 1] = bytes[1];
			dest[offset + 2] = bytes[2];
			dest[offset + 3] = bytes[3];
		}

		private void Parse(string text)
		{
			var toks = Tokenize(text);
			var index = 0;
			while (index < toks.Count)
			{
				if (!LooksLikeObject(toks, index))
				{
					index++;
					continue;
				}
				index = ReadObject(toks, index, Matrix.Identity);
			}
		}

		private int ReadObject(List<Tok> toks, int index, Matrix parent)
		{
			var kind = toks[index++].Text;
			string name = null;
			if (index < toks.Count && toks[index].Kind == TokKind.Ident)
			{
				name = toks[index++].Text;
			}
			index++;
			if (kind == "template")
			{
				return SkipBlock(toks, index);
			}
			if (kind == "Frame")
			{
				return ReadFrame(toks, index, parent);
			}
			if (kind == "Mesh")
			{
				return ReadMesh(toks, index, parent);
			}
			if (kind == "Material")
			{
				return ReadMaterial(toks, index, name);
			}
			return SkipBlock(toks, index);
		}

		private int ReadFrame(List<Tok> toks, int index, Matrix parent)
		{
			var local = Matrix.Identity;
			while (index < toks.Count && toks[index].Kind != TokKind.RBrace)
			{
				if (!LooksLikeObject(toks, index))
				{
					index++;
					continue;
				}
				var kind = toks[index].Text;
				if (kind == "FrameTransformMatrix")
				{
					index = SkipName(toks, index + 1);
					index++;
					local = ReadMatrix(toks, ref index);
					if (index < toks.Count && toks[index].Kind == TokKind.RBrace)
					{
						index++;
					}
				}
				else if (kind == "Frame")
				{
					index = ReadObject(toks, index, local * parent);
				}
				else if (kind == "Mesh")
				{
					index = ReadObject(toks, index, local * parent);
				}
				else
				{
					index = ReadObject(toks, index, parent);
				}
			}
			if (index < toks.Count && toks[index].Kind == TokKind.RBrace)
			{
				index++;
			}
			return index;
		}

		private int ReadMesh(List<Tok> toks, int index, Matrix world)
		{
			var nVerts = ReadInt(toks, ref index);
			var pos = new float[Math.Max(0, nVerts) * 3];
			for (var i = 0; i < pos.Length; i++)
			{
				pos[i] = ReadFloat(toks, ref index);
			}
			var nFaces = ReadInt(toks, ref index);
			var faces = new int[Math.Max(0, nFaces)][];
			for (var i = 0; i < faces.Length; i++)
			{
				var n = ReadInt(toks, ref index);
				var idx = new int[Math.Max(0, n)];
				for (var j = 0; j < idx.Length; j++)
				{
					idx[j] = ReadInt(toks, ref index);
				}
				faces[i] = idx;
			}
			float[] normals = null;
			int[][] normalFaces = null;
			float[] uvs = null;
			string[] matNames = null;
			int[] faceMats = null;
			while (index < toks.Count && toks[index].Kind != TokKind.RBrace)
			{
				if (!LooksLikeObject(toks, index))
				{
					index++;
					continue;
				}
				var kind = toks[index].Text;
				var body = SkipName(toks, index + 1);
				body++;
				if (kind == "MeshNormals")
				{
					var nNorm = ReadInt(toks, ref body);
					normals = new float[Math.Max(0, nNorm) * 3];
					for (var i = 0; i < normals.Length; i++)
					{
						normals[i] = ReadFloat(toks, ref body);
					}
					var nNormFaces = ReadInt(toks, ref body);
					normalFaces = new int[Math.Max(0, nNormFaces)][];
					for (var i = 0; i < normalFaces.Length; i++)
					{
						var n = ReadInt(toks, ref body);
						var idx = new int[Math.Max(0, n)];
						for (var j = 0; j < idx.Length; j++)
						{
							idx[j] = ReadInt(toks, ref body);
						}
						normalFaces[i] = idx;
					}
					index = CloseBlock(toks, body);
				}
				else if (kind == "MeshTextureCoords")
				{
					var nUv = ReadInt(toks, ref body);
					uvs = new float[Math.Max(0, nUv) * 2];
					for (var i = 0; i < uvs.Length; i++)
					{
						uvs[i] = ReadFloat(toks, ref body);
					}
					index = CloseBlock(toks, body);
				}
				else if (kind == "MeshMaterialList")
				{
					var nMat = ReadInt(toks, ref body);
					var nIdx = ReadInt(toks, ref body);
					faceMats = new int[Math.Max(0, nIdx)];
					for (var i = 0; i < faceMats.Length; i++)
					{
						faceMats[i] = ReadInt(toks, ref body);
					}
					matNames = new string[Math.Max(0, nMat)];
					for (var i = 0; i < matNames.Length; i++)
					{
						while (body < toks.Count && toks[body].Kind != TokKind.LBrace && toks[body].Kind != TokKind.RBrace)
						{
							body++;
						}
						if (body >= toks.Count || toks[body].Kind != TokKind.LBrace)
						{
							matNames[i] = "";
							continue;
						}
						body++;
						if (body < toks.Count && toks[body].Kind == TokKind.Ident && (body + 1 >= toks.Count || toks[body + 1].Kind != TokKind.LBrace))
						{
							matNames[i] = toks[body].Text;
							body++;
							if (body < toks.Count && toks[body].Kind == TokKind.RBrace)
							{
								body++;
							}
						}
						else
						{
							body = ReadMaterial(toks, body, "inline" + i.ToString(CultureInfo.InvariantCulture));
							matNames[i] = "inline" + i.ToString(CultureInfo.InvariantCulture);
						}
					}
					index = CloseBlock(toks, body);
				}
				else
				{
					index = ReadObject(toks, index, world);
				}
			}
			if (index < toks.Count && toks[index].Kind == TokKind.RBrace)
			{
				index++;
			}
			this.EmitFaces(world, pos, faces, normals, normalFaces, uvs, matNames, faceMats);
			return index;
		}

		private void EmitFaces(Matrix world, float[] pos, int[][] faces, float[] normals, int[][] normalFaces, float[] uvs, string[] matNames, int[] faceMats)
		{
			if (faces == null || pos == null)
			{
				return;
			}
			var vertCount = pos.Length / 3;
			for (var f = 0; f < faces.Length; f++)
			{
				var face = faces[f];
				if (face == null || face.Length < 3)
				{
					continue;
				}
				var matIndex = faceMats != null && f < faceMats.Length ? faceMats[f] : 0;
				var matName = matNames != null && matNames.Length > 0 ? matNames[Math.Clamp(matIndex, 0, matNames.Length - 1)] : "";
				var subset = this.SubsetFor(matName);
				var norms = normalFaces != null && f < normalFaces.Length ? normalFaces[f] : null;
				for (var corner = 1; corner < face.Length - 1; corner++)
				{
					var i0 = face[0];
					var i1 = face[corner];
					var i2 = face[corner + 1];
					if (i0 < 0 || i1 < 0 || i2 < 0 || i0 >= vertCount || i1 >= vertCount || i2 >= vertCount)
					{
						continue;
					}
					this.EmitCorner(subset, world, pos, vertCount, uvs, normals, i0, norms, 0);
					this.EmitCorner(subset, world, pos, vertCount, uvs, normals, i1, norms, corner);
					this.EmitCorner(subset, world, pos, vertCount, uvs, normals, i2, norms, corner + 1);
				}
			}
		}

		private void EmitCorner(Subset subset, Matrix world, float[] pos, int vertCount, float[] uvs, float[] normals, int vertex, int[] normFace, int corner)
		{
			if (vertex < 0 || vertex >= vertCount)
			{
				return;
			}
			var x = pos[vertex * 3];
			var y = pos[vertex * 3 + 1];
			var z = pos[vertex * 3 + 2];
			var ox = x * world.M11 + y * world.M21 + z * world.M31 + world.M41;
			var oy = x * world.M12 + y * world.M22 + z * world.M32 + world.M42;
			var oz = x * world.M13 + y * world.M23 + z * world.M33 + world.M43;
			var u = 0f;
			var v = 0f;
			if (uvs != null && vertex * 2 + 1 < uvs.Length)
			{
				u = uvs[vertex * 2];
				v = uvs[vertex * 2 + 1];
			}
			subset.Verts.Add(ox);
			subset.Verts.Add(oy);
			subset.Verts.Add(oz);
			subset.Verts.Add(u);
			subset.Verts.Add(v);
			this.Include(ox, oy, oz);
			if (normals != null && normFace != null && corner < normFace.Length)
			{
				var n = normFace[corner];
				if (n >= 0 && n * 3 + 2 < normals.Length)
				{
					var nx = normals[n * 3];
					var ny = normals[n * 3 + 1];
					var nz = normals[n * 3 + 2];
					var nnx = nx * world.M11 + ny * world.M21 + nz * world.M31;
					var nny = nx * world.M12 + ny * world.M22 + nz * world.M32;
					var nnz = nx * world.M13 + ny * world.M23 + nz * world.M33;
					var len = (float)Math.Sqrt(nnx * nnx + nny * nny + nnz * nnz);
					if (len > 1E-08f)
					{
						nnx /= len;
						nny /= len;
						nnz /= len;
					}
				}
			}
		}

		private void Include(float x, float y, float z)
		{
			if (!this.hasBound)
			{
				this.BoundMin = new Vector3(x, y, z);
				this.BoundMax = new Vector3(x, y, z);
				this.hasBound = true;
				return;
			}
			var min = this.BoundMin;
			var max = this.BoundMax;
			if (x < min.X) min.X = x;
			if (y < min.Y) min.Y = y;
			if (z < min.Z) min.Z = z;
			if (x > max.X) max.X = x;
			if (y > max.Y) max.Y = y;
			if (z > max.Z) max.Z = z;
			this.BoundMin = min;
			this.BoundMax = max;
		}

		private Subset SubsetFor(string name)
		{
			var rec = this.MaterialRec(name);
			for (var i = 0; i < this.subsets.Count; i++)
			{
				if (object.ReferenceEquals(this.subsets[i].Material, rec.Material))
				{
					return this.subsets[i];
				}
			}
			var subset = new Subset();
			subset.Material = rec.Material;
			this.subsets.Add(subset);
			return subset;
		}

		private MatRec MaterialRec(string name)
		{
			if (name == null)
			{
				name = "";
			}
			MatRec rec;
			if (!this.materials.TryGetValue(name, out rec))
			{
				rec = new MatRec();
				var material = default(Material);
				material.Diffuse = new Color4(1f, 1f, 1f, 1f);
				material.Ambient = new Color4(1f, 1f, 1f, 1f);
				rec.Material.MaterialD3D = material;
				this.materials[name] = rec;
			}
			return rec;
		}

		private int ReadMaterial(List<Tok> toks, int index, string name)
		{
			var rec = this.MaterialRec(name ?? "");
			var red = ReadFloat(toks, ref index);
			var green = ReadFloat(toks, ref index);
			var blue = ReadFloat(toks, ref index);
			var alpha = ReadFloat(toks, ref index);
			var power = ReadFloat(toks, ref index);
			var sr = ReadFloat(toks, ref index);
			var sg = ReadFloat(toks, ref index);
			var sb = ReadFloat(toks, ref index);
			var er = ReadFloat(toks, ref index);
			var eg = ReadFloat(toks, ref index);
			var eb = ReadFloat(toks, ref index);
			var material = default(Material);
			material.Diffuse = new Color4(alpha, red, green, blue);
			material.Ambient = material.Diffuse;
			material.Specular = new Color4(1f, sr, sg, sb);
			material.Emissive = new Color4(1f, er, eg, eb);
			material.Power = power;
			rec.Material.MaterialD3D = material;
			while (index < toks.Count && toks[index].Kind != TokKind.RBrace)
			{
				if (LooksLikeObject(toks, index) && toks[index].Text == "TextureFilename")
				{
					index = SkipName(toks, index + 1);
					index++;
					if (index < toks.Count && toks[index].Kind == TokKind.String)
					{
						rec.Material.TextureFileName = this.ResolveTexture(toks[index].Text);
						index++;
					}
					index = CloseBlock(toks, index);
				}
				else
				{
					index++;
				}
			}
			if (index < toks.Count && toks[index].Kind == TokKind.RBrace)
			{
				index++;
			}
			return index;
		}

		private string ResolveTexture(string raw)
		{
			if (string.IsNullOrEmpty(raw))
			{
				return "";
			}
			var flat = raw.Replace("/", "").Replace("\\", "");
			var file = Path.GetFileName(raw.Replace('\\', '/'));
			var match = this.MatchFile(flat);
			if (match == null)
			{
				match = this.MatchFile(file);
			}
			if (match == null)
			{
				match = this.MatchFile(raw);
			}
			return match ?? flat;
		}

		private string MatchFile(string name)
		{
			if (string.IsNullOrEmpty(name))
			{
				return null;
			}
			for (var i = 0; i < this.modelFiles.Length; i++)
			{
				if (string.Equals(this.modelFiles[i], name, StringComparison.OrdinalIgnoreCase))
				{
					return this.modelFiles[i];
				}
			}
			return null;
		}

		private static Matrix ReadMatrix(List<Tok> toks, ref int index)
		{
			var matrix = default(Matrix);
			matrix.M11 = ReadFloat(toks, ref index);
			matrix.M12 = ReadFloat(toks, ref index);
			matrix.M13 = ReadFloat(toks, ref index);
			matrix.M14 = ReadFloat(toks, ref index);
			matrix.M21 = ReadFloat(toks, ref index);
			matrix.M22 = ReadFloat(toks, ref index);
			matrix.M23 = ReadFloat(toks, ref index);
			matrix.M24 = ReadFloat(toks, ref index);
			matrix.M31 = ReadFloat(toks, ref index);
			matrix.M32 = ReadFloat(toks, ref index);
			matrix.M33 = ReadFloat(toks, ref index);
			matrix.M34 = ReadFloat(toks, ref index);
			matrix.M41 = ReadFloat(toks, ref index);
			matrix.M42 = ReadFloat(toks, ref index);
			matrix.M43 = ReadFloat(toks, ref index);
			matrix.M44 = ReadFloat(toks, ref index);
			return matrix;
		}

		private static int ReadInt(List<Tok> toks, ref int index)
		{
			return (int)ReadFloat(toks, ref index);
		}

		private static float ReadFloat(List<Tok> toks, ref int index)
		{
			while (index < toks.Count && toks[index].Kind != TokKind.Number)
			{
				if (toks[index].Kind == TokKind.RBrace || toks[index].Kind == TokKind.Ident)
				{
					throw new InvalidDataException("Unexpected token in .x mesh.");
				}
				index++;
			}
			if (index >= toks.Count)
			{
				throw new EndOfStreamException("Truncated .x mesh.");
			}
			return (float)toks[index++].Number;
		}

		private static int SkipName(List<Tok> toks, int index)
		{
			if (index < toks.Count && toks[index].Kind == TokKind.Ident)
			{
				index++;
			}
			return index;
		}

		private static int CloseBlock(List<Tok> toks, int index)
		{
			while (index < toks.Count && toks[index].Kind != TokKind.RBrace)
			{
				if (toks[index].Kind == TokKind.LBrace)
				{
					index = SkipBlock(toks, index + 1);
				}
				else
				{
					index++;
				}
			}
			if (index < toks.Count && toks[index].Kind == TokKind.RBrace)
			{
				index++;
			}
			return index;
		}

		private static int SkipBlock(List<Tok> toks, int index)
		{
			var depth = 1;
			while (index < toks.Count && depth > 0)
			{
				if (toks[index].Kind == TokKind.LBrace)
				{
					depth++;
				}
				else if (toks[index].Kind == TokKind.RBrace)
				{
					depth--;
				}
				index++;
			}
			return index;
		}

		private static bool LooksLikeObject(List<Tok> toks, int index)
		{
			if (index >= toks.Count || toks[index].Kind != TokKind.Ident)
			{
				return false;
			}
			index++;
			if (index < toks.Count && toks[index].Kind == TokKind.Ident)
			{
				index++;
			}
			return index < toks.Count && toks[index].Kind == TokKind.LBrace;
		}

		private static List<Tok> Tokenize(string text)
		{
			var toks = new List<Tok>();
			var i = 0;
			while (i < text.Length)
			{
				var c = text[i];
				if (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == ';' || c == ',' || c == '<' || c == '>')
				{
					i++;
					continue;
				}
				if (c == '/' && i + 1 < text.Length && text[i + 1] == '/')
				{
					i += 2;
					while (i < text.Length && text[i] != '\n')
					{
						i++;
					}
					continue;
				}
				if (c == '{')
				{
					toks.Add(new Tok { Kind = TokKind.LBrace });
					i++;
					continue;
				}
				if (c == '}')
				{
					toks.Add(new Tok { Kind = TokKind.RBrace });
					i++;
					continue;
				}
				if (c == '"')
				{
					i++;
					var builder = new StringBuilder();
					while (i < text.Length && text[i] != '"')
					{
						if (text[i] == '\\' && i + 1 < text.Length && (text[i + 1] == '\\' || text[i + 1] == '"'))
						{
							builder.Append(text[i + 1]);
							i += 2;
						}
						else
						{
							builder.Append(text[i]);
							i++;
						}
					}
					if (i < text.Length && text[i] == '"')
					{
						i++;
					}
					toks.Add(new Tok { Kind = TokKind.String, Text = builder.ToString() });
					continue;
				}
				if (c == '-' || c == '+' || c == '.' || (c >= '0' && c <= '9'))
				{
					var start = i;
					if (c == '-' || c == '+')
					{
						i++;
					}
					var digit = false;
					while (i < text.Length && text[i] >= '0' && text[i] <= '9')
					{
						digit = true;
						i++;
					}
					if (i < text.Length && text[i] == '.')
					{
						i++;
						while (i < text.Length && text[i] >= '0' && text[i] <= '9')
						{
							digit = true;
							i++;
						}
					}
					if (i < text.Length && (text[i] == 'e' || text[i] == 'E'))
					{
						var exp = i;
						i++;
						if (i < text.Length && (text[i] == '+' || text[i] == '-'))
						{
							i++;
						}
						var expDigits = i;
						while (i < text.Length && text[i] >= '0' && text[i] <= '9')
						{
							i++;
						}
						if (i == expDigits)
						{
							i = exp;
						}
					}
					if (!digit)
					{
						i = start + 1;
						continue;
					}
					var slice = text.Substring(start, i - start);
					double number;
					if (!double.TryParse(slice, NumberStyles.Float, CultureInfo.InvariantCulture, out number))
					{
						number = 0.0;
					}
					toks.Add(new Tok { Kind = TokKind.Number, Number = number, Text = slice });
					continue;
				}
				if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_')
				{
					var start = i;
					i++;
					while (i < text.Length)
					{
						var n = text[i];
						if ((n >= 'A' && n <= 'Z') || (n >= 'a' && n <= 'z') || (n >= '0' && n <= '9') || n == '_')
						{
							i++;
							continue;
						}
						break;
					}
					toks.Add(new Tok { Kind = TokKind.Ident, Text = text.Substring(start, i - start) });
					continue;
				}
				i++;
			}
			return toks;
		}

		private static string ResolvePath(string fileName)
		{
			var path = (fileName ?? "").Replace('\\', '/');
			if (File.Exists(path))
			{
				return path;
			}
			var dir = Path.GetDirectoryName(path);
			var name = Path.GetFileName(path);
			if (!string.IsNullOrEmpty(dir) && Directory.Exists(dir))
			{
				var files = Directory.GetFiles(dir);
				for (var i = 0; i < files.Length; i++)
				{
					if (string.Equals(Path.GetFileName(files[i]), name, StringComparison.OrdinalIgnoreCase))
					{
						return files[i];
					}
				}
			}
			throw new FileNotFoundException("Mesh file not found.", fileName);
		}

		private static string[] ListFileNames(string dir)
		{
			if (string.IsNullOrEmpty(dir) || !Directory.Exists(dir))
			{
				return new string[0];
			}
			var files = Directory.GetFiles(dir);
			var names = new string[files.Length];
			for (var i = 0; i < files.Length; i++)
			{
				names[i] = Path.GetFileName(files[i]);
			}
			return names;
		}
	}
}
