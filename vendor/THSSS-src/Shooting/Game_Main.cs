using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Threading;
using System.Windows.Forms;
using SlimDX;
using SlimDX.Direct3D9;
using SlimDX.DirectInput;

namespace Shooting;

public class Game_Main
{
	public RenderForm_Main Form_Main;

	private GlobalDataPackage GDP = new GlobalDataPackage();

	private IGameState PresentState;

	private Dictionary<string, IGameState> GameStatesDictionary;

	private Thread LoadingThread;

	private Watch fpsTimer = new Watch();

	private KeyboardCapture KCapture;

	private JoystickCapture JCapture;

	private MapKey mapkey = new MapKey();

	private bool fullWindow;

	private bool useDirectInput;

	private bool verticalSync;

	private bool antiAlias;

	private bool deviceLost;

	private bool singleThreaded;

	private int windowSize;

	private int presentInterval = 1;

	private bool fixedFPS;

	private int delayTolerance = 10;

	private int bgmVolume;

	private int seVolume;

	private string fontType;

	public static bool useUnrealFullScreen = true;

	public static bool stretchScreen = false;

	public bool initSuccess;

	private bool firstFrame = true;

	private bool OnPause;

	private bool InitFlag;

	public GlobalDataPackage GlobalData
	{
		get
		{
			return GDP;
		}
		set
		{
			GDP = value;
		}
	}

	public Device DeviceMain
	{
		get
		{
			return GlobalData.DeviceMain;
		}
		set
		{
			GlobalData.DeviceMain = value;
		}
	}

