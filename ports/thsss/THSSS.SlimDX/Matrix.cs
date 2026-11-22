using System;

namespace SlimDX
{
	public struct Matrix
	{
		public float M11;
		public float M12;
		public float M13;
		public float M14;
		public float M21;
		public float M22;
		public float M23;
		public float M24;
		public float M31;
		public float M32;
		public float M33;
		public float M34;
		public float M41;
		public float M42;
		public float M43;
		public float M44;

		public static Matrix Identity
		{
			get
			{
				var matrix = default(Matrix);
				matrix.M11 = 1f;
				matrix.M22 = 1f;
				matrix.M33 = 1f;
				matrix.M44 = 1f;
				return matrix;
			}
		}

		public float[] ToArray()
		{
			return new float[]
			{
				this.M11, this.M12, this.M13, this.M14,
				this.M21, this.M22, this.M23, this.M24,
				this.M31, this.M32, this.M33, this.M34,
				this.M41, this.M42, this.M43, this.M44
			};
		}

		public static Matrix operator *(Matrix a, Matrix b)
		{
			var matrix = default(Matrix);
			matrix.M11 = a.M11 * b.M11 + a.M12 * b.M21 + a.M13 * b.M31 + a.M14 * b.M41;
			matrix.M12 = a.M11 * b.M12 + a.M12 * b.M22 + a.M13 * b.M32 + a.M14 * b.M42;
			matrix.M13 = a.M11 * b.M13 + a.M12 * b.M23 + a.M13 * b.M33 + a.M14 * b.M43;
			matrix.M14 = a.M11 * b.M14 + a.M12 * b.M24 + a.M13 * b.M34 + a.M14 * b.M44;
			matrix.M21 = a.M21 * b.M11 + a.M22 * b.M21 + a.M23 * b.M31 + a.M24 * b.M41;
			matrix.M22 = a.M21 * b.M12 + a.M22 * b.M22 + a.M23 * b.M32 + a.M24 * b.M42;
			matrix.M23 = a.M21 * b.M13 + a.M22 * b.M23 + a.M23 * b.M33 + a.M24 * b.M43;
			matrix.M24 = a.M21 * b.M14 + a.M22 * b.M24 + a.M23 * b.M34 + a.M24 * b.M44;
			matrix.M31 = a.M31 * b.M11 + a.M32 * b.M21 + a.M33 * b.M31 + a.M34 * b.M41;
			matrix.M32 = a.M31 * b.M12 + a.M32 * b.M22 + a.M33 * b.M32 + a.M34 * b.M42;
			matrix.M33 = a.M31 * b.M13 + a.M32 * b.M23 + a.M33 * b.M33 + a.M34 * b.M43;
			matrix.M34 = a.M31 * b.M14 + a.M32 * b.M24 + a.M33 * b.M34 + a.M34 * b.M44;
			matrix.M41 = a.M41 * b.M11 + a.M42 * b.M21 + a.M43 * b.M31 + a.M44 * b.M41;
			matrix.M42 = a.M41 * b.M12 + a.M42 * b.M22 + a.M43 * b.M32 + a.M44 * b.M42;
			matrix.M43 = a.M41 * b.M13 + a.M42 * b.M23 + a.M43 * b.M33 + a.M44 * b.M43;
			matrix.M44 = a.M41 * b.M14 + a.M42 * b.M24 + a.M43 * b.M34 + a.M44 * b.M44;
			return matrix;
		}

		public static Matrix Translation(float x, float y, float z)
		{
			var matrix = Identity;
			matrix.M41 = x;
			matrix.M42 = y;
			matrix.M43 = z;
			return matrix;
		}

		public static Matrix Scaling(float x, float y, float z)
		{
			var matrix = Identity;
			matrix.M11 = x;
			matrix.M22 = y;
			matrix.M33 = z;
			return matrix;
		}

		public static Matrix Scaling(Vector3 v)
		{
			return Scaling(v.X, v.Y, v.Z);
		}

		public static Matrix RotationX(float angle)
		{
			var num = (float)Math.Cos((double)angle);
			var num2 = (float)Math.Sin((double)angle);
			var matrix = Identity;
			matrix.M22 = num;
			matrix.M23 = num2;
			matrix.M32 = -num2;
			matrix.M33 = num;
			return matrix;
		}

		public static Matrix RotationY(float angle)
		{
			var num = (float)Math.Cos((double)angle);
			var num2 = (float)Math.Sin((double)angle);
			var matrix = Identity;
			matrix.M11 = num;
			matrix.M13 = -num2;
			matrix.M31 = num2;
			matrix.M33 = num;
			return matrix;
		}

