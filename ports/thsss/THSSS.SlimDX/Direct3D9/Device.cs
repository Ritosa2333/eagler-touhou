using System;
using System.Drawing;
using SlimDX.Gl;

namespace SlimDX.Direct3D9
{
	public class Device : ComObject
	{
		public static Device Active { get; private set; }

		public int BackBufferWidth = 640;

		public int BackBufferHeight = 480;

		public Viewport Viewport;

		public VertexDeclaration VertexDeclaration { get; set; }

		public VertexFormat VertexFormat { get; set; }

		public Material Material { get; set; }

		public bool AlphaBlendEnable = true;

		public bool DepthEnable = true;

		public bool DepthWrite = true;

		public bool Lighting = true;

		public bool FogEnable;

		public float FogStart;

		public float FogEnd = 1000f;

		public int FogColor;

		public int BlendSrc = (int)Blend.SourceAlpha;

		public int BlendDst = (int)Blend.InverseSourceAlpha;

		public int BlendEquation = (int)BlendOperation.Add;

		public int CullMode = 1;

		private VertexBuffer stream;

		private int streamStride = 20;

		private Texture boundTexture;

		private Matrix world = Matrix.Identity;

		private Matrix view = Matrix.Identity;

		private Matrix projection = Matrix.Identity;

		private float[] vertSrc = new float[576];

		private float[] vertDst = new float[576];

		private readonly float[] worldBuf = new float[16];

		private readonly float[] viewBuf = new float[16];

		private readonly float[] projBuf = new float[16];

		private static readonly float[] IdentityWorld = new float[]
		{
			1f, 0f, 0f, 0f,
			0f, 1f, 0f, 0f,
			0f, 0f, 1f, 0f,
			0f, 0f, 0f, 1f
		};

		private float[] worldBatch = new float[8192];

		private int worldBatchCount;

		private bool worldBatchOpen;

		private int worldBatchTex;

		private int worldBatchTarget;

		private bool worldBatchAlpha;

		private int worldBatchSrc;

		private int worldBatchDst;

		private int worldBatchEq;

		private bool worldBatchDepth;

		private bool worldBatchWrite;

		private int worldBatchCull;

		private bool worldBatchFog;

		private float worldBatchFogStart;

		private float worldBatchFogEnd;

		private int worldBatchFogColor;

		private int batchVx;

		private int batchVy;

		private int batchVw;

		private int batchVh;

		private int batchCw;

		private int batchCh;

		private Surface backBuffer;

		private Surface currentColor;

		public Direct3D Direct3D { get; private set; }

		public Surface DepthStencilSurface { get; set; }

		public Device(Direct3D direct3D, int adapter, DeviceType deviceType, IntPtr window, CreateFlags createFlags, PresentParameters[] presentParameters)
		{
			Active = this;
			this.Direct3D = direct3D;
			if (presentParameters != null && presentParameters.Length > 0 && presentParameters[0] != null)
			{
				if (presentParameters[0].BackBufferWidth > 0)
				{
					this.BackBufferWidth = presentParameters[0].BackBufferWidth;
				}
				if (presentParameters[0].BackBufferHeight > 0)
				{
					this.BackBufferHeight = presentParameters[0].BackBufferHeight;
				}
			}
			this.Viewport = new Viewport
			{
				X = 0,
				Y = 0,
				Width = this.BackBufferWidth,
				Height = this.BackBufferHeight,
				MinZ = 0f,
				MaxZ = 1f
			};
			this.backBuffer = new Surface
			{
				Width = this.BackBufferWidth,
				Height = this.BackBufferHeight,
				GlTextureId = 0
			};
			this.currentColor = this.backBuffer;
			var material = default(Material);
			material.Diffuse = new Color4(1f, 1f, 1f, 1f);
			material.Ambient = new Color4(1f, 1f, 1f, 1f);
			this.Material = material;
			GlHost.Need().BindTarget(0);
			this.ApplyViewport();
		}

		public void Clear(ClearFlags flags, Color color, float z, int stencil)
		{
			this.Clear(flags, new Color4(color), z, stencil);
		}

		public void Clear(ClearFlags flags, Color4 color, float z, int stencil)
		{
			this.FlushWorldBatch();
			this.ApplyTarget();
			this.ApplyViewport();
			GlHost.Need().Clear(color.Red, color.Green, color.Blue, color.Alpha, (flags & ClearFlags.Target) != 0, (flags & ClearFlags.ZBuffer) != 0);
		}