	public Game_Main()
	{
		//IL_037b: Unknown result type (might be due to invalid IL or missing references)
		//IL_039b: Unknown result type (might be due to invalid IL or missing references)
		//IL_03a5: Expected Obj, but got Unknown
		//IL_04bc: Unknown result type (might be due to invalid IL or missing references)
		//IL_04c6: Expected Obj, but got Unknown
		//IL_04d3: Unknown result type (might be due to invalid IL or missing references)
		//IL_04dd: Expected Obj, but got Unknown
		try
		{
			INI_RW iNI_RW = new INI_RW(".\\Setting.INI");
			if (iNI_RW.ExistINIFile())
			{
				fullWindow = iNI_RW.IniReadValue("DirectX", "FullWindow") == "1";
				antiAlias = iNI_RW.IniReadValue("DirectX", "AntiAlias") == "1";
				verticalSync = iNI_RW.IniReadValue("DirectX", "VerticalSync") == "1";
				useDirectInput = iNI_RW.IniReadValue("DirectInput", "UseDirectInput") == "1";
				GlobalData.Clear = iNI_RW.IniReadValue("Mode", "Clear") == "yes";
				singleThreaded = iNI_RW.IniReadValue("Mode", "SingleThreaded") == "1";
				fixedFPS = iNI_RW.IniReadValue("Mode", "FixedFPS") == "1";
				delayTolerance = Convert.ToInt16(iNI_RW.IniReadValue("Mode", "DelayTolerance"));
				windowSize = Convert.ToInt16(iNI_RW.IniReadValue("Mode", "WindowSize"));
				presentInterval = Convert.ToInt16(iNI_RW.IniReadValue("Mode", "PresentInterval"));
				useUnrealFullScreen = iNI_RW.IniReadValue("Mode", "UseFullScreenWindow") == "1";
				stretchScreen = iNI_RW.IniReadValue("Mode", "StretchScreen") == "1";
				mapkey.Left = iNI_RW.IniReadValue("Key", "Left");
				mapkey.Right = iNI_RW.IniReadValue("Key", "Right");
				mapkey.Up = iNI_RW.IniReadValue("Key", "Up");
				mapkey.Down = iNI_RW.IniReadValue("Key", "Down");
				mapkey.Slow = iNI_RW.IniReadValue("Key", "Slow");
				mapkey.Shoot = iNI_RW.IniReadValue("Key", "Shoot");
				mapkey.Bomb = iNI_RW.IniReadValue("Key", "Bomb");
				MapJoyStick.Skip = Convert.ToInt16(iNI_RW.IniReadValue("JoyStick", "Skip"));
				MapJoyStick.Pause = Convert.ToInt16(iNI_RW.IniReadValue("JoyStick", "Pause"));
				MapJoyStick.Slow = Convert.ToInt16(iNI_RW.IniReadValue("JoyStick", "Slow"));
				MapJoyStick.Shoot = Convert.ToInt16(iNI_RW.IniReadValue("JoyStick", "Shoot"));
				MapJoyStick.Bomb = Convert.ToInt16(iNI_RW.IniReadValue("JoyStick", "Bomb"));
				MapJoyStick.C = Convert.ToInt16(iNI_RW.IniReadValue("JoyStick", "C"));
				bgmVolume = Convert.ToInt16(iNI_RW.IniReadValue("Volume", "BGMVolume"));
				seVolume = Convert.ToInt16(iNI_RW.IniReadValue("Volume", "SEVolume"));
				fontType = iNI_RW.IniReadValue("Game", "FontType");
			}
			iNI_RW = null;
		}
		catch
		{
			MessageBox.Show("读取配置文件失败", "Setting Load Error");
		}
		Form_Main = new RenderForm_Main();
		((Form)Form_Main).FormClosing += Form_Main_FormClosing;
		((Form)Form_Main).Deactivate += Form_Main_Deactivate;
		((Form)Form_Main).Activated += Form_Main_Activated;
		MouseCtrl.DrawCursor(!fullWindow);
		GlobalData.TestFontScale();
		if (windowSize == 1)
		{
			((Form)Form_Main).ClientSize = new Size(800, 600);
		}
		else if (windowSize == 2)
		{
			((Form)Form_Main).ClientSize = new Size(1024, 768);
		}
		else if (windowSize == 3)
		{
			((Form)Form_Main).ClientSize = new Size(1280, 960);
		}
		((Control)Form_Main).Show();
		if (!Direct3DInit())
		{
			Application.ExitThread();
			return;
		}
		initSuccess = true;
		if (useDirectInput)
		{
			KCapture = new KeyboardCapture(((Control)Form_Main).Handle);
			JCapture = new JoystickCapture(((Control)Form_Main).Handle);
		}
		else
		{
			((Control)Form_Main).KeyDown += Form_Main_KeyDown;
			((Control)Form_Main).KeyUp += Form_Main_KeyUp;
		}
		PresentState = new GameState_SplashScreen(GlobalData);
		PresentState.Init();
		PresentState.BGM_ON();
		GlobalData.LoadResources1();
		LoadingThread = new Thread(AddState);
		LoadingThread.Start();
	}

