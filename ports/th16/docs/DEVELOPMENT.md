# th16 开发指南

## 原版与校验

| 文件 | 字节 | SHA-256 |
| --- | --- | --- |
| `th16.exe`（v1.00a，PE 时间戳 2017-07-31T20:47:09Z，入口 0x0047537b） | 683520 | `c11776019f083978e66027e7394dafb1fb9543afca986f28049a49417e341929` |
| `th16.dat`（THA1） | 77693268 | `c84e04181c1c08c80fb0eaba641cec9eb27244ac372749314bd886e3e5a4b64f` |
| `thbgm.dat` | 388876780 | `2904df926a97cd04c7c9d96abdcea802fab76b5432bc7359d4c3f4d11e055e4d` |

放在工作区根 `games/th16/original/`（ignored）。`web/target.json` 登记同一组身份，`analyze.mjs` 导出前校验。

## 工具链（工作区根 `toolchains/`，ignored）

| 工具 | 版本 | 用途 |
| --- | --- | --- |
| emsdk | 6.0.9 | `portable/build.mjs`，`--use-port=sdl3`，WebGL2 ↔ GLES 3.0 |
| wasi-sdk | 34 | `portable/build-harness.mjs`，无头 harness（wasm32-wasip1） |
| JDK + Ghidra | 21 / 12.1.3 | `web/scripts/cpp/analyze.mjs` headless 导出 |
| thtk | 源码构建 | `thdat -x16` 解包、`thanm -l16/-x16`、`thecl -d16`、`thstd`、`thmsg` 对照 |
| python venv | unicorn 2.1.4、capstone、pefile、pillow、numpy | `tools/oracle/*.py` |

## 容器内开发

上表工具链全部固定在 `container/Dockerfile`，本机不安装任何依赖：构建、oracle 与测试都在 `podman run --rm` 的临时容器中进行。仓库根挂载为 `/work`，容器退出即销毁；落在挂载卷里的只有 git 忽略的产物（`.cache/emscripten`、`artifacts/`、`ports/th16/tests/build/`、`ports/th16/web/artifacts/`）。镜像与 `ports/th13/container` 同构，相同阶段共享层缓存。

```bash
ports/th16/container/podman.sh bash ports/th16/tools/thtk-dumps.sh    # thtk 对照基准：artifacts/th16/{dat,dumps/anm}
ports/th16/container/podman.sh bash -lc 'bash ports/th16/tests/run.sh'        # 首次运行自动构建镜像 th16-dev
ports/th16/container/podman.sh bash -lc 'node ports/th16/web/scripts/cpp/analyze.mjs'
ports/th16/container/podman.sh bash -lc 'cd ports/th16 && node portable/build.mjs && node portable/build-harness.mjs'
```

容器以仓库根为工作目录，镜像内已设好 `EMSDK`、`WASI_SDK_PATH`、`TH16_PYTHON`（`/opt/venv`）与 `TH16_DECOMP_TOOLS`（Ghidra 12.1.3 + JDK 21），thtk 各工具在 `PATH` 上。下文各命令在容器里运行时，去掉 `toolchains/venv/bin/python`、`EMSDK=`、`WASI_SDK_PATH=` 这几处前缀即可；原版文件仍放在 `games/th16/original/`。

## 构建

```bash
EMSDK=../../toolchains/emsdk node portable/build.mjs            # web/artifacts/sdl3/th16-sdl.{mjs,wasm} + build.json
WASI_SDK_PATH=../../toolchains/wasi-sdk node portable/build-harness.mjs   # web/artifacts/wasi/th16-harness.wasm
node tools/run-harness.mjs anm games/th16/original/th16.dat title.anm      # 与 tests/build/harness 同参数
```

两条构建共用 `tools/build/source-object.mjs` 的逐 TU 对象缓存；任一头文件、flags、编译器身份变化即全部失效。失败的构建会删除产物与 manifest。

## 浏览器渲染

浏览器版使用 640×480 逻辑坐标和 1280×960 后备缓冲。整屏渲染目标和深度缓冲使用相同的 2× 缩放，HUD 数字取原版 `ascii_1280.anm`。动态文字以 A4R4G4B4 保存，GPU 只更新重绘的矩形。

## 浮点语义

