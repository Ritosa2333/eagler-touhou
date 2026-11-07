# TH05 原生帧调度、关卡背景与低帧率触控

本次针对 90 Hz 手机屏幕、三／四面背景、激光宽度和卡顿中的原生菜单交互更新测试站。游戏按原生 56 Hz 推进；本作高刷入口关闭，Runtime 强制保留呈现上限。

## 发布身份

- [test.touhou.vip](https://test.touhou.vip/)，单人 TH05 入口。
- Runtime：`8c2905cda2de1658096c792582c275e2b38fcef3506a73e86f7598e91ee49457`。
- Wasm SHA-256：`206c010d333e371e198ca6005b95c8af7cd2b93b527d09dc978ff6ef9d04d0e5`。
- Package revision：`c2bdca7b5b842835`，继续使用上一发布的 DATA、原版 SE 与 PR 音乐文件。
- 发布目录、其他站点身份见 [published.json](published.json)，输入与构建来源见 [input.json](input.json)、[build.json](build.json) 和 [runtime-files.json](runtime-files.json)。

## 90 Hz 交付与工作量

先前 56 Hz 游戏时钟和独立 60 Hz 呈现门控在 90 Hz 回调中错拍，即使 560 个逻辑帧准时完成，十秒内也约有 480 个新画面实际提交。当前在原生帧率不超过 60 Hz 时，将每个更新后的画面直接提交，空回调保留已显示画面；加速练习／Replay 的呈现继续限制在 60 Hz。时钟保留最多 1 ms 的负余额，吸收回调抖动而不改变长期逻辑速度。

[presentation.log](presentation.log) 覆盖 74 组逻辑节奏、刷新率、抖动和加速上限检查。[native-1x.json](native-1x.json) 以原生 Wasm、流式 Ogg 和 90 Hz 回调调度模拟，在 12 秒内交付 673 个更新后的画面，更新、绘制与呈现数量一致。

## 关卡背景与激光

600 个 16×16 背景图块使用同一张 256×256 图集，一次提交绘制全部图块。生产类在原有顺序、滚动坐标和最近邻采样下生成相同画面。

| 背景 | 原背景提交（含最终呈现） | 图集提交（含最终呈现） |
| --- | --- | --- |
| 三面起点 | 381 | 2 |
| 四面滚动采样 | 601 | 2 |

两份 native 报告记录一至五面及 Extra 的 18 个滚动采样；每项对比 640×480 RGBA 画面，差异字节数均为 0。六面的单图背景沿用原路径。图集随关卡释放。

WebGL 的宽线实现不再承担激光主体：彩色外沿与白色核心由实心几何绘制，1 像素预告线沿用原路径。[激光截图](laser.png) 与原生探针记录水平、垂直和斜向光束；测试中生长完成的光束外沿为 9 像素、白芯为 5 像素，水平与垂直读回均符合原宽度。

## 卡顿中的原生 UI

追帧的中间步骤推进游戏逻辑，ImGui 帧与原生鼠标边沿队列随实际绘制步骤推进。这样暂停菜单的活动控件不会在未绘制步骤丢失按下状态。诊断用例在每次点击边沿之间插入三次不绘制的逻辑更新，确认手机作弊面板展开、工具切换、暂停与点击继续均正常。

## 验证范围

- [native-1x.json](native-1x.json)、[native-4x.json](native-4x.json)：原生背景像素对照、激光读回、低帧率 UI 与一／三／四面分段采样。4 倍 CPU 降速使用桌面 Chromium CDP；记录包括实际耗时和 FPS 窗口，用于观察工作量及调度，不等同于手机芯片实测。
- [browser-production-validation.json](browser-production-validation.json)：生产程序的启动、语言、音频、暂停重开、手柄及触控 Replay。
- [browser-native-validation.json](browser-native-validation.json)：84 组角色／关卡／起点检查。
- [unit.log](unit.log)：14 项协议、触控、SE 与音乐元数据检查。
- [port-check.log](port-check.log)：11/11 接口检查。
- `public-android.json`、`public-ios.json`：公开启动器的冷启动菜单、Music Room 简体字形、原生工具及拖动。Android PointerEvent 与 iPhone UA／TouchEvent 由桌面 Chromium 模拟。
- `public-files.json`：公开文件身份；`source-history.json`：源码子树导出与原 uth05win 祖先核对。

上一轮的字体、音乐与帧率显示证据见[渲染与输入记录](../th05-render-input-20261101/README.md)。
