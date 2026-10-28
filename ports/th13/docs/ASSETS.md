# th13 资源格式清单

来源：th13.dat（SHA256 `3443fe0d…089`，v1.00c），thtk 提取。原始资源与 dump 保留在开发工作区 `artifacts/reference/`，不入库。

## 总量

202 个文件：59 wav、55 anm、41 msg、21 ecl、8 png、7 std、4 sht、4 rpy（dat 内演示录像 demo0–demo2 等）、ver、fmt、txt。

## 格式版本（thtk 实测）

| 类型 | 版本 | 说明 |
|---|---|---|
| DAT | 13 | thdat `-x 13` 解包正常 |
| ECL | 13 | `thecl -d13 -j` 反汇编干净；未映射指令显示为 `ins_NNN`（可后续挂 eclmap 提升可读性，不影响行为还原） |
| ANM | 8（entry version 字段） | `thanm -l13` 清单正常；含 lowResScale 等 th13 代字段 |
| STD | 13 | `thstd -d13` 正常 |
| MSG | 13 | `thmsg -d13` 可导出；已知警告「bytes left over」为 th13 对话格式扩展字段，非致命 |

## 脚本结构

- ECL：`default.ecl` + `st01`–`st07`（Extra=st07），每关另有 `bs`（Boss）与 `mbs` 变体脚本，共 21 个。
- ECL 头部声明依赖的 ANM（`anim`）与包含的 ECL（`ecli`），与运行时装载顺序一致。
- ANM 55 个：effect/enemy/player/stage 等；贴图 format 5（ARGB4444 系）为主。
- SHT 4 个：自机射击配置。
- dat 内 rpy：原版演示录像，可直接作 oracle 驱动样本（另有用户提供的 `th13_udSr01.rpy`）。

## 音乐

当前开发资源为 OGG 补丁版：`thbgmogg.dat`（30.5MB）提供单个连续 Vorbis 流。生产 `AudioDevice` 读取 `/assets/thbgm.ogg` 和原版 `/assets/thbgm.fmt`，按 fmt 的虚拟 PCM 偏移与循环点定位曲目；不是逐曲 OGG 文件。补丁 DLL 不进入网页运行时。Launcher 的内容准备层须保持该文件和偏移契约，原版音乐数据不进入源码仓库。
