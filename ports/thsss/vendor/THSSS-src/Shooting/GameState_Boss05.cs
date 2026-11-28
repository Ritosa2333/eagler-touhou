namespace Shooting;

internal class GameState_Boss05 : GameState_SSS05, IGameState
{
	public GameState_Boss05(GlobalDataPackage GlobalData)
		: base(GlobalData)
	{
		StageName = "Bs5";
		testStartTime = 8600;
	}

	public override void Drama()
	{
		base.Drama();
		MyPlane.EnchantmentCount = MyPlane.EnchantmentCountNeeded;
	}
}
