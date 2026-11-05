# 萃梦想普通版开发

从仓库根目录运行，使用显式 Emscripten SDK：

```sh
npm run build:runtime -- --game=th075 --variant=normal --emsdk=/path/to/emsdk
node ports/th075/portable/package-eagler.mjs
node --test ports/th075/tests/eagler/content-bundle.test.mjs
```

源码位于 `web/cpp/`，浏览器 Shell 位于 `web/sdl-runtime/`。构建输出到本作 `web/artifacts/sdl3/`，打包输出到 `build-eagler/`。启动器与构建工具统一来自本仓库。

原生源码、渲染器头文件和 `portable/input/KeyboardMap.inc` 共同参与编译输入身份核对，修改按键映射后会重新编译对应程序。

私有素材通过 `portable/prepare-content.mjs --assets=/path/to/assets --output=/path/to/output` 准备。该输入包含已提取的原版资源、字形和音乐，生成结果保持本地。

正式发布的 `GameDirectories.th075` 指向本作原版资源或准备目录，配置格式见 `launcher/tools/maintainer/release-input.example.json`。

`web/reference/release-acceptance.json` 保存原始验收的哈希与路径。构建将历史 `th075_web/` 映射到 `web/`，按原始身份核验；当前构建和运行检查分别记录在适配表中。

普通版与 MP 分别编译和验收，见[适配表](../ADAPTATION.md)。
