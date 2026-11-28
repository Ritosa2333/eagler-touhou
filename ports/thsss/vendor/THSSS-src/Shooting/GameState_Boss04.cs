namespace Shooting;

internal class GameState_Boss04 : GameState_SSS04, IGameState
{
	public GameState_Boss04(GlobalDataPackage GlobalData)
		: base(GlobalData)
	{
		StageName = "Bs4";
		testStartTime = 8700;
	}

	public override void Drama()
	{
		base.Drama();
		MyPlane.EnchantmentCount = MyPlane.EnchantmentCountNeeded;
	}
}
