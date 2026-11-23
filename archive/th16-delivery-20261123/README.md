# TH16 构建、性能与逐帧验收

二十九段整段对拍共 **1,594,703 帧逐帧位级一致**；最终隔离测试 **855 / 855 通过**；normal Runtime 从启动器入口独立构建并完成发布打包；WASI 3600 帧逻辑加完整绘制三次实测为 **8.230–8.451 秒**，通过 10 秒门禁。

## 构建身份

| 字段 | 记录 |
| --- | --- |
| 提交 | `47c3f30ca9889119365f86b88bc69241ae7fd51a`（Runtime、WASI 与性能测量；后续黄金和验收文档不改 C++ 源码） |
| 源码 | `ports/th16` |
| 构建 | `normal`；Emscripten `-O2`、C++17、`-ffp-contract=off`；WebAssembly 哈希见下表 |
| 公共库 | 同一提交的 `common/`；构建计划中的 common 路径与输入清单见 `build-plan.json`、`sdl3-build.json` |
| 环境 | rootless Podman 临时容器 `localhost/th16-dev`（`--rm`），Debian bookworm；Node 24.22.3、emsdk 6.0.9、wasi-sdk 34 / clang 23.1.0；Linux 7.3.5-2-cachyos x86_64，AMD Ryzen 9 9955HX3D |
| 网络 | 仅容器内安装固定版本 Bun 1.4.4 与 `launcher/bun.lock` 依赖；游戏对照不使用网络 |
| 附件 | `build-and-package.log`、`build-plan.json`、`source-builds-th16.json`、`build-outputs.txt`、`runtime-files.json`、`wasi-perf.json` |

## 检查结果

| 检查编号 | 操作 | 结果 | 附件 |
| --- | --- | --- | --- |
| BUILD-02 | 在提交的浅克隆中运行 `node tools/workspace.mjs list` 与 `node launcher/scripts/build-runtime.mjs --game=th16 --variant=normal --emsdk=/opt/emsdk --dry-run` | 计划正确指向 `ports/th16`、`common/` 与 emsdk 6.0.9，退出码 0 | `build-plan.json`、`environment.txt` |
| BUILD-03 | 去掉 `--dry-run`，从启动器入口构建 normal；另运行 `ports/th16/portable/build-harness.mjs` | Web Runtime 2,827,674 字节，SHA-256 `3f9502c4d00588ecb2bda3bae43bbb888c77164f10467ba9be247f3f05072367`；WASI harness 3,935,657 字节，SHA-256 `c3dfb4c452a7d2202532b68e8d67dafdef6f4034dfe00459adbbe4bf899682f8` | `build-outputs.txt`、`source-builds-th16.json`、`sdl3-build.json`、`wasi-build.json` |
| BUILD-04 | 运行 `node ports/th16/portable/package-eagler.mjs`，逐文件核对名称、字节数与 SHA-256 | 发布目录恰好 9 个文件；清单中的 WASM 与加载器哈希指向本次构建 | `runtime-files.txt`、`runtime-files.json`、`runtime-manifest.json` |
| VERIFY-01 | 运行 `demo-replay.test.mjs`，四份内置 Demo 各自回放到 `0xf3c` 回标题请求 | 四份 Demo 的场景退出、退出帧及玩家、敌机、敌弹、道具、激光、Bomb、菜单与声音摘要全部一致 | `original-hashes.txt`、`native-test-files.log` |
| VERIFY-02 | 对第 1–6 面、Extra、七段通关和整局结局运行长程集合，覆盖 Lunatic、Extra、四机体与多个子季节 | 二十九段共 1,594,703 帧全部逐帧位级一致；各段帧数与段表身份固定 | `golden-summary.json`、`oracle-compare.log` |
| VERIFY-03 | 对原版 EXE、DAT 与四份内置 Demo 重新计算 SHA-256 | EXE `c1177601…1929`、DAT `c84e0418…64f` 与开发指南登记一致；四份 Demo 哈希完整归档 | `original-hashes.txt` |
| VERIFY-04 | 在隔离副本以普通 candidate harness 跑整套黄金，核对 `draw-long.json` | 黄金 SHA-256 `b1ad188ead4176c693d62b22b2c3e285b5f0f4562940cb38af2fdf4dbaf078fa`；29 个整段用例与两个摘要自检全部通过 | `golden-summary.json`、`native-test-files.log` |
| VERIFY-05 | 从 Unicorn 原版回放重新采集逐帧绘制摘要，经 `draw-compare.py --compare-digests` 折成黄金段表 | 二十九段最终复采均为 `structure-differs=0`、`vertices-differ=0`、`only-one-side=0`，全部帧位级一致 | `oracle-compare.log`、`golden-summary.json` |
| VERIFY-08 | 核对游戏场景退出、换关、结局与整段样本的结束条件 | 通关段进入下一面或结局；场景切换帧在两侧统一不记录，段表连续覆盖到退出前帧 | `golden-summary.json`、`oracle-compare.log`、`../../ports/th16/docs/DEVELOPMENT.md` |

## WASI 3600 帧性能

输入为第 1 面 Easy、灵梦春、持续射击，残机 8 并补满。每次测量包含 Node/WASI 启动、DAT 装载与 3600 帧执行；逻辑加绘制路径同时生成每帧绘制摘要。三条逻辑运行的滚动摘要完全相同。

| 路径 | 第 1 次 | 第 2 次 | 第 3 次 | 门禁 |
| --- | ---: | ---: | ---: | ---: |
| 逻辑 | 0.363 s | 0.348 s | 0.334 s | < 10 s，通过 |
| 逻辑 + 完整绘制 | 8.230 s | 8.451 s | 8.239 s | < 10 s，通过 |

Snapdragon 865 级设备的 60fps 验收保持待验证，测量目标与当前性能记录分开登记。
