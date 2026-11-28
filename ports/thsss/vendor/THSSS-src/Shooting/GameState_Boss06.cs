namespace Shooting;

internal class GameState_Boss06 : GameState_SSS06, IGameState
{
	public GameState_Boss06(GlobalDataPackage GlobalData)
		: base(GlobalData)
	{
		StageName = "Bs6";
		testStartTime = 8400;
	}

	public override void Drama()
	{
		base.Drama();
		MyPlane.EnchantmentCount = MyPlane.EnchantmentCountNeeded;
	}
}
