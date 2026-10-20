using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Threading;
using System.Windows.Forms;
using SlimDX;
using SlimDX.Direct3D9;
using SlimDX.DirectInput;

namespace Shooting
{
	public class Game_Main
	{
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

		public SlimDX.Direct3D9.Device DeviceMain
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
			try
			{
				var ini_RW = new INI_RW(".\\Setting.INI");
				if (ini_RW.ExistINIFile())
				{
					fullWindow = ini_RW.IniReadValue("DirectX", "FullWindow") == "1";
					antiAlias = ini_RW.IniReadValue("DirectX", "AntiAlias") == "1";
					verticalSync = ini_RW.IniReadValue("DirectX", "VerticalSync") == "1";
					useDirectInput = ini_RW.IniReadValue("DirectInput", "UseDirectInput") == "1";
					GlobalData.Clear = ini_RW.IniReadValue("Mode", "Clear") == "yes";
					singleThreaded = ini_RW.IniReadValue("Mode", "SingleThreaded") == "1";
					fixedFPS = ini_RW.IniReadValue("Mode", "FixedFPS") == "1";
					delayTolerance = (int)Convert.ToInt16(ini_RW.IniReadValue("Mode", "DelayTolerance"));
					windowSize = (int)Convert.ToInt16(ini_RW.IniReadValue("Mode", "WindowSize"));
					presentInterval = (int)Convert.ToInt16(ini_RW.IniReadValue("Mode", "PresentInterval"));
					useUnrealFullScreen = ini_RW.IniReadValue("Mode", "UseFullScreenWindow") == "1";
					stretchScreen = ini_RW.IniReadValue("Mode", "StretchScreen") == "1";
					mapkey.Left = ini_RW.IniReadValue("Key", "Left");
					mapkey.Right = ini_RW.IniReadValue("Key", "Right");
					mapkey.Up = ini_RW.IniReadValue("Key", "Up");
					mapkey.Down = ini_RW.IniReadValue("Key", "Down");
					mapkey.Slow = ini_RW.IniReadValue("Key", "Slow");
					mapkey.Shoot = ini_RW.IniReadValue("Key", "Shoot");
					mapkey.Bomb = ini_RW.IniReadValue("Key", "Bomb");
					MapJoyStick.Skip = (int)Convert.ToInt16(ini_RW.IniReadValue("JoyStick", "Skip"));
					MapJoyStick.Pause = (int)Convert.ToInt16(ini_RW.IniReadValue("JoyStick", "Pause"));
					MapJoyStick.Slow = (int)Convert.ToInt16(ini_RW.IniReadValue("JoyStick", "Slow"));
					MapJoyStick.Shoot = (int)Convert.ToInt16(ini_RW.IniReadValue("JoyStick", "Shoot"));
					MapJoyStick.Bomb = (int)Convert.ToInt16(ini_RW.IniReadValue("JoyStick", "Bomb"));
					MapJoyStick.C = (int)Convert.ToInt16(ini_RW.IniReadValue("JoyStick", "C"));
					bgmVolume = (int)Convert.ToInt16(ini_RW.IniReadValue("Volume", "BGMVolume"));
					seVolume = (int)Convert.ToInt16(ini_RW.IniReadValue("Volume", "SEVolume"));
					fontType = ini_RW.IniReadValue("Game", "FontType");
				}
			}
			catch
			{
				MessageBox.Show("读取配置文件失败", "Setting Load Error");
			}
			Form_Main = new RenderForm_Main();
			Form_Main.FormClosing += Form_Main_FormClosing;
			Form_Main.Deactivate += Form_Main_Deactivate;
			Form_Main.Activated += Form_Main_Activated;
			MouseCtrl.DrawCursor(!fullWindow);
#if !BROWSER
			this.GlobalData.TestFontScale();
#endif
			if (windowSize == 1)
			{
				Form_Main.ClientSize = new Size(800, 600);
			}
			else if (windowSize == 2)
			{
				Form_Main.ClientSize = new Size(1024, 768);
			}
			else if (windowSize == 3)
			{
				Form_Main.ClientSize = new Size(1280, 960);
			}
			Form_Main.Show();
			if (!Direct3DInit())
			{
				Application.ExitThread();
				return;
			}
			initSuccess = true;
				if (useDirectInput)
				{
					KCapture = new KeyboardCapture(Form_Main.Handle);
					JCapture = new JoystickCapture(Form_Main.Handle);
				}
				else
				{
					Form_Main.KeyDown += Form_Main_KeyDown;
					Form_Main.KeyUp += Form_Main_KeyUp;
				}
				PresentState = new GameState_SplashScreen(GlobalData);
				PresentState.Init();
				PresentState.BGM_ON();
				GlobalData.LoadResources1();
#if BROWSER
				browserAwaitLoad = true;
#else
				this.LoadingThread = new Thread(new ThreadStart(this.AddState));
				this.LoadingThread.Start();
#endif
		}

