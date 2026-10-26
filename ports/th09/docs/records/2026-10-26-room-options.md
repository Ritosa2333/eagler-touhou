# 房间原生选项操作

房间标题接收本地操作，进入原生 Options 调整设置后，由房间统一启动多人对局。th09 与 th075 保留原生键盘、手柄和触控采样，主标题的开始对局与退出由房间管理。

## 八作浏览器检查

以公网发布 `sha256-0d2dc5f810ff4241a0c81be6f815edec2fbc76e161c8cacc6797b4935e1890e4` 的 Runtime 文件清单核对输入，使用本地原版 DATA。th09 与 th075 的调整版本分别使用正式 WASM `e9b8298d0814c6f493dc1ef83663d9a5f6f9c93ed07d5d75ddb234d74b57c6f1`、`f01d2c0ebff9d375ec387ba8c3d35c8b63b975386005526587e00b9c8d9ce0cc`。

| 作品 | 运行版本 | 行为 |
| --- | --- | --- |
| th06、th07、th08、th10、th11、th15 | 公网发布包 | 托管按键进入 Options，聚焦后的物理按键调整选项，再返回标题 |
| th09 | 调整版本 | 原生菜单从主标题进入 Options，调整设置后返回主标题 |
| th075 | 调整版本 | 原生场景从标题进入 Options，调整设置后返回标题 |

`launcher/tests/browser/test-room-options.mjs` 保存每作标题、Options、调整后与返回时的画面，以及原生状态、配置文件和 WASM 摘要。`ROOM_OPTIONS_INPUTS` 指向私有输入 JSON，格式为 `{"games":{"th09":{"runtime":"/path/to/runtime","data":"/path/to/th09.data"}}}`。字体通过 `EAGLER_SHARED_GOTHIC`、`EAGLER_SHARED_UNIFONT` 提供，可从 `launcher/` 运行 `npm run test:room-options:browser`。

## 调整选项后联机

`ROOM_PREVIEW_OPTIONS=1 node tests/browser/test-room-title-preload.mjs th09 0` 与 `th075 0` 使用两台独立游戏实例和本地 Relay。两端均进入原生选项，其中一端用物理按键调整设置，分别返回标题、等待并由房间同步开局。检查双端相同逻辑帧状态一致、标题持续呈现、程序与 DATA 复用。

th09 对照 102 个同帧状态。th075 使用有效的每 60 帧确认摘要，对照 4 个确认点，覆盖开局后 240 帧。

th09 的托管输入测试覆盖预加载前拒绝输入、房间标题接收键盘与触控，以及配置网络开局后停止接收本地菜单输入。对应 `managed-touch-queue.test.mjs`、`managed-storage.test.mjs`、`spectator-room-state.test.mjs` 共 42 项通过。th075 的打包与联机选项组件检查共 4 项通过。

## 返回房间后的本地配置

th06、th07、th08、th09、th10、th11、th15 的保留实例返回使用同一启动器交接：先完成原生返回，再下发开局前的本地菜单配置，Runtime 确认后恢复输入与房间通知。直接创建的联机实例按本地游戏选项恢复，联机角色与上一局的网络字段由交接清理。

`ROOM_OPTIONS_RETURN=1` 在正式 Runtime 中配置网络角色、执行原生返回和本地配置恢复，再操作 Options。七作均通过原生菜单操作检查；返回保留同一游戏实例。th075 使用其独立房间退出入口，首次预加载的原生 Options 单独验收。

`runtime-service.test.mjs` 与 `player-keyboard.test.mjs` 共 59 项通过，覆盖实际配置消息、确认前输入隔离、原生自动返回、旧开局事件隔离及键盘映射。恢复配置复用已加载的程序、DATA、字体与语言资源。
