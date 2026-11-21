# 符卡实时 mode 传递（640–645）

2026-11-16。private104的真实Perfect Freeze激活会把玩家窗口mode置1，但符卡上下文仍保留初始化的0，导致符卡开始时缺少0x40。private105把已有mode同步到已有符卡字段；没有新增C导出、额外mode存储或直接改写flags。

两份产品源码共四处变化：StageWorld新增仅写现有context字段的内部setter；WorldExports在初始绑定、实际激活置1及phase180置0后同步。同步发生在后续ice/frozen/world tick之前。现有SpellRules的标志、HP、奖励与RNG规则不变；中途mode1本身不会直接补置0x40。

| 编号 | 实际结果 | 范围 |
|---|---|---|
| 640 | fixture失败 | 1Factory/1World/0tick；漏载enemy外观bank，首raw保留 |
| 641 | fixture失败 | 1Factory/1World/0tick；漏配真实respawn前置条件，首raw保留 |
| 642 | 产品RED | immutable104，1Factory/2World/4tick；实际X激活mode1、phase1、charge0，但id70符卡flags7，缺0x40 |
| 643 | 42项组件GREEN | 6个World/18tick，普通与EX、初值0/1/-1；实际StageWorld/EclVm，合成ECL/效果回调；完整WorldExports语法检查通过 |
| 644 | 首次private105链接PASS | 226源/83TU/205导出/390相对include；238输入pin，compilerCalls1/exit0，日志空 |
| 645 | 实际WASM正向PASS | 1Factory/2World/183tick；32输入pin，runtime0、stderr0，ROOT释放 |

645使用真实WASM World、PerfectFreezeHost、SpellVisuals和原资源，但ECL是显式生成的180字节组件脚本，并非原作关卡历史。初始charge10000；只用原生X输入触发激活，随后输入归零。输出队列和声音收据存储模拟，无normalRecord、clear、carry或原录像操作，无浏览器/物理设备。

| 645观测 | 游戏frame | Perfect phase | 窗口mode | 符卡flags |
|---|---:|---:|---:|---:|
| neutral对照开始 | 2 | 0 | 0 | 7 |
| 实际X后的符卡开始 | 2 | 1 | 1 | 71（0x47） |
| 边界前段 | 120 | 119 | 1 | 71 |
| 内部阶段切换 | 121 | 120 | 1 | 71 |
| 清除前 | 180 | 179 | 1 | 71 |
| mode清除 | 181 | 180 | 0 | 7 |

phase180时PerfectHost仍active；RNG保持seed34772/calls0，无录制或debug辅助。205项仅检查导出为函数，实际调用范围由测试源码决定，不代表205接口功能全覆盖。

643准备阶段曾因include闭包遗漏runtime/Memory.hpp停止，发生在request/grant/compiler/ROOT之前，原失败说明保留后修正查找根；它不属于产品RED。成功643/644/645及640/641/642首失败均封存，不为重复PASS重跑。

核心105 generation：`ca54de1bd68ec0b71cc9946bf0a107d89f2b74488886235dd7568633988f5886`。

- MJS：134120字节，SHA256 `13bc9cb03fd4346692ef3baf47c4528afc946607d9730edda02e3271d53e4aca`。
- WASM：2169773字节，SHA256 `101f84c53431cb7d639882aac2181499332a7dde14495a936eec00b9142124eb`。
- 642首raw：17558字节，SHA256 `33fd17efadedb3d6d63c70a9857deb9f039c9d7c5f9b7abbe5078f70c113178a`。
- 645实际记录：12278字节，SHA256 `a986ebccf9a75c4e97b1f5c5454268a364a90276e0f0a517c4b65050a45636b2`。

A2独立审阅无代码阻塞，最终12160字节/SHA256 `59024967fb0896784bdcf4d94f6e80b39e9ad0959c0b3e018c9d601840220673`作为644不可变输入。其“105尚未构建/实际X待验”是构建前审阅边界；当前由644/645新增证据更新。

本轮只闭合既有生产mode到符卡上下文的传递与上述实际边界。命中/死亡tailControl快照、keys生产链及原版mode写者/帧序是独立源线索，尚未独立验收；没有把它们并入本修复。既有原版构造前缀证据634支持同mode下的标志规则，但本次不重跑原EXE、不证明所有原版调用时序。617的23707帧录像仍在4906处GameOver，未证明本修复解决该问题。JA、续关完整终局、浏览器、设备、独立部署、第三阶段ZIP与第四阶段多人均保持各自待验。
