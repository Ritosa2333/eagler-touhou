# 验证与交付

[适配流程](README.md) · [文档目录](../README.md)

每条检查保存可重复的操作和结果，再更新本作适配表。

## 准备

先运行本作源码检查与编译，再跑浏览器操作、真实设备和网络对局。Replay 对照使用[共用工具](../../common/testkit/replay-verifier/README.md)和本作适配器。

普通版与原版做逐帧对照；MP 检查各端和多人 Replay 的自一致性。记录模板见[验证记录](record-template.md)。

## 逐项检查

| 编号 | 操作 | 通过时应看到 |
| --- | --- | --- |
| <a id="verify-01"></a>VERIFY-01 · 全部内置 Demo 对照 | 运行本作全部内置 Demo 的 quick 检查。 | 每个 Demo 覆盖到其声明结束点。 |
| <a id="verify-02"></a>VERIFY-02 · 长 Replay 样本 | 运行本作登记的 Lunatic、Extra 等长回放集合。 | 每份样本都有帧数、结束原因和结果。 |
| <a id="verify-03"></a>VERIFY-03 · 原版基准与内容哈希 | 核对基准所用 Replay、EXE、资源及 trace 的哈希。 | 来源与校验对象可完整定位。 |
| <a id="verify-04"></a>VERIFY-04 · 读取固定基准 | 以普通 candidate 检查运行一次，再核对基准文件。 | 预期数据保持原值。 |
| <a id="verify-05"></a>VERIFY-05 · 单独维护原版基准 | 在单独的基准维护流程从原版重新采集。 | 新基准附来源与审核记录。 |
| <a id="verify-06"></a>VERIFY-06 · 逐逻辑帧比较 | 比较同输入的逐帧状态，并注入一个已知差异验证报告。 | 报告首个差异帧及字段；帧序列完整对齐。 |
| <a id="verify-07"></a>VERIFY-07 · 诊断构建与正常构建 | 比较正常构建与诊断构建的编译选项和观察接口。 | 观察器只读，正常游戏沿用自己的 Demo 和输入流程。 |
| <a id="verify-08"></a>VERIFY-08 · 本作状态映射与结束条件 | 核对本作状态字段、样本选择和完成条件。 | 本作维护这些映射，共用工具负责采集格式与比较。 |

## 记录

在本作 `ADAPTATION.md` 的“运行验收记录”按对应编号填写操作和结果。通过时链接[验证记录](record-template.md)，并写明本次构建模式。

## 源码入口

[工具契约](../../common/testkit/presentation-lab/CONTRACT.md) · [启动器测试](../../launcher/tests/README.md)

## 测试辅助器

在 `launcher/` 运行 `npm run test:support`，检查浏览器辅助器、独立 Package 模块准备、当前键盘入口和本地 HTTP 服务。历史场景输入从固定夹具读取并校验哈希，当前代码沿用生产模块。根 CI 在界面行为任务中执行这组检查。

独立 Package 服务通过父进程管道管理生命周期，结束检查时关闭管道并等待服务退出，同时核对端口释放和临时站点清理。

Windows 本地发布检查验证 `EAGLER_ATOMIC_DEPLOYMENT_UNSUPPORTED_PLATFORM` 提前返回且无写入；POSIX 检查验证目录符号链接原子替换。预备目录刷新另行验证共享锁重试、完整候选检查和失败回滚。按实际平台记录对应结果。
