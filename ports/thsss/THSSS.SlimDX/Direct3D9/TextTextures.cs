using System.Collections.Generic;
using System.Drawing;
using System.Globalization;
using SlimDX.Gl;

namespace SlimDX.Direct3D9
{
	public static class TextTextures
	{
		private const int Limit = 6;

		private static readonly List<Slot> slots = new List<Slot>();

		private static int clock;

		public static Texture Get(string text, System.Drawing.Font font, Brush brush, int width, int height)
		{
			width = Texture.ClampEdge(width);
			height = Texture.ClampEdge(height);
			var body = text ?? "";
			var solidBrush = brush as SolidBrush;
			var argb = solidBrush != null ? solidBrush.Color.ToArgb() : -1;
			var size = font != null && font.Size > 0f ? font.Size : 16f;
			var bold = font != null && font.Bold;
			var italic = font != null && font.Italic;
			var family = font != null && !string.IsNullOrEmpty(font.Name) ? font.Name : "sans-serif";
			var style = family + "|" + size.ToString(CultureInfo.InvariantCulture) + "|" + (bold ? "1" : "0") + "|" + (italic ? "1" : "0") + "|" + argb.ToString(CultureInfo.InvariantCulture) + "|" + width.ToString(CultureInfo.InvariantCulture) + "|" + height.ToString(CultureInfo.InvariantCulture);
			var key = body + "\n" + style;
			Slot exact = null;
			Slot grown = null;
			Slot oldest = null;
			for (var i = 0; i < slots.Count; i++)
			{
				var slot = slots[i];
				if (slot.Key == key)
				{
					exact = slot;
					break;
				}
				if (oldest == null || slot.Tick < oldest.Tick)
				{
					oldest = slot;
				}
				if (slot.Style == style && body.StartsWith(slot.Text) && (grown == null || slot.Text.Length > grown.Text.Length))
				{
					grown = slot;
				}
			}
			if (exact != null)
			{
				exact.Tick = ++clock;
				return Texture.Wrap(exact.Texture.GlId, exact.Texture.Width, exact.Texture.Height);
			}
			var target = grown;
			if (target == null && slots.Count < Limit)
			{
				target = new Slot();
				slots.Add(target);
			}
			if (target == null)
			{
				target = oldest;
			}
			if (target.Texture == null || target.Texture.Width != width || target.Texture.Height != height)
			{
				if (target.Texture != null)
				{
					target.Texture.Dispose();
				}
				target.Texture = Texture.AllocateEmpty(width, height);
			}
			GlHost.Need().PaintText(target.Texture.GlId, body, width, height, size, bold, italic, argb, family);
			target.Key = key;
			target.Text = body;
			target.Style = style;
			target.Tick = ++clock;
			return Texture.Wrap(target.Texture.GlId, target.Texture.Width, target.Texture.Height);
		}

		private sealed class Slot
		{
			public Texture Texture;

			public string Key;

			public string Text;

			public string Style;

			public int Tick;
		}
	}
}
