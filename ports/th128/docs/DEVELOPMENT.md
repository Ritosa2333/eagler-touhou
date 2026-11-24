# TH128 单机开发入口

[作品入口](../README.md) · [适配检查](../ADAPTATION.md) · [还原基线](RESTORATION.md)

`ports/th128` 是单机源码目录。C++ 核心负责固定步进、关卡、原版菜单和 Replay；浏览器 Runtime 负责画面、音频和输入。Launcher 管理导入、会话和退出。本作使用启动测试模式，常规站点构建不自动发布。

## 构建与组装

使用 Node.js 24 和已激活的 Emscripten 4.0.22。可用 `TH128_EMCC` 指向 SDK 中的 `upstream/emscripten/emcc.py`，可用 `TH128_PYTHON` 选择 Python。以下命令从本作目录执行：

```sh
node tools/package-launcher-runtime.mjs --print-inputs
node tools/build-runtime.mjs
node tools/package-launcher-runtime.mjs --output=build-eagler
```

编译把核心、文件哈希和源码清单写入 `site/runtime`。可用 `TH128_CORE_OUTPUT` 选择单独的核心站点目录，再以 `--assets=<核心站点目录>` 组装。组装器验证收据和两个 Core 文件的字节；新编译的收据还必须匹配当前 C++ 源码和编译选项。`--output` 指向尚不存在的目录。

维护者可以使用验收过的 Core127 站点输入组装，不需要重新编译它。完整 Core 收据、WASM 与 Runtime 清单身份见[单机验收记录](../../../archive/th128-launcher-review-20261121/README.md)。按构建流程复核入口和实际使用的生成物，勿将原游戏数据作为核心构建输入。作者 Launcher 的准备步骤见[本地开发](../../../launcher/docs/DEVELOPMENT.md)和[启动与资源](../../../docs/adaptation/runtime.md)。

## 本机验证

在共用 Launcher 启用测试作品后导入自有 TH128 安装包。安装包包含原 DAT、简中资源和整曲 OGG。选中 TH128，确认进入原标题、声音模式和输入正确；退出应返回 Launcher，再打开应使用同一安装。练习语言独立于简中数据，录像管理使用原 25 槽。不开启 BGM 时保持原确认。

从仓库根目录检查作品登记与文档：

```sh
node tools/ports/check.mjs --game=th128 --variant=normal
python tools/ports/check-history.py --base upstream/main --changed-subtrees
npm run docs:check
```

兼容性和本机行为按[验收记录](../../../docs/adaptation/record-template.md)填写。原始 DAT、OGG、翻译包和个人档案均由本机安装或存储管理。不声明 MIDI、THCRAP 或多人构建。第三方许可和署名见 [NOTICE](../NOTICE.md)。
