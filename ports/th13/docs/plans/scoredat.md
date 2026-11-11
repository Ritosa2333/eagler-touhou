# scoreth13.dat 原版格式落移植（ScoreDat + FileStore 集成）

状态：阶段 A（镜像层）与阶段 B（整文件）均已完成并验证；ReplayFile::deflate
（bit-exact 0x458c30，35/35 Unicorn 向量）已接通。格式证据基线 =
外层 `oracle/scorefmt.py` docstring（全字段偏移 + 校验和 + LCG 递推 + 写入点
地址，[D] 反汇编直读 / [U] Unicorn 实证 / [G] Ghidra 旁证）；黄金样本
`artifacts/score_golden/`（Unicorn 原版代码产出，两次生成逐字节一致）。

## 文件与镜像

- 文件 = 0x18 明文头（`TH31` + u32 总长 + u16 版本4 + u16 0 + u32 0x100 +
  u32 compSize + u32 uncompSize）+ `xor_encrypt(LZ(deflate 镜像), 0xAC, 0x35, 0x10)`。
- 镜像 = 5×CR 块（Easy..Extra 槽 0..4，各 0x56DC）+ ST 块（0x42C）。
- 保存剔除怪癖：保存逐槽校验 `u16=='CR'`（0x449ec5），坏块整块不进文件、
  uncompSize 缩短 0x56DC（route_c 黄金实证）。模型以 `cr_present=false` 表达。
- parse 严格化（与 oracle/scorefmt.parse 同一口径）：头/魔数/版本/块长/校验和/
  槽号/未知魔数任一不符即失败；装载方按"坏档回退"消费，不静默跳过。

## 字段映射表（端口字段 ↔ 镜像偏移 ↔ 原版写入点）

角色映射（`FileStore::score_dat_chara`）：端口 0 霊夢→chara 0（ReimuA）、
1 魔理沙→chara 2（MarisaA）、2 早苗→chara 4（SanaeA）；端口 3 妖夢为端口
扩展角色，原版 6 chara 无对应——不写、不覆盖（v6 自留）。B 型行（1/3/5）
恒为 parse/默认，不臆造。

| 端口字段 | 镜像位置 | 原版写入点 | 说明 |
|---|---|---|---|
| `ordinary_records[c].ranking[d][r]` | CR 槽=d，+0x10+(chara*10+r)*0x1C | 0x43d6b0（默认 0x449760） | 条目 +0 分数 u32、+4 关卡 u8、+6..0xE 名字 9B、+0x10 __time64、+0x18 f32 达成率 |
| 同上 +5 通关旗 | +0x05 | 0x43d6b0 写 DAT_004be7c8 | 端口未跟踪 per-entry 通关旗，恒写 0（v6 +5 存 continues，不映射） |
| `clear_count[d]` | CR 槽=d，+0x7C0+chara*4 | 0x42cd40（0042cd40.c：`(diff)*0x56dc+0x7c0`，结构常量=块内+8） | 上限 99999 |
| `no_continue_clear_count[d]` | CR 槽=d，+0x7DC+chara*4 | 0x42cd40 | overlay 时反推 cleared 位（镜像 v6 写语义） |
| 游戏回数 `play_count` | （不映射） | 0x42b990 写块内 +0x7B8 | 原版按难度分块计数；v6 是全难度合计，写入每块=臆造 per-difficulty 计数。v6 自留、dat 保持 parse/默认 |
| `total_frames` | （不映射） | — | 原版无此字段，v6 自留 |
| `practice_played[c*4+d]` bit(stage-1) | CR 槽=d，+0x7F0+(chara*8+stage-1)*8 的 +5 | 0x42b990（0042b990.c:72） | 仅 diff 0..3；+4 通过旗（0x42cd40）端口未跟踪，不写 |
| 行最高分 row+0 | +0x7F0+(chara*8+stage)*8 | 0x43f8f0 max 写 | 端口未跟踪 per-stage 最高分：读透传、不写 |
| 练习最好成绩 | CR 槽=d，+0x978+i*0x9C 的 +0x98 | 0x42cd40 max 写 | 原版每难度每符单值；端口 5 列（4 角色+合计）取 max 合流写所有 present 块；读侧只回填合计列（per-char 列为端口扩展，不臆造归属）。多难度差异在端口无维度，取 max |
| 练习记录号/关卡 id | 同上 +0x90 / +0x94 | 0x449760 默认（0x4bbd30 表） | 默认值，不另行写 |
| 练习记录主体 +0x00..+0x90 | 同上 | 仅 +0x80/+0x84 有 [G] 读旁证 | 无文件偏移写入证据：透传不写（spell 名/尝试/捕获计数留在 v6） |
| `ending_seen_bytes[13]` | ST+0x16..0x22 | 0x416130（读写 score+0x1b26a..0x1b276；ST 块在结构 +0x1B254） | overlay 时 `ending_history_complete=true` |
| ST 上次输入名 | ST+0x0C（9B） | 静态 0x4ab984 / 0x449a55；名牌确认 0x440960 case 0x5a 写回 | 端口暂无名牌输入追踪：透传保真，不臆造 |
| ST LCG 表 494×u16 | ST+0x50 | 0x449a30；种子 = boot `(u16)timeGetTime()`（0x43a040）→ 端口 boot steady_clock 毫秒 u16 | 每次开机生成一次；文件 ST 块校验通过则整体覆盖回文件值（0x449b00→0x449cd0），即首存后随存档持久 |
| Extra 解禁 `extra_unlocked_mask` | （不映射） | — | 原版档内无实证槽位，v6 自留 |
| 音量/手柄/听曲位/legacy 档 | （不映射） | — | 设置类字段，全部留在 v6 "TH13SAVE" |

