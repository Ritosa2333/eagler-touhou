# TH17 开发指南

[开发入口](../README.md) · [适配表](../ADAPTATION.md) · [Runtime 契约](../../../launcher/docs/GAME_ADAPTER_CONTRACT.md)

## 构建

使用 Node.js 22.18 以上、Python 3、Emscripten 6.0.9。编译默认读取仓库 `common/` 的键盘与手柄边界，在每次原生模拟采样时消费 Host 映射。TH17 的渲染、原版帧节奏与触控运动轨迹保留本作实现。

在仓库根目录运行：

```sh
node tools/workspace.mjs select th17 --variant=normal
node launcher/scripts/build-runtime.mjs --game=th17 --variant=normal --emsdk=/path/to/emsdk
node ports/th17/web/scripts/build-sdl-application.mjs --release --thprac --print-inputs
```

直接构建可设置 `TH17_EMSDK` 或 `EMSDK`，Python 可由 `TH_PYTHON` 指定。产物位于 `web/artifacts/sdl-release/`。生产构建去除开发探针，保留游戏逻辑、文件格式验证与原生录像启动接口。

打包 Runtime：

```sh
node ports/th17/portable/package-eagler.mjs
```

`build-eagler/` 输出 `th17.html`、程序、CP932 转码表、资源清单和逐文件 SHA-256，可通过共用 Runtime Release 打包入口使用。`EAGLER_RUNTIME_OUTPUT` 可指定输出目录。Host 从 Package Store 挂载共用 MS Gothic 和 Unifont、原版 DATA 与音乐。浏览器渲染时按需生成和缓存字形。SDL 浏览器测试可用 `TH17_SHARED_FONT_ROOT` 指向这两份共用字体。

## 本地输入

`games/th17/original/` 放个人原版 `th17.dat`、`thbgm.dat` 与开发对照用 `th17.exe`。`web/assets/`、`web/reference/assets/` 为被忽略的开发输入，可从个人交付包连接或准备，源码迁移清单记录了来源身份。原版素材、音乐、录像、测量字体、SDK 和编译输出分别位于仓库忽略目录。

```sh
node launcher/scripts/prepare-th17-content.mjs --original=/path/to/original --output=/path/to/assets-ogg
```

该入口生成原版 archive、18 首 OGG 及来源记录；编码与共用 Host 的固定 OGG 基线核对。已准备的内容可由 Host 的 `assets-ogg` 输入使用。单个 `th17.dat` 也可通过共用启动器的原版文件导入流程安装。

## thcrap 语言包

启动器为 TH17 提供日语原文、简体中文和英文。Host 使用共用的 thcrap 编译入口，从补丁依赖链读取贴图和文本。翻译使用 Unifont，MS Gothic 补充缺字；日语原文使用 MS Gothic，Unifont 补充缺字。离线语言包附带 Unifont 子集。手工准备时，对每个语言执行一次，并用 `--refresh 1` 获取当前补丁清单。

```sh
node launcher/scripts/prepare-th06-language-pack.mjs --game th17 --language lang_zh-hans --archive /path/to/th17.dat --thdat /path/to/thdat --thmsg /path/to/thmsg --font-file /path/to/unifont.otf --output /path/to/th17-language-packs --refresh 1
```

`assets-ogg/th17.data` 也是可用的原版档案输入。英文使用 `lang_en`。标题、菜单、关卡标识、人物名牌和结局等 ANM 贴图按原图集的精灵区域覆盖。Music Room 按列表序号匹配曲名和评论，播放仍使用原作的曲目文件。符卡名在战斗、符卡练习和玩家资料中按当前难度显示，存档保存原版名称。

## thprac 练习

TH17 普通版 Runtime 使用 `--thprac` 编入练习面板。启动前启用 thprac，在关卡练习中选完动物灵便打开 thprac 选关。面板可选择原版或增强练习；增强练习可以选整关、道中波次、道中 Boss、关底 Boss、非符或符卡。偶像恶魔和鬼畜生的所业在符卡练习中也有阶段选择。按 Z 或 Enter 确认，按 X 或 Esc 返回。触屏可轻点面板外确认。增强练习可配置残机、炸弹、火力、分数、擦弹和起始灵体，结果保存在练习录像里。

桌面用 Backspace 打开作弊菜单、F12 打开高级设置、Tab 切换状态信息。手机启用 thprac 触控键后，通过触控栏打开可折叠的 ImGui 面板。对话期间可继续拖动或用摇杆移动，轻点推进对话。高级设置提供键位和按键速率监视、反应测试、偶像恶魔阶段记录及咆哮录像修复。增强练习录像也记录 Boss 移动范围、灵体生成、分数上限和全通奖励设置。练习面板采用启动器语言和 Unifont。

分段与阶段规则取自 [thprac purple](https://github.com/rueee/thprac/tree/master_2) 的 TH17 源码，运行时验证 1.00b ECL。更新生成文件时，在仓库根目录运行：

```sh
node ports/th17/portable/generate-thprac.mjs /path/to/thprac_purple
node ports/th17/portable/generate-thprac.mjs /path/to/thprac_purple --check
```

练习录像保留原版 `t17r` 输入和阶段快照格式。Web 练习扩展记录起点、阶段、灵体和作弊键状态，导入时恢复这些设置。Runtime 也识别桌面 thprac 的 `USER/PRAC` 参数。

## 协议与存储

Runtime 使用 `eagler-touhou/1`，校验同源父窗口、作品与正整数 `runtimeEpoch`，按 `ready → configure → launch → first-frame` 运行。DATA provider 为 `retail-memory`，Host 提供原始 `/th17.dat`，资源清单提供字体表，OGG 挂载到 `/music/`。原版文件与 OGG 的安装代次由 Host 统一管理。

配置支持 OGG 与无音乐、连续触控、摇杆、100–300 灵敏度、双指低速、双击符卡，以及指定文件录像启动。用户文件限定 `/savesth17/scoreth17.dat`、`th17.cfg` 和 `replay/th17_*.rpy[x]`；写入先由原生解析器验证。Runtime 定期保存，失焦、隐藏、退出时清除临时输入，复用 Host 音频上下文。帧与音频健康事件携带同一会话代次。

TH17 普通版的产品能力表登记实际构建功能。逐项运行验收记录见适配表。

当前生产构建的接入结果见[2026-10-30 验证记录](../../../archive/th17-development-20261030/migration-validation.md)，覆盖桌面 Chromium 的生产 Wasm、两种音乐模式、存档恢复与指定原版演示文件启动。

## 多人版与同步

联机会话、完整战斗回滚、房间与录像测试在 [TH17 MP 开发指南](../../th17-mp/docs/DEVELOPMENT.md)维护。通用修复沿[目录同步流程](../../../docs/adaptation/port-sync.md)进入多人版。
