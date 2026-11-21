# 普通Perfect激活声音请求

`fixture.cpp`与`actual-perfect-activation.inc`为657实际执行源码，完整提取当前生产activation块，真实PerfectFreezeHost配明确模拟的World/Hit/Ice/HUD/输入/请求容器。13项检查、9Host、10次调用通过；容量65534接受两项，65535/65536报告fault且不追加部分请求。此时Host已提交而caller窗口未同步，是fail-stop，非事务回滚；不声称vector allocation异常原子性。

`actual-phase-requests.cjs`是656/659相同实际测试源码，以本地已准入request.json为参数：真实WASM、原资源、180字节合成ECL、音频输出及收据存储模拟。7项检查验证6个请求：phase0的46play→51play，phase120的47play→69play→51stop，phase180的71play。非零playerX100用于区分位置与固定声；1Factory/1World/181tick。完整本地准入记录、资源和运行产物不随Git分发。

不得覆写首FAIL或重复运行成功657/658/659，成功核心93至108不可重编以增加PASS。独立新验证需新目的、新输出和完整准入。原作完整时序、mode180重触发、死炸、617、物理听感与浏览器仍独立待验，见`docs/native-perfect-sound656-659.md`。
