using System;
using System.Drawing;

namespace Shooting;

internal class Background3DParticle : Background3DObject
{
	private double tr;

	public Background3DParticle(StageDataPackage StageData, string modelName, PointF OriginalPosition, bool InsertAtFirst)
		: base(StageData, modelName, OriginalPosition, InsertAtFirst)
	{
	}

	public override void Show()
	{
		float transparentValueF = TransparentValueF;
		tr = 50.0 * Math.Sin((float)Time / 2f);
		TransparentValueF += (int)tr;
		base.Show();
		TransparentValueF = transparentValueF;
	}
}
