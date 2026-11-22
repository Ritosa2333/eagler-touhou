using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

internal struct TexturedColoredVertex
{
	public Vector3 Position
	{
		[CompilerGenerated]
		get
		{
			//IL_0001: Unknown result type (might be due to invalid IL or missing references)
			return field;
		}
		[CompilerGenerated]
		set
		{
			//IL_0001: Unknown result type (might be due to invalid IL or missing references)
			//IL_0002: Unknown result type (might be due to invalid IL or missing references)
			field = value;
		}
	}

	public int Color { get; set; }

	public Vector2 TextureCoordinate
	{
		[CompilerGenerated]
		get
		{
			//IL_0001: Unknown result type (might be due to invalid IL or missing references)
			return field;
		}
		[CompilerGenerated]
		set
		{
			//IL_0001: Unknown result type (might be due to invalid IL or missing references)
			//IL_0002: Unknown result type (might be due to invalid IL or missing references)
			field = value;
		}
	}

	public static int SizeBytes => Marshal.SizeOf(typeof(TexturedColoredVertex));

	public static VertexFormat Format => (VertexFormat)322;

	public static VertexElement[] VertexElements
	{
		get
		{
			//IL_000e: Unknown result type (might be due to invalid IL or missing references)
			//IL_0013: Unknown result type (might be due to invalid IL or missing references)
			//IL_0021: Unknown result type (might be due to invalid IL or missing references)
			//IL_0026: Unknown result type (might be due to invalid IL or missing references)
			//IL_0035: Unknown result type (might be due to invalid IL or missing references)
			//IL_003a: Unknown result type (might be due to invalid IL or missing references)
			//IL_0041: Unknown result type (might be due to invalid IL or missing references)
			//IL_0046: Unknown result type (might be due to invalid IL or missing references)
			return new VertexElement[4]
			{
				new VertexElement((short)0, (short)0, (DeclarationType)2, (DeclarationMethod)0, (DeclarationUsage)0, (byte)0),
				new VertexElement((short)0, (short)12, (DeclarationType)1, (DeclarationMethod)0, (DeclarationUsage)5, (byte)0),
				new VertexElement((short)0, (short)20, (DeclarationType)4, (DeclarationMethod)0, (DeclarationUsage)10, (byte)0),
				VertexElement.VertexDeclarationEnd
			};
		}
	}

	public TexturedColoredVertex(Vector3 position, Vector2 textureCoordinate, int color)
	{
		//IL_0008: Unknown result type (might be due to invalid IL or missing references)
		//IL_000f: Unknown result type (might be due to invalid IL or missing references)
		this = default;
		Position = position;
		TextureCoordinate = textureCoordinate;
		Color = color;
	}
}
