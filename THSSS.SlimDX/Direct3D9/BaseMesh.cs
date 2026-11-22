namespace SlimDX.Direct3D9
{
	public class BaseMesh : ComObject
	{
		public virtual VertexFormat VertexFormat { get; set; }

		public virtual Mesh Clone(Device device, MeshFlags flags, VertexFormat format)
		{
			return null;
		}

		public virtual void DrawSubset(int subset)
		{
		}
	}
}
