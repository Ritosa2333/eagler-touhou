# TH20 开发指南

[适配记录](../ADAPTATION.md) | [源码目录](../../../docs/architecture/source-layout.md) | [共用构建步骤](../../../docs/adaptation/build.md)

TH20 使用 SDL3、WebGL2、FreeType 和 CP932 字符映射。游戏模块在 `src/`，浏览器平台层与 Runtime 在 `web/`，构建和资源准备使用共用启动器入口。

## 编译与打包

准备 Node.js 22.18 以上、Python 3 和 Emscripten 6.0.9。从仓库根目录运行：

```sh
npm --prefix launcher ci --ignore-scripts
node launcher/scripts/build-runtime.mjs --game=th20 --variant=normal --emsdk=/path/to/emsdk --emscripten-version=6.0.9
node launcher/scripts/package-runtime.mjs --game=th20 --variant=normal
```

编译输出在 `ports/th20/web/artifacts/sdl3/`，完整 Runtime 在 `ports/th20/build-eagler/`。打包器核对源码与编译产物的 SHA-256，并生成 `runtime-files.json`。字体通过启动器的共用资源供给。

模块头文件搜索目录按名称固定排序，Windows 与 Linux 使用相同的搜索优先级和源码身份。

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

运行状态的输入字段读取游戏当前绑定的 `button_slot(0)`，未绑定时为 `-1`。正常启动由协议事件和源码／产物身份确认，标准错误流保留实际错误诊断。

`th20_native_observation_v1()` 使用共用 12-word 只读 ABI，提供阶段、实际消费输入、玩家坐标、PauseInf 状态和 GameController 逻辑计时。有效位区分未建立的字段；读取不推进游戏，也不清空触控脉冲。

`th20_touch_motion_trace_v2(seat, executionKind)` 按席位与执行类型读取已完成的运动 Tick。SDL 输入层保存限速前的原始目标，`player_entity::update_movement` 在 `advance_fixed_motion` 完成量化与边界约束后记录实际位置。位置与速度以像素表示，原生位置单位为 1/128；记录附本次 `GameController::frame_timer`、实际焦点状态、移动倍率、时钟倍率和触控灵敏度。过场位移及外部速度保留记录并设置未知模式标志，由对应机制场景解释。共用运动方法见[输入验收](../../../docs/adaptation/input.md)。

触控记录使用 SDL 本帧实际传入的限速预算，焦点位另取角色实际状态。目标到达后未提供模拟向量的 Tick 按原生数字输入路径记录；共用判定要求角色已到达独立目标且保持原位，才接受这类中立尾帧。

`th20_ui_observation_v1()` 提供版本化的 9-word `i32` 菜单快照：`version, validMask, pauseState, pauseSubstate, pauseCursor, retryConfirmInputReady, stoneState, stoneVisible, stoneCategory`。validMask 位 0 表示 pause 字段有效，位 1 表示 stone-menu 字段有效；无对应控制器时其字段为 `-1`。`retryConfirmInputReady` 仅在暂停页的重开确认子页且 `PauseInf.age >= 30` 时为 1，与菜单处理器开始接收该子页输入的条件一致。该 getter 只读取控制器状态，不推进菜单或消费输入。

浏览器 SDL 手柄以 DirectInput 状态接入原生输入层。六根 SDL 轴按原生范围 `[-1000, 1000]` 映射，中心值保持为 0；方向键也写入同一范围。原生输入层继续使用作品自身的 deadzone 阈值，因此中立手柄不会触发菜单方向或玩家移动。
