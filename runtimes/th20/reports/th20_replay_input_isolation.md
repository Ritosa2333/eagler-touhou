# 对白回放输入污染与帧调度核验（th20_02.rpy）

本报告回答三件事：手机/电脑端在回放时是否会互相污染、对白处是否更容易失步、以及原版 oracle 在本机能否执行。
结论先行：**污染源头是「宿主键盘桥」在回放期间仍然驱动回放的快进（`update_fast_forward`），修复后 20000 帧逐帧对照（含两段对白和一次换关）完全一致；而本次被测试的线上生成物早于修复，所以问题仍被观测到。**

## 1. Oracle 状态（必须先说清楚）

`C:\Program Files (x86)\上海アリス幻樂団\東方錦上京\th20.exe` 存在，但整文件哈希与仓库固定值不符：

| 项 | 值 |
| --- | --- |
| 实测 SHA-256 | `e6c4371e214c95dd4dbcaaf6818c79c5e89881270f65b62b8c3c965b65851e2d` |
| `reports/source_manifest.json` | `a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897` |

其余文件（`th20.dat`、`thbgm.dat`、`custom.exe`、`omake.txt`）哈希全部吻合，安装目录另有 `VERSION.txt`（`v0.1.0-beta.6`），说明是同一份 1.00c 的**非代码字节被改写**。

因此我没有拿它冒充"已验证的 oracle"，而是分开核验：`tools/oracle_code_check.py` 把 `analysis/binary/disassembly.asm` 里每一个操作码字节与实测 EXE 逐一比对：

```
code_range            0x401000..0x56b47a
code_bytes_compared   1483899
code_bytes_differing  0
```

即**回放/输入/对白所依赖的全部指令与本机安装的 EXE 逐字节一致**（证据：`reports/oracle_code_check.json`）。但按 `AGENTS.md` 约定，整文件 SHA 必须匹配才算合格 oracle，所以：

- 依赖原 EXE 的可执行对照（`source_reconstruction/*/oracle`、`title_system/replay_format_probe.cpp`、`replay_system/oracle/compare.cpp`）**本轮无法运行**——本机没有 MSVC/x86 工具链（`cl.exe`/`msbuild` 均不存在），这些工具自身就是 32 位 Windows 程序。
- 本轮的可执行验证改用下面的**确定性浏览器回放探针**（不需要 oracle：回放是仿真，同一录像的两次运行必须逐帧相同）。

## 2. 根因：回放期间实时输入仍能驱动"快进"

`source_reconstruction/replay_system/frame.cpp:45`（忠实还原 0x5081e0）：

```cpp
const auto* physical=input::button_slot(0);const bool held=physical&&(physical->current&0x201u);
```

回放输入本身只覆盖 `retained_298[1]/[4]/[5]`，但快进判定直接读**实时** `button_slot(0)->current`。命中后回调返回 6，调度器 `core_scheduler/scheduler.cpp:159` 会**从头重启整条 update 链**（`do{...}while(restart)`），于是 `o.frame%8` 每轮变化、每显示帧最多跑 8 次游戏帧——这就是原版的"按住射击键快进录像"。

问题在于浏览器端口里 `current` 的来源不止实体键盘：

- `th20_web/cpp/sdl/InputHost.cpp:312` 的 `sdl_key` 是启动器**屏幕按键与实体键盘共用的宿主键盘桥**（`k.hosted`）；
- 手机端"射击/低速/方向"按钮正是通过 `sdl_key` 注入的（触摸手势那条路 `InputHost.cpp` 在 playback 上下文已经屏蔽，所以只堵了手势的一半）。

结果：回放中任何一次射击/方向/触摸按键都会把录像推成 8 倍速，表现为"失去同步"。手势路径（`sdl_touch_controls`）在旧构建里已被屏蔽，而宿主键盘路径没有——所以"手机仍在污染"和"这次电脑端也失步"是**同一个漏洞的两种入口**。

修复（已在本工作树中，`-DTH_SDL3` 专属，原生/oracle 构建行为不变）：

- `source_reconstruction/input/input_win32.cpp:238-246`：回放期间把 `slots[0].current` 收窄为暂停键 `0x100`，并清空 `slots[2]/[3]`；回放期间"实时输入不得进入仿真或快进调度"。
- `th20_web/cpp/sdl/InputHost.cpp:189`：`sdl_replay_input_locked()` 判定（`mode==1` 且未暂停）。
- 手机端启动器的 Esc 按钮属于带外控制，单独经 `playback_escape_ticks`（`InputHost.cpp:362`）保留暂停能力，不把通用触摸 Escape/Bomb 混进回放输入。

## 3. 可执行证据：确定性回放探针

新增 `th20_web/replay-probe/`（探针页面 + 只读静态服务器 + 结果 JSON）。它不依赖 rAF：用游戏自带的定步长入口 `sdl_loop_tick` 逐帧推进，靠按键注入走完标题→Replay→关卡选择，然后播放 `th20_02.rpy`，同时注入实时输入。每帧对 `[自机x, 自机y, 状态, 场景]` 取 FNV-1a，每 300 帧一个块哈希。