`th16.exe` 以 SSE2 标量单精度编译：4289 条 SSE 标量指令对 428 条 x87；RNG 的浮点变体是 `CVTPD2PS` 后 `MULSS/SUBSS/DIVSS`。因此 C++ 侧全部使用 `float` 并禁用 FMA 融合（`-ffp-contract=off`），对 Unicorn 直跑原版函数的结果位级一致（`tests/golden/rng.json`）。仅 43 个游戏函数使用 x87 算术（34 处超越函数），这些在还原时逐个对照 `.asm`。

## Oracle 与测试

```bash
bash tests/run.sh                                   # 本机 harness + node --test
toolchains/venv/bin/python tools/oracle/rng-oracle.py          # 重生成 tests/golden/rng.json（Unicorn 直跑 0x402b70/0x402be0）
toolchains/venv/bin/python tools/oracle/thtx-compare.py tests/build/harness games/th16/original/th16.dat \
    artifacts/th16/dumps/anm artifacts/th16/dumps/textures title.anm sig.anm      # THTX 解码对 thanm -x16 逐像素
```

`tests/archive.test.mjs` 在 `artifacts/th16/dat`（thdat 解包）与 `artifacts/th16/dumps/anm`（thanm -l16）存在时对照 215 个档案条目的字节与 ANM 结构；缺原版时跳过并说明。

### ANM VM oracle

```bash
# Unicorn 直跑原版：构造 AnmManager(0x46a3a0) → ascii.anm 进槽 2 → 装载目标 ANM → 每个脚本 spawn(0x406380) + 60 帧 tick(0x46e340)
toolchains/venv/bin/python ports/th16/tools/oracle/anm-oracle.py games/th16/original/th16.exe title.anm \
    artifacts/th16/dat/title.anm all 60 artifacts/th16/oracle/title.json --extra 2:ascii.anm:artifacts/th16/dat/ascii.anm
node tools/oracle/anm-golden.mjs ../../artifacts/th16/oracle/*.json      # 压成 tests/golden/anm-vm.json（每脚本 sha256）
tests/build/harness anm-run ../../games/th16/original/th16.dat title.anm all 60   # 本实现的同格式快照
tests/build/harness anm-bench ../../games/th16/original/th16.dat title.anm 0 3600 # 只推进（性能口径）
```

oracle 只桩掉堆分配、档案读取、D3D 纹理（按 THTX 头尺寸回报）、临界区与 Sleep，其余全为原版代码；装载完成后做内存检查点，每个脚本从同一状态开始。快照为 VM 结构体 0x568 字节纯数据区（其后为链表指针）加 36 条 layer 绘制链；`tests/anm-vm.test.mjs` 对 54 个 ANM 文件 1887 个脚本逐一比对摘要，失败时若本机有完整 oracle JSON 则报出首个差异字段。`effect.anm` 155–196 需 EffectManager（ins_508），待该子系统接入后加入 golden。

`tools/run-harness.mjs` 对 stdout/stderr 的 `fd_write` 用 JS 同步写出：Node 22 自带 uvwasi 在输出到文件或 `/dev/null` 时会段错误（同一 wasm 在 wasmtime 下正常）。

### 绘制调用流 oracle

渲染侧的对拍单位是"每次 Clear / 绘制时生效的设备状态 + 顶点字节"，不比较冗余的 SetRenderState 调用次数：

```bash
# 原版：--draw 让每帧按 0x45a8a0 顺序跑绘制链，D3D 调用由 tools/oracle/d3d-recorder.py 录成 JSON Lines
toolchains/venv/bin/python ports/th16/tools/oracle/game-oracle.py games/th16/original/th16.exe artifacts/th16/dat 820 \
    /tmp/st1.json --stage 1 --shot 1 --season 2 --difficulty 1 --lives 8 --draw --draw-out /tmp/st1.draw.jsonl --draw-from 800
# 移植版：game-draw 与 game-run 同参，末尾加 <out.jsonl> [from_frame]，用 tests/harness/RecordingDevice.hpp 录同口径数据
tests/build/harness game-draw ../../games/th16/original/th16.dat 1 1 2 820 "" "" "" 1 -1 8 0 0 /tmp/st1.port.jsonl 800
# 对拍：目标 / 视口 / 清屏参数 / 图元 / 顶点格式 / 顶点字 / 渲染状态子集 / 世界格式的矩阵；退出码 1 表示有差异
python3 tools/oracle/draw-compare.py /tmp/st1.draw.jsonl /tmp/st1.port.jsonl --frames 800-820
```

