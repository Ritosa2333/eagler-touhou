# th19 原版对拍交接

从 Draft PR #214 拉取代码，在独立工作树继续。先复现第 125 帧的差异，再按下面的流程推进。采集源码、配置和第 125–126 帧对照见 [tools/parity](../tools/parity/)。

## 最终目标

用真实运行的 Windows 原版 th19 当参照，让新版 Web 在相同对局设置、种子、输入及模拟帧下复现原版行为和画面。目标是覆盖范围内的完整对局状态逐帧一致，画面在相同渲染条件下逐像素一致。先从 VS → CPU 模式入手，自动批跑所有机体和所有卡各一遍，无需排列组合。用现有回滚与录像路径做回归，画面同时检查菜单、轮次演出、实战和结算。

## 用户已授权的工作

- 可以直接反汇编、反编译和调试本地原版 th19.exe，可以读进程内存、跟踪调用，并在隔离副本上加观测钩子。使用新电脑本地安装的原版游戏，按下方 SHA-256 验证版本。原版文件和个人存档留在本机，实验写入测试副本和独立 AppData。
- 用户明确要求把此前手动填写的偏移都反编译核查。包括采集器的地址、结构字段和调用点，以及 Web 为了贴近原版手调的坐标、布局、动画时点和判定常量。沿原版调用链追到读写点、资产或脚本表，核对数值、单位、坐标系和更新阶段，并用实际运行采样交叉验证。每条记录对应的 exe 哈希、地址或模块 RVA、函数或脚本、原版证据和 Web 使用点。仍待验证的条目标清楚。
- 可在独立工作树修改新版并运行构建、测试、批量采集和对拍。先复用自己社区已有的 replay verifier、rollback、原版采集器和 Ju.N.Owen 驱动。优先走脚本、命名管道、进程观测和浏览器自动化，Computer Use 留给脚本暂时进不了的界面。GitHub 的标题、正文和评论先给项目维护者审阅。

## 工作流程

1. **先复现基线。** 校验原版版本，启动原版 VS CPU，记录左右机体、卡、难度、两路初始 RNG、规则、输入与原版采集版本。先用下文两组无卡 Lunatic 对局，确认原版重复跑出的开局状态一致，再让 Web 用相同设置和种子跑。用轮次段和逻辑帧对齐，先查第 113 帧命中和第 125–126 帧差异。
2. **用游戏状态验证。** Web 侧优先复用现有回滚快照的取数能力。原版侧按同一语义采集。参考仓库内的 Replay Verifier 和 th19 回滚测试，复用采集契约、逐帧比较、轨迹拼接和报告。参见 [Replay Verifier](../../../common/testkit/replay-verifier/README.md) 与 [回滚测试](../tests/rollback-session.test.mjs)。先比较双方位置、输入、生命、计时、两路 RNG、敌机、弹丸、命中和脚本进度，逐步补齐卡片、计分、轮次及结算。对象按稳定的生成顺序或可验证的对应关系匹配。缺帧、字段缺失和卡片未触发要单独标记。文件能读、能启动和总帧数相同只算流程检查。
3. **定位首个分叉。** 按轮次和模拟帧找第一个不同字段。对分叉前后几帧加窄采样，跟踪原版调用顺序、状态读写、RNG 取数和 ANM/ECL 脚本，必要时反编译到对应函数。保留原版与 Web 的同帧值、调用证据和截图。常量优先追到原版逻辑或资产，确认差异的原因后再改 Web。
4. **修完立即回归。** 改动集中在导致分叉的执行阶段。先跑原用例、交换双方的对局和相关旧录像，再跑回滚重演及现有构建和测试。比较修复前后的首个分叉和通过区间。发现下一处差异就记录并继续定位。碰撞时序试验与已验证提交分别保存。
5. **并行验证画面。** 在对应逻辑帧抓原版 D3D9 输出和 Web canvas。对齐分辨率、裁切、缩放和呈现阶段，再输出原版、Web 和绝对 RGB 差分，记录变动像素比例与范围。旧截图降采样可用于诊断，最终逐像素比较需同分辨率。画面按 HUD、层级、坐标、字体、混合、菜单和转场查。状态一致、画面仍有差异时沿渲染路径追查。
6. **批跑并及时交接。** 机体扫描固定难度、种子、无卡规则和基准对手，每场只换一台机体。卡片扫描固定双方机体及其他规则，每场只换一张卡，确认卡进入对局且已触发对应行为。遇到单侧特有分歧时再换边复跑。先扫开局，遇到分叉就存证并继续下一例。开局已对齐、要跑到后段时再尝试变速，并先核对变速前后相同逻辑帧。让采集工具后台跑，同时整理已有证据或检查下一处调用，持续维护覆盖清单供接手。

