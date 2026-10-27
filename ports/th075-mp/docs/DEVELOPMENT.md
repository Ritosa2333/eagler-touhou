# 萃梦想多人版开发

从仓库根目录运行：

```sh
npm run build:runtime -- --game=th075 --variant=multiplayer --emsdk=/path/to/emsdk
node ports/th075-mp/portable/package-eagler.mjs --multiplayer
node --test ports/th075-mp/tests/eagler/*.test.mjs
```

源码位于 `web/cpp/`，浏览器 Shell 位于 `web/sdl-runtime/`。本作使用 `common/include/`、`common/src/netplay/` 和 `common/browser/adonis-calibration.mjs`；编译记录同时包含实际公共输入哈希，打包时重新核验。构建输出到 `web/artifacts/sdl3-multiplayer/`，打包输出到 `build-eagler-multiplayer/`。

启动器开发站将 `th075mp` 指向本目录的 `build-eagler-multiplayer/th075.html`，普通版使用 `ports/th075` 的独立输出。开发站清单检查逐项核对全部多人产品的 Runtime 归属。

`portable/input/KeyboardMap.inc` 与原生源码、渲染器和 common 头文件共同参与构建输入身份核对，按键映射更新会使对应编译缓存失效。

原生协议检查需要 WASI SDK，通过 `EAGLER_WASI_CXX` 指定 `clang++`，然后运行 `node ports/th075-mp/web/scripts/test-multiplayer.mjs`。检查覆盖 D=0/1/2/9、丢包重排、回滚修正和冲突状态中止。

真实双端标题预加载检查从 `launcher/` 运行 `node tests/browser/test-room-title-preload.mjs th075 0`，纯延迟模式用最后参数 `1`。通过 `ROOM_PREVIEW_RUNTIME`、`ROOM_PREVIEW_DATA`、`EAGLER_SHARED_GOTHIC`、`EAGLER_SHARED_UNIFONT` 显式提供当前 Runtime 和私有输入。测试比较不同等待时长后的对局状态、禁止 demo，并验证开局复用实例。

房间标题可使用本地键盘、手柄和触控进入 Options 并调整设置。原生采样保留方向与选项操作，标题中的开始对局和退出由房间接管。`launcher/tests/browser/test-room-options.mjs` 通过托管按键进入选项，再用聚焦后的物理按键调整设置，保存原生场景号和各阶段画面；通过 `ROOM_OPTIONS_INPUTS` 提供各作的 `runtime`、`data` 路径，通过 `ROOM_OPTIONS_GAMES=th075` 选择本作。

八作菜单检查与调整设置后的双端开局见[房间原生选项操作](../../th09/docs/records/2026-10-26-room-options.md)。双端驱动增加 `ROOM_PREVIEW_OPTIONS=1`，分别进入原生选项、调整一端设置，再比较正式对局的同帧状态与实例复用。

历史验收按原始哈希核验，目录映射与普通版共用。原生协议、双端浏览器、手机设备和公网长时间对局按场景分别记录输入与结果。

旁观沿原生对局入口重放全席位已确认输入。Relay 使用 th075 独立的 `EPTM` 与 `EPNP` 标记核验时序和输入，双端浏览器驱动同时比较旁观者、玩家的同帧状态与暂停／重开流程。
