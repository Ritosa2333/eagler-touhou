# 普通版与多人版同步

每对独立源码目录拥有自己的同步基线和历史。普通版持续演进，多人版通过三方合并吸收对应修改，并保留联机专属实现。同步关系来自 `launcher/config/workspace.json`，th08、th10、th11、th15 和 th075 分别管理。

## 分目录维护与 Git 跟踪

维护者在 `ports/<作品>/` 目录开发，在 monorepo 中正常提交。提交后为对应作品刷新目录历史，MP 分支以普通版历史分支作为 Git 上游：

```sh
python tools/ports/project-history.py --repo . --game th08 --write-refs --track-normal
git branch -vv --list 'port-history/*'
git log --oneline port-history/th08mp..port-history/th08
```

`--game` 支持已保留独立导入来源的 th08、th10、th11、th15。每次刷新读取选定提交，相关目录的新修改进入对应分支，跨目录大修分别进入各自历史。`git branch -vv` 中 MP 的 `ahead` 表示 MP 独有提交，`behind` 表示普通版独有提交。已有上游配置与目标不同的分支由工具提示核对。

当前本地已建立八个 `port-history/` 分支以及四组 MP→普通版跟踪关系，快照为下表所列提交。分支位于总仓库，维护者进入源码目录后可直接运行上述 Git 查询。源码目录继续沿用 monorepo 布局；刷新读取已提交快照，工作区修改随正常提交进入下一轮历史。

跟踪配置属于本地 Git 配置；新克隆补全迁移来源历史后，运行上述命令重建。历史分支和当前源码分支分别承担历史合并与日常开发职责，切换到历史分支时使用其根目录源码布局。仅查看关系使用 `git branch -vv` 或 `git log`。

## 独有提交对账

[提交对应表](../history/port-sync-audit.md)将 Git 独有提交与已经出现过的普通版／MP 修改联系起来，附两侧固定快照和[逐文件证据](../history/port-sync-audit.json)。维护者按整提交补丁、文件补丁、修改块、共同 monorepo 来源逐层阅读对应关系。

```sh
npm run ports:audit -- --report docs/history/port-sync-audit.json --markdown docs/history/port-sync-audit.md
```

可用 `--game th08` 选择单作，默认核对 th08、th10、th11、th15。工具读取 `port-history/` 分支，输出历史对应证据；原分支、暂存区、工作区与同步接收基线保持原状。补丁等价遵循 Git 的空白、行号忽略规则。修改块对应供定位适配与拆分迁入，当前文本证据按目录改名映射到 MP 固定快照，并列出新增片段、删除片段和后像对应。[逐项决定](../history/port-sync-decisions.json)覆盖普通版 59 个独有非合并提交、MP 253 个独有非合并提交和 29 个合并节点，逐项保存来源、适配与冲突解决范围。八项待同步范围附来源补丁新增及目标代码缺失证据。快照变化后，工具标记已有决定需要复核，再完成实际合并。

MP 独有提交的整提交补丁来源比较覆盖普通版完整历史，包括当前共同祖先之前的提交；两侧独有提交继续逐文件、逐修改块比较。th08 的 `b5b4091cb1` 与普通版 `bbaab995cd` 的背景残留修复补丁等价；th10 的 `5973c99f01` 与普通版 `8d828fd5b0` 的启动加载画面补丁等价。这两项由完整来源比较识别，普通版当前领先范围中已没有对应提交。

