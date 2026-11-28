using System;

namespace SlimDX.Gl
{
	public interface IGlApi
	{
		void Clear(float r, float g, float b, float a, bool color, bool depth);

		int CreateTexture(int width, int height, byte[] rgba);

		int CreateRenderTarget(int width, int height);

		void UpdateTexture(int id, int width, int height, byte[] rgba);

		void DeleteTexture(int id);

		void BindTarget(int textureId);

		void SetViewport(int x, int y, int width, int height, int canvasWidth, int canvasHeight);

		void SetBlend(bool enable, int src, int dst, int equation);

		void SetDepth(bool enable, bool write);

		void SetCull(int mode);

		void DrawSprites(int textureId, float flipV, Span<float> vertices);

		void DrawEffect(string name, int textureId, float flipV, Span<float> vertices, float blurLevel, float x, float y, float time, float radius, int colorKey);

		void DrawFixed(int textureId, Span<float> vertices, Span<float> world, Span<float> view, Span<float> proj, float mr, float mg, float mb, float ma, float lighting, float fog, float fogStart, float fogEnd, float fr, float fg, float fb);

		byte[] RasterText(string text, int width, int height, float sizePx, bool bold, bool italic);

		void PaintText(int textureId, string text, int width, int height, float sizePx, bool bold, bool italic, int argb, string family);
	}

	public static class GlHost
	{
		public static IGlApi Api { get; set; }

		public static IGlApi Need()
		{
			if (Api == null)
			{
				throw new InvalidOperationException("WebGL bridge is not installed.");
			}
			return Api;
		}
	}
}