	public bool Direct3DInit()
	{
		//IL_0000: Unknown result type (might be due to invalid IL or missing references)
		//IL_0006: Expected Obj, but got Unknown
		//IL_001f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0029: Unknown result type (might be due to invalid IL or missing references)
		//IL_002f: Invalid comparison between Unknown and I4
		//IL_01e6: Unknown result type (might be due to invalid IL or missing references)
		//IL_0038: Unknown result type (might be due to invalid IL or missing references)
		//IL_0033: Unknown result type (might be due to invalid IL or missing references)
		//IL_0064: Unknown result type (might be due to invalid IL or missing references)
		//IL_0069: Unknown result type (might be due to invalid IL or missing references)
		//IL_0074: Unknown result type (might be due to invalid IL or missing references)
		//IL_007f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0081: Unknown result type (might be due to invalid IL or missing references)
		//IL_0086: Unknown result type (might be due to invalid IL or missing references)
		//IL_008a: Unknown result type (might be due to invalid IL or missing references)
		//IL_0094: Unknown result type (might be due to invalid IL or missing references)
		//IL_009b: Unknown result type (might be due to invalid IL or missing references)
		//IL_00a2: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ba: Unknown result type (might be due to invalid IL or missing references)
		//IL_00c1: Unknown result type (might be due to invalid IL or missing references)
		//IL_00c8: Unknown result type (might be due to invalid IL or missing references)
		//IL_00d4: Expected Obj, but got Unknown
		//IL_0176: Unknown result type (might be due to invalid IL or missing references)
		//IL_018b: Unknown result type (might be due to invalid IL or missing references)
		//IL_0195: Expected Obj, but got Unknown
		try
		{
			Direct3D val = new Direct3D();
			AdapterInformation defaultAdapter = val.Adapters.DefaultAdapter;
			CreateFlags val2 = (((val.GetDeviceCaps(defaultAdapter.Adapter, (DeviceType)1).DeviceCaps & 0x10000) != 65536) ? ((CreateFlags)32) : ((CreateFlags)64));
			IntPtr intPtr = (useUnrealFullScreen ? ((Control)Form_Main.panel1).Handle : ((Control)Form_Main).Handle);
			GlobalDataPackage globalData = GlobalData;
			PresentParameters val3 = new PresentParameters
			{
				BackBufferWidth = 640,
				BackBufferHeight = 480
			};
			DisplayMode currentDisplayMode = defaultAdapter.CurrentDisplayMode;
			val3.BackBufferFormat = currentDisplayMode.Format;
			val3.BackBufferCount = 1;
			val3.DeviceWindowHandle = intPtr;
			val3.Windowed = !(fullWindow & !useUnrealFullScreen);
			val3.Multisample = (MultisampleType)0;
			val3.PresentFlags = (PresentFlags)2;
			val3.SwapEffect = (SwapEffect)1;
			globalData.presentParams = val3;
			if (presentInterval == 1)
			{
				if (verticalSync)
				{
					GlobalData.presentParams.PresentationInterval = (PresentInterval)0;
				}
				else
				{
					GlobalData.presentParams.PresentationInterval = (PresentInterval)int.MinValue;
				}
			}
			else if (fullWindow && verticalSync)
			{
				if (presentInterval == 2)
				{
					GlobalData.presentParams.PresentationInterval = (PresentInterval)2;
				}
				else if (presentInterval == 3)
				{
					GlobalData.presentParams.PresentationInterval = (PresentInterval)4;
				}
			}
			else
			{
				GlobalData.presentParams.PresentationInterval = (PresentInterval)int.MinValue;
			}
			DeviceMain = new Device(val, defaultAdapter.Adapter, (DeviceType)1, intPtr, val2, new PresentParameters[1] { GlobalData.presentParams });
			GlobalData.SpriteMain = new MySprite(DeviceMain);
			if (fullWindow)
			{
				windowSize = 0;
				if (useUnrealFullScreen)
				{
					fullWindow = !fullWindow;
					ToggleUnrealFullScreen();
				}
			}
			return true;
		}
		catch
		{
			MessageBox.Show("DirectX初始化失败", "DirectX Initial Error");
			return false;
		}
	}

