# 第三方软件与资源致谢

本文列出 Launcher 和 Host 使用的第三方代码、字体、图标与格式。资源附带的许可证保留在资源旁；根目录 [`LICENSE`](LICENSE) 和 [`NOTICE.txt`](NOTICE.txt) 按原样保留。

## 浏览器与构建依赖

| 组件 | 来源 | 许可证 | 用途 |
| --- | --- | --- | --- |
| `webaudio-tinysynth@1.1.3` | [g200kg/webaudio-tinysynth](https://github.com/g200kg/webaudio-tinysynth) | Apache-2.0 | MIDI Web 构建中的免采样 General MIDI 合成；随包文件为 `public/vendor/webaudio-tinysynth.min.js`。 |
| `fflate@0.8.3` | [101arrowz/fflate](https://github.com/101arrowz/fflate) | MIT | 导入、导出保持 `.rpy` 字节不变的原格式 Replay；随包文件为 `public/vendor/fflate.min.js`。 |
| `marked@18.0.12` | [marked.js.org](https://marked.js.org/) | MIT | 将 FAQ、首次使用说明和多人指南 Markdown 构建为静态 HTML；属于构建依赖，不在 Launcher 运行时加载。 |

重新生成随包 JavaScript：

```sh
npm ci --ignore-scripts
npm run vendor
```

## 字体

| 字体 | 来源与作者 | 许可证 | 用途与随包文件 |
| --- | --- | --- | --- |
| Touhou 98 | [`thwiki/font-touhou98`](https://github.com/thwiki/font-touhou98)，`font-touhou98@1.0.0` | MIT | 站点字标；`public/assets/fonts/touhou98.woff2`；README 字标 SVG 基于同一字体生成。 |
| Yatra One | [Google Fonts, Yatra One](https://github.com/google/fonts/tree/main/ofl/yatraone)；Copyright 2014 The Yatra Project Authors | SIL Open Font License 1.1 | 拉丁字母和数字；`public/assets/fonts/yatra-one-latin.woff2`，许可证 `OFL-YatraOne.txt`。 |
| ChillRoundGothic | [Warren2060/ChillRoundGothic](https://github.com/Warren2060/ChillRoundGothic)；Copyright 2023 The ChillRoundGothic Project Authors | SIL Open Font License 1.1 | 界面文字；`public/assets/fonts/` 中的 Medium、Bold、Heavy WOFF2 子集，许可证 `OFL-ChillRoundGothic.txt`。Medium 用于正文、Bold 用于标题、Heavy 用于两张主卡片标题。 |
| GNU Unifont 15.1.05 | [Unifoundry](https://unifoundry.com/unifont/)；Copyright 1998–2024 Roman Czyborra, Paul Hardy, Qianqian Fang, Andrew Miller, Johnnie Weaver, David Corbett, Nils Moskopp, Rebecca Bettencourt, Ho-Seok Ee et al. | 本分发采用 SIL Open Font License 1.1；Unifont 也以 GPLv2+ 和 GNU Font Embedding Exception 提供。 | 缺字回退；`public/assets/fonts/unifont-site.woff2`，许可证 `OFL-Unifont.txt`。 |

## Host 适配器与 Runtime 工具链

- 翻译适配器参考 [thcrap](https://github.com/thpatch/thcrap)，采用 Unlicense；用于发现、校验并准备选定语言包资源。
- Practice 适配器参考 [thprac](https://github.com/touhouworldcup/thprac)，采用 GNU GPL v3；接入受支持的 Practice 选项和 Replay 元数据，并映射为 Web Runtime 行为。Windows DLL 不属于此 Web 接入路径。
- Host 使用 [thtk](https://github.com/thpatch/thtk) 12 中的 `thdat`、`thmsg` 进行归档和消息转换，采用 BSD 风格两条款许可证。自托管包不分发 thtk 二进制文件。Windows Host 下载固定的官方 Release ZIP，校验 SHA-256 `f6acc00f377b6537e8d504794aec8445cb1e0d6490d2c89d56b3315677765154` 后存入本地 `.cache/` 工具缓存；其他平台将 `thdat`、`thmsg` 安装到 `PATH`。
- 游戏 Runtime 使用 SDL 系列库和 Emscripten；组件版本与许可证说明位于对应 Runtime 源仓库或工具链中。上游项目：[SDL](https://github.com/libsdl-org/SDL)、[SDL_image](https://github.com/libsdl-org/SDL_image)、[SDL_ttf](https://github.com/libsdl-org/SDL_ttf) 和 [Emscripten](https://github.com/emscripten-core/emscripten)。

## 图标与服务标志

- 公告栏图标来自 `@iconify-json/fa7-brands@1.2.4`（Font Awesome Brands 7.3.1），采用 CC BY 4.0。Bilibili、QQ、GitHub SVG 位于 `public/assets/notice-bilibili.svg`、`notice-qq.svg` 和 `notice-github.svg`；该包也是 Mizuki 前端使用的同一图标集。
- 站点页眉使用 `@iconify-json/material-symbols@1.2.86`，来源为 [Google Material Design Icons](https://github.com/google/material-design-icons)，采用 Apache License 2.0。内嵌图形为 `language`、`history`、`person`、`warning`；触控帮助还内嵌 `fullscreen`、`fullscreen_exit`、`help`、`screen_rotation`。
- 房间和输入控件使用 [Phosphor](https://github.com/phosphor-icons/core) 中未修改的 regular 图标，包为 `@phosphor-icons/core@2.1.1`，采用 MIT。随包图标包括 caret、copy、gear、sliders、chart、user 和 users；许可证为 `public/assets/LICENSE-Phosphor.txt`。
- `public/assets/notice-touhou-cloud.png` 是 [cloud.touhou.best](https://cloud.touhou.best/) 发布的 Touhou Cloud 标志；项目负责人确认可用于标识服务提供方。

资源放置、Host 生成封面和发布规则见 [`ASSETS.md`](ASSETS.md)。
