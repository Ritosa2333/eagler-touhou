# 神灵庙对话文字缩放

对话正文和注音使用三角滤波缩入 `text.anm` 条带，保留落在两个目标像素之间的细笔画，并分别重建 ARGB4444 的透明度和颜色通道。

## 原版依据

- 基线：PR #159，`dc3002cb3d4329c38b370d93767612e12dd57f03`。
- 日文版 `th13.exe` SHA256：`422f40aa176c9aab7d0178ce6033802b4143921a5e0e463ef99259ef6d100a58`。
- `0x45b14e` 与 `0x45b39d` 均向 `D3DXLoadSurfaceFromMemory` 传入滤波参数 `4`，对应描边与普通文字路径。
- [Microsoft D3DX_FILTER 定义](https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dx-filter)：`D3DX_FILTER_TRIANGLE` 在两个轴使用三角滤波；未设置镜像标志时边缘采样回绕。
- 原有对话缩放每个目标像素读取一个源像素；菜单与结局已有三角滤波路径。对话改为按源像素中心计算三角权重，保留原有 1024×44 至 512×20 的区域、行槽、上传范围和 VM 生命周期。

## 验证范围

| 检查 | 结果 |
| --- | --- |
| 最终对话定向回归 | 7/7，包含真实 `text.anm`、`front.anm`、细笔画通道重建和边缘黑字透明度 |
| PR 基线负控 | 两条新增用例均失败，确认能够识别原来的单点采样 |
| 网页编译与生产打包 | Emscripten 6.0.9，`portable/build-eagler.mjs` 通过 |
| 浏览器冒烟 | 同一生产 WASM 的标题、难度、选人、开局、暂停取像，页面异常为零 |
| 完整 UI 入口补充检查 | 497 项通过，2 项失败；该批次在边缘回绕补充前启动，最终版本另有上述 7 项定向回归及网页重编译 |

完整 UI 入口的两项失败为 `stage3_real_card_uses_64_sample_curve_strip`、`stage4_real_card_uses_curved_40_sample_strip`；运行目录缺少 `artifacts/verification/curve-production-20261027/card-draw-events.jsonl` 历史捕获文件。

浏览器冒烟通过真实 SDL/WebGL 绘制链运行，记录的是菜单与开局场景；此次没有完成对话帧与原版 GPU 的逐像素认证，也没有覆盖全部关卡。

本机容器数据位于 `D:\Containers\podman\data`，资源及验证产物位于 `D:\Containers\th13-work`。原版资产、字体、音乐转换结果、反汇编和取像均保存在开发目录。