	public void AddState()
	{
		//IL_02cd: Unknown result type (might be due to invalid IL or missing references)
		Watch watch = new Watch();
		watch.Start();
		GlobalData.BGMVolume = bgmVolume;
		GlobalData.SEVolume = seVolume;
		GlobalData.FontType = fontType;
		GlobalData.LoadResources();
		GameStatesDictionary = new Dictionary<string, IGameState>
		{
			{ "SplashScreen", PresentState },
			{
				"Stage_test",
				new GameState_testStage(GlobalData)
			},
			{
				"St1",
				new GameState_SSS01(GlobalData)
			},
			{
				"St2",
				new GameState_SSS02(GlobalData)
			},
			{
				"St3",
				new GameState_SSS03(GlobalData)
			},
			{
				"St4",
				new GameState_SSS04(GlobalData)
			},
			{
				"St5",
				new GameState_SSS05(GlobalData)
			},
			{
				"St6",
				new GameState_SSS06(GlobalData)
			},
			{
				"StEx",
				new GameState_SSSEx(GlobalData)
			},
			{
				"Bs1",
				new GameState_Boss01(GlobalData)
			},
			{
				"Bs2",
				new GameState_Boss02(GlobalData)
			},
			{
				"Bs3",
				new GameState_Boss03(GlobalData)
			},
			{
				"Bs4",
				new GameState_Boss04(GlobalData)
			},
			{
				"Bs5",
				new GameState_Boss05(GlobalData)
			},
			{
				"Bs6",
				new GameState_Boss06(GlobalData)
			},
			{
				"All",
				new GameState_ED(GlobalData)
			},
			{
				"PauseMenu",
				new GameState_PauseMenu(GlobalData)
			},
			{
				"MainMenu",
				new GameState_MainMenu(GlobalData)
			},
			{
				"GameOverMenu",
				new GameState_GameOverMenu(GlobalData)
			},
			{
				"GameSetMenu",
				new GameState_GameSetMenu(GlobalData)
			},
			{
				"FinalReplaySaveMenu",
				new GameState_ReplaySaveMenu(GlobalData)
			},
			{
				"GAME_OVER",
				new GameState_SplashScreen2(GlobalData)
				{
					TxtureObject = GlobalData.TextureObjectDictionary["GAME_OVER"]
				}
			},
			{
				"此功能尚未开通",
				new GameState_SplashScreen2(GlobalData)
				{
					TxtureObject = GlobalData.TextureObjectDictionary["此功能尚未开通"]
				}
			},
			{
				"MusicLoading",
				new GameState_MusicLoading(GlobalData)
			}
		};
		GlobalData.Rep = new Replay();
		if (!singleThreaded)
		{
			watch.SleepTo(3000f);
		}
		if (antiAlias)
		{
			DeviceMain.SetRenderState((RenderState)161, true);
		}
		GlobalData.LastState = PresentState;
		InitFlag = true;
	}

