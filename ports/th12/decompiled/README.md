# TH12 1.00b 反编译索引

此目录只提交本说明。`assembly/`、`pseudocode/`、`native-entries/` 和
`functions.json` 为本地分析生成物，由 Git 忽略，不参与运行时构建。
已有本地资料可继续使用；原始快照来自 `Tweet-META/th12` 的 `eagler` 分支，
固定提交为 `c322e8ad7a3fa1276dd6dc377c0b5184d4f834bd`。
重新分析使用 `scripts/reverse/` 下的脚本，输出保存在忽略的 `local/reverse/` 中。

`pseudocode/` 和 `assembly/` 按十六进制地址一一对应，`functions.json` 保存函数候选、
基本块、调用关系和输出内容身份。`native-entries/` 另保留四个经过入口对齐复核的版本。

原版 EXE SHA-256：
`99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417`。

伪代码由 angr 10.0.1.post1 生成，不是恢复的原作者源码。自动名称、调用约定、类型、
函数边界及 x87 浮点结果可能不准确；不能直接编译这些文件作为游戏运行时。
需要按照同地址的原版指令及独立执行证据复核。

此目录不包含原版 EXE／DAT、原版游戏资源、分析缓存、诊断日志或过程报告。
原版程序衍生内容不属于仓库 MIT 许可的授权范围，版权仍属于上海爱丽丝幻乐团。