定位差异时先看单侧的逐操作清单：`draw-compare.py … --dump oracle`（或 `--dump port`）按帧列出每个 Clear / 绘制的纹理槽、视口、
前四个顶点；oracle 侧每项带 `[0x地址]`，即 DrawPrimitiveUP 发生时正在运行的绘制链回调（0x4015a0 链游标处的钩子记录），
因为批量顶点是惰性出批，归因可能落到下一条链。移植版再配 `TH16_DUMP_LAYERS=<帧>` 把该帧各图层链上的 VM
（槽 / 脚本 / 精灵及其来源槽 / 渲染模式 / 位置 / 颜色 / 标志）打印到 stderr，逐 VM 对账。
需要排除的子系统用 `--skip-textures 2`（AnmFile 槽号，2 = ascii.anm 文字）在两侧同时剔除其绘制，其余操作就能逐条对位；
不能按链地址剔除，因为链回调里的出批会把上一层的残批记在它名下。注意被剔除的子系统若切相机（如 AsciiManager 的
0x408ef0 切到场地相机再切回 UI 相机），紧随其后出批的那条绘制在移植版会记到不同的视口，这类差异要等该子系统移植后才消失。

回归守护在 `tests/draw.test.mjs`：原版流经 `draw-compare.py <oracle.jsonl> - --structure-golden tests/golden/draw-st1.json --frames a-b`
折叠成每帧一条 sha1（口径 = `structure_lines`：目标 / 视口 / 图元 / 格式 / 纹理 / 状态子集，**不含顶点与矩阵**），测试跑 `game-draw` 后按同一口径复算；原版 jsonl 本身（数十 MB）不入库。
顶点级对拍仍用上面的 `--eps` 命令手工做。要让黄金只守一个管理器时加 `--only-textures 7`：摘要只含用该 AnmFile 槽贴图的绘制
（Clear 一并丢弃），槽号写进 golden 的 `texture_slots`，测试侧按同一过滤复算——`draw-st3-curve-laser.json` 等激光黄金就是这样
只守 bullet.anm（弹幕 + 激光），不受同场景的 Boss 畸变光环 / 特效条带 / Boss 血环影响。golden 的 `config.replay_slot`
（默认 −1）用于符卡练习槽直入。

`scene-boot` / `anm-draw` 也经 RecordingDevice 输出可读的 `quad` 行（屏幕四边形直接取顶点，世界四边形按 world·view·projection 投影并算线性顶点雾）。XYZRHW 顶点不经 TnL，原版从不关 TEXTURETRANSFORMFLAGS 而移植版对屏幕批显式关掉，draw-compare 对屏幕格式不比较该位。

### 整段绘制对拍

窗口对拍每次都要让原版从第 0 帧跑到窗口起点，约 30 帧/秒；整关、整局的覆盖改为一次回放比完整段。两侧在各自进程里把每一帧的操作流折成四条 sha1，只交换摘要（`tools/draw-structure.mjs` 的 `frameDigests`，与 `tools/oracle/draw-compare.py` 的 `frame_digests`、`tests/harness/DrawDigest.hpp` 逐字一致）：

- `s`：结构，即 `structure_lines`。
- `v`：位级顶点字与世界矩阵。
- `a` / `b`：量化顶点。浮点字按二进制档位取格宽 `1e-4 × 2^max(0, 指数位 − 127)`，格号 `floor((|v| + half·格宽) / 格宽)`；网格 a 的 half 为 0，网格 b 为 0.5。个别的 1 ulp 差在至少一张网格里同格，等价于窗口对拍的 `--eps 1e-4`；报告行的 bit-identical 给出位级一致的帧数，浮点次序对齐后的整段是逐帧位级一致的。

