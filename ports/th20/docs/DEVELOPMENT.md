# TH20 开发指南

[适配记录](../ADAPTATION.md) · [源码目录](../../../docs/architecture/source-layout.md) · [共用构建步骤](../../../docs/adaptation/build.md)

TH20 使用 SDL3、WebGL2、FreeType 和 CP932 字符映射。游戏模块在 `src/`，浏览器平台层与 Runtime 在 `web/`，构建和资源准备使用共用启动器入口。

## 编译与打包

准备 Node.js 22.18 以上、Python 3 和 Emscripten 6.0.9。从仓库根目录运行：

```sh
npm --prefix launcher ci --ignore-scripts
node launcher/scripts/build-runtime.mjs --game=th20 --variant=normal --emsdk=/path/to/emsdk --emscripten-version=6.0.9
node ports/th20/portable/package-eagler.mjs
```

编译输出在 `ports/th20/web/artifacts/sdl3/`，完整 Runtime 在 `ports/th20/build-eagler/`。打包器核对源码与编译产物的 SHA-256，并生成 `runtime-files.json`。字体通过启动器的共用资源供给。

## 原版内容与联调

使用自己提供的 `th20.dat`、`thbgm.dat` 和 ffmpeg 准备数据与 OGG：

```sh
node launcher/scripts/prepare-th20-content.mjs --original=/path/to/TH20 --output=ports/th20/assets-ogg
```

结果包括 `th20.data` 和 19 首 `bgm-ogg/` 音乐。按[启动与资源](../../../docs/adaptation/runtime.md)设置 `EAGLER_TH20_CONTENT_DIR`、开发字体和 `EAGLER_DEVELOPMENT_GAMES=th20`，然后运行 `npm --prefix launcher run dev:workspace`。

## 自动检查

```sh
npm run ports:check -- --game=th20 --variant=normal
```

检查读取真实构建入口的编译输入，执行协议、输入、显示节奏及共用模块检查。各项结果及后续运行验收写入[适配记录](../ADAPTATION.md)。Replay 的本作映射由 `tools/replay-verifier/` 维护，共用采集与比较工具见[Replay 验证器](../../../common/testkit/replay-verifier/README.md)。源码导入与旧工作流可从[固定历史入口](../../../docs/history/README.md)查询。
