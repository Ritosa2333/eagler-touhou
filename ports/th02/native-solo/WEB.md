# SoEW Native Web：完整单人版

网页与 Win32 版运行同一个 SoEW Native 单人程序。从原版标题进入 Story 1–5 面、Extra、练习、录像、排行榜和音乐室。Canvas 显示原生画面，Web Audio 播放原生合成器输出，IndexedDB 保存该网址下的设置、成绩和录像。Extra 沿用原生版解锁条件。

## 本地运行

从 SoEW Native 根目录运行（Eagler 源码树内为 `ports/th02/native-solo`）。使用 Emscripten 4.0.15、CMake、Ninja 与 Node.js；先激活 Emscripten 环境，并用 `bootstrap.ps1` 准备 SoEW 的 th98port 依赖，或用 `-DTH98PORT_DIR=<目录>` 指向已准备好的 checkout。

```sh
emcmake cmake -S . -B build-web -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-web --target soew-full soew-web
node web/stage-content.mjs <解包资源目录> build-web
node web/stage-glyphs.mjs <此分支编译的 Windows SoEW.exe> build-web
node web/serve.mjs build-web 18323
```

打开 [本地游戏](http://127.0.0.1:18323/)，按 Z 或 Enter 操作原生菜单；首次按键或点击启用音频。标题界面的 F8 打开本机 `.rep` 文件。

练习参数中把「种子模式」设为固定，点「输入种子」整行，或选中后直接打数字。Enter 保存，可用 Ctrl+V 粘贴十进制种子（0–4294967295）。同一套练习参数使用同一固定种子重开。

改动界面文字或文本资源后，重新运行 `web/stage-glyphs.mjs`，让预览同步使用最新的 Win32 字形表。

资源目录可用 `python tools/prepare_content.py <自己的日文 TH02 HDI> <新的资源目录>` 生成，也可以直接使用已有 Win32 版的 `content`。浏览器沿用原生版对 `locale/`、`zunsoft/` 和 MIDI SoundFont 的资源约定。固定字格（包括 F12 练习工具和退格辅助面板）使用 Win32 版从本机 GDI 导出的字形位图。当前入口独立于大厅和 DOSBox-X 版。对外分发前核对字形位图、游戏素材与 [th98port](README.md#build-and-test-notes) 的授权。

## 音乐室与音源

在 `stage-content.mjs` 之后、导出字形之前准备本地音乐资源：

```sh
node web/stage-midi.mjs "<GeneralUser GS SoftSynth v1.44.sf2>" build-web
node web/stage-akyu.mjs "<旧 TH02 分支的 web/bgm>" build-web
uv run tools/import_wiki_music.py build-web/content
```

MIDI 使用原生版指定的 GeneralUser GS SoftSynth 1.44，可从 [th98port 采用的上游镜像](https://github.com/CoderLine/alphaSynth/tree/7514e992fb8bf9840067ad447a4932f885fe82e2/Samples/Data) 取得。导入时校验文件哈希和 15 首原盘 `.MMD` 的清单。幺乐团使用旧版的 Vol. 3 录音和循环点，随游戏换曲、暂停和恢复，音效仍走原生引擎。游戏设置的音乐行可选关闭、FM、MIDI 或幺乐团；音乐室 F5 切换同一个音源设置，F4 试听或停止。幺乐团的练习曲从录音开头播放。

OPTION 的「音乐音量」和「音效音量」各自保存，左右键每次调整 5%，0% 静音。FM、MIDI 和幺乐团按同一套音乐音量控制，使用各自的响度校准，切换音源时无需重新调音量。

日文曲名与曲评沿用原盘。英文采用 [Touhou Wiki](https://touhou.fandom.com/wiki/Story_of_Eastern_Wonderland/Music?oldid=155789) 贡献者的译文（CC BY-SA）；简中采用 [THBWiki](https://thwiki.cc/index.php?oldid=712594) 贡献者的译文（CC BY-NC-SA 3.0），繁中由该译文转换。长曲评可用滚轮或 Page Up / Page Down 翻阅。录音、音色库及译文包由上述命令导入本地预览。

## 跨平台录像校验

保留 `?stage1` 作为逐帧诊断入口。用 Windows MinGW 构建录像探针，从同一份资源生成原生 Story `.rep` 与每帧状态摘要，再在 Chromium 中回放并逐帧对比。使用 `npm install --no-save --no-package-lock puppeteer-core` 安装测试依赖，或将 `SOEW_BROWSER_DEPS` 指向已有安装的 `package.json`。将 `SOEW_CHROME_PATH` 设为本机 Chromium 的可执行文件路径。

```sh
cmake -S . -B build-probe -G Ninja -DCMAKE_BUILD_TYPE=Release -DSOEW_BUILD_DEV_TOOLS=ON
cmake --build build-probe --target soew-replay-probe SoEW-Native-Prototype
node web/browser.test.mjs build-web build-probe/soew-replay-probe.exe build-probe/SoEW.exe
```
