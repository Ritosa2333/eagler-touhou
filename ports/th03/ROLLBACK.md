# TH03 原生联机回滚

联机战斗使用 th98port 固定版本 `0139181441e9eb895a311da66aa08463ba98ac54` 的 `MainProcessRollbackSnapshot` 捕获游戏逻辑和画面状态，并保存对战驱动及本项目触控、判定点、低速状态。网络收包、输入历史与音频设备不回退；音频命令在逻辑帧确认后依次提交。已确认帧每 120 帧检查画面及上游 `canonical_state().state_hash`；该 hash 不是完整的规范 SHA-256 摘要。

最多预测 6 帧；选人、暂停、失败动画、回合切换与资源生命周期边界等待真实输入。普通预测触发边界时阻止驱动完成不可逆的切换，恢复后等待确认。为允许恢复这种刚触发的边界，构建脚本在上游恢复时允许捕获当前恢复用状态；应继续验证其适用于更多特殊场景。

本地测试：原生快照恢复后以同一输入重演，逐帧逻辑 hash 和画面一致；单向三帧延迟双实例触发约 100 次恢复，两端共同确认的帧和音频一致。双浏览器覆盖选人、触控、延迟回滚、暂停、本局认输和最终结算。测试没有覆盖真实公网、所有角色与高负载场景。

## Native multiplayer Replay and spectators

The host publishes only confirmed packed input pairs through the PC-98 relay.
Viewers initialize at character selection and re-simulate that history, so a
late join or backward seek costs time proportional to the replayed prefix.
The relay limits history to 216000 frames. Touch deltas, focus, charge and the
custom pause commands are included in the packed inputs.

After the native versus result is dismissed and the core finalizes a V14 Replay,
each player saves a `.th03r` file in the multiplayer save namespace. This is a
versioned, SHA-256 checked container holding both the native V14 bytes and the
confirmed input history needed for this port's touch and focus extensions. It
is not a standalone `.RPY` file. The Replay workspace plays the input history
from selection and supports pause, seek and the same speed choices as TH09.
Single-player recordings retain the initial configuration and score files and
capture each applied input, focus flag and touch movement from startup, so they
can be reconstructed without overwriting the viewer's own save files. A saved
native replay is produced once the upstream core finalizes a run. During seek and
non-1x playback audio is muted. Replay and spectator simulation require the same
native build and game content as the recorded match.

TH03 focus uses Space by default; Shift remains the charge input. In solo play
`th03FocusEnabled` gates the half-speed movement assist. In multiplayer the
room rule gates it for both players. `focusHitboxEnabled` and `alwaysHitbox`
are local presentation settings and never change simulated inputs or collision.
The marker is centered at the native player position and is drawn after the
native frame, so it is not included in rollback state checks.
