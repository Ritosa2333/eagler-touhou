# TH128 thprac chapter 跳转组件

当前交付是实际TH128 thprac的原生组件，完整练习功能尚未完成。第三阶段完成结论仍已撤回，临时七项选关不计作thprac；多人后续工作继续等待。

原Windows thprac直接覆盖已加载ECL内存。本工程严格解析整份ECL，24字节raw写入覆盖原20/32/40字节指令后会留下不可解析尾部。现使用Program::withPracticeBranches：核对整份原字节，验证同一子程序内的指令边界，复制不可变解析结果，仅替换跳转指令语义，保留原offset、其他指令与资产字节。虚拟size24用于Vm操作数检查，不能把覆盖后的对象重新序列化为原ECL。

Thprac128Chapter.hpp移植固定上游THStageWarp的16stage、126chapter、143操作。stageId使用核心的1..16；上游参数stage为0..15，之后的会话codec必须显式转换。prepareChapter只校验原资源名称/大小与整字节一致性，资源提供者仍须校验完整SHA；这不是发布者身份认证。MIT原作者声明保留于licenses/THPRAC-MIT.txt，记录的是本地LF规范化副本。

| 用例 | 实际结果与边界 |
|---|---|
| 865→867 | 旧Program缺消费者RED；866原控制夹具误用opcode22，换为测试host操作后867编译/运行0，141断言。真实Program/Vm，0World |
| 868→870 | 缺TH128 adapter RED；869误认原补丁处是子程序入口，修正夹具后870运行0。16原ECL/126计划/143操作比对，110首跳转由真实Vm到精确PC/时钟 |
| 871→872→873 | 原reader通过，现存VM总预算夹具失败；872以改动前Program精确复现。主线程先return会退休worker；仅修夹具保留未来return，加预算4/5对照与退休控制。873三组完整组件编译/运行均0，stderr空 |

870入口跳板是明确的测试边界，用来绕过原游戏初始化；在目标host副作用前故意触发指令预算停止。所有第二跳的形状和原目标均已比对，未宣称完整游戏路径均执行。380864是遍历原指令的断言数，不是游戏场景数或性能指标。867/870成功均未重跑刷PASS。

详细指纹与结果见thprac128-branch-overlay865-873.json；原request/actual-manifest/MAIN-APPROVAL、首失败raw及新号修正保留在owned工作目录。871/873准备工具stdout误写1compiler/3TUs；实际manifest和grant明确3compiler/8TUs，871只执行2次后停止，873执行完整3次。872的HostResultRules头副本仅多一个EOF空行，Program/Vm四文件与指定基线逐字节相同，未把该头称为字节同一。

当前没有StageWorld调用、Web启动入口或新WASM API，也没有配置参数、Boss/EX phase、菜单、热键/速度、练习Replay和launcher开关的完整闭合。本轮0World/Factory/全核心编译/browser/GPU；core112、DATOGG资源包、本地Runtime和公开页面未变。