## FileStore 集成语义（2026-11-06 定死；Application 零改动）

- 通道与权威：v6 "TH13SAVE" 应用档有效 ⇒ v6 是应用侧唯一权威，加载/内存/
  再保存全走 v6 原语义。th13.scoredat 只是 `save_score()` 保存点写出的原版
  格式镜像：可映射字段照映射；continues 等 v6 独有字段在镜像中缺席是原版
  格式本性，不算丢数据。
- 后端：web = localStorage `th13.scoredat`（base64 存原版格式字节），唯一
  激活后端。native/WASI 构建通道**完全不激活**——不读、不写、无任何文件
  访问（`TH13_SCOREDAT_PATH`/cwd `scoreth13.dat` 均无语义；native 仅单测/
  诊断用，行为与无此通道版本逐字节一致。历史教训：按 cwd 找 dat 曾把仓库
  根的无主 scoreth13.dat 卷进装载并互相改写，污染 tests/ui 四套件）。
- dat 导入裁决（`score_dat_should_import`）仅两种情况：
  (a) v6 缺失/无效（`!loaded`）；(b) 应用档有效但 dat 明显更新——dat 全条目
  最大 u64 时间戳（`ScoreDat::max_entry_timestamp`，只数 present 块）> 应用档
  榜表最大时间戳（`FileStore::max_ranking_timestamp`）。相等不算更新；
  除此之外 dat 永不覆盖 v6，也不在装载期落盘。
- `save_score()`（0x42bfa0/0x4399e0 保存点）：写 v6 应用档（含 END1 提交）→
  sync 得分面进 ScoreDat → `build_file`（deflate→加密→头）→ 写通道（仅 web）。
  v6 写失败即失败；web 通道镜像写失败如实 error+false。native 上 save_score
  语义 = 仅写 v6，与旧版逐字节一致。
- `save()`（设置类保存点）只写 v6，不写通道。
- `initialize()`：boot 生成 ST 种子+默认档 → 读 v6 → （仅 web）读通道：
  parse 在局部副本上进行，通过裁决才采纳（`score_dat_loaded()=true`，overlay
  覆盖得分面字段，设置类字段不动）；未采纳/失败/缺档一律保持 v6/默认，
  不写盘（镜像只由 `save_score()` 落）。
- v6 兼容：格式（v1–v6）、读写路径、错误语义完全不变；旧用户无感升级。

## 测试与资产依赖

- `tests/score/run.sh`（仿 tests/replay）：`score_dat_test.cpp`（黄金镜像/整文件
  逐字节、defaults 冻结种子、剔除怪癖、字段视图、LCG 向量、失败路径）、
  `filestore_score_test.cpp`（native 直测映射层与裁决：v6→镜像 sync、镜像往返、
  黄金 overlay、v6 权威/坏档不覆盖、save_score 冒烟 + 规则 (a)/(b)）。
- 资产（gitignored，容器内须在位，缺位时相应用例如实失败）：
  `artifacts/score_golden/*.bin`（黄金档）与 `*.image.bin`（黄金未压缩镜像，
  由外层 `oracle/scorefmt.py` image_bytes 解出冻结）。
- 关键验证输出：`build_file(parse(黄金))` 与 5 个黄金 .bin 整文件逐字节相等
  （含 route_c 的缩短 uncompSize 90012=0x15F9C）；`defaults(4660)` 镜像与
  route_a 黄金镜像全等；`parse(route_c)` 槽 1 缺席、其余块逐字节不变。
