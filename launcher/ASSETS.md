# 资源来源与生成

本文记录 Launcher 图像、字体的来源、Host 生成方式和发布位置。许可证见 [`THIRD_PARTY.md`](THIRD_PARTY.md)；保留随资源提供的上游许可证文件和 [`NOTICE.txt`](NOTICE.txt)。

## 游戏封面

书库卡片复用 `docs/assets/game-covers/` 中已登记的原作封面。界面使用的 WebP 位于 `app/components/launcher/book-covers/`，最长边为 640px，质量为 82；同目录 `sources.json` 记录来源、原图与生成文件的 SHA-256。Vite 将这些封面纳入前端资源图与离线缓存。封面素材不决定作品是否展示，卡片成员始终由主项目的发布目录决定。

Host 从运营者本地游戏安装目录的标题图生成 `assets/th06-card.webp`、`th07-card.webp`、`th08-card.webp`、`th09-card.webp`、`th10-card.webp` 和 `th15-card.webp`。TH11、TH20 使用 Host 封面覆盖目录中的 `th11-card.webp`、`th20-card.webp`。运营者可覆盖任意作品的封面。原作封面保存在本地游戏目录，不进入公开仓库或 Runtime Release。

Host 使用 Pillow 将提取的图片及 PNG/JPEG 覆盖图转为 WebP，质量为 55、method 为 6；WebP 覆盖图保持原格式。封面仅用于页面展示，不属于游戏资源。

PWA 图标 `pwa/icon-192.png`、`icon-512.png`、`icon-maskable-512.png` 和 `apple-touch-icon.png` 在 Host 组装时由站点阴阳标记生成，属于 Host 输出。

## 字体与站点标识

站点字标使用 `public/assets/fonts/touhou98.woff2`，来自 `font-touhou98@1.0.0`。README 的描边字标为 `docs/assets/eagler-touhou-wordmark.svg`，图标预览为 `docs/assets/eagler-touhou-icon.png`。来源和许可见 [`THIRD_PARTY.md`](THIRD_PARTY.md)。

站点界面使用自行托管的 Yatra One 拉丁子集和 ChillRoundGothic 界面子集：Medium 用于正文，Bold 用于标题，Heavy 用于两张主卡片标题。子集构建脚本为 `scripts/build-site-ui-fonts.mjs`；上游版本与 OFL 声明记在该脚本及生成字体旁。GNU Unifont 作为缺字时的最后回退字体。完整日文字体与 Runtime 游戏字体由本地 Host 输入或 Runtime 包提供。

站点图标由 `font-touhou98@1.0.0` 的 U+262F 字符渲染，Host 将其复制为 `assets/favicon.ico`；运营者也可提供自定义 favicon。

公告栏使用本地 Bilibili、QQ、GitHub SVG，来自 `@iconify-json/fa7-brands@1.2.4`（Font Awesome Brands 7.3.1）。`notice-touhou-cloud.png` 是 [Touhou Cloud](https://cloud.touhou.best/) 发布的标志，项目负责人已确认可用于标识服务提供方。其他服务入口保留文字链接，其图标的再分发许可尚未记录。

作品集合菜单内嵌 Material Symbols `language`、`history`、`person`、`warning`；触控帮助内嵌 `fullscreen`、`fullscreen_exit`、`help`、`screen_rotation`。资源来自 `@iconify-json/material-symbols@1.2.86`，随站点内联，不请求图标服务。

多人房间复用 `assets/launcher-background.webp`。房间控件和输入设置使用 Phosphor regular 图标，许可文件为 [`public/assets/LICENSE-Phosphor.txt`](public/assets/LICENSE-Phosphor.txt)。仓库中玩家和旁观者头像均为占位图。

## 发布范围

[`scripts/audit-publication.mjs`](scripts/audit-publication.mjs) 维护明确的源码资源允许清单。新增 `public/assets/` 文件不会自动进入公开源码包。Host 生成的原作封面保留在本地构建输出中，不进入源码发布包或不含原作资源的 Runtime Release。
