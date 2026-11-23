# th16 资源与格式

`th16.dat`：THA1 档案，215 个条目。`thdat -x16` 解包到 `artifacts/th16/dat/`（ignored）作为对照基准；移植版 `Archive` 对全部 215 条目逐字节一致（`tests/archive.test.mjs`）。

| 类型 | 数量 | 格式 | 对照工具 |
| --- | --- | --- | --- |
| `.anm` | 54 | ANM version 8（entry 头 0x40 字节，精灵 u32 id + 4×f32，脚本 {i32 id, u32 offset}，指令 u16 type / u16 length / i16 time / u16 param_mask，哨兵 type=0xffff length=0） | `thanm -l16` / `-x16` |
| `.ecl` | 22 | ECL v16（st01–st07、st0Nbs、default） | `thecl -d16` |
| `.msg` | 40 | MSG（stage 对话 ×4 机体、ending、staff） | `thmsg -d16` |
| `.std` | 7 | STD（st01–st07 场景） | `thstd -d16` |
| `.sht` | 9 | 机体弹幕表（pl00–pl03 各子季节） | — |
| `.wav` | 67 | PCM 音效 | — |
| `.png` | 9 | 外部贴图（entry hasdata=0 时由 `AnmResource::attach_external_texture` 另读） | — |
| `.rpy` | 4 | 演示录像 | — |
| `thbgm.fmt` / `th16_0100a.ver` / `musiccmt.txt` | 3 | 曲目循环表 / 版本 / 音乐室文本 | — |

## THTX 纹理

| format | 像素 | 解码 | 对照 |
| --- | --- | --- | --- |
| 1 | BGRA8888 | 原样 | thanm -x16 逐像素一致 |
| 3 | RGB565 | 5/6 位按 GPU 规则扩展（`v<<3 \| v>>2`），与 D3D9/GLES 采样 R5G6B5 一致 | 源 5/6/5 位一致（thanm 只复制最低位） |
| 5 | ARGB4444 | 4 位 ×17 | 逐像素一致 |
| 7 | GRAY8 | 灰度复制到 RGB，A=255 | 逐像素一致 |

`tools/oracle/thtx-compare.py` 覆盖 title/sig/ascii/text/front/bullet/effect/enemy/pl00/st01wl/e01（106 张纹理，四种格式）。

## 1280×960 资源

`sig.anm`（`loading/sig1280.png`，1024+256 两 entry 以 xOffset 拼接）、`ascii_1280.anm` 等以 `ins_431(1)` 标记 1280×960 坐标系，逻辑 640×480 画布需 ×0.5；原版按 `lowResScale` 与显示设置选择 `ascii.anm` / `ascii_960.anm` / `ascii_1280.anm`。

## 音乐

`thbgm.dat`（388876780 字节）为原始 PCM 拼接，`thbgm.fmt` 给出各曲起点/循环点；`musiccmt.txt` 为 cp932 曲目说明。
