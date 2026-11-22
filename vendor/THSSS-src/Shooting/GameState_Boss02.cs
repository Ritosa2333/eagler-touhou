namespace Shooting;

internal class GameState_Boss02 : GameState_SSS02, IGameState
{
	public GameState_Boss02(GlobalDataPackage GlobalData)
		: base(GlobalData)
	{
		StageName = "Bs2";
		testStartTime = 7740;
	}

	public override void Drama()
	{
		base.Drama();
		MyPlane.EnchantmentCount = MyPlane.EnchantmentCountNeeded;
	}
}
