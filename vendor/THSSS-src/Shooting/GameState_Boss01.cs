namespace Shooting;

internal class GameState_Boss01 : GameState_SSS01, IGameState
{
	public GameState_Boss01(GlobalDataPackage GlobalData)
		: base(GlobalData)
	{
		StageName = "Bs1";
		testStartTime = 6290;
	}

	public override void Drama()
	{
		base.Drama();
		MyPlane.EnchantmentCount = MyPlane.EnchantmentCountNeeded;
	}
}
