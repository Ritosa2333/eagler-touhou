# 東方天空璋 TH16

Hidden Star in Four Seasons v1.00a 的网页移植：C++17 / SDL3 / GLES 3.0 / Emscripten。还原方式是**行为还原**——以 Ghidra 对 `th16.exe` 的反编译导出为唯一依据，目标是“稳定输入 → 原版输出 = 还原输出”（行为与画面）。

- 开发入口：[docs/DEVELOPMENT.md](docs/DEVELOPMENT.md)
- 逐项适配检查：[ADAPTATION.md](ADAPTATION.md)
- 反编译分区与地址锚点：[docs/DECOMP.md](docs/DECOMP.md) · [web/cpp/rebuild/AddressMap.hpp](web/cpp/rebuild/AddressMap.hpp)
- 资源与格式：[docs/ASSETS.md](docs/ASSETS.md)
- 路线图与里程碑：[docs/plans/roadmap.md](docs/plans/roadmap.md)
- 代理约束：[AGENTS.md](AGENTS.md)

## 目录

| 路径 | 内容 |
| --- | --- |
| `web/cpp/game/` | 游戏内核：RNG、THA1 档案、LZSS、ANM v8 解析、THTX 解码、ANM VM（`AnmVm.cpp`）与管理器（`AnmManager.cpp`）；每个函数头注明原版地址 |
| `web/cpp/rebuild/AddressMap.hpp` | 原版地址 ↔ C++ 还原件登记表（已还原 / 已定位 / 未定位） |
| `web/cpp/sdl/` | 设备层：`GraphicsDevice`（纹理句柄 → 共享 GLES 后端）、`Application`（场景驱动）、`BrowserRuntime`（Emscripten 导出） |
| `web/cpp/platform/` | 浏览器导出（RNG oracle 直调） |
| `portable/sdl/` | 与 th13/th15 共享的语义 GLES3 渲染后端（`Renderer.cpp`），不得分叉 |
| `portable/build.mjs` / `build-harness.mjs` | 网页运行时（emcc）与 WASI 无头 harness 构建 |
| `web/sdl-runtime/th16-dev.html` | 开发页壳：`assets/th16.dat` 整档进 MEMFS → `th16_initialize` → rAF 主循环 |
| `web/sdl-runtime/th16.html` + `shell.mjs` | 启动器运行时页（eagler-touhou/1 协议；`portable/package-eagler.mjs` 打包为 `build-eagler/`） |
| `web/scripts/cpp/analyze.mjs` + `decomp/ExportTh16.java` | Ghidra headless 全函数导出 |
| `container/` | 开发环境镜像（`Dockerfile`）与 `podman.sh`：本机不装依赖，构建、oracle 与测试都在 `podman run --rm` 临时容器里进行 |
| `tests/` | `run.sh`：本机 harness + `node --test`（RNG golden、THA1/ANM 对 thtk 逐字节/逐像素对照、ANM VM 1887 脚本对 Unicorn 直跑原版逐字节对照、绘制窗口与整段对拍） |
| `tools/oracle/` | Unicorn 整局 oracle（`game-oracle.py`）、绘制调用流对拍（`draw-compare.py`）、RNG / ANM VM / 标题 / 音乐 / 音效 / 成绩各 oracle、THTX 像素对照脚本 |
| `tools/draw-structure.mjs` + `draw-digest.mjs` | 绘制流的结构行与整段摘要口径，及把逐帧操作流折成摘要的命令行入口 |
| `tools/thtk-dumps.sh` | 用 thtk 生成对照基准（`artifacts/th16/dat` 解包与 `thanm -l16` 列表） |

## 运行

```bash
# 构建网页运行时（EMSDK 指向 emsdk 根）
EMSDK=../../toolchains/emsdk node portable/build.mjs
# 原版 th16.dat 放入页面壳资源目录（不入库）；乐曲为 assets/music/<曲名>.ogg（见 docs/DEVELOPMENT.md「音频」）
ln -s /path/to/th16.dat web/sdl-runtime/assets/th16.dat
python3 web/sdl-runtime/serve.py 8446   # http://127.0.0.1:8446/sdl-runtime/th16-dev.html
```

## 测试

```bash
bash tests/run.sh          # 本机 clang++ 构建 harness → node --test tests/*.test.mjs
```

原版文件（`th16.exe`、`th16.dat`、`thbgm.dat`）与反编译导出只放外层 ignored 的 `games/`、`artifacts/`，不入库。
