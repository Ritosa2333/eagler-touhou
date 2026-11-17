# TH05 多人版开发指南

TH05 将 uth05win 的 C++ 游戏逻辑直接编译为 WebAssembly。`portable/` 提供 GLES2 绘制、浏览器输入、字体和音频接口；DAT 解码、弹幕、角色、场景与 Replay 由 `Game/`、`gameLib/` 执行。浏览器程序无需 PC-98 CPU、DOS 或硬件模拟器。

## 构建

依赖 Node.js 24+、Python 3 和 Emscripten。资源制备的标题图解码使用 Pillow。

```sh
node launcher/scripts/build-runtime.mjs --game=th05 --variant=multiplayer --emsdk=/path/to/emsdk
```

也可在本目录设置 `EMSDK` 后运行 `node scripts/build-eagler.mjs`。产物位于 `artifacts/runtime/`，包括独立多人 `th05.wasm`、ES modules、Runtime 文件身份和构建来源。`--print-inputs` 输出实际编译输入，不启动编译器。

`TH05_BROWSER_TESTS=1` 构建到 `artifacts/test-runtime/`，增加角色／关卡矩阵检查入口。生产程序不包含该入口。

## 游戏资源与 PR 音频

```sh
node launcher/scripts/prepare-th05-content.mjs --original=/path/to/TH05 --resources=/path/to/uth05win --output=/path/to/assets-eagler
node launcher/scripts/prepare-th05-language-pack.mjs --original=/path/to/uth05win --output=/path/to/languages
```

资源制备核对原版 `kaiki1.dat`、`kaiki2.dat`、`zun.com` 的 MD5，也通过文件内容识别日文名称的 DAT。`--resources` 提供 `anex86.bmp`、练习／Replay 图片和额外 Music Room 数据；原版 `SE/s01.wav`～`s15.wav` 可通过 `--se=DIR` 单独指定。汉化使用 uth05win 的 `chs.dat`，以静态语言包交给启动器安装。

