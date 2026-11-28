namespace Shooting;

internal class GameState_Boss03 : GameState_SSS03, IGameState
{
	public GameState_Boss03(GlobalDataPackage GlobalData)
		: base(GlobalData)
	{
		StageName = "Bs3";
		testStartTime = 7150;
	}

	public override void Drama()
	{
		base.Drama();
		MyPlane.EnchantmentCount = MyPlane.EnchantmentCountNeeded;
	}
}
