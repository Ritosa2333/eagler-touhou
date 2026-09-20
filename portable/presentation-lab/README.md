# TH08 Presentation Lab / 帧间显微镜

这是 TH08 高刷新 presentation 管线的隔离诊断工具，不是可以直接部署的正式 Runtime。主线游戏和 Launcher 不需要为它改变资源路径；诊断代码只在 `TH_PRESENTATION_AUDIT` 构建中启用。

## 使用

在这份测试树内运行：

```powershell
# 已经编译、装配过时
./portable/presentation-lab/start-lab.ps1

# 修改 C++ 观察器后重新编译、装配、启动
./portable/presentation-lab/start-lab.ps1 -Build
```

默认地址是 `http://127.0.0.1:8132/`。脚本不会停止占用端口的其他服务，也不会推送、发布、改变正式站存档。已有服务必须与预期 WASM 身份一致，否则选择另一个端口。重新编译后仍在运行的旧进程需要由它的启动者退出，不能自动杀死一个身份不明的进程。

页面支持选择用户自己持有的 `th08.dat`。本次开发使用的命名测试 ZIP 可以通过以下命令准备，不扫描目录或硬盘：

```powershell
python portable/presentation-lab/prepare.py --zip 'D:\workspace\东方测试.zip\东方测试 (1).zip'
```

只解出明确列出的 DATA 和四份 Replay 到忽略目录，并核对 DATA SHA-256。游戏资源和字体不随工具源码发布。字体由本机现有路径读取；其他机器可设置 `TH08_LAB_FONT`。编译需要 `EMSDK`，装配需要 `EAGLER_FONT_ROOT`。

操作流程：启动实验 → 正常游玩 → 冻结并检查 → 拖动 α → 点击问题组/对象框 → 导出报告。原作暂停和检查器冻结是两回事：前者会冻结世界，后者保留两帧端点并检查显示过程。

导入 Replay 会重新启动这个测试会话，再从游戏 Replay 菜单进入。原因是 TH08 的 `FileHost` 在创建 `BrowserRuntime` 时读入存档/录像；不能把“启动后写入 FS”误认为已经更新了游戏内资源缓存。本工具不绕过这条加载路径。

## 检测结构

1. `PresentationAudit.*` 在原始、固定逻辑帧的 Draw 中独立记录提交结果，保留相邻两次原始绘制。它不把游戏自身的插值 sidecar 当作正确答案，因此能够发现端点快照过晚、已被覆盖等问题。
2. 绘制 owner 的只读作用域标记携带原始对象身份、部件和年龄。临时 VM 副本的栈地址不充当稳定对象 ID，未接入身份的绘制也不靠四边形邻近匹配猜测身份。
3. `audit_draw` 只调用 `GameApplication::draw(alpha,true,true,...)`。不会补跑游戏 tick、轮询输入或额外执行 ANM。顺序为 `0,.25,.5,.75,1,.5,.5,1`，既检查中间值，也检查同 α 的重画一致性与 α=1 的原始端点。
4. 普通 sprite、显式 quad、条带、扇形在 `AnmRenderer` 的提交边界采样。3D 世界几何用实际提交的 world/view/projection 矩阵做只读投影；不冒充 GPU 的最终 UV、雾效和颜色读回。
5. `analyzer.mjs` 独立分析字段。原作 ANM 持续缩放、角速度、透明度渐变等提供连续性证据。没有连续性证据的保持行为只列为待核对，不要求一切都插值。
6. 相同 owner / ANM / script / 属性的问题合并成问题组；原始窗口保留实例和字段证据。跨窗口统计区分观察次数、窗口数、峰值实例数，不把循环复用的内存地址宣称为全流程唯一对象。

`audit_fault(1)` 是负对照：只让已有 `lerp` / `lerp_world` 返回当前值，不改变逻辑。直接用 `alpha` 写出的自定义公式不受这个负对照影响，因此它不是“关闭全游戏全部插值”的开关。

## 证据与边界

- 静止、未出现、缺失样本、换脚本/生命周期、屏幕外、未标记对象，不能被计为插值通过。
- 原作离散贴图换帧和分数变化不要求混合。换贴图也不能自动豁免位置插值。
- 几何最多记录 16 个顶点；大型几何标为抽样。扇形转换另有 1,024 顶点上限，超限会记录丢弃并使窗口不完整。
- 每次绘制最多 8,192 条记录。当前/上一原始帧和当前显示帧使用有界缓冲。历史保留最多 16 个窗口的前 96 条详情；扫描最多 120 个窗口，截断详情会明确标记，完整计数仍保留。
- 画面框是提交区域，不是遮挡测试，不证明每个像素最终可见。低于容差的变化无法可靠分类。
- 当前覆盖标记包括标题、帮助文字、暂停/重试、敌弹、激光、特效、敌人、道具、部分 HUD、背景、符卡 UI、自机和自机弹。没有标记的 ASCII/特殊绘制会列为未接入，不假装已覆盖。
- `gate` 表示当前场景是否允许走高刷路径，不是显示器刷新率、实际浏览器帧率或已经成功进入高刷模式的证明。强制 alpha 扫描可检查门控关闭的场景，但报告会说明这一点。
- 九组指纹监测 RNG/资源数值、自机、敌弹、激光、敌人、特效、标题 VM、场景/逻辑计数、背景镜头的指定字段；不是整个游戏的完整状态序列化。发现变化会停止自动扫描。
- 多帧全局纯度还要看对照测试，而不只是“同 α 重画一致”。`verify-noninterference.py` 会比较关闭观察、只观察、插入重画三种运行方式。

## 验证命令

```powershell
node --test portable/presentation-lab/test-analyzer.mjs
python portable/presentation-lab/verify-browser.py
python portable/presentation-lab/verify-noninterference.py --ticks 1200
python portable/presentation-lab/verify-replays.py --fixtures 4 --ticks 1200
node portable/check-high-refresh-contract.mjs
node portable/check-presentation-purity.mjs
```

浏览器验证使用实际 TH08 WASM、DATA、输入和绘制，不用假 Runtime 替代。自动化只启动一个浏览器、一次一个测试页面；不接管用户浏览器。默认使用 Playwright Chromium + SwiftShader，不等价于 Edge / Linux / 手机 / 实际高刷显示器验收。

输出位于 `artifacts/presentation-lab/evidence/`：浏览器捕获、截图、报告导入导出结果、三路无干扰对照、Replay 巡检。每份报告带构建身份。`commit` 是测试分支基线；未提交改动由 WASM、源码清单摘要和检查器 JavaScript 摘要标识，不能只凭 commit 认定测试了相同代码。

## 延伸方式

先增加明确 owner 的观察作用域，补充正/负/离散边界样例，再扩大真实 Replay/菜单场景覆盖。不要把检测器变成第二套插值实现，不要靠删除报警、降低游戏频率或修改碰撞/RNG 来让结果变好。

诊断代码由 `TH_PRESENTATION_AUDIT` 控制，普通构建不启用；不得将这份测试树的 instrumented Runtime 当作正式发布成果。当前检查器没有承诺全游戏零遗漏，未访问场景和未观测字段需要继续扩展验证。

冻结、固定 tick 快进、alpha-only Draw、重复采样和有界扫描由固定版本的
`third_party/eagler-common/testkit/presentation-lab/controller-core.mjs`
统一编排。TH08 本地 adapter 仍拥有记录布局、owner 含义、状态指纹、相机时序和分类策略。克隆仓库后需初始化 submodule；正式 Runtime 不加载该工具模块。
