using SlimDX;

namespace Shooting;

public interface IEnvironment
{
	float CameraAngle { get; }

	Vector3 CameraPos { get; }

	void Ctrl();

	void SetRenderPara();
}
