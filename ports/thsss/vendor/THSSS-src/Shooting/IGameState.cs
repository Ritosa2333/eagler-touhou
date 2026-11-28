namespace Shooting;

public interface IGameState
{
	GlobalDataPackage GlobalData { get; set; }

	StageDataPackage StageData { get; set; }

	StateSwitchDataPackage StateSwitchData { get; set; }

	string StageName { get; set; }

	void Init();

	void UpdateData();

	void Render();

	void BGM_ON();

	void BGM_Pause();

	void BGM_Resume();

	void Dispose();
}
