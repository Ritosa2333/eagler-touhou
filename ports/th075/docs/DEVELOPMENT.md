# 萃梦想普通版开发

从仓库根目录运行，使用显式 Emscripten SDK：

```sh
npm run build:runtime -- --game=th075 --variant=normal --emsdk=/path/to/emsdk
node ports/th075/portable/package-eagler.mjs
node --test ports/th075/tests/eagler/content-bundle.test.mjs
```

源码位于 `web/cpp/`，浏览器 Shell 位于 `web/sdl-runtime/`。构建输出到本作 `web/artifacts/sdl3/`，打包输出到 `build-eagler/`。不依赖旁边的旧独立仓库或内嵌启动器。

私有素材通过 `portable/prepare-content.mjs --assets=/path/to/assets --output=/path/to/output` 准备。该输入包含已提取的原版资源、字形和音乐，生成结果保持本地。

原始 `web/reference/release-acceptance.json` 保留原始哈希和路径。构建时仅把历史 `th075_web/` 路径映射到 `web/` 核验，不改写历史验收，也不把目录搬迁视为重新完成游戏验收。

普通版与 MP 分别编译和验收，见[适配表](../ADAPTATION.md)。
