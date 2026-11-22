using System.Drawing;
using SlimDX;
using SlimDX.Direct3D9;

namespace Shooting;

public class Model_Mesh : Model, IModel
{
	public GlobalDataPackage GlobalData;

	private Mesh mesh;

	private ExtendedMaterial[] materials;

	private Texture[] textures;

	public bool CullMode;

	public Model_Mesh(Device DeviceMain, string ModelName)
	{
		TransparentValue = 255;
		ColorValue = Color.White;
		base.DeviceMain = DeviceMain;
		Name = ModelName;
		SetupVertex();
	}

	public override void SetupVertex()
	{
		//IL_0042: Unknown result type (might be due to invalid IL or missing references)
		//IL_0049: Unknown result type (might be due to invalid IL or missing references)
		//IL_0063: Unknown result type (might be due to invalid IL or missing references)
		//IL_006a: Unknown result type (might be due to invalid IL or missing references)
		//IL_0072: Unknown result type (might be due to invalid IL or missing references)
		mesh = Mesh.FromFile(DeviceMain, ".\\Model\\" + Name + ".x", (MeshFlags)544);
		materials = mesh.GetMaterials();
		if ((((BaseMesh)mesh).VertexFormat & 0x10) == 0)
		{
			Mesh val = ((BaseMesh)mesh).Clone(DeviceMain, (MeshFlags)272, (VertexFormat)(((BaseMesh)mesh).VertexFormat | 0x10));
			val.ComputeNormals();
			((ComObject)mesh).Dispose();
			mesh = val;
		}
		if (materials.Length < 1)
		{
			return;
		}
		textures = new Texture[materials.Length];
		for (int i = 0; i < materials.Length; i++)
		{
			textures[i] = null;
			if (!string.IsNullOrEmpty(materials[i].TextureFileName))
			{
				textures[i] = Texture.FromFile(DeviceMain, ".\\Model\\" + materials[i].TextureFileName);
			}
		}
	}

	public override void SetModel()
	{
	}

	public override void Draw()
	{
		//IL_0010: Unknown result type (might be due to invalid IL or missing references)
		//IL_0015: Unknown result type (might be due to invalid IL or missing references)
		//IL_0029: Unknown result type (might be due to invalid IL or missing references)
		//IL_0046: Unknown result type (might be due to invalid IL or missing references)
		//IL_0056: Unknown result type (might be due to invalid IL or missing references)
		//IL_006b: Unknown result type (might be due to invalid IL or missing references)
		//IL_0078: Unknown result type (might be due to invalid IL or missing references)
		//IL_0096: Unknown result type (might be due to invalid IL or missing references)
		for (int i = 0; i < materials.Length; i++)
		{
			Material materialD3D = materials[i].MaterialD3D;
			materialD3D.Diffuse = new Color4(Color.FromArgb(TransparentValue, ColorValue));
			materialD3D.Ambient = new Color4(Color.FromArgb(TransparentValue, ColorValue));
			DeviceMain.Material = materialD3D;
			DeviceMain.SetTexture(0, (BaseTexture)(object)textures[i]);
			((BaseMesh)mesh).DrawSubset(i);
		}
		DeviceMain.VertexFormat = TexturedVertex.Format;
	}

	public override void Dispose()
	{
		Texture[] array = textures;
		foreach (Texture val in array)
		{
			if (val != null)
			{
				((ComObject)val).Dispose();
			}
		}
		((ComObject)mesh).Dispose();
	}
}