		public void BeginScene()
		{
		}

		public void EndScene()
		{
			this.FlushWorldBatch();
		}

		public void Present(Present flags)
		{
			this.FlushWorldBatch();
			this.ApplyTarget();
		}

		public Result TestCooperativeLevel()
		{
			var result = Result.Success;
			Result.Last = result;
			return result;
		}

		public void Reset(PresentParameters[] presentParameters)
		{
		}

		public void SetRenderState(RenderState state, bool value)
		{
			this.ApplyInt(state, value ? 1 : 0);
		}

		public void SetRenderState(RenderState state, int value)
		{
			this.ApplyInt(state, value);
		}

		public void SetRenderState(RenderState state, float value)
		{
			if (state == RenderState.FogStart)
			{
				this.FogStart = value;
			}
			else if (state == RenderState.FogEnd)
			{
				this.FogEnd = value;
			}
			else
			{
				this.ApplyInt(state, (int)value);
			}
		}

		public void SetRenderState<T>(RenderState state, T value) where T : struct
		{
			this.ApplyInt(state, Convert.ToInt32(value));
		}

		public void SetTextureStageState(int stage, TextureStage type, int value)
		{
		}

		public void SetTextureStageState(int stage, TextureStage type, TextureArgument value)
		{
		}

		public void SetTextureStageState(int stage, TextureStage type, TextureOperation value)
		{
		}

		public void SetTransform(TransformState state, Matrix matrix)
		{
			if (state == TransformState.View || state == TransformState.Projection)
			{
				this.FlushWorldBatch();
			}
			if (state == TransformState.World)
			{
				this.world = matrix;
			}
			else if (state == TransformState.View)
			{
				this.view = matrix;
			}
			else if (state == TransformState.Projection)
			{
				this.projection = matrix;
			}
		}

		public void SetTexture(int stage, Texture texture)
		{
			this.boundTexture = texture;
		}

		public void SetTexture(int stage, BaseTexture texture)
		{
			this.boundTexture = texture as Texture;
		}

		public void SetStreamSource(int streamNumber, VertexBuffer buffer, int offset, int stride)
		{
			this.stream = buffer;
			this.streamStride = stride > 0 ? stride : 20;
		}

		public void SetLight(int index, Light light)
		{
		}

		public void EnableLight(int index, bool enable)
		{
		}

		public void SetRenderTarget(int index, Surface surface)
		{
			this.FlushWorldBatch();
			this.currentColor = surface ?? this.backBuffer;
			this.ApplyTarget();
		}

		public Surface GetRenderTarget(int index)
		{
			return this.currentColor ?? this.backBuffer;
		}

		public Surface GetBackBuffer(int swapChain, int backBuffer)
		{
			return this.backBuffer;
		}

		public void DrawPrimitives(PrimitiveType primitiveType, int startVertex, int primitiveCount)
		{
			if (this.stream == null || this.stream.Data == null || primitiveCount <= 0)
			{
				return;
			}
			var srcCount = primitiveType == PrimitiveType.TriangleList ? primitiveCount * 3 : primitiveCount + 2;
			if (startVertex < 0 || srcCount <= 0)
			{
				return;
			}
			this.EnsureDecoded();
			if (this.stream.Decoded == null || startVertex + srcCount > this.stream.DecodedCount)
			{
				return;
			}
			if (Sprite.Current != null)
			{
				Sprite.Current.FlushPending();
			}
			var tex = this.boundTexture != null && !this.boundTexture.Disposed ? this.boundTexture.GlId : 0;
			if (this.worldBatchOpen && !this.BatchSame(tex))
			{
				this.FlushWorldBatch();
			}
			if (!this.worldBatchOpen)
			{
				this.OpenBatch(tex);
			}
			var outVerts = primitiveType == PrimitiveType.TriangleList ? srcCount : primitiveCount * 3;
			this.EnsureBatchRoom(outVerts * 9);
			var mr = 1f;
			var mg = 1f;
			var mb = 1f;
			var ma = 1f;
			if (this.Lighting)
			{
				var color = this.Material.Diffuse;
				mr = color.Red;
				mg = color.Green;
				mb = color.Blue;
				ma = color.Alpha;
			}
			if (primitiveType == PrimitiveType.TriangleStrip)
			{
				for (var j = 0; j < primitiveCount; j++)
				{
					this.BakeVertex(startVertex + j, mr, mg, mb, ma);
					this.BakeVertex(startVertex + j + 1, mr, mg, mb, ma);
					this.BakeVertex(startVertex + j + 2, mr, mg, mb, ma);
				}
			}
			else if (primitiveType == PrimitiveType.TriangleFan)
			{
				for (var k = 0; k < primitiveCount; k++)
				{
					this.BakeVertex(startVertex, mr, mg, mb, ma);
					this.BakeVertex(startVertex + k + 1, mr, mg, mb, ma);
					this.BakeVertex(startVertex + k + 2, mr, mg, mb, ma);
				}
			}
			else
			{
				for (var i = 0; i < srcCount; i++)
				{
					this.BakeVertex(startVertex + i, mr, mg, mb, ma);
				}
			}
		}

