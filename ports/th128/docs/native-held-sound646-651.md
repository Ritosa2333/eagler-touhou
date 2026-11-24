# 受击蓄力声音停止请求（646–651）

2026-11-16。原版受击窗口在held计时器为正时，先清计时器，再依次登记31、70两个声音停止请求。旧PlayerHitHost只提交31，漏掉70。private106只修改PlayerHitHost.cpp：以有序31→70循环替代单次31调用，保留原有错误、抛错、重入保护及定时器/动画推进边界。

MAIN逐字核对固定原始JA EXE：0x43b8ad装载31，0x43b8b2调用458590；0x43b8b7装载70，0x43b8bc调用458590。下一条0x43b8c1读取控制对象，两次请求在后续mode/charge/key门槛之前。458590的137字节、56条指令与旧封存raw完全相同（SHA256 `8a34caa6deab834b9ae287abb2acf21fef04fbe62ff707bbd48b5db2542c59aa`）：非负ID登记count=-1停止请求，未证明即时voice停止或物理听感。本轮原EXE及旧probe执行均为0。

| 编号 | 结果 | 实际范围 |
|---|---|---|
| 646 | fixture失败 | compiler0/runtime2；未将pl00嵌入MEMFS，0个Host，首raw封存 |
| 647 | 产品RED | 39项检查17失败；6个真实Host/11次窗口调用，compiler0/runtime1 |
| 648 | 同39项全部通过 | exact647 fixture，9TU/20include闭包，UBSan，compiler/runtime0 |
| 649 | 完整Host回归通过 | 现有主程序仅更新31/70契约后在新源码上返回0，全部日志空，无虚构检查计数 |
| 650 | 首次private106链接通过 | 226源/83TU/205导出/390相对include，compilerCalls1/exit0，日志空 |
| 651 | 实际产物ABI通过 | 1Factory/0World；205函数存在、内存往返、5项无会话拒绝，未创建结束owner |

647与648使用真实原pl00资源、真实confirmedOverlap和ANM；初始快照与效果/音频输出为明确的合成组件边界。控制中没有X，因此不进入死炸分支。17项RED由两个31/70顺序比较和三个70错误变体各五项断言组成；修复后39项通过。31拒绝时不能请求70；70拒绝、抛错或返回true却带error时不能推进后续stateTimer、保护与ANM；成功后的下一无held步不重复请求。重入保护仅作源码保留，本轮没有新增重入运行用例。

649完整回归保留原有hit、death/respawn/entry、movement/focus和camera拒绝检查，只修改停止请求预期。旧Git `tests/native/player-hit-host.cpp`作为固定历史输入保留；当前106测试版本在`tests/held-window-sound/player-hit-host-regression.cpp`。649准备阶段将从聚焦fixture复制的容量元数据修正为12个实例/60次调用的静态上界，发生在sourcecheck/grant/runtime之前；原request/manifest版本保留。这些上界不作为动态覆盖计数。

原版字节中两次调用的返回值未被检查；C++遇到拒绝/错误/抛错即停止，是已有Host契约。本轮只补齐请求，不宣称该错误契约与原作返回值处理完全相同。

核心106 generation：`ff89526ed15293870bc33785b2522422443435c8beaa7215f3c44f60f51c5f90`。

- MJS：134120字节，SHA256 `13bc9cb03fd4346692ef3baf47c4528afc946607d9730edda02e3271d53e4aca`。
- WASM：2169994字节，SHA256 `8c708e6a2daad3a04b7c2095e10f93a897b18f67b567cd79d27d225eef63c9f0`。
- 647首raw：8905字节，SHA256 `0f91640a04b689101bbf7e389a6c4dbfc838b571b1a33e720b78b248025d64fa`。
- 648result：557字节，SHA256 `4f0715cb78a425df72ca79324a1b76c533309983e067f197498b6ca41bfa5ee1`。
- 649result：531字节，SHA256 `66f02c9a9689d68318ebc4550efac4f6eb8ef529ee1107174e2dfa2ffe672ffa`。

A2独立源码/fixture审阅12777字节、SHA256 `6ec6fca8e1cef9d49d2258017f5bc4151adf190fe32aa394e90235fc931a5a04`已封为650输入；其“尚未106构建”的文字是当时边界，当前650/651提供后续证据。全部首失败及成功记录保持不可变，没有重编105或重跑旧六线、EX与原录像。

这只闭合no-X受击蓄力停止请求。完整state4、死炸、keys生产链、同帧held后继续检查X、命中/死亡tail、617原版录像兼容、JA、续关完整终局、浏览器与设备仍分别待验。生产的>=8死亡路径已由WorldExports中的DeathOperation承接，598实际失败/续关证据仍有效，不能将单窗口方法的拒绝误称为整个死亡流程缺失。公开Runtime保持原版本；最终网页Runtime、资源ZIP及解压目录仍属交付范围，ZIP实施依第三阶段顺序继续。