```bash
# 原版：--draw-ops - 把折叠后的逐帧操作流写上标准输出，交给 Node 折摘要（重弹幕帧上 Python 逐字折要慢一个量级）
toolchains/venv/bin/python ports/th16/tools/oracle/game-oracle.py games/th16/original/th16.exe artifacts/th16/dat 23000 \
    /tmp/st1.json --stage 1 --shot 0 --season 0 --difficulty 0 --lives 8 --refill-lives --input 0:0 --every 1000000 \
    --draw --draw-ops - 2> /tmp/st1.oracle.log | node ports/th16/tools/draw-digest.mjs - /tmp/st1.oracle.digest.jsonl
# 移植版：harness 内直接折（输出路径写成 digest:<dst>；dst 可为 fd:<n>）
tests/build/harness game-draw ../../games/th16/original/th16.dat 1 0 0 23000 "0:0" "" "" 0 -1 8 1 0 digest:/tmp/st1.port.digest.jsonl 1
# 逐帧比摘要：结构相同且顶点在网格 a 或 b 之一同格即通过；--golden-segments 把通过帧按网格折成段表
python3 tools/oracle/draw-compare.py /tmp/st1.oracle.digest.jsonl /tmp/st1.port.digest.jsonl --compare-digests --golden-segments /tmp/st1.segments.json
```

报告行给出位级一致帧数、同格帧数、结构不同的帧与越格的帧。整段跑出差异时，用上面的窗口对拍定位到具体操作：原版补跑一次 `--draw-out`，窗口取报告里的首个差异帧。`TH16_DIGEST_DUMP=<帧>` 让 digest 模式把该帧的结构行写到 stderr，用于核对三处实现。

定位到具体操作后，`TH16_VM_DUMP=<帧>`（或 `<起帧>-<止帧>`）让 harness 把这些帧进入 `AnmRenderer::draw_vm` 的每个 VM 的身份与状态写到 stderr：层、ANM 槽、脚本、精灵、位置、缩放、颜色、脚本时刻、存活帧数、父 VM，再对照 `harness anm-script` 的脚本反汇编查到指令。超过命令行单参数长度的通关脚本用 `@<路径>` 从文件读入，harness 的输入脚本参数与 `game-oracle.py --input` 同口径。原版在收到回标题 / 结局请求的那一帧并不切换场景（`switch_scene` 只记录请求），两侧在这一帧画的分别是游戏场景的重复帧与新场景首帧，所以两侧都不记录场景切换帧（`exit_frame`），绘制流止于它的前一帧。

回归测试 `tests/draw-long.test.mjs` 对 `tests/golden/draw-long.json` 中已重采验证的用例逐帧核对，另有两个用例检查 harness 与 Node 生成的摘要是否一致。重采用 `tools/oracle/verify-long-draw.py --case-regex <名称正则>`。验证器要求摘要从指定起点连续覆盖到目标末帧，原版提前切换场景时覆盖到切换前一帧。加 `--update` 可在所选用例全部通过后更新段表并标记为已验证。

## 页面壳

`web/sdl-runtime/th16-dev.html` 把 `assets/th16.dat` 整档 fetch 进 MEMFS `/assets/th16.dat`，C++ `Archive` 按名解密/LZSS 解码——与原版读档路径同构，不预解包。`python3 web/sdl-runtime/serve.py` 提供 no-store 服务。观测面：`mod._th16_frame()`、`mod._th16_phase()`、`mod._th16_audio_status()`。

## 音频

`web/cpp/sdl/AudioDevice.*`：miniaudio 无设备混音 + SDL3 音频流（44100 Hz 立体声 f32，队列保持 2048..3072 帧）。音效声部按 `kSoundDefinitions` 绑定 th16.dat 内 `se_*.wav`；乐曲串流器首选与 th16.dat 同目录的 `thbgm.dat`（按 `thbgm.fmt` 条目直接读 PCM、循环点同原版），否则取 `<同目录>/music/<曲名>.ogg`（PCM 帧数须与 `thbgm.fmt` 一致，否则报错不播）。浏览器不进 388 MiB 的 `thbgm.dat`：把 18 首 `assets/music/<曲名>.ogg`（不入库，例如 `ffmpeg -f s16le -ar 44100 -ac 2 -i <按 fmt 偏移截出的 PCM> -c:a libvorbis -q:a 4`）放到 `web/sdl-runtime/assets/music/`，页面壳并行取进 MEMFS `/assets/music`，缺则该曲静音。音频打不开时逻辑（命令队列 / 音效队列）照常推进，只是无输出。 浏览器构建里音效声部另经共享 `common/include/eagler/audio/WebSfx.hpp` 输出：装载时把 `se_*.wav` 整段解码上传为 WebAudio 独立声部（`web_sfx::upload`），play/stop/pan/volume/seek 与 `suspend` 同步下发，绕开混音队列的 2048 帧延迟；上传失败或非 Emscripten 构建时所有 `web_sfx` 命令返回 0，自动回落到 miniaudio 声部。`tools/ports/sfx-routing.test.mjs` 要求该头参与构建身份（`portable/build.mjs` 的 `allHeaders`）。