	public void MainProcess()
	{
		//IL_03b9: Expected Obj, but got Unknown
		//IL_03bb: Unknown result type (might be due to invalid IL or missing references)
		//IL_03c0: Unknown result type (might be due to invalid IL or missing references)
		//IL_03ff: Unknown result type (might be due to invalid IL or missing references)
		//IL_0404: Unknown result type (might be due to invalid IL or missing references)
		//IL_0419: Unknown result type (might be due to invalid IL or missing references)
		//IL_02a2: Unknown result type (might be due to invalid IL or missing references)
		//IL_02a7: Unknown result type (might be due to invalid IL or missing references)
		//IL_0357: Unknown result type (might be due to invalid IL or missing references)
		//IL_0362: Unknown result type (might be due to invalid IL or missing references)
		//IL_036e: Unknown result type (might be due to invalid IL or missing references)
		//IL_0385: Unknown result type (might be due to invalid IL or missing references)
		//IL_0392: Unknown result type (might be due to invalid IL or missing references)
		if (firstFrame)
		{
			fpsTimer.Start();
			firstFrame = false;
		}
		else if (!fixedFPS || PresentState is GameState_SplashScreen)
		{
			fpsTimer.Start();
		}
		else
		{
			for (int i = 0; i < presentInterval; i++)
			{
				fpsTimer.IncreaseOneFrame();
			}
		}
		if (InitFlag && GlobalData.LoadingList.Count == 0)
		{
			GlobalData.LoadModelDictionary();
			InitFlag = false;
			PresentState.StateSwitchData = new StateSwitchDataPackage
			{
				NextState = "MainMenu",
				NeedInit = true,
				SDPswitch = new StageDataPackage(GlobalData)
			};
		}
		if (PresentState.StateSwitchData != null)
		{
			StateSwitchDataPackage stateSwitchData = PresentState.StateSwitchData;
			if (stateSwitchData.NextState == "GameExit")
			{
				((Form)Form_Main).Close();
				return;
			}
			if (stateSwitchData.NextState != null)
			{
				if (stateSwitchData.NextState == "MainMenu" && GlobalData.Rep != null)
				{
					GlobalData.Rep.CloseRpy();
				}
				GlobalData.BGM_Pause();
				PresentState.StateSwitchData = null;
				PresentState = GameStatesDictionary[stateSwitchData.NextState];
				if (stateSwitchData.SDPswitch != null)
				{
					PresentState.StageData = stateSwitchData.SDPswitch;
				}
				if (stateSwitchData.NeedInit)
				{
					PresentState.Init();
					PresentState.BGM_ON();
				}
				else
				{
					GlobalData.BGM_Resume();
				}
			}
		}
		if (useDirectInput)
		{
			KeyClass KClass = new KeyClass();
			KeyClass KClass2 = new KeyClass();
			try
			{
				KCapture.UpdateInput(ref KClass);
			}
			catch
			{
			}
			try
			{
				JCapture.UpdateInput(ref KClass2);
			}
			catch
			{
			}
			KeyClass.KeyMix(ref GlobalData.KClass, KClass, KClass2);
			if (KCapture.currentKeyboardState != null)
			{
				if (KCapture.currentKeyboardState.IsPressed((Key)77) && KCapture.currentKeyboardState.IsPressed((Key)57))
				{
					((Form)Form_Main).Close();
					return;
				}
				if (KCapture.currentKeyboardState.IsPressed((Key)61) && KCapture.lastKeyboardState.IsReleased((Key)61))
				{
					WindowSwitch();
				}
				if (KCapture.currentKeyboardState.IsPressed((Key)25) && KCapture.lastKeyboardState.IsReleased((Key)25))
				{
					ScreenShot();
				}
			}
		}
		if (deviceLost)
		{
			Thread.Sleep(15);
			if (DeviceMain.TestCooperativeLevel() == ResultCode.DeviceNotReset)
			{
				GlobalData.OnResetDevice();
				deviceLost = false;
				firstFrame = true;
			}
			return;
		}
		try
		{
			if (OnPause && PresentState is BaseStage)
			{
				GlobalData.KClass.Key_ESC = true;
			}
			PresentState.UpdateData();
			for (int j = 1; j < presentInterval; j++)
			{
				if (PresentState.StateSwitchData != null)
				{
					if (PresentState.StateSwitchData.NextState == null)
					{
						PresentState.UpdateData();
					}
				}
				else
				{
					PresentState.UpdateData();
				}
			}
			DeviceMain.Clear((ClearFlags)3, Color4.op_Implicit(Color.Black), 1f, 0);
			DeviceMain.BeginScene();
			PresentState.Render();
			DeviceMain.EndScene();
			DeviceMain.Present((Present)0);
			if (GlobalData.BGM_Player.OnPause)
			{
				PresentState.BGM_Resume();
			}
		}
		catch (Direct3D9Exception ex)
		{
			Direct3D9Exception ex2 = ex;
			if (((SlimDXException)ex2).ResultCode == ResultCode.DeviceLost)
			{
				GlobalData.OnLostDevice();
				deviceLost = true;
			}
			else
			{
				string message = "SlimDX错误: " + ((Exception)(object)ex2).Message + "\n堆栈:\n" + ((Exception)(object)ex2).StackTrace;
				Result resultCode = ((SlimDXException)ex2).ResultCode;
				ErrorLog.SaveErrorLog(message, resultCode.Description);
				MessageBox.Show(((Exception)(object)ex2).Message);
				((Form)Form_Main).Close();
			}
		}
		if (verticalSync && (presentInterval == 1 || fullWindow))
		{
			return;
		}
		float num = 16f * (float)presentInterval - fpsTimer.GetDuration();
		if (num > 0f)
		{
			Thread.Sleep((int)num);
		}
		while (16.62f * (float)presentInterval - fpsTimer.GetDuration() > 0f)
		{
		}
		if (fixedFPS && num < -16.66667f * (float)delayTolerance)
		{
			for (int k = 0; (float)k < (0f - num) / 16.66667f - (float)delayTolerance; k++)
			{
				fpsTimer.IncreaseOneFrame();
			}
		}
	}