		public void FlushWorldBatch()
		{
			if (!this.worldBatchOpen)
			{
				return;
			}
			this.worldBatchOpen = false;
			var count = this.worldBatchCount;
			this.worldBatchCount = 0;
			if (count < 9)
			{
				return;
			}
			var fr = (float)((this.worldBatchFogColor >> 16) & 255) / 255f;
			var fg = (float)((this.worldBatchFogColor >> 8) & 255) / 255f;
			var fb = (float)(this.worldBatchFogColor & 255) / 255f;
			Store(this.view, this.viewBuf);
			Store(this.projection, this.projBuf);
			var api = GlHost.Need();
			api.BindTarget(this.worldBatchTarget);
			api.SetViewport(this.batchVx, this.batchVy, this.batchVw, this.batchVh, this.batchCw, this.batchCh);
			api.SetBlend(this.worldBatchAlpha, this.worldBatchSrc, this.worldBatchDst, this.worldBatchEq);
			api.SetDepth(this.worldBatchDepth, this.worldBatchWrite);
			api.SetCull(this.worldBatchCull);
			api.DrawFixed(this.worldBatchTex, this.worldBatch.AsSpan(0, count), IdentityWorld.AsSpan(), this.viewBuf.AsSpan(), this.projBuf.AsSpan(), 1f, 1f, 1f, 1f, 0f, this.worldBatchFog ? 1f : 0f, this.worldBatchFogStart, this.worldBatchFogEnd, fr, fg, fb);
		}

		private static void Store(Matrix matrix, float[] dest)
		{
			dest[0] = matrix.M11;
			dest[1] = matrix.M12;
			dest[2] = matrix.M13;
			dest[3] = matrix.M14;
			dest[4] = matrix.M21;
			dest[5] = matrix.M22;
			dest[6] = matrix.M23;
			dest[7] = matrix.M24;
			dest[8] = matrix.M31;
			dest[9] = matrix.M32;
			dest[10] = matrix.M33;
			dest[11] = matrix.M34;
			dest[12] = matrix.M41;
			dest[13] = matrix.M42;
			dest[14] = matrix.M43;
			dest[15] = matrix.M44;
		}

		private static void EnsureScratch(ref float[] buffer, int floats)
		{
			if (buffer.Length >= floats)
			{
				return;
			}
			var num = buffer.Length;
			while (num < floats)
			{
				num *= 2;
			}
			buffer = new float[num];
		}

		private static void Copy9(float[] src, int srcVert, float[] dst, int dstVert)
		{
			var num = srcVert * 9;
			var num2 = dstVert * 9;
			for (var i = 0; i < 9; i++)
			{
				dst[num2 + i] = num + i < src.Length ? src[num + i] : 0f;
			}
		}

