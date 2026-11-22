using System.Drawing;
using SlimDX.Direct3D9;

namespace Shooting;

public interface IModel
{
	Color ColorValue { get; set; }

	Device DeviceMain { get; set; }

	TextureObject TxtureObject { get; set; }

	int TransparentValue { get; set; }

	string Name { get; set; }

	void Draw();

	void SetModel();

	void SetupMaterial();

	void SetupVertex();

	void SetVertexFormat();

	void Dispose();
}
