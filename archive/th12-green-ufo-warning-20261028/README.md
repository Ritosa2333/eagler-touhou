# Extra 绿 UFO 激光预告线

TH12 普通版与多人版恢复 Extra 绿 UFO 发射前的细预告线。旋转 ANM 四边形在提交 WebGL 顶点时补齐 D3D9 到 WebGL 的半像素转换，保留原脚本的线宽、颜色、透明度和旋转。

## 定位与修复

原版 `stage07boss.ecl` 的 `BossCard6Laser` 绑定 `stgenm07.anm` 脚本 48。其子脚本 44 使用 sprite 51：512×32 的区域、`scale(1, .032)`、左侧锚点、mode 1 和加法混合。预告线由动画子节点绘制。

原有 mode 1 几何保留原版 CPU 顶点坐标，但 WebGL 像素中心与 D3D9 相差半像素。约 1 像素宽的水平预告线因此采样到纹理上下透明边缘。修复在 `Renderer.region` 提交 mode 1 顶点时统一增加 `(0.5, 0.5)`；mode 0 已通过现有取整路径处理该转换。

像素中心依据：[Microsoft 坐标系说明](https://learn.microsoft.com/en-us/windows/win32/direct3d10/d3d10-graphics-programming-guide-resources-coordinates)。原版顶点依据为两侧既有的 `native-quad-original.json`；逻辑坐标和 CPU 顶点计算继续沿用该夹具，WebGL 提交测试核对转换后的坐标。

## 验证

- Chromium WebGL2 使用本地原版贴图，水平预告线峰值亮度由 6 恢复为 254；结果见 [retail-pixels.json](retail-pixels.json)。原版资源未进入提交。
- 两侧新增浏览器回归覆盖水平、竖直、斜向预告线和完整、部分、零透明度，并检查绘制过程不改变动画状态。旧代码在水平线用例失败，修复后通过。
- 单人、多人共 126 项绘制相关测试通过，覆盖原版四边形、浏览器预告线、批处理、图层、相机、背景表面、符卡、UFO、呈现连续性和节奏。

普通版和多人版在同一主分支基点上各自修改所属目录，提交不引入目录投影历史。

## 主站发布

发布输入为修复提交 `9249907f6d7af3c161fb2d34ca816a9fd739191b`。以两台主站当时的在线版本为基底，分别保留 TH12 普通版 57 个文件、多人版 64 个文件，每个运行包替换 `renderer.mjs` 后重新生成版本身份与站点发布清单。其他游戏运行包保留原版本。

| 运行包 | 新版本 |
| --- | --- |
| TH12 普通版 | `cc7ccce73a10fef65ff860dbaa002a498812e7dc9119dc191010cacb684fe908` |
| TH12 多人版 | `204d0c9d67ed3a09b27c8e2d1d68ab995b99e58456032ed7701a5a15e86a9145` |

新域名候选完整校验覆盖 6832 个文件；旧域名覆盖 6843 个文件。旧域名的动态 `sync-version.json` 按既有发布流程核对其 schema 与当前部署摘要，再生成新清单，逐文件记录见 [source-sync-stamp-audit.json](deployment/source-sync-stamp-audit.json)。

新域名主站于北京时间 18:44:42 切换；旧域名主站于 18:55:36 切换。两个发布流程均完成 CDN 的 76 个文件探针和 3 条资源路由校验，再切换源站。公开主站及 JP、CN 镜像的两组 TH12 文件共 121 项全部通过下载哈希校验；两个 CDN 入口的相同文件也分别完成全量哈希核对。

候选身份、切换回执与公开入口验证保存在 [deployment/](deployment/)。两个主站域名的浏览器验证从线上版本目录导入实际 `Renderer` 模块，分别确认单人和多人水平预告线中连续 280 个测试像素正常显示。联机服务保持运行，测试站保留原发布版本。