MP 来源记录还包含结局改写的对应：th08 `f4dd742d12` 与普通版 `ead783f76` 的结局源码后像相同，th10 `546cf6853a` 与普通版 `b40616245` 的结局源码后像相同。合并节点通过 `remerge-diff` 列出冲突解决范围。MP 侧通用运行时优化另列[回迁候选](../history/port-sync-audit.md#可向普通版与同架构作品评估的改进)，供普通版和同架构作品沿提交评估。

### 已核验的源码对应

th15 的普通版 `9f8f976f2f2567694b957d47c7fec78a734f5a40` 与 MP `d17a5fb9bbb0945508ccf3e1b929d7d790ff47a0` 都实现了 WebSfx 音效输出与 SDL 音频块调整。两个提交后的 `th15_web/cpp/sdl/AudioDevice.cpp` 均为 Git blob `3767cac6960bcb0cf2c5eccd9cdb8651f7a0e7d7`，构建脚本都将 `eagler/audio/WebSfx.hpp` 登记为头文件输入。

当前核对快照为普通版 `70ae0f8563cd34b7d4c8802c3539cbb4ef91a113`、MP `268c61e41a1a2ff69a1f037818372ffdb7463e04`。MP 在这份音效实现上增加 `TH15_MULTIPLAYER` 下的旁观静音：恢复、定位和暂停期间关闭本地输出。相对普通版的音效文件差异由该适配构成，WebSfx 输出与构建输入均保留。本项音效变更记为两侧已有对应，旁观静音继续归属 MP。

这对记录中 MP 提交时间为 2026-10-16，普通版为 2026-10-17。对应表沿内容证据建立联系，迁入方向继续引用原始来源说明；两侧作者与提交时间均保存在 JSON 中。

th08 的音效记录 `6ca5e8d9a7` 与 `69a4945bf3` 有 4/8 个修改块对应；当前 MP 保留 WebSfx 与低延迟队列。th10 的 `9e4c14246d` 与 `75a0ac5760` 有 6/19 个修改块对应；当前 MP 保留相同音效策略并增加回放 seek 门控，练习触屏修复单列待同步范围。补丁统计与逐项源码结论分别保存。

## 原分支历史投影

`tools/ports/project-history.py` 将原仓库历史与 monorepo 中各自目录的修改接为独立 Git 分支。原仓库提交保持原样；monorepo 提交按所属目录投影，保留作者、时间和有效合并父节点，并用 `Monorepo-Source` 标明对应的原提交。目录迁移沿 `runtimes/<title>` 和 `ports/<title>` 衔接。

工具从选定提交读取迁移表和源码，只处理已提交的快照。先在临时克隆验证：

```sh
python tools/ports/project-history.py --repo <temporary-clone> --game th10 --write-refs --report <report.json>
git -C <temporary-clone> rev-list --left-right --count port-history/th10...port-history/th10mp
git -C <temporary-clone> log --oneline port-history/th10mp..port-history/th10
```

两个数字分别是普通版、MP 的独有提交数，表示 Git 祖先关系。目录内容相同或经过独立 cherry-pick 的改动，需要结合提交差异判断处理方式。没有共同祖先的来源分别保留原图，报告中的 `aheadBehind` 为 `null`，先核对来源关系。

只有 `--write-refs` 更新 `port-history/<title>` 分支；两侧引用在同一事务中前进，已有分支保持快进关系。工具保留当前分支、暂存区和文件内容。未指定该选项时，生成 Git 对象并输出结果。目录内容、原提交祖先和引用状态检查完成后才更新引用。

维护者在投影分支上使用普通 Git 合并。写回入口将合并结果的完整内容树放回 MP 目录，并保留 monorepo 与合并分支两个父提交，以及 subtree 的来源和目录元数据。下一次投影可识别这一真实合并关系。冲突由维护者结合两侧语义处理，源码、测试与同步说明一并提交。

```sh
git -C <temporary-clone> switch -c maintenance/th10mp port-history/th10mp
git -C <temporary-clone> merge --no-ff port-history/th10 -m "同步普通版修改"
python tools/ports/project-history.py --repo <temporary-clone> --head main --game th10 --prepare-writeback maintenance/th10mp --report <plan.json>
```

先阅读写回计划中的文件范围、父提交和内容树。确认后，加 `--create-commit --message "同步十代多人历史"` 生成候选合并提交对象；此步骤保持分支与文件原状。候选提交通过审阅后，在干净的 monorepo 分支上执行 `git merge --ff-only <candidateCommit>` 完成写回，再刷新投影。

写回前验证合并分支包含两侧当前投影提交，写回后验证 MP 内容树与合并结果一致、其他目录保持原树。旧导入关系中的全仓祖先与目录迁移由该入口明确处理。

### 2026-10-23 历史衔接验证

验证快照为 `9b9417f7bd88f13e11eacd4ea69fae4dd4b27884`，在临时克隆生成投影分支。

| 作品 | 共同祖先 | 普通版独有提交 | MP 独有提交 | 内容与祖先 |
| --- | --- | --- | --- | --- |
| th10 | `eb895d2e854d763570122cb1a9d655572501675f` | 20 | 91 | 两侧内容树与原目录相同，初始和刷新来源均为祖先，启动器和对侧独有原提交未混入 |
| th15 | `e96fcf8d7734a76f56eeb9b4a54334fc3b4e1baf` | 13 | 46 | 同上 |
| th08 | `48394852c196a48abc296305139327bf1f957c65` | 13 | 101 | 补全原仓库历史后，两侧内容树、初始与刷新来源祖先检查通过 |
| th11 | `79f22cb628446ac323f76ece84f4a42cb0150201` | 16 | 41 | 同上 |

祖先检查先补全历史。总仓库的浅克隆边界曾截断 th08、th11 的共同祖先；原独立仓库保存完整父链。在临时克隆从本地原仓库补全历史后，原提交编号和目录投影编号保持一致，Git 直接识别上述共同祖先。表中数字表示两侧各自独有提交数。

生产源码的原生合并会列出两侧需要处理的源码冲突。自动回归使用独立的导入、目录迁移和成对大修夹具，验证混合提交分别投影、输出可重复、引用保持快进、完整合并检查，以及精确写回后的领先数归零和下一轮新增提交识别。候选写回只修改 MP 子树，原生合并保留普通版功能和 MP 专属文件。

```sh
python -m unittest discover -s tools/ports -p test_project_history.py -v
```

## 按文件夹监控

MP 维护者为关心的文件夹或文件建立监控，每个监控独立记录已处理到的普通版提交。定期运行 `status` 查看新增提交及涉及文件，再用 `diff` 阅读具体修改。查看过程保留接收进度，由维护者完成同步后推进。

```sh
node tools/ports/sync.mjs watch th10 --name gameplay --path web/cpp/game --base <commit>
node tools/ports/sync.mjs status th10
node tools/ports/sync.mjs diff th10 --watch gameplay
node tools/ports/sync.mjs log th10 --watch gameplay
```

`--path` 相对普通版目录，可重复指定文件或文件夹。每个监控的基线与历史单独保存；处理一个监控不会推进其他监控。查看整个普通版目录使用 `status th10 --all`。修改已经提交时按提交显示，当前工作区修改另列。

维护者可以三方合并，也可以逐项人工迁入：

```sh
node tools/ports/sync.mjs merge th10 --watch gameplay --from <commit>
node tools/ports/sync.mjs merge th10 --watch gameplay --from <commit> --apply --note "同步本轮游戏逻辑，保留多人状态"
node tools/ports/sync.mjs record th10 --watch gameplay --from <commit> --note "已人工迁入并验证本轮修改"
```

`record` 也可说明某项改动属于单人专属、多人版按现有目标保留差异。接收记录包含来源提交和维护者决定，供下一轮继续查看。

## 基线与历史

`port-sync.json` 随源码保存每对目录的原仓库分支身份、共同祖先、本轮普通版提交基线、同步时的目标 HEAD 和处理说明。导入来源沿用 `docs/migration/` 的提交记录，并验证原提交属于当前仓库祖先。`status` 展示基线以来的普通版提交及工作区修改，`log` 展示多人版已经接收的历史。已有记录跨运行保留。

```sh
node tools/ports/sync.mjs status
node tools/ports/sync.mjs log th10
node tools/ports/sync.mjs log th10 --original
```

第一次接入一对目录时，显式选择本轮普通版修改之前的提交：

```sh
node tools/ports/sync.mjs init th10 --base <commit>
```

每个作品采用自身同步基线。原仓库共同祖先来自该作品普通版与 MP 的真实导入提交，`log --original` 沿保留的提交图查看历史。没有共同祖先的来源分别登记，人工确认映射关系。本轮目录基线单独表示后续增量同步的起点，历史改动的接收情况继续由原提交图和处理记录表达。MP 的现有源码作为独立合并方参与计算。共享程序覆盖普通和多人两种构建的作品，直接维护同一源码。

现有来源身份已从迁移表接入：th10 的共同祖先为 `eb895d2e854d763570122cb1a9d655572501675f`，th15 为 `e96fcf8d7734a76f56eeb9b4a54334fc3b4e1baf`。th08 和 th11 的共同祖先分别为 `48394852c196a48abc296305139327bf1f957c65`、`79f22cb628446ac323f76ece84f4a42cb0150201`，由完整原仓库历史验证。th075 在新增时直接进入 monorepo，使用目录级提交历史。

## 三方同步

先完成普通版的源码提交，再预览其到多人版的差异：

```sh
node tools/ports/sync.mjs merge th10 --from <commit>
node tools/ports/sync.mjs merge th10 --from <commit> --apply --note "同步神德难度规则，保留多人会话实现"
```

合并的三方分别是普通版基线、当前多人版文件和普通版来源提交。路径按目录映射转换，文件内容使用 Git 三方合并。预览列出目标文件和冲突；整批合并成功后，`--apply` 写入目标文件、推进基线并追加历史。源码和历史由维护者一并审阅、提交。

已有监控的作品在 `merge`／`record` 中显式选择 `--watch <name>`；操作整个目录时显式加 `--all`。

遇到冲突时，工具保留目标文件与基线。人工参照两侧代码处理对应文件，再重试；目标文件已经包含来源内容时，工具识别为已吸收。人工结合两侧逻辑完成的定制合并使用 `record` 登记，处理说明与源码一起接受审阅：

```sh
node tools/ports/sync.mjs record th10 --from <commit> --note "已手工合并冲突，保留多人会话分支"
```

`record` 由调用者确认本轮来源修改已逐项处理，保存人工决定并推进基线。二进制冲突、增删冲突、链接及特殊文件模式交由人工处理。普通版来源提交需要继承该作品的已有基线。

## 维护约定

接收结论按功能范围核验。整提交等价核对当前保留情况；拆分迁入逐项说明已接收功能与待同步范围；同次维护按两侧实际代码核对。共同来源、标题和局部补丁对应承担定位职责。

普通版修改默认进入 MP 同步清单，按具体功能范围完成迁入或适配。构建开关与源码同步分别维护，关闭功能的构建继续保留可维护的对应源码。模式专属决定逐项记录适用条件、代码范围与理由。通用修复落到两侧，模式专属行为写入同步说明。每条记录说明来源和多人版的实际处理，使后续维护能沿历史继续合并。手工迁入的 PR 在历史中标注来源和工作区状态，随本次修复一起提交。

数值键盘覆盖练习、暂停和高级工具。反应测试使用公共控件；th07 保留游戏逻辑按键，th08、th10、th11、th15 沿用各自键盘状态。th15 普通版与 MP 的可选键盘依赖使用相同条件编译边界。th10 普通版与 MP 的神德范围、默认值和录像参数采用相同规则。