原版 BGM / SE 音量变量（DAT_004df49c / DAT_004df4a0）只在 Option 画面 0x44dc70 写入（开机到首次进 Option 为 0），端口开机即以配置值出声；0x470bb0 每次开曲重建 DirectSound 缓冲，端口在 open 时按配置音量重置。这些只影响宿主音量，不进任何对拍。

## 配置与输入

`th16.cfg`（0x43c050，`game/GameConfig.*`）：100 字节块，magic 0x160002，开机读 + 六字节上限校验（失败整块回默认）+ 总是回写，退出场景再写（0x459830）；路径 `<存档根>/th16.cfg`（浏览器 IDBFS `/save`，桌面同 `scoreth16.dat` 目录）。块内保存手柄按钮表 DAT_004a52e4（10×i16，exe 初值 `{0,1,2,5,-1,-1,-1,-1,-1,3}`）、BGM / SE 音量、Option 标志、输入配置等。

键盘键位固定（0x401d50：Z / X / Shift / Ctrl / C / 方向 / Esc / Q / S / P / Enter / D / R / F10），标题 Key Config（0x44e930）配置的是**手柄按钮号**：每帧取 0x401c30 快照中首个按下的按钮写入光标槽并对调冲突槽（0x44f710），Default 重载全局表，Quit 写回；0x4018e0 按表把按钮位图合成 Shot / Bomb / Slow / Pause / Release 位。宿主 `sdl/Application.cpp` 用 SDL_Gamepad 采样按钮位图（标准布局序 = 按钮号）交给 `GameSession::pad_buttons`，左摇杆按原版“中心 ± 1/4 量程”出方向位（POV 帽原版不读，端口把十字键一并映射）；物理键盘（SDL_Scancode）与启动器托管键（DOM `KeyboardEvent.code`，`ScancodeMap.hpp`）先折成 0x402130 的 DIK 快照，再由 `keyboard_input_bits` 按 DirectInput 分支合成输入位。harness `scene-boot` 以 `Pf:button` 注入手柄按钮、`config=<path>` 走开机读写；`pad-input-bits` / `config-file` 为单元入口。

## 启动器接入（eagler-touhou/1）

`web/sdl-runtime/th16.html` + `shell.mjs`（传输层 `eagler-host.mjs`、键盘桥 `directory-keyboard.mjs` 与 th20 同源）实现启动器协议：命令 configure / resources / keyboard / keyboard-clear / touch-controls / touch-cancel / direct-touch / launch / list / read / write / remove / sync，事件 ready / transfer / first-frame / runtime-info / frame-health / audio-health / exit / error，消息绑定 `runtimeEpoch`。数据：启动器经 `__eaglerPrepareManagedRuntimeDataV1` 交付 th16.dat → MEMFS `/game/th16.dat`；OGG 乐曲 `/bgm-ogg/<曲名>.ogg`（含 th128_08）；共享字体 `/msgothic.ttc`、`/unifont.otf`；存档根 `/savesth16/<jp|chs>`（IDBFS）。原生导出见 `web/cpp/sdl/BrowserRuntime.cpp`（`sdl_game_open` / `sdl_loop_*` / `sdl_key` / `sdl_touch_controls` / `application_save` / `sdl_game_status` / `sdl_audio_stats` …；`sdl_canvas` / `sdl_stats` 来自共享 `portable/sdl/Renderer.cpp`）。画面上的触点手势只接收不解释（th16 的移动是按位输入，不接受位置注入）；触屏按钮与虚拟摇杆经 `sdl_touch_controls` 合成输入位。

`node portable/package-eagler.mjs` 把 `web/artifacts/sdl3` 的 wasm / loader 与壳文件打成启动器运行时目录 `build-eagler/`（`manifest.json`、`resources.json`、`runtime-files.json`）。

## 代码质量与可读性标准

见 [`docs/CODE_QUALITY.md`](./CODE_QUALITY.md)：命名规范（领域语言、禁止反编译偏移名）、文件与模块结构、函数控制流拆分、注释三要素以及精度/对拍/双处登记门禁。
