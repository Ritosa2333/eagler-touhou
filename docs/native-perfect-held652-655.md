# 普通Perfect Freeze的held输入门槛（652–655）

2026-11-16。原版正常激活门槛检查按住X的holding位；旧移植还要求新的pressed边沿和私有latch未消耗，因此holding2/pressed0时跳过激活。private107只改PerfectFreezeHost.cpp/.hpp和WorldExports.cpp：移除额外fresh-press/latch、caller使用holding、删除原为latch服务的无held重置分支。活动对象、系统、mode、playerState、charge、位置、laser及必需服务错误保护保留；Context.xPressed字段为兼容保留，内部私有布局改变，无新C导出或前端参数。

MAIN只读原JA EXE的字节：录像word0经4413bc写入4db210（holding），word1经4413d6写入4db21c（pressed），word2经4413f5写入4db220（released）。原43b7b7比较charge，43b7c3测试[4db210]&2，75字节门槛SHA256为`9991f3c4275dc654fb9774d9c5b967ee49e085269d3feccba128f886e8e1c119`。物理输入4411ec/f8复制holding，411be0..411c68完整137字节helper构造内部edge。这是静态字节核对，未执行原EXE或证明全部间接writer及调度。

| 编号 | 结果 | 范围 |
|---|---|---|
| 652 | 实际产品RED | immutable106，1Factory/2World/4tick；holding2/pressed0保持mode0、charge10000、activefalse、flags7，nativeerror空 |
| 653 | 组件及语法通过 | 44检查/30Host/34activate，5组件TU及完整WorldExports语法检查，227pins，compiler/runtime/syntax0 |
| 654 | 首次private107完整链接通过 | 226源/83TU/205导出/390相对include/239pins，compilerCalls1/exit0，stdout/stderr空 |
| 655 | 实际产物输入矩阵通过 | 1Factory/4World/11tick，205函数存在检查；runtime0/stderr空 |

655的neutral(holding0/pressed0)与pressedOnly(0/2)保持mode0、flags7、charge10000；heldOnly(2/0)与heldPressed(2/2)得到mode1、flags71、charge0。heldOnly再继续按住3帧，active与charge0保持，未重复消费。全部场景RNG34772/calls0，normalRecord未启动/绑定，taint与infiniteRespawn均false；完整快照保留。原资源、World、PerfectFreezeHost及SpellVisuals真实执行；180字节ECL、音频输出和收据存储明确模拟，未执行原作整局或写入游戏历史。

653覆盖两个edge值下的held接受、pressed而无held拒绝、活动对象重复请求、12个保留gate、charge/config边界、setCharge后无需新edge重试、六种服务失败/抛错/true带error及必需服务配置拒绝。组件输出模拟且0World；runtime stdout为2657字节44行及summary，compiler stdout/stderr、runtime stderr、syntax stdout/stderr为空。组件GREEN后才首次构建107，并单独获准655真实caller验收。

核心107 generation：`aee898468bc4c286c2c989729e5b3220e289c1e0e10b5214f298ad4599c12019`。

- MJS：134120字节，SHA256 `13bc9cb03fd4346692ef3baf47c4528afc946607d9730edda02e3271d53e4aca`。
- WASM：2169308字节，SHA256 `8d71a43c2c818c8c8de19235e2792c901bd178f148252781cd67d42aa28eefcf`。
- 652首raw：17587字节，SHA256 `36c98c3f04c376d107f92bf4d781d9c8a64d2feb10349dfc06fba5acf649c183`。
- 653result：659字节，SHA256 `3c752ed0d11edd4a05a3db135c888fad7fab05fe88e7a63336930d4cfce63dd7`。
- 655实际结果：18966字节，SHA256 `e096fccbf0aebbed949d3f95f8cf7bdab36f710280fd55ec2df28d44d42813dd`。

A2独立审阅13786字节、SHA256 `2372ff7b9d2ec36382b0853261ef394de2a1e9353c901c155b5c4cae4c0b05c4`已成为654不可变输入，其“实际107待验”是构建前边界，现由654/655接续。每个新运行均有完整输入hash、带引号Windows命令、sourcecheck0、fresh MAIN工程grant及exclusive once ROOT；首FAIL保留，成功不可重跑。MAIN另一次654启动请求在once/ROOT检查处被拒绝，未调用第二次编译；首次构建记录未改。

这只闭合普通Perfect held激活门槛。mode180再次触发与Hostactive200、完整Perfect时序/声音、state4/deathbomb/keys窗口写入、命中死亡tail、617完整录像兼容、JA、续关完整终局、浏览器/物理音频/GPU/设备及独立部署各自待验。旧Perfect测试的fresh-press/服务契约是历史源码，当前门槛回归放在tests/perfect-held-gate，不声称旧源码原样通过107。公开Runtime未更新；最终匹配Runtime、同TH12规范资源ZIP与解压目录仍须交付，ZIP实施第三阶段，多人第四阶段。