	private void ToggleFullScreen()
	{
		fullWindow = !fullWindow;
		GlobalData.presentParams.Windowed = !fullWindow;
		if (fullWindow && verticalSync)
		{
			if (presentInterval == 2)
			{
				GlobalData.presentParams.PresentationInterval = (PresentInterval)2;
			}
			else if (presentInterval == 3)
			{
				GlobalData.presentParams.PresentationInterval = (PresentInterval)4;
			}
		}
		else if (presentInterval == 2 || presentInterval == 3)
		{
			GlobalData.presentParams.PresentationInterval = (PresentInterval)int.MinValue;
		}
		((Form)Form_Main).ClientSize = new Size(640, 480);
		((Form)Form_Main).FormBorderStyle = (FormBorderStyle)3;
		GlobalData.OnLostDevice();
		GlobalData.OnResetDevice();
		MouseCtrl.DrawCursor(!fullWindow);
	}

	private void WindowSwitch()
	{
		if (fullWindow)
		{
			if (useUnrealFullScreen)
			{
				ToggleUnrealFullScreen();
			}
			else
			{
				ToggleFullScreen();
			}
			((Form)Form_Main).ClientSize = new Size(640, 480);
		}
		else
		{
			switch (windowSize)
			{
			case 0:
				windowSize++;
				((Form)Form_Main).ClientSize = new Size(800, 600);
				break;
			case 1:
				windowSize++;
				((Form)Form_Main).ClientSize = new Size(1024, 768);
				break;
			case 2:
				windowSize++;
				((Form)Form_Main).ClientSize = new Size(1280, 960);
				break;
			case 3:
				windowSize = 0;
				if (useUnrealFullScreen)
				{
					ToggleUnrealFullScreen();
				}
				else
				{
					ToggleFullScreen();
				}
				break;
			default:
				windowSize = 0;
				((Form)Form_Main).ClientSize = new Size(640, 480);
				break;
			}
		}
		((Control)Form_Main).Refresh();
	}

	private void Form_Main_Deactivate(object sender, EventArgs e)
	{
		if (initSuccess)
		{
			OnPause = true;
		}
	}

	private void Form_Main_Activated(object sender, EventArgs e)
	{
		if (initSuccess)
		{
			OnPause = false;
			GlobalData.KClass.Key_ESC = false;
		}
	}

	private void Form_Main_FormClosing(object sender, FormClosingEventArgs e)
	{
		if (LoadingThread != null && LoadingThread.IsAlive)
		{
			LoadingThread.Abort();
		}
		GlobalData.PData.SaveData();
		GlobalData.Dispose();
		if (!useDirectInput)
		{
			((ComObject)GlobalData.DeviceMain).Dispose();
		}
		if (GameStatesDictionary != null)
		{
			foreach (IGameState value in GameStatesDictionary.Values)
			{
				value.Dispose();
			}
		}
		try
		{
			INI_RW iNI_RW = new INI_RW(".\\Setting.INI");
			if (iNI_RW.ExistINIFile())
			{
				iNI_RW.IniWriteValue("Volume", "BGMVolume", GlobalData.BGMVolume.ToString());
				iNI_RW.IniWriteValue("Volume", "SEVolume", GlobalData.SEVolume.ToString());
			}
			iNI_RW = null;
		}
		catch
		{
		}
	}

