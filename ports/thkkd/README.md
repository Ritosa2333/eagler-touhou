# thkkd（東方海恵堂）

[适配表](ADAPTATION.md) · [开发指南](docs/DEVELOPMENT.md) · [资源与打包](docs/packaging.md)

本目录提供東方海恵堂的 C++ / WebAssembly 单机移植，保留六面、EX、Omniverse、九种机体、原生菜单、存档与 Replay。游戏逻辑保持 60 Hz，接入启动器的键盘、手柄、触控、高刷新率和浏览器持久化存储。

## 构建网页程序

准备 Node.js 24、Bun 1.4.4、Python 3.14 和 Emscripten 6.0.9。EMSDK 可以指向已安装的 SDK；默认位置为仓库根目录的 `toolchains/emsdk`。从仓库根目录执行：

```powershell
bun install --cwd launcher --frozen-lockfile --ignore-scripts
python -m pip install -r ports/thkkd/scripts/web-requirements.txt
powershell -ExecutionPolicy Bypass -File ports/thkkd/scripts/build-site.ps1
node launcher/scripts/serve-static.mjs ports/thkkd/build/site 8174
```

打开 `http://127.0.0.1:8174/`，选择海惠堂。构建输出包含网站与运行程序，游戏资源由玩家本地导入。

## 原版资源

原版资源保留在本机，`assets/` 和 `content/` 均由 Git 忽略。导入一个包含原版 `dat.dxa`、`bgm.dxa` 的 ZIP，两份文件可以位于 ZIP 根目录，也可以位于同一个单层游戏目录中：

```text
海惠堂资源.zip             海惠堂资源.zip
├─ dat.dxa                 └─ 海惠堂/
└─ bgm.dxa                    ├─ dat.dxa
                             └─ bgm.dxa
```

网页在本机解包、校验图片与声音，转换 WMV 视频并保存运行缓存。首次准备可能需要较长时间，界面显示当前资源进度。安装完成后再次启动复用缓存，资源内容留在浏览器中。资源功能与网站其他作品一致，提供导入、安装和清理。

## 存档与录像

原版五份 DAT 存档及 `config.txt` 由启动器导入导出，导出文件名为 `thkkd-save.zip`。原版 `.rpy` 支持导入与播放；标准操作录制继续使用原版格式。触控自由坐标等扩展操作录制为 `.thkkdr`，在网页录像工作台回放。

默认方向键移动、Z 射击或确认、X Bomb 或取消、Shift 低速、Esc 暂停；暂停时 R 重开。网页对话支持 Z、Enter、Ctrl 和触控确认。输入映射与录像规则见开发指南。

## 源码与依赖

游戏逻辑在 `src/core`，显示计算在 `src/render`，浏览器后台在 `src/platform/web` 与 `web`；构建入口位于 `scripts`。Windows 后台继续保存在 `src/platform/windows`，其本地构建入口为 `tools/build.ps1`。第三方库、字体、解包和媒体转换组件见 [THIRD_PARTY.md](THIRD_PARTY.md)。