每例至少留下用例设置、原版和 Web 版本、已比较帧范围、首个状态差异、对应截图、重跑命令和当前状态。全程完成的判据是每台机体、每张卡都已覆盖且相关行为已触发，验证区间推进到完整对局，状态与画面均有同帧证据。阶段性成果按实际对齐区间汇报。

## 恢复代码

在本仓库根目录取回 PR 分支并创建独立工作树。

~~~powershell
git fetch origin refs/pull/214/head
git worktree add -b work/th19-parity-followup ../th19-parity-followup FETCH_HEAD
Set-Location ../th19-parity-followup/ports/th19
node --test tests/*.test.mjs
node scripts/build-runtime.mjs
~~~

构建前把 TH19_EMSDK 指向本机 Emscripten SDK。基线提交为 c5946410e。提交覆盖 VS 时钟与轮次演出、原版共享 RNG 顺序、ECL 波次与异步执行、敌人初始停顿、开局射击门槛和灵梦追踪符。已通过 Web 构建和 53 项 Node 测试。浏览器诊断版在回滚重演中复现相同状态与画面。开局对拍的两场 Lunatic CPU 对局交换了灵梦与魔理沙的位置，均为两命、无卡、600 秒。两路 RNG 帧 0–124 逐帧一致。第 113 帧首发追踪符命中一致。浏览器通过诊断入口启动同种子对局。全局游戏状态、完整对局和逐像素一致仍待验证。

## 采集真正的原版

在 Windows 上使用本地安装的原版 th19。已验证的原版可执行文件 SHA-256 为 `804107d9033601f69c251a5e6e449bd88d9add36591129db394765295893bac6`。

基线对局为 Lunatic 灵梦对魔理沙和魔理沙对灵梦，均为 VS CPU、两命、无卡、600 秒。两路第 0 帧 RNG 为 3058262 和 424182232。先确认原版进程实际选中了对应模式、难度和机体。

原版采集用独立 AppData 和隔离副本，保留原版游戏逻辑。此前的 CPU 驱动基于 Ju.N.Owen 的 `534f26466c10a5ebab1fac222b1a410b6b053424`，通过命名管道提交对局配置，在对局初始化时设置第二路 RNG。可从现有社区驱动和仓库的 Windows 进程采集器接入。原版画面采样点为 D3D9 Present 前。采集 RNG 调用与第 122–130 帧碰撞数据时，给状态、调用和截图都附上轮次段和模拟帧号。参见 [th19 录像说明](REPLAY.md) 与 [Windows 进程采集器](../../../common/testkit/replay-verifier/windows-process-reader.py)。

从源码仓库根目录取工具和对局配置。用 Windows x86 Native Tools 命令行编译 D3D9 观测器。工作目录与原版资源需要在同一 NTFS 卷。Ju.N.Owen 补丁沿用上游 GPL-3.0，随附 [许可](../tools/parity/LICENSE.JuNOwen)。

~~~powershell
$tools = (Resolve-Path ports/th19/tools/parity).Path
$work = New-Item -ItemType Directory -Force "$env:TH19_PARITY_WORK"
git clone https://github.com/progre/junowen.git "$work/junowen"
git -C "$work/junowen" checkout --detach 534f26466c10a5ebab1fac222b1a410b6b053424
git -C "$work/junowen" apply "$tools/junowen-cpu.patch"
Push-Location "$work/junowen"
cargo build --release --locked -p th19replayplayer --target i686-pc-windows-msvc
Pop-Location
Push-Location $work
cl /nologo /O2 /EHsc /LD "$tools/observe-d3d9.cpp" /link /OUT:d3d9.dll
Pop-Location

$driver = "$work/junowen/target/i686-pc-windows-msvc/release/th19replayplayer_hook.dll"
$hash = (Get-FileHash $driver -Algorithm SHA256).Hash.ToLower()
foreach ($match in @('reimu-marisa', 'marisa-reimu')) {
    python "$tools/capture-native.py" --original "$env:TH19_ORIGINAL" `
        --observer "$work/d3d9.dll" --output-dir "$work/native" `
        --cpu-fixture "$tools/samples/cpu-lunatic-$match-no-cards.bin" `
        --cpu-driver $driver --cpu-driver-sha256 $hash --cpu-second-rng 424182232 --rng-calls --timeout 60
}
~~~

先设置 TH19_ORIGINAL 指向本机原版安装目录，TH19_PARITY_WORK 指向同卷的实验目录。两场顺序运行，避免命名管道冲突。每次运行打印采集目录。也可以直接用仓库提供的样本验证 Web 工具。

## Web 对拍与待验证试验

PR 分支是已知差异的基线。以下命令在源码仓库根目录运行。诊断补丁只加浏览器步进和观测接口，在独立实验树应用。先把本机 th19.dat 和系统 MS Gothic 字体分别放到实验树的 ports/th19/content/th19.dat 和 ports/th19/content/msgothic.ttf。设置 TH19_EMSDK 与 TH19_CHROME 指向本机对应程序。

~~~powershell
$tools = (Resolve-Path ports/th19/tools/parity).Path
git worktree add --detach ../th19-parity-diagnostics HEAD
Push-Location ../th19-parity-diagnostics
git apply "$tools/web-observer.patch"
Set-Location ports/th19
npm install --no-save --package-lock=false puppeteer-core@25.10.0
node scripts/build-runtime.mjs
node tools/parity/check-web.mjs tools/parity/samples/reimu-marisa
node tools/parity/check-web.mjs tools/parity/samples/marisa-reimu --reverse
python -m pip install numpy pillow
python tools/parity/compare-frames.py tools/parity/samples/reimu-marisa tools/parity/samples/reimu-marisa "$env:TH19_PARITY_WORK/pixel-diff"
Pop-Location
~~~

浏览器脚本自行启动回环服务，在正常加载后用原版种子启动对局并固定步进。用新采集的原版目录替换样本目录即可复跑。脚本打印报告及 Web 截图目录。轨迹样本保留两组开局帧 0–130、第一组第 122–130 帧调用和第 125–126 帧画面。已移除本机路径和进程指针。采集器和对局驱动从源码构建。

早期浏览器脚本的断言只覆盖到第 120 帧，后续报告将 RNG 连续比较推进到了第 124 帧。验证试验时把连续比较延伸过第 126 帧，并记录两组对局各自下一次分歧。对画面用相同模拟帧比较。原版 960×720，现有 Web 截图先从 1280×960 降采样，再输出差分。最终逐像素验收应使用同分辨率和可控渲染设置。

## 首个分歧和接手顺序

- 基线在 Web 第 125 帧先命中，原版第 126 帧才命中。第 125 帧原版符弹中心约 (-98.34, 262.70)，与 Web (-98.33, 262.69) 接近。Web 对比移动前敌人中心 (-117.81, 253.76)，原版对比移动后中心 (-116.17, 251.72)。原版 0x5089f4 调用 0x509030 更新敌人，0x508a03 再调用 0x509980 检查伤害。第 126 帧原版命中动画脚本 8 的指令 117 从 2057716148 继续取数。
- 试验把追踪符判定放在敌人更新后，并按原版区分圆形和旋转矩形。先验证同帧碰撞、双方交换、动画只推进一次、弹体当帧清理、非追踪弹和旧录像。原版逐敌更新后立即判断伤害，试验是整侧更新后再判定，后续需核实遍历顺序与反馈阶段。

已验证的浏览器报告来自试验前的诊断构建。新环境先复跑原版采集和浏览器对拍，再判断试验结果。

## 已核查的手填偏移

以下地址对应上文指定的原版 exe。复查时从实际调用或脚本取值，再用运行数据验证。

| 区域 | 原版依据 |
| --- | --- |
| 轮次横幅 | `0x50efc0` 建立两份 front.anm 实例，`0x50f167` 和 `0x50f1a7` 分别发送中断 7、8。 |
| VS 倒计时 | `0x512f19` 选字体 4，`0x512f40` 传入逻辑横坐标 318，`0x4e6639` 选择数字 sprite。 |
| VS 规则菜单 | `0x519a60` 读取 `0x59aaf0` 的布局表。 |
| Skill、Ex.Attack 和 Boss 文本 | `0x4e903d` 与 `0x4e8f4c` 设置逻辑纵坐标 448，Boss 共用布局。 |
| 暂停菜单 | `0x5394ed` 创建脚本 86，`0x539507` 按 PauseInf 的 `+0x400` 字段分支，`0x53953f` 写位置。 |
| 对战区边框 | 运行 front.anm 脚本 5–11，取其坐标与左上角锚点。 |
| 对话字形 | `0x4cd610` 格式化原版字形位图。 |
