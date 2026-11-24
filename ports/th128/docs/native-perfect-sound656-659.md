# 普通Perfect激活声音请求（656–659）

2026-11-16。旧107的正常Perfect成功激活没有登记46、51播放请求；停止51与播放51是不同操作。108只在成功activation分支补容量保护与playerAimX下的46→51两项play，共WorldExports.cpp四行。phase120的47play/69play/51stop和phase180的71play已经存在，原样保留，没有新增71或改Host/phase/HP/RNG/history。

MAIN只读固定JA EXE：407884/40788c经4584a0登记46，407b81/407b89经同wrapper登记51，均取实际playerX；51在zero(4b4cf4*2+4b4cf8)effect-helper域。458410使用固定0位置、4584a0使用位置参数，均登记请求，未证明物理voice。407c50完整函数是471字节至407e27 exclusive（SHA256 `6afa5b11f7e9316351cfc53e9342abfb37ab6378f2d3dbece876da8333b8a6f8`）；旧source报告469字节只截到ret4的c2，缺04 00，旧文件保留。这些静态字节不证明原版完整调度与phase等价。

| 编号 | 结果 | 范围 |
|---|---|---|
| 656 | 实际产品RED | 107真实1Factory/1World/181tick，7检查3失败：缺46、51和整轨迹；原有4项各一次通过 |
| 657 | 13项组件通过 | 真实Host＋完整实际caller块，9Host/10调用；World/Hit/Ice/HUD/请求容器模拟，5TU/371合并include/229pins，完整WorldExports语法通过 |
| 658 | 首次108链接通过 | 226源/83TU/205导出/390include/238pins，compilerCalls1/exit0，stdout/stderr空 |
| 659 | 实际同7项通过 | 1Factory/1World/181tick，6请求完整顺序/位置/操作精确，runtime0/stderr空 |

659以native holding2/pressed0和初始rawX12800（实际X100）运行。phase0依次play46(x100)、play51(x100)；phase120依次play47(x100)、play69(x0)、stop51；phase180play71(x0)。每项恰一次，无额外目标请求。RNG34772/calls0，normalRecord未启动/绑定，taint/infinitefalse。完整phase0/1/119/120/179/180快照保留。原资源、World、PerfectHost/SpellVisuals真实；180字节ECL、音频输出和收据存储模拟，非整局或物理听感验收。205项只typeof函数存在，不代表所有功能执行。

657的13项检查覆盖正常pair/order/x100/原mode-stock-HUD尾部、active不重复、无held/pressedOnly/stock/state4/laser拒绝、容量65534接受到65536以及65535/65536拒绝无partialpair。容量检查位于Host提交后：失败时Host active/charge0，caller窗口仍10000/mode0，报告Worldfault并return0；这是明确fail-stop，不是回滚。两个vector push的allocation异常未测试，不声称通用异常原子性。runtime.stdout962字节非空13rows+summary，compiler stdout/stderr、runtime stderr、syntax stdout/stderr空。

核心108 generation：`b7f885be544802659eda1848a86bd9ee964a985f13b73d9a4705df2d9b25e744`。

- MJS：134120字节，SHA256 `13bc9cb03fd4346692ef3baf47c4528afc946607d9730edda02e3271d53e4aca`。
- WASM：2169783字节，SHA256 `03b4c0f0c0465a393b38a3ef2430936b55d45e56f9dc276add9c4d2f28650c21`。
- 656首raw：45984字节，SHA256 `f5b42e7e3010fe6b8ad0ef9c2ce7a6999374364489553deb06f87282ddf03617`。
- 657result：675字节，SHA256 `b25fbc1c6e910092f84a7a1cec2547c1a2d2a8365d96c7fb7d67702ce4799ac2`。
- 659实际结果：37867字节，SHA256 `4fa938bcf72bc10aabceab41acd6e888769725c915c2fb692f1c3ec8c2516990`。

A2独立review10433字节/SHA256 `68ada4f07f45f4e5ca8084dc6303c567b1ca1f9e358f3e3b5ee02283a9ce6319`已封为658输入，其108pending是构建前边界。ANM审计只说明本端口的执行路径无声音journal出口；未解析原ice.anm script0x24，不伪造原opcode证明。每个新运行都经完整hash/manifest/quotedWindowscommand/sourcecheck0/freshMAIN工程grant/exclusiveonceROOT；首FAIL与所有成功封存，没有重编107或重跑656/657/658/659。

测试源码见tests/perfect-activation-sound；旧holding等测试字节保持。完整Perfectphase、mode180再次触发vsHost200、effectchild/410a60、state4/deathbomb/keys/tail、617完整原录像、JA、续关完整终局、浏览器/设备与独立部署仍各自待验。公开Runtime未更新；最终Runtime＋同TH12规范资源ZIP/解压目录要求保持，ZIP第三阶段、多人第四阶段。