	private void Form_Main_KeyDown(object sender, KeyEventArgs e)
	{
		//IL_000c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0011: Unknown result type (might be due to invalid IL or missing references)
		//IL_0055: Unknown result type (might be due to invalid IL or missing references)
		//IL_005a: Unknown result type (might be due to invalid IL or missing references)
		//IL_009e: Unknown result type (might be due to invalid IL or missing references)
		//IL_00a3: Unknown result type (might be due to invalid IL or missing references)
		//IL_00e7: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ec: Unknown result type (might be due to invalid IL or missing references)
		//IL_0130: Unknown result type (might be due to invalid IL or missing references)
		//IL_0135: Unknown result type (might be due to invalid IL or missing references)
		//IL_0179: Unknown result type (might be due to invalid IL or missing references)
		//IL_017e: Unknown result type (might be due to invalid IL or missing references)
		//IL_01c2: Unknown result type (might be due to invalid IL or missing references)
		//IL_01c7: Unknown result type (might be due to invalid IL or missing references)
		//IL_0200: Unknown result type (might be due to invalid IL or missing references)
		//IL_0205: Unknown result type (might be due to invalid IL or missing references)
		GlobalData.KClass.ArrowLeft = ((object)e.KeyCode/*cast due to constrained. prefix*/).ToString() == mapkey.Left || GlobalData.KClass.ArrowLeft;
		GlobalData.KClass.ArrowRight = ((object)e.KeyCode/*cast due to constrained. prefix*/).ToString() == mapkey.Right || GlobalData.KClass.ArrowRight;
		GlobalData.KClass.ArrowUp = ((object)e.KeyCode/*cast due to constrained. prefix*/).ToString() == mapkey.Up || GlobalData.KClass.ArrowUp;
		GlobalData.KClass.ArrowDown = ((object)e.KeyCode/*cast due to constrained. prefix*/).ToString() == mapkey.Down || GlobalData.KClass.ArrowDown;
		GlobalData.KClass.Key_Shift = ((object)e.KeyCode/*cast due to constrained. prefix*/).ToString() == mapkey.Slow || GlobalData.KClass.Key_Shift;
		GlobalData.KClass.Key_Z = ((object)e.KeyCode/*cast due to constrained. prefix*/).ToString() == mapkey.Shoot || GlobalData.KClass.Key_Z;
		GlobalData.KClass.Key_X = ((object)e.KeyCode/*cast due to constrained. prefix*/).ToString() == mapkey.Bomb || GlobalData.KClass.Key_X;
		switch (((object)e.KeyCode/*cast due to constrained. prefix*/).ToString())
		{
		case "R":
			GlobalData.KClass.Key_C = true;
			break;
		case "ControlKey":
			GlobalData.KClass.Key_Ctrl = true;
			GlobalData.KClass.Key_plus = true;
			break;
		case "ShiftKey":
			GlobalData.KClass.Key_minus = true;
			break;
		case "Return":
			GlobalData.KClass.Key_Z = true;
			break;
		case "Escape":
			GlobalData.KClass.Key_ESC = true;
			break;
		case "Oemplus":
			GlobalData.KClass.Key_plus = true;
			break;
		case "OemMinus":
			GlobalData.KClass.Key_minus = true;
			break;
		case "F8":
			WindowSwitch();
			break;
		case "P":
			ScreenShot();
			break;
		}
	}

