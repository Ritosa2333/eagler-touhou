namespace SlimDX.Direct3D9
{
	public struct VertexElement
	{
		public short Stream;

		public short Offset;

		public DeclarationType Type;

		public DeclarationMethod Method;

		public DeclarationUsage Usage;

		public byte UsageIndex;

		public VertexElement(short stream, short offset, DeclarationType type, DeclarationMethod method, DeclarationUsage usage, byte usageIndex)
		{
			this.Stream = stream;
			this.Offset = offset;
			this.Type = type;
			this.Method = method;
			this.Usage = usage;
			this.UsageIndex = usageIndex;
		}

		public static VertexElement VertexDeclarationEnd
		{
			get
			{
				return new VertexElement(255, 0, DeclarationType.Float1, DeclarationMethod.Default, DeclarationUsage.Position, 0);
			}
		}
	}
}
