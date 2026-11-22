using System;
using System.Runtime.InteropServices;

namespace SlimDX
{
	[StructLayout(LayoutKind.Sequential, Pack = 4)]
	public struct Vector2
	{
		public float X;

		public float Y;

		public Vector2(float x, float y)
		{
			this.X = x;
			this.Y = y;
		}

		public static Vector2 Zero
		{
			get
			{
				return new Vector2(0f, 0f);
			}
		}

		public float Length()
		{
			return (float)Math.Sqrt((double)(this.X * this.X + this.Y * this.Y));
		}

		public static float Dot(Vector2 a, Vector2 b)
		{
			return a.X * b.X + a.Y * b.Y;
		}

		public static Vector2 Normalize(Vector2 v)
		{
			var num = v.Length();
			if (num < 1E-08f)
			{
				return Zero;
			}
			return new Vector2(v.X / num, v.Y / num);
		}

		public static Vector2 Lerp(Vector2 a, Vector2 b, float t)
		{
			return new Vector2(a.X + (b.X - a.X) * t, a.Y + (b.Y - a.Y) * t);
		}

		public static Vector2 operator +(Vector2 a, Vector2 b)
		{
			return new Vector2(a.X + b.X, a.Y + b.Y);
		}

		public static Vector2 operator -(Vector2 a, Vector2 b)
		{
			return new Vector2(a.X - b.X, a.Y - b.Y);
		}

		public static Vector2 operator -(Vector2 v)
		{
			return new Vector2(-v.X, -v.Y);
		}

		public void Normalize()
		{
			var num = this.Length();
			if (num < 1E-08f)
			{
				this.X = 0f;
				this.Y = 0f;
				return;
			}
			this.X /= num;
			this.Y /= num;
		}

		public static Vector2 operator *(Vector2 a, Vector2 b)
		{
			return new Vector2(a.X * b.X, a.Y * b.Y);
		}

		public static Vector2 operator *(Vector2 v, float s)
		{
			return new Vector2(v.X * s, v.Y * s);
		}

		public static Vector2 operator *(float s, Vector2 v)
		{
			return v * s;
		}

		public static Vector2 operator /(Vector2 v, float s)
		{
			return new Vector2(v.X / s, v.Y / s);
		}

		public override string ToString()
		{
			return this.X + "," + this.Y;
		}
	}
}
