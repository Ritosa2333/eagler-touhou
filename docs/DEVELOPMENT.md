# TH05 开发指南

TH05 将 uth05win 的 C++ 游戏逻辑直接编译为 WebAssembly。`portable/` 提供 GLES2 绘制、浏览器输入、字体和音频接口；DAT 解码、弹幕、角色、场景与 Replay 由 `Game/`、`gameLib/` 执行。浏览器程序无需 PC-98 CPU、DOS 或硬件模拟器。

## 构建

依赖 Node.js 24+、Python 3 和 Emscripten。资源制备的标题图解码使用 Pillow。

```sh
node launcher/scripts/build-runtime.mjs --game=th05 --variant=normal --emsdk=/path/to/emsdk
```

也可在本目录设置 `EMSDK` 后运行 `node scripts/build-eagler.mjs`。产物位于 `artifacts/runtime/`，包括单一 `th05.wasm`、ES modules、Runtime 文件身份和构建来源。`--print-inputs` 输出实际编译输入，不启动编译器。

`TH05_BROWSER_TESTS=1` 构建到 `artifacts/test-runtime/`，增加角色／关卡矩阵检查入口。生产程序不包含该入口。

## 游戏资源与 PR 音频

```sh
node launcher/scripts/prepare-th05-content.mjs --original=/path/to/TH05 --resources=/path/to/uth05win --output=/path/to/assets-eagler
node launcher/scripts/prepare-th05-language-pack.mjs --original=/path/to/uth05win --output=/path/to/languages
```

资源制备核对原版 `kaiki1.dat`、`kaiki2.dat`、`zun.com` 的 MD5，也通过文件内容识别日文名称的 DAT。`--resources` 提供 `anex86.bmp`、练习／Replay 图片和额外 Music Room 数据。汉化使用 uth05win 的 `chs.dat`，以静态语言包交给启动器安装。

制备脚本自动下载 [PR #143](https://github.com/Ritosa2333/eagler-touhou/pull/143) 对应源码 `th05-coop-lab@6eb46ed1d4f581f109ffa494ea0d21d54c8b38df` 的音频；缓存目录可由 `--pr-audio=DIR` 指定。首次下载核对 Git blob SHA-1，制备及播放核对 SHA-256。`provenance.json` 保留原始文件名与全部身份。

`th05.data` 内置独立 `sfx/effects.pcm`；BGM 放在独立 `music/` 组件。原版采用 PR 的 M86 曲目和循环点；“幺乐团专辑版”采用 PR 的 Akyu 音轨和整曲循环。Opus 的 Ogg 容器以扁平 `.ogg` 包文件名接入现有音乐清单，文件内容不转码。`resources/audio/` 保留原始元数据，`node scripts/generate-audio-layout.mjs --check` 核对生成模块。

开发启动器可设置 `EAGLER_TH05_CONTENT_DIR`、`EAGLER_TH05_LANGUAGE_PACK_DIR`；默认 DATA／音乐目录为 `ports/th05/artifacts/content/`。TH05 属于测试构建产品。

## 运行接口

`web/managed.mjs` 验证 `eagler-touhou/1`、同源父窗口和 `runtimeEpoch`，串行处理命令并回应请求。DATA 由父窗口的 managed provider 供应，文件操作进入 `/savesth05` 的 IDBFS。

原版与 thprac 共用程序。启动器的 `thpracEnabled` 决定练习 UI、工具和热键是否启用。游戏语言为 `ja`／`lang_zh-hans`；练习界面通过 `thpracLocale` 选择 `ja-JP`／`en-US`／`zh-CN`。`musicSource` 为 `original`／`akyu`，`oggDecodeMode` 为 `stream`／`full`，`music: none` 保留音效。

游戏按原生场景的 56／26 Hz 等节奏推进。独立 FBO 呈现保留逻辑帧；产品目录暂不声明画面插值能力。

Replay 文件保持 uth05win 的原始头部与输入流，`TCH5` 扩展记录每帧真实触控位移。旧录像继续可读。工作区支持检查、选择关卡、暂停、重播、定位和倍速；定位重建选中关卡的保存状态后执行原生输入流。单关卡帧数构成播放时间线。浏览器 Music Room 提供 TH05 的 23 首曲目，跟随启动器所选音源。

## 检查

```sh
node --test ports/th05/tests/managed-adapter.test.mjs
node ports/th05/tests/presentation.mjs
node ports/th05/tests/browser.mjs
node ports/th05/tests/launcher.mjs
npm run ports:check -- --game=th05 --variant=normal
```

浏览器检查使用真实 Wasm、DATA、PR 音频与语言包，默认读取本目录 `artifacts/content/`、`artifacts/languages/`。`TH05_TEST_CONTENT` 和 `TH05_TEST_FONT` 可指定 DATA 与字体。原生矩阵构建与运行均设置 `TH05_BROWSER_TESTS=1`。

来源父链见 [迁移登记](../../../docs/migration/th05-eagler-20261101.json)，上游工作区改动清单见 [source-import.json](../source-import.json)。