制备脚本自动下载 [PR #143](https://github.com/Ritosa2333/eagler-touhou/pull/143) 对应源码 `th05-coop-lab@6eb46ed1d4f581f109ffa494ea0d21d54c8b38df` 的音频；缓存目录可由 `--pr-audio=DIR` 指定。首次下载核对 Git blob SHA-1，制备及播放核对 SHA-256。`provenance.json` 保留原始文件名与全部身份。

`th05.data` 内置 uth05win 原版 15 个 SE WAV。相同音效按原来的 Stop／Reset／Play 语义重播，其他正在播放的音效继续运行；BGM 放在 `music/` 组件。原版采用 PR 的 M86 曲目和循环点；“幺乐团专辑版”采用 PR 的 Akyu 音轨和整曲循环。Opus 的 Ogg 容器以扁平 `.ogg` 包文件名接入现有音乐清单，文件内容不转码。`resources/audio/` 保留原始元数据，`node scripts/generate-audio-layout.mjs --check` 核对生成模块。

开发启动器可设置 `EAGLER_TH05_CONTENT_DIR`、`EAGLER_TH05_LANGUAGE_PACK_DIR`；默认 DATA／音乐目录为 `ports/th05/artifacts/content/`。TH05 属于测试构建产品。

TH01–04 的曲目制备使用原 uth05win 的 PMDWin DLL 和 32 位 Windows 渲染工具 `scripts/render-music-room.cpp`。编译工具后运行：

```sh
python scripts/prepare-music-room-audio.py --archive=/path/to/music.dat --dll=/path/to/PMDWin.dll --renderer=/path/to/render-music-room.exe --output=artifacts/music-room-audio
```

`resources/audio/music-room.json` 固定各曲目的源数据、音频身份和循环点。内容制备可通过 `--music-room-audio=DIR` 指定结果目录。曲目顺序和评论继续读取原 Music Room 数据。

同一渲染工具还记录 107 首原始 PMD 曲目的钢琴按键与小节时间。运行 `python scripts/prepare-music-notes.py --archive=/path/to/music.dat --dll=/path/to/PMDWin.dll --renderer=/path/to/render-music-room.exe --output=artifacts/music-room-audio`，然后以 `--music-notes=DIR` 交给内容制备脚本。`music-notes.bin.gz` 随 DATA 安装，浏览器按 Ogg 的播放时间查询显示；离线制备的工具不进入游戏运行时。专辑音源使用其自身音频，原始 PMD 的音符记录用于原版音源。

## 手机输入

启动器的 `direct-touch` 坐标按 iframe 比例转换为 640×480 游戏坐标，并扣除横竖屏的黑边。`touch-controls` 驱动移动、低速、射击和原生练习面板；在触控布局设置中开启 thprac 按键后，点击“作弊和高级设置”进入与桌面共用的 C++ 工具。

`PracticeMobilePanels.hpp` 提供作弊、高级设置和信息折叠面板，`PracticePointer.hpp` 在按下时确定 UI／游戏归属。信息区内开始的拖动可继续交给自机，原生按钮点击不会移动自机。对话支持单击确认、长按跳过；触控回放记录实际位移。

自动开火在游玩场景注入射击键，标题与选人保留单指确认、双指返回的独立按键边沿。原生 ImGui 的默认窗口不参与触控命中，折叠的工具面板按其可见区域接收输入。

## 运行接口

`web/managed.mjs` 验证 `eagler-touhou/1`、同源父窗口和 `runtimeEpoch`，串行处理命令并回应请求。DATA 由父窗口的 managed provider 供应，文件操作进入 `/savesth05` 的 IDBFS。

原版与 thprac 共用程序。启动器的 `thpracEnabled` 决定练习 UI、工具和热键是否启用。游戏语言为 `ja`／`lang_zh-hans`；练习界面通过 `thpracLocale` 选择 `ja-JP`／`en-US`／`zh-CN`。`musicSource` 为 `original`／`akyu`，`oggDecodeMode` 为 `stream`／`full`，`music: none` 保留音效。

游戏按原生场景的 56／26 Hz 等节奏推进。独立 FBO 呈现保留逻辑帧；产品目录暂不声明画面插值能力。

图形适配器按相邻且兼容的纹理、混合、深度、裁剪和颜色掩码状态合并几何，保持原始绘制顺序。纹理变更与帧缓冲切换会提交待绘制几何。时钟将积累限制为 70 ms，每次回调最多推进 4 个逻辑帧，并绘制最后一帧。右下角 FPS 统计实际呈现的新画面，重复呈现同一帧不增加计数。

本作高刷入口关闭，Runtime 强制保留呈现上限。56 Hz 游玩与 26／60 Hz 场景的新画面直接随对应更新提交，避免独立的 60 Hz 门控在 75／90 Hz 屏幕覆盖未呈现的帧。时钟允许最多 1 ms 的提前量并保留负余额，吸收回调抖动，长期逻辑速度保持原值。追帧的中间步骤执行游戏逻辑，原生 ImGui 及鼠标边沿队列在实际绘制步骤推进，保留暂停菜单与 thprac 点击。

关卡背景将原 16×16 MPN 图块合成一张 256×256 图集，600 个图块在一次几何提交中绘制，原有滚动位置与纹理采样保持一致。激光的宽线采用实心四边形，分别绘制彩色外沿和白色核心；1 像素预告线保留原表现。

弹幕、发弹动画、爆炸、火花、自机与道具的小图在关卡资源中共享精灵图集。图形适配器将原纹理 ID 的局部 UV 映射到图集位置，保留相邻绘制顺序、混合、透明遮罩及原图的 CPU 读取。对话脚本加载或替换 Boss 图像后重建图集；纹理更新和采样参数变化会撤销对应映射，图集随关卡释放。片段着色器在支持时使用高精度纹理坐标。

待机 Demo 保留原始输入流。原生输入场景 `14` 表示演示，自动开火在此场景释放射击键；手动点击与按键继续触发原有退出逻辑。

简体语言在原 CP932 图集位置替换 1829 个字形，Music Room 的曲目说明和标题沿用原始文字布局；动态文本使用 512 项 LRU 缓存。APS 导出进入 `/savesth05/APS.csv`，通过现有用户文件接口与 IDBFS 保存。

Replay 文件保持 uth05win 的原始头部与输入流，`TCH5` 扩展记录每帧真实触控位移。旧录像继续可读。工作区支持检查、选择关卡、暂停、重播、定位和倍速；定位重建选中关卡的保存状态后执行原生输入流。单关卡帧数构成播放时间线。浏览器 Music Room 保留 TH01–05 共 107 首曲目。TH05 的 23 首跟随启动器所选音源，TH01–04 的 84 首 Arrange／Special MIX 从原始 MUSIC.DAT 离线制备为 Ogg；播放使用音乐文件，浏览器程序继续直接运行 C++ 游戏逻辑。

## 检查

```sh
node --test ports/th05/tests/managed-adapter.test.mjs ports/th05/tests/pointer.test.mjs ports/th05/tests/sfx.test.mjs ports/th05/tests/music-notes.test.mjs
node ports/th05/tests/presentation.mjs
node ports/th05/tests/browser.mjs
node ports/th05/tests/launcher.mjs
npm run ports:check -- --game=th05 --variant=multiplayer
```

浏览器检查使用真实 Wasm、DATA、PR 音频与语言包，默认读取本目录 `artifacts/content/`、`artifacts/languages/`。`TH05_TEST_CONTENT` 和 `TH05_TEST_FONT` 可指定 DATA 与字体，`TH05_TEST_RUNTIME` 和 `TH05_TEST_ASSETS` 可指定构建与资源根目录。原生矩阵构建与运行均设置 `TH05_BROWSER_TESTS=1`。

来源父链见 [迁移登记](../../../docs/migration/th05-eagler-20261101.json)，上游工作区改动清单见 [source-import.json](../source-import.json)。

多人构建入口固定为本目录，无须模式开关。单人构建位于 `ports/th05`；构建缓存、产物和本地存档各自独立。显示配置见 [多人规则与接口](MULTIPLAYER.md)。