		public bool Direct3DInit()
		{
			bool flag;
			try
			{
				var direct3D = new Direct3D();
				var defaultAdapter = direct3D.Adapters.DefaultAdapter;
				var deviceCaps = direct3D.GetDeviceCaps(defaultAdapter.Adapter, SlimDX.Direct3D9.DeviceType.Hardware);
				CreateFlags createFlags;
				if ((deviceCaps.DeviceCaps & DeviceCaps.HWTransformAndLight) == DeviceCaps.HWTransformAndLight)
				{
					createFlags = CreateFlags.HardwareVertexProcessing;
				}
				else
				{
					createFlags = CreateFlags.SoftwareVertexProcessing;
				}
				var deviceWindow = useUnrealFullScreen ? Form_Main.panel1.Handle : Form_Main.Handle;
				GlobalData.presentParams = new PresentParameters
				{
					BackBufferWidth = 640,
					BackBufferHeight = 480,
					BackBufferFormat = defaultAdapter.CurrentDisplayMode.Format,
					BackBufferCount = 1,
					DeviceWindowHandle = deviceWindow,
					Windowed = !(fullWindow & !useUnrealFullScreen),
					Multisample = MultisampleType.None,
					PresentFlags = PresentFlags.DiscardDepthStencil,
					SwapEffect = SwapEffect.Discard
				};
				if (presentInterval == 1)
				{
					if (verticalSync)
					{
						GlobalData.presentParams.PresentationInterval = PresentInterval.Default;
					}
					else
					{
						GlobalData.presentParams.PresentationInterval = PresentInterval.Immediate;
					}
				}
				else if (fullWindow && verticalSync)
				{
					if (presentInterval == 2)
					{
						GlobalData.presentParams.PresentationInterval = PresentInterval.Two;
					}
					else if (presentInterval == 3)
					{
						GlobalData.presentParams.PresentationInterval = PresentInterval.Three;
					}
				}
				else
				{
					GlobalData.presentParams.PresentationInterval = PresentInterval.Immediate;
				}
				DeviceMain = new SlimDX.Direct3D9.Device(direct3D, defaultAdapter.Adapter, SlimDX.Direct3D9.DeviceType.Hardware, deviceWindow, createFlags, new PresentParameters[] { GlobalData.presentParams });
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
				flag = true;
			}
			catch
			{
				MessageBox.Show("DirectX初始化失败", "DirectX Initial Error");
				flag = false;
			}
			return flag;
		}

		public void AddState()
		{
			var watch = new Watch();
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
				DeviceMain.SetRenderState(RenderState.MultisampleAntialias, true);
			}
			GlobalData.LastState = PresentState;
			InitFlag = true;
		}

		public static bool RefreshUnlocked;

		public void MainProcess()
		{
			if (!BeginFrame() || deviceLost)
			{
				return;
			}
			SimulateFrame();
			if (deviceLost)
			{
				return;
			}
			DrawFrame();
			PaceFrame();
		}

		public void SimulateOnly()
		{
			if (!BeginFrame() || deviceLost)
			{
				return;
			}
			SimulateFrame();
		}

		public void DrawOnly(float alpha)
		{
			if (deviceLost || PresentState == null)
			{
				return;
			}
			if (alpha < 0f)
			{
				alpha = 0f;
			}
			else if (alpha > 1f)
			{
				alpha = 1f;
			}
			BaseObject.PresentLerp = alpha;
			try
			{
				DrawFrame();
				NoteRefresh();
			}
			finally
			{
				BaseObject.PresentLerp = 1f;
			}
		}

		private bool BeginFrame()
		{
#if BROWSER
			if (browserAwaitLoad && GlobalData.LoadingList != null && GlobalData.LoadingList.Count == 0 && Thsss.Browser.ContentLoader.BackgroundIdle)
			{
				browserAwaitLoad = false;
				AddState();
			}
#endif
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
				for (var i = 0; i < presentInterval; i++)
				{
					fpsTimer.IncreaseOneFrame();
				}
			}