		private void WriteVertex(float[] dest, int vertexIndex, int index)
		{
			var data = this.stream.Data;
			var num = index * this.streamStride;
			var num2 = 0f;
			var num3 = 0f;
			var num4 = 0f;
			var num5 = 0f;
			var num6 = 0f;
			var num7 = 1f;
			var num8 = 1f;
			var num9 = 1f;
			var num10 = 1f;
			var array = this.VertexDeclaration != null ? this.VertexDeclaration.Elements : null;
			if (array == null || array.Length == 0)
			{
				num2 = ReadSingle(data, num);
				num3 = ReadSingle(data, num + 4);
				num4 = ReadSingle(data, num + 8);
				num5 = ReadSingle(data, num + 12);
				num6 = ReadSingle(data, num + 16);
			}
			else
			{
				for (var i = 0; i < array.Length; i++)
				{
					var vertexElement = array[i];
					if (vertexElement.Stream == 255)
					{
						break;
					}
					var num11 = num + vertexElement.Offset;
					if (vertexElement.Usage == DeclarationUsage.Position)
					{
						num2 = ReadSingle(data, num11);
						num3 = ReadSingle(data, num11 + 4);
						num4 = ReadSingle(data, num11 + 8);
					}
					else if (vertexElement.Usage == DeclarationUsage.TextureCoordinate)
					{
						num5 = ReadSingle(data, num11);
						num6 = ReadSingle(data, num11 + 4);
					}
					else if (vertexElement.Usage == DeclarationUsage.Color)
					{
						var num12 = ReadInt(data, num11);
						num10 = (float)((num12 >> 24) & 255) / 255f;
						num7 = (float)((num12 >> 16) & 255) / 255f;
						num8 = (float)((num12 >> 8) & 255) / 255f;
						num9 = (float)(num12 & 255) / 255f;
					}
				}
			}
			var num13 = vertexIndex * 9;
			dest[num13] = num2;
			dest[num13 + 1] = num3;
			dest[num13 + 2] = num4;
			dest[num13 + 3] = num5;
			dest[num13 + 4] = num6;
			dest[num13 + 5] = num7;
			dest[num13 + 6] = num8;
			dest[num13 + 7] = num9;
			dest[num13 + 8] = num10;
		}

		private static float ReadSingle(byte[] data, int offset)
		{
			if (data == null || offset < 0 || offset + 4 > data.Length)
			{
				return 0f;
			}
			return BitConverter.ToSingle(data, offset);
		}

		private static int ReadInt(byte[] data, int offset)
		{
			if (data == null || offset < 0 || offset + 4 > data.Length)
			{
				return -1;
			}
			return BitConverter.ToInt32(data, offset);
		}

		private void EnsureDecoded()
		{
			var buffer = this.stream;
			var stride = this.streamStride;
			if (buffer.Data == null || stride <= 0)
			{
				return;
			}
			var count = buffer.Data.Length / stride;
			if (!buffer.DecodeDirty && buffer.DecodedDecl == this.VertexDeclaration && buffer.Decoded != null && buffer.DecodedCount == count)
			{
				return;
			}
			if (buffer.Decoded == null || buffer.Decoded.Length < count * 9)
			{
				buffer.Decoded = new float[count * 9 > 0 ? count * 9 : 9];
			}
			for (var i = 0; i < count; i++)
			{
				this.WriteVertex(buffer.Decoded, i, i);
			}
			buffer.DecodedCount = count;
			buffer.DecodedDecl = this.VertexDeclaration;
			buffer.DecodeDirty = false;
		}

		private bool BatchSame(int tex)
		{
			var viewport = this.Viewport;
			var width = viewport.Width > 0 ? viewport.Width : this.BackBufferWidth;
			var height = viewport.Height > 0 ? viewport.Height : this.BackBufferHeight;
			var target = this.currentColor != null ? this.currentColor.GlTextureId : 0;
			return this.worldBatchTex == tex && this.worldBatchTarget == target && this.worldBatchAlpha == this.AlphaBlendEnable && this.worldBatchSrc == this.BlendSrc && this.worldBatchDst == this.BlendDst && this.worldBatchEq == this.BlendEquation && this.worldBatchDepth == this.DepthEnable && this.worldBatchWrite == this.DepthWrite && this.worldBatchCull == this.CullMode && this.worldBatchFog == this.FogEnable && this.worldBatchFogStart == this.FogStart && this.worldBatchFogEnd == this.FogEnd && this.worldBatchFogColor == this.FogColor && this.batchVx == viewport.X && this.batchVy == viewport.Y && this.batchVw == width && this.batchVh == height && this.batchCw == this.BackBufferWidth && this.batchCh == this.BackBufferHeight;
		}

