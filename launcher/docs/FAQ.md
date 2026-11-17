# 常见问题

感谢 Goan114 的整理，以及各位玩家的测试与反馈。

如果下述内容未能解决问题，请通过 [Discord](https://discord.gg/HwVz9ZTSF)、[QQ 群](https://qm.qq.com/q/eeUrxIltug)或 [GitHub Issues](https://github.com/Ritosa2333/eagler-touhou/issues/new/choose) 提交反馈。

## 游戏闪退、卡顿、黑屏、加载异常或断触怎么办？

请先尝试使用推荐浏览器：

| 设备 | 推荐浏览器 |
| --- | --- |
| Android | 最新版 [Edge](https://www.microsoft.com/edge/download) 或 [Chrome](https://www.google.com/chrome/)，不建议使用 Via |
| 华为设备 | 可以尝试系统浏览器，不要使用卓易通或 Via |
| Windows | 系统自带的 Edge、[Chrome](https://google.cn/chrome) 或 [Firefox](https://www.firefox.com/) |
| Windows 7 及更早版本 | [Supermium](https://www.supermium.net/) |
| 苹果设备 | 浏览器不限 |

不建议使用系统自带浏览器和各种国内浏览器。例如目前已知夸克浏览器会断触。

如果使用推荐浏览器后仍有问题，请向我们反馈。

> [!NOTE]
> 如果旧版浏览器无法进入首页，或 WebGL 2.0 检测失败，请查看[设备与浏览器兼容指南](compatibility.html)，其中有 Supermium 安装和 Chrome OpenGL 后端设置步骤。

## 哪里能找到操作说明、攻略和剧情？

- **操作说明**：启动游戏后，点击右上角的「?」。
- **原版玩法说明和剧情**：查看[东方 Project 附带文档总览](https://omake.thwiki.cc/)。
- **游戏攻略**：查看 [THBWiki 游戏攻略页](https://thbwiki.cc/%e6%b8%b8%e6%88%8f%e6%94%bb%e7%95%a5)。例如，想了解樱点系统怎么玩，就可以在那找。

如果对东方 Project 的世界观和游戏玩法了解不深，我们更建议先查阅 THBWiki，它对游戏机制的解析十分全面易懂。

## 游戏可以存档吗？

「存档」有两种不同的意思：

### 中途退出后，能继续这一局吗？

**不能。** 原版游戏的机制就是如此：退出后，不能在下次打开时继续使用这一局原先的残机等状态。一局游戏平均约 25 分钟。

### 通关记录等游玩数据能保存吗？

**可以。** 通关记录、游玩时长等非游戏内状态数据可以保存。经过测试，存档与原版游戏互通，存档文件为 `score.dat`。

> [!WARNING]
> **不同作品的存档不能混用。** 例如，th06 的 `score.dat` 不能用于 th07。

## 还会增加其他作品吗？

请看[作品适配进度表](faq.html?view=progress)。

您可以通过启用「开发人员选项」中的「测试版功能」，来检查即将上架的新内容。

## 下载不了资源怎么办？

1. 确认没有使用 VPN。
2. 避免使用接管网页下载的浏览器插件。目前已确认 Internet Download Manager 等插件可能与网页资源加载不兼容。
3. 如果仍然无法下载，请使用导入包。

站点成功打开过一次后，即使服务器随后不可用或设备断网，刷新或重新打开页面也可以进入启动器；较新的浏览器几乎都支持。

## 遇到联机问题怎么办？

请不要使用 WebRTC Control 等会影响 WebRTC 的浏览器扩展。

联机卡顿一般有两个原因：

1. 网络波动或丢包（如 50ms->200ms 时会出现显著卡顿，这是完全正常的，如果您接触过非想天则等游戏就会知道）。
2. 游戏性能问题。

如果您网络波动较大，那么就不建议玩联机了。只要一个人波动，就会让一起联机的所有人都一起卡顿。

## 支持项目

如果觉得这个项目很好，欢迎在 [GitHub](https://github.com/YomotsuHisami/eagler-touhou/) 页面点亮右上角的 Star。
