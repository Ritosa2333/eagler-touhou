# TH20 资源与脚本恢复实测报告

本次从用户指定的 `th20.dat` 恢复出 **283 个唯一路径的原始文件**。其中 **191 个唯一脚本档案全部完成“原始二进制 → 可编辑文本（ANM 另含原压缩纹理）→ 重新编译”验证，SHA-256 与原文件完全一致**。没有使用原文件字节回填、容器替换或启动原 EXE 来完成这一验证。

这些是游戏的数据及脚本语言，不是原游戏 C++ 引擎的完整源代码。脚本的无损恢复不能单独证明玩法、碰撞、随机数、渲染、音频时序或输入响应已经实现 1:1。

## 结果

| 文件类型 | 唯一文件 | 文本源码回编译逐字节一致 |
|---|---:|---:|
| ECL 敌人、关卡与弹幕脚本 | 21 | 21 |
| ANM 动画、精灵与纹理档案 | 73 | 73 |
| MSG 对话、结局与制作人员脚本 | 87 | 87 |
| STD 背景场景与摄像机脚本 | 10 | 10 |
| 合计 | **191** | **191** |

其余原始资源：71 WAV、10 PNG、4 RPY、3 TXT、2 SHT、1 FMT、1 VER。原始 ECL 解码得到 730 个子程序、23,760 条指令，与项目另一条独立 C++ 二进制解析流水线统计一致。

ANM 导出 **660 个独立纹理文件**，按文件实际内容识别为 **482 PNG、178 JPEG**，均经 Pillow 完整像素解码成功。原档案里部分 JPEG 使用 `.png` 文件名；纹理清单记录真实格式。使用 `-uu` 导出每个原始纹理，不按 x/y 偏移做图像拼接或重新压缩。原始偏移仍保存在 ANM 文本中，由引擎按其含义解释。

最后一次恢复运行有 482 条工具调用，失败 0、警告 0。完整命令参数、退出码、标准错误和哈希见 `assets_recovery.json` 与 `assets_logs/`。

## 原归档和重名项

归档目录包含 **285 条记录**，其中两个名称各出现两次，因此导出到普通目录为 283 个唯一路径。已用直接调用 thtk 索引 API 的小型 C 工具，对四条重名记录逐个独立提取并比较；每一组的内容完全一致，未把覆盖后的文件哈希冒充独立验证结果。

| 名称 | 从 0 开始的归档索引 | 原归档字节偏移 | 解压后字节数 |
|---|---:|---:|---:|
| aura.anm | 43 | 65,123,118 | 25,097 |
| aura.anm | 44 | 65,138,708 | 25,097 |
| se_cardget.wav | 232 | 146,269,778 | 222,864 |
| se_cardget.wav | 240 | 146,868,862 | 222,864 |

- `aura.anm` 两份 SHA-256：`b385c5a58820c3accfba333b5e4547ff1384b8cf5dca7a7b2f856042764a5018`
- `se_cardget.wav` 两份 SHA-256：`825b2e9657219a3800add15dde3346ba95455c10e55fa7953b5c547816ac96ab`
- 四份独立提取结果保存在 `assets/duplicate_entries/`。清单每条记录包含 index、archive_offset 和独立哈希。
- `th20.dat` 提取前后 SHA-256 均为 `9db8d7c43fbacec95614163d4c3e1d254e82169f8550177ee849831130590494`。

## 工具修复及证据

