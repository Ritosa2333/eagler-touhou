using System;
using System.Runtime.InteropServices;

namespace SlimDX
{
	[StructLayout(LayoutKind.Sequential, Pack = 4)]
	public struct Vector3
	{
		public float X;

		public float Y;

		public float Z;

		public Vector3(float x, float y, float z)
		{
			this.X = x;
			this.Y = y;
			this.Z = z;
		}

		public static Vector3 Zero
		{
			get
			{
				return new Vector3(0f, 0f, 0f);
			}
		}

		public float Length()
		{
			return (float)Math.Sqrt((double)(this.X * this.X + this.Y * this.Y + this.Z * this.Z));
		}

		public static float Dot(Vector3 a, Vector3 b)
		{
			return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
		}

		public static Vector3 Cross(Vector3 a, Vector3 b)
		{
			return new Vector3(a.Y * b.Z - a.Z * b.Y, a.Z * b.X - a.X * b.Z, a.X * b.Y - a.Y * b.X);
		}

		public static Vector3 Normalize(Vector3 v)
		{
			var num = v.Length();
			if (num < 1E-08f)
			{
				return Zero;
			}
			return new Vector3(v.X / num, v.Y / num, v.Z / num);
		}

		public static Vector3 Lerp(Vector3 a, Vector3 b, float t)
		{
			return new Vector3(a.X + (b.X - a.X) * t, a.Y + (b.Y - a.Y) * t, a.Z + (b.Z - a.Z) * t);
		}

		public static Vector3 operator +(Vector3 a, Vector3 b)
		{
			return new Vector3(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
		}

		public static Vector3 operator -(Vector3 a, Vector3 b)
		{
			return new Vector3(a.X - b.X, a.Y - b.Y, a.Z - b.Z);
		}

		public static Vector3 operator -(Vector3 v)
		{
			return new Vector3(-v.X, -v.Y, -v.Z);
		}

		public static Vector3 operator *(Vector3 v, float s)
		{
			return new Vector3(v.X * s, v.Y * s, v.Z * s);
		}

		public static Vector3 operator *(float s, Vector3 v)
		{
			return v * s;
		}

		public override string ToString()
		{
			return this.X + "," + this.Y + "," + this.Z;
		}
	}
}
