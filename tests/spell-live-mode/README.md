# 符卡实时 mode 验证源码

这里保存实际执行的原始测试源码，未附原作资源、运行WASM或本地准入request。

- `actual-red104.cjs`：642在immutable104上实际X激活，证明窗口mode1未传到符卡（flags7）。
- `fixture.cpp`和`ecl-fixture.hpp`：643的42项完整StageWorld/EclVm组件检查，6个World/18tick；ECL和效果回调为明确合成边界。
- `actual-green105.cjs`：645使用首次构建105和原资源，1Factory/2World/183tick；X按键激活后符卡flags71，phase119/120/179保持，phase180清为7且PerfectHost仍active。

两个CJS脚本读取一个本地准入request路径（包含固定核心、原作资产、sound模块、合成ECL和全新output目录）。它们通过既有原生接口载入合法输入缓冲区，不直接写游戏状态；声音输出和收据存储模拟。全量request/manifest/hash、sourcecheck、fresh MAIN工程grant、ROOT独占及结果在本地640–645证据中封存。不要把脚本参数改为历史成功输出目录，也不要重编已成功的核心或重复运行成功用例。

643的`actual-component.json`只含aggregate和空rows，42项断言及计数由fixture强制；不能冒逐行日志。640/641的0tick错误是fixture遗漏资源/respawn配置，不是产品RED。详见[证据说明](../../docs/native-spell-mode640-645.md)。