		private void OpenBatch(int tex)
		{
			var viewport = this.Viewport;
			this.worldBatchOpen = true;
			this.worldBatchCount = 0;
			this.worldBatchTex = tex;
			this.worldBatchTarget = this.currentColor != null ? this.currentColor.GlTextureId : 0;
			this.worldBatchAlpha = this.AlphaBlendEnable;
			this.worldBatchSrc = this.BlendSrc;
			this.worldBatchDst = this.BlendDst;
			this.worldBatchEq = this.BlendEquation;
			this.worldBatchDepth = this.DepthEnable;
			this.worldBatchWrite = this.DepthWrite;
			this.worldBatchCull = this.CullMode;
			this.worldBatchFog = this.FogEnable;
			this.worldBatchFogStart = this.FogStart;
			this.worldBatchFogEnd = this.FogEnd;
			this.worldBatchFogColor = this.FogColor;
			this.batchVx = viewport.X;
			this.batchVy = viewport.Y;
			this.batchVw = viewport.Width > 0 ? viewport.Width : this.BackBufferWidth;
			this.batchVh = viewport.Height > 0 ? viewport.Height : this.BackBufferHeight;
			this.batchCw = this.BackBufferWidth;
			this.batchCh = this.BackBufferHeight;
		}

		private void EnsureBatchRoom(int floats)
		{
			if (this.worldBatchCount > 0 && this.worldBatchCount + floats > 65536)
			{
				var tex = this.worldBatchTex;
				this.FlushWorldBatch();
				this.OpenBatch(tex);
			}
			if (this.worldBatchCount + floats > this.worldBatch.Length)
			{
				var num = this.worldBatch.Length;
				while (num < this.worldBatchCount + floats)
				{
					num *= 2;
				}
				var next = new float[num];
				if (this.worldBatchCount > 0)
				{
					Array.Copy(this.worldBatch, next, this.worldBatchCount);
				}
				this.worldBatch = next;
			}
		}

		private void BakeVertex(int srcIndex, float mr, float mg, float mb, float ma)
		{
			var src = this.stream.Decoded;
			var num = srcIndex * 9;
			var x = src[num];
			var y = src[num + 1];
			var z = src[num + 2];
			var matrix = this.world;
			var dest = this.worldBatchCount;
			var data = this.worldBatch;
			data[dest] = matrix.M11 * x + matrix.M21 * y + matrix.M31 * z + matrix.M41;
			data[dest + 1] = matrix.M12 * x + matrix.M22 * y + matrix.M32 * z + matrix.M42;
			data[dest + 2] = matrix.M13 * x + matrix.M23 * y + matrix.M33 * z + matrix.M43;
			data[dest + 3] = src[num + 3];
			data[dest + 4] = src[num + 4];
			data[dest + 5] = src[num + 5] * mr;
			data[dest + 6] = src[num + 6] * mg;
			data[dest + 7] = src[num + 7] * mb;
			data[dest + 8] = src[num + 8] * ma;
			this.worldBatchCount = dest + 9;
		}

		private void ApplyInt(RenderState state, int value)
		{
			if (state == RenderState.AlphaBlendEnable)
			{
				this.AlphaBlendEnable = value != 0;
			}
			else if (state == RenderState.SourceBlend)
			{
				this.BlendSrc = value;
			}
			else if (state == RenderState.DestinationBlend)
			{
				this.BlendDst = value;
			}
			else if (state == RenderState.BlendOperation)
			{
				this.BlendEquation = value;
			}
			else if (state == RenderState.ZEnable)
			{
				this.DepthEnable = value != 0;
			}
			else if (state == RenderState.ZWriteEnable)
			{
				this.DepthWrite = value != 0;
			}
			else if (state == RenderState.Lighting)
			{
				this.Lighting = value != 0;
			}
			else if (state == RenderState.FogEnable)
			{
				this.FogEnable = value != 0;
			}
			else if (state == RenderState.FogColor)
			{
				this.FogColor = value;
			}
			else if (state == RenderState.CullMode)
			{
				this.CullMode = value;
			}
		}

		private void ApplyTarget()
		{
			var num = this.currentColor != null ? this.currentColor.GlTextureId : 0;
			GlHost.Need().BindTarget(num);
		}

		private void ApplyViewport()
		{
			var viewport = this.Viewport;
			var num = viewport.Width > 0 ? viewport.Width : this.BackBufferWidth;
			var num2 = viewport.Height > 0 ? viewport.Height : this.BackBufferHeight;
			GlHost.Need().SetViewport(viewport.X, viewport.Y, num, num2, this.BackBufferWidth, this.BackBufferHeight);
		}
	}
}
