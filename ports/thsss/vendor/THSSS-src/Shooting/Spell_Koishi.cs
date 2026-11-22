namespace Shooting;

public class Spell_Koishi : BaseSpell
{
	public Spell_Koishi(StageDataPackage StageData)
	{
		base.StageData = StageData;
		Position = MyPlane.Position;
		Damage = 0;
		Region = 1000;
		LifeTime = 60;
		SpellList.Add(this);
		new BulletRemover_Small(StageData, MyPlane.OriginalPosition).Region = 150;
		StageData.SoundPlay("se_focusrot.wav");
	}
}
