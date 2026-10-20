# 萃梦想多人版开发

从仓库根目录运行：

```sh
npm run build:runtime -- --game=th075 --variant=multiplayer --emsdk=/path/to/emsdk
node launcher/scripts/package-runtime.mjs --game=th075 --variant=multiplayer
node --test ports/th075-mp/tests/eagler/*.test.mjs
node --test tools/ports/th075-package.test.mjs
```

源码位于 `web/cpp/`，浏览器 Shell 位于 `web/sdl-runtime/`。本作使用 `common/include/`、`common/src/netplay/` 和 `common/browser/adonis-calibration.mjs`；编译记录同时包含实际公共输入哈希，统一打包入口也把 Adonis calibration 绑定为 MP 打包控制输入。Runtime 清单与 HTML 明确标记 `th075mp`、`multiplayer` 模式，构建输出到 `web/artifacts/sdl3-multiplayer/`，打包输出到 `build-eagler-multiplayer/`。

`tools/ports/th075-package.test.mjs` 在临时目录分别运行 MP 源码 owner 的默认 normal 分支和 `--multiplayer` 分支，检查身份、闭集文件哈希与错误模式拒绝，并用旧 `profile: production` 行为的临时变体验证 MP profile 断言。Launcher 的 normal 构建计划由 `ports/th075` owner 提供，正式打包入口为 `node launcher/scripts/package-runtime.mjs --game=th075 --variant=normal`；这里的组件回归聚焦 MP owner 自身的两个打包器分支。

启动器开发站将 `th075mp` 指向本目录的 `build-eagler-multiplayer/th075.html`，普通版使用 `ports/th075` 的独立输出。开发站清单检查逐项核对全部多人产品的 Runtime 归属。

`portable/input/KeyboardMap.inc` 与原生源码、渲染器和 common 头文件共同参与构建输入身份核对，按键映射更新会使对应编译缓存失效。

原生协议检查需要 WASI SDK，通过 `EAGLER_WASI_CXX` 指定 `clang++`，然后运行 `node ports/th075-mp/web/scripts/test-multiplayer.mjs`。检查覆盖 D=0/1/2/9、丢包重排、回滚修正和冲突状态中止。

真实双端标题预加载检查从 `launcher/` 运行 `node tests/browser/test-room-title-preload.mjs th075 0`，纯延迟模式用最后参数 `1`。通过 `ROOM_PREVIEW_RUNTIME`、`ROOM_PREVIEW_DATA`、`EAGLER_SHARED_GOTHIC`、`EAGLER_SHARED_UNIFONT` 显式提供当前 Runtime 和私有输入。测试比较不同等待时长后的对局状态、禁止 demo，并验证开局复用实例。

历史验收按原始哈希核验，目录映射与普通版共用。原生协议、双端浏览器、手机设备和公网长时间对局按场景分别记录输入与结果。