20000 帧（覆盖对白窗口 7014–7281、15184–19494 与 19805 帧换关）：

| 构建 | 注入 | 结果 |
| --- | --- | --- |
| 当前 | 无 | 基准（67 块） |
| 当前 | 键盘 Z 连打 | **完全一致** |
| 当前 | 左右方向循环 | **完全一致** |
| 当前 | 触控 fire+focus 常按 | **完全一致** |
| 修复前（线上生成物 17:15） | 无 | 与基准一致（纯仿真无差异） |
| 修复前 | 键盘 Z 连打 | **从第 0 帧起不同**；20000 帧时已冲到第 4 关、录像游标 2160（基准为第 2 关、195） |
| 修复前 | 触控手势常按 | 一致（手势路径当时已被屏蔽） |

6000 帧的快速矩阵（`results.json` 的 `quick_matrix_6000_frames`）另有 `key-slow`、`touch-drag` 两项，同样一致。

**并且我确认了被测试的构建早于修复**：线上最新生成物 `runtime/th20/2e442ee6…/th20-sdl.wasm` 写于 `2026-09-29 17:15:05`（2752198 字节），而隔离补丁 `source_reconstruction/input/input_win32.cpp` 写于 18:07:44、`th20_web/cpp/sdl/InputHost.cpp` 写于 18:22:22。也就是说"手机依旧污染、电脑也失步"来自**没有含修复的构建**；修复只在未提交的工作树里，从未被部署。

## 4. 顺带修掉的两类真实缺陷

1. **热路径诊断日志**（上一轮排查留下的），已删除：
   - `portable/sdl/Renderer.cpp` 每次大纹理上传的 `SDL upload surface=...`（弹幕密集时每个动态图集都会周期性打印，正好落在"高弹幕卡顿"的路径上）；
   - `th20_web/cpp/sdl/GraphicsHost.cpp` 的 `SDL create texture ...`；
   - `th20_web/cpp/sdl/ApplicationHost.cpp` 的每场景/每 600 帧 `tick ... scene ...`；
   - `source_reconstruction/runtime_core/worker.hpp` 每次任务分派的 `worker[...]: begin/end`。

   另外工作树里保留了上一轮未提交的**按脏矩形上传**路径（`portable/sdl/Renderer.cpp` 的 `surface()` + `Renderer.hpp` 的 `Surface::dirtyX/Y/W/H`）：TH20 的动态文字图集是 2048×2048，原来每次 LockRect 都会整图转换并上传，现在只上传改动矩形——对白逐字显示时的带宽从每行 16 MiB 降到字形矩形。
2. **静默的图形设备失败**（`th20_web/cpp/sdl/GraphicsHost.cpp` 的 `GraphicsDevice::create`）：GL 初始化失败时 `host.gpu` 不会清空，于是下一个后备缓冲候选会"成功"返回，得到一个没有 GL 上下文却能启动的设备。本轮探针就踩到了它（画布选择器没绑定时 SDL 建上下文失败），现象是首帧纹理创建抛异常。现在改为打印真实 SDL 错误并失败，不再伪造成功。

本轮重建的产物与源码绑定可核验：`th20_web/artifacts/sdl3/build.json` 记录 887 个编译输入，逐一与工作树比对 0 处不一致；wasm `sha256 25ae65e999ac6bad3b8829a3419a23c7acf59b3841fda04ba49d70218fd0098b`（2752858 字节）。

## 5. 仍未完成 / 明确限制

- **硬件 oracle 未执行**：安装版 EXE 整文件哈希不符（代码段逐字节一致），且本机无 MSVC，无法构建 `*_oracle` 与 `replay_format_probe`。需要一份与 `source_manifest.json` 完全一致的原版 EXE，外加 VS2019 x86 工具链，才能跑"同输入同种子逐帧"的原版对照。
- **对白处"仿真是否 1:1"无法在本轮判定**：探针只能证明"输入不再污染"，不能证明端口仿真与原版一致。若换关/对白仍有失步，需要上面那套 oracle 逐帧对照来定位未恢复模块，而不是继续改输入层。
- **未部署**：`th20_web/artifacts/sdl3` 已按修复后源码重建（wasm sha256 `25ae65e999ac6bad3b8829a3419a23c7acf59b3841fda04ba49d70218fd0098b`），但 `deploy-th20/` 的站点生成物没有重新打包，线上仍是 17:15 的旧生成物。

## 6. 复现方式

```powershell
cd touhou20
python tools\oracle_code_check.py                       # oracle 代码段核验
node th20_web\replay-probe\serve-replay-probe.mjs --port 8791
# 打开 http://127.0.0.1:8791/replay-probe.html?build=new
#   window.H.longRun('none',       20000)                  -> 基准块哈希
#   window.H.longRun('key-mash',   20000, 基准块哈希)       -> firstDiff 应为 -1
# 把修复前的生成物放进 th20_web/replay-probe/baseline/ 后
#   打开 ?build=base 并重跑 key-mash                        -> firstDiff 应为 0
```
