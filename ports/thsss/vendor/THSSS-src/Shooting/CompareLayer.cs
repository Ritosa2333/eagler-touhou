using System.Collections.Generic;

namespace Shooting;

public class CompareLayer : IComparer<BaseBullet_Touhou>
{
	public int Compare(BaseBullet_Touhou b1, BaseBullet_Touhou b2)
	{
		return b1.Layer - b2.Layer;
	}
}
