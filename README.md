<h1 align="center">
  <img src="launcher/docs/assets/eagler-touhou-wordmark.svg" alt="EAGLER TOUHOU" width="312">
</h1>

<div align="center">
  <p>一个旨在<strong>极大降低玩家入门门槛</strong>的，东方 Project 官方游戏、同人游戏及其工具、联机 Mod 的<strong> Web 适配计划</strong>。</p>
  <p>
    <a href="https://qm.qq.com/q/eeUrxIltug?from=tim"><img src="https://img.shields.io/badge/QQ%20Group-1124121427-12B7F5?style=flat-square&amp;logo=tencentqq&amp;logoColor=white" alt="QQ Group 1124121427"></a>
    <img src="https://img.shields.io/badge/Chromium-%E2%89%A5108-4285F4?style=flat-square&amp;logo=googlechrome&amp;logoColor=white" alt="Chromium >=108">
    <a href="launcher/LICENSE"><img src="https://img.shields.io/badge/License-GPL--3.0--or--later-blue.svg?style=flat-square" alt="License: GPL-3.0-or-later"></a>
  </p>
  <p>简体中文 | <a href="README.en.md">English</a></p>
</div>

> [!TIP]
> 对于游戏支持情况，参见[游戏还原和适配总进度表](launcher/docs/TOUHOU_PROGRESS.md)。<br>
> 如果您已经着手做某作了，直接提交修改表格的 Issue 或 PR 即可。<br>
> 如果您对原作逆向或原作/东二适配感兴趣，欢迎加入我们的 [QQ 群](https://qm.qq.com/q/eeUrxIltug?from=tim)，我们有个开发者群。

## 概要

这计划的基本目标是用户在**任何现代设备下都能无门槛地**游玩。

在保证原游戏体验的基础上（包括保留原游戏的一些 Bug），

1. 提供高刷新率、触控适配（直触移动、菜单手势）、STG 社区热门工具（thcrap、thprac）。
2. 提供以预测回滚为主，输入延迟为辅，高延迟鲁棒性优秀的多人联机实现。
3. 提供一套能实时更新的，能管理触控布局、存档 & REPLAY 的，设备离线后也能运行的 Web App。
4. 提供一个完全无门槛，只需点点按钮就可以快速联机的联机大厅。
5. 有一个更新速度快，维护热情高，广为接受意见和批评的开发组。

## 目录

- [Runtime 适配](#runtime-适配)
  - [完整的原作体验](#完整的原作体验)
  - [触控适配](#触控适配)
  - [STG 社区热门工具](#stg-社区热门工具)
  - [多人游戏](#多人游戏)
- [启动器](#启动器)
  - [游戏与文件管理](#游戏与文件管理)
  - [多人联机大厅](#多人联机大厅)
  - [游戏包与离线运行](#游戏包与离线运行)
- [自托管](#自托管)
- [常见问题](#常见问题)
- [许可证](#许可证)
- [星标历史](#星标历史)

## Runtime 适配

### 完整的原作体验

- 在浏览器中运行游戏，支持键盘、手柄和触控操作。
- 支持原版存档和 Replay 的导入导出。原版格式可以记录的输入保持和原版互通；原版格式无法记录的适配输入使用扩展数据。
- 力求完美体验。解决了原 portable 分支中出现的 弹幕抖动、弹幕运动不流畅、闪烁、单线程切换音乐卡顿、手机卡顿 等大量问题。
- 支持高刷新率（>60Hz），th09 除外。\[感谢 [reallyportable](https://github.com/some100/th07/tree/reallyportable)\]

不同作品的适配情况有所不同，见[作品进度表](launcher/docs/TOUHOU_PROGRESS.md)。

### 输入设置

高级设置上方提供默认展开的输入设置，分为键盘和手柄改键。键盘提供「上下左右」「WASD」预设，可将自定义键位命名保存。手柄支持按设备学习按钮和摇杆、调整阈值并保存。前端手柄映射启用后优先于游戏内手柄设置，需配套更新后的游戏运行包。

WASD 布局保留原键位：方向键 / WASD 移动，Z / K 射击，X / J 使用 Bomb，Shift / L / 空格低速，C / H 沿用作品的 C 键功能。新键位在冲突时优先生效，需要配套更新后的游戏运行包。

花映冢另有「本地双人」配置，可分别为两位玩家选择键盘或手柄，保存键位并检查冲突。保留原版左右分区预设，也可搭配 WASD 与方向键布局。此配置与单人、联机改键独立，启用后接管本地双人的游戏内设备设置。

### 触控适配

![可调整位置、大小、灵敏度和低速操作方式的触控布局](launcher/docs/assets/readme/touch-controls.webp)

- 触摸轨迹 = 游戏人物移动轨迹。提供直接触摸、无限速直接触摸，除此之外，还提供轮盘等移动方式。\[感谢 [reallyportable](https://github.com/some100/th07/tree/reallyportable)\]
- 触控布局自定义。触控按键可以随意拖动、缩放（60% ~ 300%）。
- 横屏和竖屏可以分别保存布局。
- 使用超过 180% 的布局后，回退到旧版启动器会重置该保存布局。
- 提供放大镜，允许玩家在游戏中即时地用双指缩放手势去放大游戏画面。再也不怕看不清弹幕了（x
- 触控输入也可以记录和回放 Replay。
- 触控菜单手势。\[by [reallyportable](https://github.com/some100/th07/tree/reallyportable)\]
- 触控灵敏度。
- 可以调整游戏画面的默认位置。
- 提供按住按钮、切换按钮和双指操作等低速方式。

### STG 社区热门工具

![在浏览器中使用 thprac 练习菜单](launcher/docs/assets/readme/stg-community-tools.webp)

#### thprac

- 为 th06、th07、th08 和 th10 适配 [thprac](https://github.com/touhouworldcup/thprac)。
- 支持在触控设备上操作练习菜单，并提供打开 Tab Tracker 和作弊菜单的模拟按键。
- th09 不提供 thprac。

#### thcrap 多语言

- 提供 [thcrap](https://srv.thpatch.net/) 语言包适配。
- 服务器可以按作品指定语言包。你可以往里面加一堆奇怪的语言包，玩家只下载他选中的那个。
- 具体可用语言取决于作品的适配情况和站点提供的语言包。

### 多人游戏

- 提供 th06MP、th07MP、th08MP、th09MP 和 th10MP。
- th06MP 和 th07MP 均基于 [th07MP 的规则](https://github.com/sbrik1111/th07_multi_player)。
- th06MP、th07MP、th08MP 和 th10MP 支持 2～3 人，th09MP 支持 2 人。
- 支持暂停后按 R 或者手动重开本局。
- 可以保存多人 Replay。
- 单人和多人的存档和 Replay 相互隔离，不过允许共用设置。

## 启动器

### 游戏与文件管理

![Eagler Touhou 启动器与游戏管理界面](launcher/docs/assets/readme/launcher.webp)

- 提供统一的游戏入口，可以选择作品并启动对应的 Runtime。
- 提供导入导出存档、Replay 等文件的界面。
- 管理已经安装的游戏包，以及游戏数据、OGG、字体和语言包。

### 多人联机大厅

![支持玩家与旁观者的多人联机大厅](launcher/docs/assets/readme/multiplayer-lobby.webp)

- 提供了一个大厅，支持创建房间后输入房间号加入、选择角色和难度、玩家都准备后再开始、旁观者。
- 可选角色和难度取决于作品，包括对应作品的 Extra 和 Phantasm。
- 优先使用 WebRTC，也可以通过 WebSocket Relay 连接。
- 支持旁观。旁观者不占玩家席位，只能观看，不会参与游戏状态（但不能中途进入旁观）。

### 游戏包与离线运行

- 可以从站点安装游戏包，也可以导入自己合法持有的游戏包 ZIP。
- 游戏包安装后保存在浏览器本地，游戏数据、OGG、字体和语言包可以分别管理；运行组件由站点统一提供。
- 玩家导入的原版游戏数据不会上传服务器，只会保存在浏览器本地。

在 HTTPS 或可信 loopback 等安全上下文条件下，已经安装的游戏和启动器**可以被离线运行**。在玩家离线或服务器宕机时，玩家即使刷新了页面也可以使用本地保存的文件正常进行游戏。

## 自托管

部署者可以根据资源和带宽条件选择三种模式：

- **Hosted**：站点直接提供启动器、运行组件和全部游戏资源。玩家可以直接安装，部署最集中，站点需要承担全部资源流量。
- **External**：用户站点提供启动器和运行组件，游戏大文件由单独的完整资源站或 CDN 提供。玩家仍可直接安装，前端服务器无需承担大文件流量。
- **Import**：站点只提供启动器和运行组件，不发布原版游戏数据。玩家导入自己合法持有的游戏包 ZIP，适合没有 CDN 流量预算或不希望托管原版资源的部署者。

三种模式共用同一套前端、浏览器本地游戏包和离线运行能力。完整部署方法见[自托管指南](launcher/docs/SELF_HOSTING.md)。

## 常见问题

详见[常见问题](launcher/docs/FAQ.md)。

## 许可证

启动器采用 [GNU General Public License v3.0 or later](launcher/LICENSE)。各游戏 Runtime 的许可证见 `ports/` 下对应源码目录。

完整的第三方来源、素材归属和许可信息见 [THIRD_PARTY.md](launcher/THIRD_PARTY.md) 与 [ASSETS.md](launcher/ASSETS.md)。

## 星标历史

[![Star History Chart](https://api.star-history.com/chart?repos=yomotsuhisami/eagler-touhou&type=date&legend=top-left)](https://www.star-history.com/?repos=yomotsuhisami%2Feagler-touhou&type=date&legend=top-left)