	private void Form_Main_KeyUp(object sender, KeyEventArgs e)
	{
		//IL_000c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0011: Unknown result type (might be due to invalid IL or missing references)
		//IL_0055: Unknown result type (might be due to invalid IL or missing references)
		//IL_005a: Unknown result type (might be due to invalid IL or missing references)
		//IL_009e: Unknown result type (might be due to invalid IL or missing references)
		//IL_00a3: Unknown result type (might be due to invalid IL or missing references)
		//IL_00e7: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ec: Unknown result type (might be due to invalid IL or missing references)
		//IL_0130: Unknown result type (might be due to invalid IL or missing references)
		//IL_0135: Unknown result type (might be due to invalid IL or missing references)
		//IL_0179: Unknown result type (might be due to invalid IL or missing references)
		//IL_017e: Unknown result type (might be due to invalid IL or missing references)
		//IL_01c2: Unknown result type (might be due to invalid IL or missing references)
		//IL_01c7: Unknown result type (might be due to invalid IL or missing references)
		//IL_0200: Unknown result type (might be due to invalid IL or missing references)
		//IL_0205: Unknown result type (might be due to invalid IL or missing references)
		GlobalData.KClass.ArrowLeft = !(((object)e.KeyCode/*cast due to constrained. prefix*/).ToString() == mapkey.Left) && GlobalData.KClass.ArrowLeft;
		GlobalData.KClass.ArrowRight = !(((object)e.KeyCode/*cast due to constrained. prefix*/).ToString() == mapkey.Right) && GlobalData.KClass.ArrowRight;
		GlobalData.KClass.ArrowUp = !(((object)e.KeyCode/*cast due to constrained. prefix*/).ToString() == mapkey.Up) && GlobalData.KClass.ArrowUp;
		GlobalData.KClass.ArrowDown = !(((object)e.KeyCode/*cast due to constrained. prefix*/).ToString() == mapkey.Down) && GlobalData.KClass.ArrowDown;
		GlobalData.KClass.Key_Shift = !(((object)e.KeyCode/*cast due to constrained. prefix*/).ToString() == mapkey.Slow) && GlobalData.KClass.Key_Shift;
		GlobalData.KClass.Key_Z = !(((object)e.KeyCode/*cast due to constrained. prefix*/).ToString() == mapkey.Shoot) && GlobalData.KClass.Key_Z;
		GlobalData.KClass.Key_X = !(((object)e.KeyCode/*cast due to constrained. prefix*/).ToString() == mapkey.Bomb) && GlobalData.KClass.Key_X;
		switch (((object)e.KeyCode/*cast due to constrained. prefix*/).ToString())
		{
		case "R":
			GlobalData.KClass.Key_C = false;
			break;
		case "ControlKey":
			GlobalData.KClass.Key_Ctrl = false;
			GlobalData.KClass.Key_plus = false;
			break;
		case "ShiftKey":
			GlobalData.KClass.Key_minus = false;
			break;
		case "Return":
			GlobalData.KClass.Key_Z = false;
			break;
		case "Escape":
			GlobalData.KClass.Key_ESC = false;
			break;
		case "Oemplus":
			GlobalData.KClass.Key_plus = false;
			break;
		case "OemMinus":
			GlobalData.KClass.Key_minus = false;
			break;
		}
	}

	private void ScreenShot()
	{
		//IL_003f: Unknown result type (might be due to invalid IL or missing references)
		Surface backBuffer = DeviceMain.GetBackBuffer(0, 0);
		int i;
		for (i = 0; File.Exists($".\\ScreenShot\\screen{i:0000}.jpg"); i++)
		{
		}
		Surface.ToFile(backBuffer, $".\\ScreenShot\\screen{i:0000}.jpg", (ImageFileFormat)1);
		if (GlobalData != null && GlobalData.SoundDictionary != null && GlobalData.SoundDictionary.ContainsKey("se_shutter.wav"))
		{
			GlobalData.SoundDictionary["se_shutter.wav"].Play();
		}
	}

	private void ToggleUnrealFullScreen()
	{
		fullWindow = !fullWindow;
		if (fullWindow)
		{
			Screen val = Screen.FromPoint(((Form)Form_Main).Location);
			((Form)Form_Main).FormBorderStyle = (FormBorderStyle)0;
			((Form)Form_Main).ClientSize = val.Bounds.Size;
			((Form)Form_Main).Location = val.Bounds.Location;
		}
		else
		{
			((Form)Form_Main).FormBorderStyle = (FormBorderStyle)3;
		}
		MouseCtrl.DrawCursor(!fullWindow);
	}
}
