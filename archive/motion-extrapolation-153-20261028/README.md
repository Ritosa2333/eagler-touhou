# #153 高刷外推与 Presentation Lab

TH06、TH07、TH08、TH10、TH11、TH12、TH15 的实时高刷呈现从当前逻辑帧向前预测最多一帧，移除回顾插值的固定一帧画面滞后。游戏逻辑保持 60 Hz，输入采样、碰撞、随机数、Replay 和网络确认使用原有状态。

分支 `feature/motion-extrapolation-153` 基于合并后的 `5aa34a56f`。TH12 多人单独位于 `feature/th12-mp-motion-extrapolation-153`，基于 `f58474333`，修复提交 `b3dcec09f`；该提交修改多人目录中的六个文件。

## 呈现规则

`weight = 1 + clamp(phase, 0, 1)`：最新帧为权重 1，向前一个逻辑帧为权重 2。异常时间样本回到权重 1。既有的对象身份、脚本、资源、可见性和位移边界继续约束可预测字段。原生诊断的 `0..1` 采样保持回顾插值含义。

TH15 多人按已发布逻辑帧与回滚代数维持预测年龄；网络等待封顶一帧，校正和重开丢弃旧年龄。TH11 为刚生成的动画标记当前生命周期，第一帧直接呈现。颜色与透明度限制在合法通道范围内。TH12 多人按实际外推子帧计数 FPS，停滞世界的重复重画不增加计数。

按本次确认范围，TH07.5、TH09、TH20 保持既有固定帧呈现。

## 验证

| 范围 | 检查 | 结果 |
| --- | --- | --- |
| 六作普通版与多人版原生包 | TH06／07／08／10／11／15 正式 WASM 编译 | 12 个变体完成，摘要见机器记录 |
| 外推时序 | 实际 C++ 公共实现，60／75／90／120／144／165／180／240 Hz、长等待、代数重置 | 通过 |
| TH08／10／11 普通与多人 | 实际呈现 helper／VisualSample，前向位置、禁用、冻结、生命周期、透明度和 UV | 通过 |
| TH11 普通与多人 | 实际动画呈现实现、附属位置、新生对象与固定帧时钟 | 通过；多人调度 26288 项检查通过 |
| TH12 | 相邻帧身份、二进制端点、外推、旧核兼容和呈现节奏 | 27 项通过 |
| TH12 多人 | 同类呈现检查及外推 FPS／停滞重画 | 28 项通过 |
| Presentation Lab | 控制器、采样模式、重复绘制与证据分类 | 12 项通过 |
| TH15 实际 Lab | 原始 DATA、正式 Draw 包、原始帧／回顾插值／外推、滑块 | 实际进入关卡，三种模式有效，已覆盖状态未变化 |

TH15 Lab 当前观察 Player 根提交的首个顶点，状态证据覆盖既有 checkpoint／world 分组；完整纯度显示为 `unknown`。全部构建摘要、采样值和验证范围见 [verification.json](verification.json)。

## Presentation Lab

本地入口：`http://127.0.0.1:8145/`。点击“启动实验”正常进入游戏；冻结后切换“原始当前帧／回顾插值／外推”，拖动帧内进度。外推进度 0.5 对应权重 1.5，显示当前状态加半帧运动。重复绘制会检查已覆盖状态是否发生变化。

公共工作台与既有 TH08、TH10、TH11、TH15 观察适配器支持外推对照；TH08 使用自身工作台，其余使用公共工作台。TH15 诊断页加载共享 Gothic 字体与 CP932 映射，沿用当前字体流程。

```powershell
node common/testkit/check-extrapolation.mjs
node --test common/testkit/presentation-lab/controller-core.test.mjs common/testkit/presentation-lab/release-contract.test.mjs
node ports/th11/portable/check-presentation.mjs
node ports/th11-mp/portable/check-presentation.mjs
node ports/th15/web/scripts/build-sdl-application.mjs --presentation-lab
# 设置本地合法 DATA、共享字体和 cp932.bin 所在目录后：
$env:TH15_LAB_DATA = '本地/th15.dat'
$env:TH15_LAB_FONT = '本地/msgothic.ttc'
$env:TH15_LAB_FONTS = '本地/fonts'
node ports/th15/portable/presentation-lab/serve.mjs
```

构建在 Windows PowerShell、Emscripten 6.0.9-git 与本机 CMake／Ninja 上完成。编译临时目录放在工作区 `artifacts/issue-153/tmp`。浏览器验证使用 Playwright Chromium／SwiftShader。扩展录像测试所需的私有 Replay／atlas 文件和记录范围列在机器记录中。

## CI 构建接入

公共原生测试检出八个实际被测的 SDL 适配目录，持续验证触控路由；外推时序契约同时注册到原生 CTest。TH12 的共享头文件固定记录对齐到已合并的 `5aa34a56f`，包括 `PracticeMobilePanels.hpp` 和 `PracticePointer.hpp`，普通版与多人版分别保留自己的固定记录。

提交 `4f1d46ae8` 的上游 CI 已通过 13 个源码祖先锚点、TH12／TH15 MP 标准子树导出、Windows／Ubuntu 适配清单及必需 Checks。标准导出身份保存在机器记录中。