		public static Matrix RotationZ(float angle)
		{
			var num = (float)Math.Cos((double)angle);
			var num2 = (float)Math.Sin((double)angle);
			var matrix = Identity;
			matrix.M11 = num;
			matrix.M12 = num2;
			matrix.M21 = -num2;
			matrix.M22 = num;
			return matrix;
		}

		public static Matrix RotationAxis(Vector3 axis, float angle)
		{
			var vector = Vector3.Normalize(axis);
			var num = (float)Math.Cos((double)angle);
			var num2 = (float)Math.Sin((double)angle);
			var num3 = 1f - num;
			var matrix = default(Matrix);
			matrix.M11 = num3 * vector.X * vector.X + num;
			matrix.M12 = num3 * vector.X * vector.Y + num2 * vector.Z;
			matrix.M13 = num3 * vector.X * vector.Z - num2 * vector.Y;
			matrix.M22 = num3 * vector.Y * vector.Y + num;
			matrix.M21 = num3 * vector.Y * vector.X - num2 * vector.Z;
			matrix.M23 = num3 * vector.Y * vector.Z + num2 * vector.X;
			matrix.M33 = num3 * vector.Z * vector.Z + num;
			matrix.M31 = num3 * vector.Z * vector.X + num2 * vector.Y;
			matrix.M32 = num3 * vector.Z * vector.Y - num2 * vector.X;
			matrix.M44 = 1f;
			return matrix;
		}

		public static Matrix LookAtLH(Vector3 eye, Vector3 at, Vector3 up)
		{
			var vector = Vector3.Normalize(at - eye);
			var vector2 = Vector3.Normalize(Vector3.Cross(up, vector));
			var vector3 = Vector3.Cross(vector, vector2);
			var matrix = Identity;
			matrix.M11 = vector2.X;
			matrix.M21 = vector2.Y;
			matrix.M31 = vector2.Z;
			matrix.M12 = vector3.X;
			matrix.M22 = vector3.Y;
			matrix.M32 = vector3.Z;
			matrix.M13 = vector.X;
			matrix.M23 = vector.Y;
			matrix.M33 = vector.Z;
			matrix.M41 = -Vector3.Dot(vector2, eye);
			matrix.M42 = -Vector3.Dot(vector3, eye);
			matrix.M43 = -Vector3.Dot(vector, eye);
			return matrix;
		}

		public static Matrix PerspectiveFovLH(float fov, float aspect, float zn, float zf)
		{
			var num = (float)(1.0 / Math.Tan((double)(fov * 0.5f)));
			var matrix = default(Matrix);
			matrix.M11 = num / aspect;
			matrix.M22 = num;
			matrix.M33 = zf / (zf - zn);
			matrix.M34 = 1f;
			matrix.M43 = -zn * zf / (zf - zn);
			return matrix;
		}

		public static Matrix Transformation2D(Vector2 scalingCenter, float scalingRotation, Vector2 scaling, Vector2 rotationCenter, float rotation, Vector2 translation)
		{
			if (scalingRotation == 0f && translation.X == 0f && translation.Y == 0f)
			{
				var cos = rotation == 0f ? 1f : (float)Math.Cos((double)rotation);
				var sin = rotation == 0f ? 0f : (float)Math.Sin((double)rotation);
				var bx = scalingCenter.X * (1f - scaling.X);
				var by = scalingCenter.Y * (1f - scaling.Y);
				var fast = default(Matrix);
				fast.M11 = cos * scaling.X;
				fast.M21 = -sin * scaling.Y;
				fast.M12 = sin * scaling.X;
				fast.M22 = cos * scaling.Y;
				fast.M33 = 1f;
				fast.M44 = 1f;
				fast.M41 = cos * (bx - rotationCenter.X) - sin * (by - rotationCenter.Y) + rotationCenter.X;
				fast.M42 = sin * (bx - rotationCenter.X) + cos * (by - rotationCenter.Y) + rotationCenter.Y;
				return fast;
			}
			var matrix = Translation(-scalingCenter.X, -scalingCenter.Y, 0f);
			var matrix2 = RotationZ(-scalingRotation);
			var matrix3 = Scaling(scaling.X, scaling.Y, 1f);
			var matrix4 = RotationZ(scalingRotation);
			var matrix5 = Translation(scalingCenter.X, scalingCenter.Y, 0f);
			var matrix6 = Translation(-rotationCenter.X, -rotationCenter.Y, 0f);
			var matrix7 = RotationZ(rotation);
			var matrix8 = Translation(rotationCenter.X, rotationCenter.Y, 0f);
			var matrix9 = Translation(translation.X, translation.Y, 0f);
			return matrix * matrix2 * matrix3 * matrix4 * matrix5 * matrix6 * matrix7 * matrix8 * matrix9;
		}
	}
}
