# 适配一个游戏

每个游戏目录放三份入口：`README.md` 说明怎样开始，`docs/DEVELOPMENT.md` 写本作构建与差异，`ADAPTATION.md` 逐项记录完成情况。

STG 从[共用适配流程](stg.md)开始，按阶段完成接入，并用单作、单模式命令核对当前产物。各分项指南提供逐编号的方法与反例；玩法差异由本作适配器维护。

## 1. 选定源码和模式

阅读[工作区](../../WORKSPACE.md)，取对应的游戏目录。修改普通版的基础行为时，同时检查 MP 的对应实现。

新游戏先在[目录映射](../../launcher/config/workspace.json)、[构建配方](../../launcher/config/runtime-builds.json)和[产品目录](../../common/contracts/product-catalog.mjs)登记。配置中的文件名、存储根和模式应对应实际程序。

## 2. 构建并接入启动器

按[源码与构建](build.md)准备工具链，编译 Runtime，再按[启动与资源](runtime.md)接入共用启动器。先完成一次从选择游戏到正常退出的过程。

## 3. 逐项适配

| 顺序 | 分项 | 表内编号 |
| --- | --- | --- |
| 1 | [源码与构建](build.md) | BUILD |
| 2 | [启动与资源](runtime.md) | RUNTIME、PACKAGE |
| 3 | [键盘、手柄与触控](input.md) | INPUT、TOUCH |
| 4 | [画面与声音](presentation.md) | DISPLAY、AUDIO、MIDI |
| 5 | [存档与 Replay](storage.md) | SAVE、REPLAY |
| 6 | [语言与练习](localization.md) | LANG、PRACTICE |
| 7 | [多人接入与规则](multiplayer.md) | MP、MP-RULE |
| 8 | [验证与交付](validation.md) | VERIFY |

每个编号都有操作与预期结果。[验收方法与新作品接入](test-methods.md)说明如何把这些要求写成输入、观察、断言和反例，以及下一部作品怎样复用驱动。MP-RULE-01 至 MP-RULE-05 只适用于产品目录标记为合作玩法的多人模式；各作生命、Power、碎片、幽灵救援、资源给予和剧情规则以本作开发指南为准。th09 是对战模式，这五项标为不适用，其对战规则在 th09 专属指南中核对。

## 4. 建立并维护适配表

新作品登记后，在仓库根目录运行：

```sh
node tools/docs/new-adaptation.mjs --game=th10
```

把 `th10` 换成新登记的作品。命令读取实际源码归属，为该作各目录创建表格，并保留已有表格。

| 状态 | 何时填写 |
| --- | --- |
| 待核对 | 尚未确认本次源码和构建的情况 |
| 进行中 | 正在实现或修复 |
| 待验证 | 实现已准备好，尚待完成该行操作 |
| 已实现 | 当前源码接口或模块检查通过，结果链接到执行报告 |
| 检查失败 | 当前检查发现差异，报告给出失败接口或测试入口 |
| 已验证 | 已运行该行操作并附结果链接 |
| 不适用 | 当前模式没有声明该可选功能，写明原因 |

验证记录写明提交、模式、环境、操作和结果，使用[记录模板](record-template.md)。单人和 MP 分别填写；例如普通版回放对照与 MP 自一致性使用各自的检查。

## 自动核对实现情况

```sh
npm run ports:check -- --game=th10 --variant=both
npm run ports:status -- --all
npm run ports:check -- --all --check-status
```

第一条检查单作的普通版与 MP，第二条执行全作检查并生成各目录的适配表，第三条同时核对已记录的源码身份。命令、范围、结果与失败依据保存在 [status.json](status.json)，完整编译输入和日志写入 `artifacts/validation/ports/`。任何检查失败都会返回非零状态。

表格按实际执行的检查更新。自动和人工运行的验收均链接到各表的“运行验收记录”节，自动更新保留该节内容。共用模块结果与各作 native 接口分别记录，普通版通过不能覆盖 MP。源文件、测试或构建选项改变后，源码身份检查要求重新执行。编译器声明、源码接口和模块结果分别标注范围；浏览器运行、设备操作和网络对局使用[验证记录](record-template.md)补充本次环境及结果。

完整检查入口和新增检查方法见[检查工具](../../tools/ports/README.md)。

## 5. 检查并提交

```sh
npm run docs:check
```

检查会核对源码归属、表格缺项、状态与证据链接。适配表中的行为条目对应[现有能力定义](../../launcher/src/contracts/adapter-capabilities.mts)；能力定义新增条目时，检查会提示同步补齐说明和游戏表。

完成代码与本作行为检查后，按[贡献指南](../../CONTRIBUTING.md)提交改动。所有游戏表可从[游戏目录](../../ports/README.md)进入。
