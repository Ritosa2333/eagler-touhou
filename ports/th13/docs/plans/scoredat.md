# scoreth13.dat 原版格式与 TH13SAVE 镜像

状态：已实现。镜像层、整文件编解码、四角色 + 合计列映射和导入裁决均已接入，
`tests/score/` 覆盖原始容器、字段视图、角色×难度坐标与 FileStore 往返。

## 文件与镜像

- 文件 = 0x18 明文头（`TH31` + u32 总长 + u16 版本 4 + u16 0 + u32 0x100 +
  u32 compSize + u32 uncompSize）+ `xor_encrypt(LZ(镜像), 0xAC, 0x35, 0x10)`。
- 镜像 = 5 个 CR 记录列（霊夢、魔理沙、早苗、妖夢、合计；各 `0x56DC`）+
  ST 块（`0x42C`）。
- 每列包含 6 组难度榜表，每组 10 条；玩家数据界面使用前 5 组。
- 保存只收录 `u16 == 'CR'` 的列，缺席列整块不进文件，uncompSize 缩短
  `0x56DC`。装载逐块校验魔数、版本、长度、校验和与列号。

## 原版寻址与字段映射

原版全局 `score` 结构从列首前 8 字节开始；下表先给列内偏移。

| TH13SAVE / FileStore 字段 | 列内位置 | 原版写入/读取点 |
|---|---|---|
| `ordinary_records[c].ranking[d][r]` | `+0x10+(d*10+r)*0x1C` | 0x54050 名字录入；默认 0x449760 |
| 高分条目 +4 关卡、+5 续关 | 同上 `+4/+5` | 0x43d6b0；`DAT_004be7c8` 为续关数 |
| `play_count` / `total_frames` | `+0x7B8` / `+0x7BC` | 0x42c550 / 0x42b990 计数路径 |
| `clear_count[d]` | `+0x7C0+d*4` | 0x42cd40；对应全局 `score+0x7C8+…` |
| `no_continue_clear_count[d]` | `+0x7DC+d*4` | 0x42cd40；通关门 0x451450 全局 `+0x7E4` |
| 练习行分数/通过/游玩旗 | `+0x7F4+(d*8+stage)*8` 的 `+0/+4/+5` | 0x451cb0 全局 `+0x7FC`，0x42cd40/0x42b990 |
| 符卡名 | `+0x978+i*0x9C` 起 128 字节 | 0x413270/0x413a60 会话路径 |
| 符卡捕获数 | 同记录 `+0x80/+0x84` | 普通/练习两列 |
| 符卡尝试数 | 同记录 `+0x88/+0x8C` | 普通/练习两列 |
| 符卡练习最好成绩 | 同记录 `+0x98` | 0x42cd40 max 写 |
| `ending_seen_bytes[13]` | ST `+0x16..0x22` | 0x416130 读写 |
| 上次输入名 | ST `+0x0C`（9B） | 0x449a55 默认，END/名录入回写 |
| ST LCG 表 494×u16 | ST `+0x50` | 0x449a30；boot 种子 `(u16)timeGetTime()` |
| Extra 解禁 | ending 字节 `1+2*character` 的 bit4 | 0x416130 写，0x44bec0/0x44c570 读 |

列索引直接使用端口列：0 霊夢、1 魔理沙、2 早苗、3 妖夢、4 合计。榜表、
通关计数、练习行只写前四列；符卡名、捕获/尝试与最好成绩写满五列，合计列
由端口事件与个人列同点维护。

## FileStore 集成语义

- TH13SAVE v7 是应用侧权威：加载、内存和再保存都走 v7 原语义。
- `save_score()` 对应原版保存点：写 v7 → 同步得分面 → 构建 scoreth13.dat
  镜像。web 后端为 localStorage `th13.scoredat`（base64）；native/WASI 不
  激活该存储通道，镜像保留在内存供单测直测。
- dat 导入仅在应用档缺失/无效，或 dat 全条目最大 `__time64` 大于应用档榜表
  最大时间戳时发生。导入覆盖可映射字段，不改变设置类字段。
- 压缩按帧分片推进（`kScoreDatTokensPerFrame`）；镜像未变化时直接跳过压缩
  与存储写入。保存点只登记待写镜像，`th13.scoredat` 键在整幅编码完成后一次
  更新；v7 `th13.save` 同步落盘且权威，滞后镜像随后续帧自行追平，导入按
  时间戳仲裁不会被滞后镜像回灌。外部观测（E2E/工具）读该键前需等编码器
  排空——`e2e-fullclear.driver.mjs` 在保存点后按块泵帧至键值连续稳定。
- v1–v7 TH13SAVE 读档与写档布局保持不变。

## 验证

- `tests/score/score_dat_test.cpp`：黄金文件 parse/build 逐字节、缺席列、
  字段视图、LCG 与失败路径。黄金样本位于 ignored `artifacts/score_golden/`。
- `tests/score/filestore_score_test.cpp`：v7 权威、镜像往返、ST 名、坏 dat
  回退、整文件冒烟与原版妖梦样本导入。
- `tests/score/filestore_score_mapping_test.cpp`：4 角色 × 5 难度坐标、妖梦
  完整读写、符卡个人/合计列、Extra 解禁与旧 v7 重存。
- `tests/e2e/run.sh`：原版 `th13_01.rpy` 输入流全流程，导出 Replay 与原文件
  逐字节相同；保存点得分面进入同一 FileStore/ScoreDat 路径。
- 原版 `0x449e50/0x449be0` 直调生成与读回证据见
  [archive/th13-scoredat-youmu-20261123](../../../../archive/th13-scoredat-youmu-20261123/)；
  该记录覆盖四角色 + 合计列、妖梦字段和黄金镜像哈希。