以 [thpatch/thtk](https://github.com/thpatch/thtk/tree/892114a0fcaa0bbdaaecf3cb4ad56f758683fb40) 源码提交 `892114a0fcaa0bbdaaecf3cb4ad56f758683fb40` 为基础，在本项目局部应用 `tools/asset_thtk_th20.patch`：

1. ECL simple 模式中的负立即数：上游把 `-9999994` 等值编译成压栈加求负指令，改变了原始程序。修复仅将 simple 模式下的负字面量保留为立即数；无变量表达式折叠。另使用 `-j` 在 Shift-JIS 与 UTF-8 间转换，避免日本文字双字节尾 `0x5C` 被当作反斜杠转义。最终全部 21 个 ECL 一致。
2. TH20 普通 MSG 的 opcode 19 实际为零参数；上游沿用 TH16 的一个参数定义，导致 64 个普通对话回编译失败。补充 TH20 专用格式覆盖后，普通对话与 23 个结局/制作人员脚本共 87 个全部一致。
3. TH20 STD 对象头实际为 48 字节；上游按旧 28 字节前进导致读入越界。新增五个尚未确认含义的 32 位字段并显式导出。补充实测 opcode 20 单浮点参数和 opcode 22 单整数参数；保存原对象 ID、原绘制实例顺序、Std_unknown 字段，并以 9 位有效数字输出 binary32。最终 10 个 STD 全部一致。未知字段名称和 opcode 语义没有凭名称推定。

独立补丁已对从固定提交重新取出的干净文件验证可应用；构建脚本已在现有已打补丁目录再次运行成功。`assets_toolchain.json` 记录原始源码哈希、修改后源码哈希、补丁哈希、子模块提交和六个工具可执行文件哈希。

## 产物位置

- `assets/raw/`：283 个原始唯一文件。
- `assets/duplicate_entries/`：按索引提取的四条重名记录。
- `assets/textures/<anm 名>/`：660 个原压缩纹理。
- `scripts/recovered/ecl/`：可读 ECL，包含表达式还原。
- `scripts/recovered/ecl_raw/`：用于精确回编译的 UTF-8 ECL；没有表达式和参数检测变换。
- `scripts/recovered/anm/`、`msg/`、`std/`：可编辑脚本文本。MSG 的 `.txt` 保留原 Shift-JIS 字节供编译，另有 `.utf8.txt` 供阅读。
- `scripts/recovered/roundtrip/{ecl,anm,msg,std}/`：191 个从文本重新生成的二进制文件。
- `reports/assets_recovery.json`：全部原档案记录、哈希、191 项验证和命令日志索引。
- `reports/assets_texture_manifest.json`：纹理路径、真实格式、尺寸、模式和哈希。
- `reports/assets_toolchain.json`：工具来源、补丁及构建证据。

## 复现命令

需要已安装 Visual Studio 2019 C/C++ 工具链、Windows SDK、Git、CMake 和 Python 3.12。构建脚本将取得固定的 thtk 源码、子模块和 win-flex-bison 2.5.25，仅写本项目。默认 CMake 路径为 `D:\cmake\bin\cmake.exe`，可通过 `-CMake` 参数更改。

```powershell
Set-Location 'D:\AIWorkspace\Touhou20Reconstruction'
& '.\tools\asset_build_thtk.ps1'
python '.\tools\asset_recover.py' --game-dir 'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders'
```

构建脚本幂等应用补丁，并构建 thdat、thecl、thanm、thmsg、thstd 和按索引提取适配器。`--game-dir` 为必填参数。只重跑脚本/纹理反编译及回编译时，可以加 `--skip-extract`；此时依然检查原归档目录、导出文件长度、重名记录和归档哈希。

单文件等价命令示例（工具完整路径参见 JSON）：

```text
thecl -jrd20 original.ecl recovered.ecl.txt
thecl -jsc20 recovered.ecl.txt rebuilt.ecl
thanm -uul20 original.anm > recovered.anm.txt
thanm -uux20 original.anm
thanm -uuc20 rebuilt.anm recovered.anm.txt
thmsg -d20 original.msg recovered.msg.txt
thmsg -c20 recovered.msg.txt rebuilt.msg
thstd -d20 original.std recovered.std.txt
thstd -c20 recovered.std.txt rebuilt.std
```

ANM 编译须在对应纹理导出目录运行。结局与制作人员 MSG 使用 `-ed20` / `-ec20`。

剩余工作属于游戏引擎实现与运行时语义验证：191 个脚本档案的局部恢复任务已完成，不能将这一结论扩大为完整 C++ 重制完成。SHT、RPY、VER 保留原始字节，本流水线不声称已解释其运行逻辑。BGM 波形与循环点由项目另一条流水线处理，见 `bgm_recovery.json`。