#if BROWSER
			if (InitFlag && GlobalData.LoadingList.Count == 0 && Thsss.Browser.ContentLoader.BackgroundIdle)
#else
			if (this.InitFlag && this.GlobalData.LoadingList.Count == 0)
#endif
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
				var stateSwitchData = PresentState.StateSwitchData;
				if (stateSwitchData.NextState == "GameExit")
				{
					Form_Main.Close();
					return false;
				}
				if (stateSwitchData.NextState != null)
				{
					if (stateSwitchData.NextState == "MainMenu")
					{
						if (GlobalData.Rep != null)
						{
							GlobalData.Rep.CloseRpy();
						}
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
				var keyClass = new KeyClass();
				var keyClass2 = new KeyClass();
				try
				{
					KCapture.UpdateInput(ref keyClass);
				}
				catch
				{
				}
				try
				{
					JCapture.UpdateInput(ref keyClass2);
				}
				catch
				{
				}
				KeyClass.KeyMix(ref GlobalData.KClass, keyClass, keyClass2);
				if (KCapture.currentKeyboardState != null)
				{
					if (KCapture.currentKeyboardState.IsPressed(Key.LeftAlt) && KCapture.currentKeyboardState.IsPressed(Key.F4))
					{
						Form_Main.Close();
						return false;
					}
					if (KCapture.currentKeyboardState.IsPressed(Key.F8) && KCapture.lastKeyboardState.IsReleased(Key.F8))
					{
						WindowSwitch();
					}
					if (KCapture.currentKeyboardState.IsPressed(Key.P) && KCapture.lastKeyboardState.IsReleased(Key.P))
					{
						ScreenShot();
					}
				}
			}
			if (deviceLost)
			{
#if !BROWSER
				Thread.Sleep(15);
#endif
				var result = DeviceMain.TestCooperativeLevel();
				if (result == SlimDX.Direct3D9.ResultCode.DeviceNotReset)
				{
					GlobalData.OnResetDevice();
					deviceLost = false;
					firstFrame = true;
				}
				return false;
			}
			return true;
		}

		private void SimulateFrame()
		{
			try
			{
				if (OnPause)
				{
					if (PresentState is BaseStage)
					{
						GlobalData.KClass.Key_ESC = true;
					}
				}
				PresentState.UpdateData();
				for (var i = 1; i < presentInterval; i++)
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
			}
			catch (Direct3D9Exception ex)
			{
				OnDeviceException(ex);
			}
		}

		private void DrawFrame()
		{
			try
			{
				DeviceMain.Clear(ClearFlags.ZBuffer | ClearFlags.Target, Color.Black, 1f, 0);
				DeviceMain.BeginScene();
				PresentState.Render();
				DeviceMain.EndScene();
				DeviceMain.Present(Present.None);
				if (GlobalData.BGM_Player.OnPause)
				{
					PresentState.BGM_Resume();
				}
			}
			catch (Direct3D9Exception ex)
			{
				OnDeviceException(ex);
			}
		}

		private void PaceFrame()
		{
			if (!verticalSync || (presentInterval != 1 && !fullWindow))
			{
				var num = 16f * (float)presentInterval - fpsTimer.GetDuration();
#if !BROWSER
				if (num > 0f)
				{
					Thread.Sleep((int)num);
				}
				while (16.62f * (float)this.presentInterval - this.fpsTimer.GetDuration() > 0f)
				{
				}
#endif
				if (fixedFPS)
				{
					if (num < -16.66667f * (float)delayTolerance)
					{
						var i = 0;
						while ((float)i < -num / 16.66667f - (float)delayTolerance)
						{
							fpsTimer.IncreaseOneFrame();
							i++;
						}
					}
				}
			}
		}

		private void OnDeviceException(Direct3D9Exception ex)
		{
			if (ex.ResultCode == SlimDX.Direct3D9.ResultCode.DeviceLost)
			{
				GlobalData.OnLostDevice();
				deviceLost = true;
			}
			else
			{
				ErrorLog.SaveErrorLog("SlimDX错误: " + ex.Message + "\n堆栈:\n" + ex.StackTrace, ex.ResultCode.Description);
				MessageBox.Show(ex.Message);
				Form_Main.Close();
			}
		}

		private void NoteRefresh()
		{
			if (!RefreshUnlocked)
			{
				return;
			}
			var state = PresentState as BaseGameState;
			if (state == null)
			{
				return;
			}
			refreshCount++;
			var now = System.Diagnostics.Stopwatch.GetTimestamp();
			if (refreshStamp == 0L)
			{
				refreshStamp = now;
				return;
			}
			var elapsed = now - refreshStamp;
			if (elapsed < System.Diagnostics.Stopwatch.Frequency / 2L)
			{
				return;
			}
			state.FPS = refreshCount * (double)System.Diagnostics.Stopwatch.Frequency / (double)elapsed;
			refreshCount = 0;
			refreshStamp = now;
		}

		private void ToggleFullScreen()
		{
			fullWindow = !fullWindow;
			GlobalData.presentParams.Windowed = !fullWindow;
			if (fullWindow && verticalSync)
			{
				if (presentInterval == 2)
				{
					GlobalData.presentParams.PresentationInterval = PresentInterval.Two;
				}
				else if (presentInterval == 3)
				{
					GlobalData.presentParams.PresentationInterval = PresentInterval.Three;
				}
			}
			else if (presentInterval == 2 || presentInterval == 3)
			{
				GlobalData.presentParams.PresentationInterval = PresentInterval.Immediate;
			}
			Form_Main.ClientSize = new Size(640, 480);
			Form_Main.FormBorderStyle = FormBorderStyle.FixedDialog;
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
				Form_Main.ClientSize = new Size(640, 480);
			}
			else
			{
				switch (windowSize)
				{
				case 0:
					windowSize++;
					Form_Main.ClientSize = new Size(800, 600);
					break;
				case 1:
					windowSize++;
					Form_Main.ClientSize = new Size(1024, 768);
					break;
				case 2:
					windowSize++;
					Form_Main.ClientSize = new Size(1280, 960);
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
					Form_Main.ClientSize = new Size(640, 480);
					break;
				}
			}
			Form_Main.Refresh();
		}

		private void ToggleUnrealFullScreen()
		{
			fullWindow = !fullWindow;
			if (fullWindow)
			{
				var screen = Screen.FromPoint(Form_Main.Location);
				Form_Main.FormBorderStyle = FormBorderStyle.None;
				Form_Main.ClientSize = screen.Bounds.Size;
				Form_Main.Location = screen.Bounds.Location;
			}
			else
			{
				Form_Main.FormBorderStyle = FormBorderStyle.FixedDialog;
			}
			MouseCtrl.DrawCursor(!fullWindow);
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
#if BROWSER
			Thsss.Browser.GameHost.MarkExited();
#endif
#if !BROWSER
			if (LoadingThread != null && LoadingThread.IsAlive)
			{
				LoadingThread.Abort();
			}
#endif
			GlobalData.PData.SaveData();
			GlobalData.Dispose();
			if (!useDirectInput)
			{
				GlobalData.DeviceMain.Dispose();
			}
			if (GameStatesDictionary != null)
			{
				foreach (var gameState in GameStatesDictionary.Values)
				{
					gameState.Dispose();
				}
			}
			try
			{
				var ini_RW = new INI_RW(".\\Setting.INI");
				if (ini_RW.ExistINIFile())
				{
					ini_RW.IniWriteValue("Volume", "BGMVolume", GlobalData.BGMVolume.ToString());
					ini_RW.IniWriteValue("Volume", "SEVolume", GlobalData.SEVolume.ToString());
				}
			}
			catch
			{
			}
#if BROWSER
			Thsss.Browser.GameHost.NotifyClosed();
#endif
		}

		private void Form_Main_KeyDown(object sender, KeyEventArgs e)
		{
			GlobalData.KClass.ArrowLeft = e.KeyCode.ToString() == mapkey.Left || GlobalData.KClass.ArrowLeft;
			GlobalData.KClass.ArrowRight = e.KeyCode.ToString() == mapkey.Right || GlobalData.KClass.ArrowRight;
			GlobalData.KClass.ArrowUp = e.KeyCode.ToString() == mapkey.Up || GlobalData.KClass.ArrowUp;
			GlobalData.KClass.ArrowDown = e.KeyCode.ToString() == mapkey.Down || GlobalData.KClass.ArrowDown;
			GlobalData.KClass.Key_Shift = e.KeyCode.ToString() == mapkey.Slow || GlobalData.KClass.Key_Shift;
			GlobalData.KClass.Key_Z = e.KeyCode.ToString() == mapkey.Shoot || GlobalData.KClass.Key_Z;
			GlobalData.KClass.Key_X = e.KeyCode.ToString() == mapkey.Bomb || GlobalData.KClass.Key_X;
			var text = e.KeyCode.ToString();
			switch (text)
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
			GlobalData.KClass.ArrowLeft = !(e.KeyCode.ToString() == mapkey.Left) && GlobalData.KClass.ArrowLeft;
			GlobalData.KClass.ArrowRight = !(e.KeyCode.ToString() == mapkey.Right) && GlobalData.KClass.ArrowRight;
			GlobalData.KClass.ArrowUp = !(e.KeyCode.ToString() == mapkey.Up) && GlobalData.KClass.ArrowUp;
			GlobalData.KClass.ArrowDown = !(e.KeyCode.ToString() == mapkey.Down) && GlobalData.KClass.ArrowDown;
			GlobalData.KClass.Key_Shift = !(e.KeyCode.ToString() == mapkey.Slow) && GlobalData.KClass.Key_Shift;
			GlobalData.KClass.Key_Z = !(e.KeyCode.ToString() == mapkey.Shoot) && GlobalData.KClass.Key_Z;
			GlobalData.KClass.Key_X = !(e.KeyCode.ToString() == mapkey.Bomb) && GlobalData.KClass.Key_X;
			var text = e.KeyCode.ToString();
			switch (text)
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
			var backBuffer = DeviceMain.GetBackBuffer(0, 0);
			var num = 0;
			while (File.Exists(string.Format(".\\ScreenShot\\screen{0:0000}.jpg", num)))
			{
				num++;
			}
			Surface.ToFile(backBuffer, string.Format(".\\ScreenShot\\screen{0:0000}.jpg", num), ImageFileFormat.Jpg);
			if (GlobalData != null)
			{
				if (GlobalData.SoundDictionary != null)
				{
					if (GlobalData.SoundDictionary.ContainsKey("se_shutter.wav"))
					{
						GlobalData.SoundDictionary["se_shutter.wav"].Play();
					}
				}
			}
		}

		public int TouchSurface()
		{
			if (PresentState is BaseMenu)
			{
				return 0;
			}
			if (PresentState is BaseGameState state)
			{
				if (state.StageData != null && state.StageData.Story != null)
				{
					return 2;
				}
				return 1;
			}
			return 0;
		}

		public void TouchShift(float dx, float dy)
		{
			if (PresentState is BaseMenu)
			{
				return;
			}
			var state = PresentState as BaseGameState;
			if (state == null || state.StageData == null || state.StageData.Story != null)
			{
				return;
			}
			var plane = state.StageData.MyPlane;
			if (plane == null)
			{
				return;
			}
			plane.ShiftBy(dx, dy);
		}

		public RenderForm_Main Form_Main;

		private GlobalDataPackage GDP = new GlobalDataPackage();

		private IGameState PresentState;

		private Dictionary<string, IGameState> GameStatesDictionary;

#if !BROWSER
		private Thread LoadingThread;
#endif

		private Watch fpsTimer = new Watch();

		private int refreshCount;

		private long refreshStamp;

		private KeyboardCapture KCapture;

		private JoystickCapture JCapture;

		private MapKey mapkey = new MapKey();

		private bool fullWindow = false;

		private bool useDirectInput = false;

		private bool verticalSync = false;

		private bool antiAlias = false;

		private bool deviceLost = false;

		private bool singleThreaded = false;

		private int windowSize = 0;

		private int presentInterval = 1;

		private bool fixedFPS = false;

		private int delayTolerance = 10;

		private int bgmVolume;

		private int seVolume;

		private string fontType;

		public static bool useUnrealFullScreen = true;

		public static bool stretchScreen = false;

		public bool initSuccess = false;

		private bool firstFrame = true;

		private bool OnPause = false;

		private bool InitFlag = false;

#if BROWSER
		private bool browserAwaitLoad;
#endif
	}
}
