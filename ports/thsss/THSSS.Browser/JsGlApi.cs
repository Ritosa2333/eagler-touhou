using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using SlimDX.Gl;

namespace Thsss.Browser
{
	public sealed class JsGlApi : IGlApi
	{
		private int viewX = int.MinValue;

		private int viewY;

		private int viewW;

		private int viewH;

		private int viewCw;

		private int viewCh;

		private int blendSrc = int.MinValue;

		private int blendDst;

		private int blendEquation;

		private bool blendEnable;

		private bool blendKnown;

		private bool depthOn;

		private bool depthWrite;

		private bool depthKnown;

		private int boundTarget = int.MinValue;

		private int cullMode = int.MinValue;

		private readonly Dictionary<int, byte[]> packed = new Dictionary<int, byte[]>();

		private readonly byte[] worldBytes = new byte[64];

		private readonly byte[] viewBytes = new byte[64];

		private readonly byte[] projBytes = new byte[64];

		public void Clear(float r, float g, float b, float a, bool color, bool depth)
		{
			ThsssJs.Clear(r, g, b, a, color, depth);
		}

		public int CreateTexture(int width, int height, byte[] rgba)
		{
			return ThsssJs.CreateTexture(width, height, rgba);
		}

		public int CreateRenderTarget(int width, int height)
		{
			return ThsssJs.CreateRenderTarget(width, height);
		}

		public void UpdateTexture(int id, int width, int height, byte[] rgba)
		{
			ThsssJs.UpdateTexture(id, width, height, rgba);
		}

		public void DeleteTexture(int id)
		{
			ThsssJs.DeleteTexture(id);
		}

		public void BindTarget(int textureId)
		{
			if (this.boundTarget == textureId)
			{
				return;
			}
			this.boundTarget = textureId;
			this.viewX = int.MinValue;
			ThsssJs.BindTarget(textureId);
		}

		public void SetViewport(int x, int y, int width, int height, int canvasWidth, int canvasHeight)
		{
			if (this.viewX == x && this.viewY == y && this.viewW == width && this.viewH == height && this.viewCw == canvasWidth && this.viewCh == canvasHeight)
			{
				return;
			}
			this.viewX = x;
			this.viewY = y;
			this.viewW = width;
			this.viewH = height;
			this.viewCw = canvasWidth;
			this.viewCh = canvasHeight;
			ThsssJs.SetViewport(x, y, width, height, canvasWidth, canvasHeight);
		}

		public void SetBlend(bool enable, int src, int dst, int equation)
		{
			if (this.blendKnown && this.blendEnable == enable && this.blendSrc == src && this.blendDst == dst && this.blendEquation == equation)
			{
				return;
			}
			this.blendKnown = true;
			this.blendEnable = enable;
			this.blendSrc = src;
			this.blendDst = dst;
			this.blendEquation = equation;
			ThsssJs.SetBlend(enable, src, dst, equation);
		}

		public void SetDepth(bool enable, bool write)
		{
			if (this.depthKnown && this.depthOn == enable && this.depthWrite == write)
			{
				return;
			}
			this.depthKnown = true;
			this.depthOn = enable;
			this.depthWrite = write;
			ThsssJs.SetDepth(enable, write);
		}

		public void SetCull(int mode)
		{
			if (this.cullMode == mode)
			{
				return;
			}
			this.cullMode = mode;
			ThsssJs.SetCull(mode);
		}

		public void DrawSprites(int textureId, float flipV, Span<float> vertices)
		{
			ThsssJs.DrawSprites(textureId, flipV, this.Pack(vertices));
		}

		public void DrawEffect(string name, int textureId, float flipV, Span<float> vertices, float blurLevel, float x, float y, float time, float radius, int colorKey)
		{
			ThsssJs.DrawEffect(name, textureId, flipV, this.Pack(vertices), blurLevel, x, y, time, radius, colorKey);
		}

		public void DrawFixed(int textureId, Span<float> vertices, Span<float> world, Span<float> view, Span<float> proj, float mr, float mg, float mb, float ma, float lighting, float fog, float fogStart, float fogEnd, float fr, float fg, float fb)
		{
			var verts = this.Pack(vertices);
			this.CopyExact(world, this.worldBytes);
			this.CopyExact(view, this.viewBytes);
			this.CopyExact(proj, this.projBytes);
			ThsssJs.DrawFixed(textureId, verts, this.worldBytes, this.viewBytes, this.projBytes, mr, mg, mb, ma, lighting, fog, fogStart, fogEnd, fr, fg, fb);
		}

		private byte[] Pack(Span<float> source)
		{
			var bytes = source.Length * 4;
			byte[] buffer;
			if (!this.packed.TryGetValue(bytes, out buffer))
			{
				buffer = new byte[bytes];
				this.packed[bytes] = buffer;
			}
			MemoryMarshal.AsBytes(source).CopyTo(buffer);
			return buffer;
		}

		private void CopyExact(Span<float> source, byte[] dest)
		{
			MemoryMarshal.AsBytes(source).CopyTo(dest);
		}

		public byte[] RasterText(string text, int width, int height, float sizePx, bool bold, bool italic)
		{
			return ThsssJs.RasterText(text, width, height, sizePx, bold, italic);
		}

		public void PaintText(int textureId, string text, int width, int height, float sizePx, bool bold, bool italic, int argb, string family)
		{
			ThsssJs.PaintText(textureId, text, width, height, sizePx, bold, italic, argb, family ?? "sans-serif");
		}
	}
}
