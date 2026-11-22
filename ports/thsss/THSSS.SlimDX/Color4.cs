using System.Drawing;

namespace SlimDX
{
	public struct Color4
	{
		public float Alpha;

		public float Red;

		public float Green;

		public float Blue;

		public Color4(float alpha, float red, float green, float blue)
		{
			this.Alpha = alpha;
			this.Red = red;
			this.Green = green;
			this.Blue = blue;
		}

		public Color4(Color color)
		{
			this.Alpha = (float)color.A / 255f;
			this.Red = (float)color.R / 255f;
			this.Green = (float)color.G / 255f;
			this.Blue = (float)color.B / 255f;
		}

		public Color4(int argb)
		{
			this.Alpha = (float)((argb >> 24) & 255) / 255f;
			this.Red = (float)((argb >> 16) & 255) / 255f;
			this.Green = (float)((argb >> 8) & 255) / 255f;
			this.Blue = (float)(argb & 255) / 255f;
		}

		public static implicit operator Color4(Color color)
		{
			return new Color4(color);
		}

		public int ToArgb()
		{
			var num = (int)(this.Alpha * 255f) & 255;
			var num2 = (int)(this.Red * 255f) & 255;
			var num3 = (int)(this.Green * 255f) & 255;
			var num4 = (int)(this.Blue * 255f) & 255;
			return num << 24 | num2 << 16 | num3 << 8 | num4;
		}
	}
}
