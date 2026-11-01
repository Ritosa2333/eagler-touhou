# TH05 Boss 小图合批与待机演示

Boss 发弹、消弹与爆炸共用精灵图集，减少小图之间的 WebGL 提交；默认自动开火在演示中释放射击键，保留玩家手动退出 Demo 的操作。

## 发布身份

- 站点：[test.touhou.vip](https://test.touhou.vip/)，单人 TH05。
- Runtime generation：`64d4b5b98858ea126ff3c687bc8014e56ada438263e58ef9a9025f916148ccc7`。
- Wasm SHA-256：`c83645b89ec0e61576a01775159f4c347774a45de82598e3eaef6539b3020367`。
- Package revision：`c2bdca7b5b842835`；DATA、原版 SE 和 PR 原版／专辑 BGM 保持上一发布身份。
- [published.json](published.json) 保存发布目录、时间与主站／beta 的保护身份；[input.json](input.json)、[build.json](build.json) 和 [runtime-files.json](runtime-files.json) 登记发布输入与程序来源。

## Boss 绘制

原纹理 ID 保留自己的尺寸和 CPU 像素，绘制时映射到共享图集的 UV。合批保持图元顺序、混合与颜色掩码，对话脚本加载／替换 Boss 图像后重建图集。纹理更新或采样参数变化时撤销旧映射，关卡释放时回收图集。

[baseline-1x.json](baseline-1x.json) 和 [fixed-1x.json](fixed-1x.json) 使用相同采样配置，各 Boss 场景推进 1344 帧，自动射击并锁定敌人血量。以下为采样内每帧绘制提交的峰值，包含最终呈现：

| 关卡 | 原发布实现 | 当前实现 |
| --- | --- | --- |
| 一面 | 69 | 17 |
| 三面 | 159 | 19 |
| 四面 | 273 | 17 |
| 五面 | 72 | 17 |
| 六面 | 125 | 18 |
| Extra | 346 | 21 |

[render-demo.json](render-demo.json) 将同一时刻的完整画面分别以原纹理和精灵图集绘制，覆盖 48 个发弹、原生 Boss 爆炸／消弹及自机 Bomb 样本，640×480 RGBA 逐字节差异均为 0。每个爆炸与 Bomb 探针均确认实际触发。

该采样在 Windows Chromium／D3D11 上执行，提交数描述渲染工作量；设备实际 FPS 使用原生呈现计数器。

## Demo

默认自动开火先前在进入演示的第一帧注入射击键，原始 Demo 输入处理将它识别为退出指令。原生输入场景 `14` 标记演示，使自动开火释放射击键；手动点击仍走原有输入及退出流程。

- [cold-demo.json](cold-demo.json)：通过生产程序、真实协议与默认自动开火，从启动到标题待机进入演示、持续播放，再单击返回标题。
- [render-demo.json](render-demo.json)：四段普通 Demo 完整运行至返回标题，Extra 前 5200 个逻辑步骤；逐帧绘制和每四帧绘制的 RNG、弹幕、位置与场景采样完全一致。
- `public-cold-demo.json`：公开启动器的手机触控、默认自动开火及流式 Ogg，待机进入演示后继续播放五秒，再通过触摸退出。

## 回归与来源

- [browser-production-validation.json](browser-production-validation.json)：生产 Wasm 的练习开关、语言、音频、暂停重开、手柄与触控 Replay。
- [browser-native-validation.json](browser-native-validation.json)：84 组角色／关卡／起点检查。
- [unit.log](unit.log)：14 项协议、触控、原版 SE 和音乐索引检查；[presentation.log](presentation.log)：帧节奏与高刷屏幕上的原生画面交付。
- [port-check.log](port-check.log)：11/11 接口检查。
- `public-files.json`：公开文件身份校验；`source-history.json`：当前源码目录树与原 uth05win 祖先的子树核对。

测试站入口验证使用 Chromium 手机配置。先前的背景图集、激光与低帧率原生 UI 证据见[关卡与帧调度记录](../th05-frame-pacing-20261101/README.md)。
