namespace SlimDX.Direct3D9
{
	public class VertexDeclaration : ComObject
	{
		public VertexElement[] Elements { get; private set; }

		public VertexDeclaration(Device device, VertexElement[] elements)
		{
			this.Elements = elements ?? new VertexElement[0];
		}
	}
}
