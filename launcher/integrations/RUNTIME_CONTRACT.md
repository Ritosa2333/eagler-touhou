# Runtime 语言包与 thprac 接口

本文列出语言包和 thprac 的 Host、Launcher、Runtime 交界。适配完整流程见 [`docs/adaptation/localization.md`](../../docs/adaptation/localization.md)；运行时配置入口见 [`../docs/GAME_ADAPTER_CONTRACT.md`](../docs/GAME_ADAPTER_CONTRACT.md)。

## 代码 Owner

| 内容 | Owner |
| --- | --- |
| 游戏内容与语言准备声明 | [`lib/content-definition.mjs`](../lib/content-definition.mjs)：`hostPreparation.languagePack` |
| thprac 产品能力 | [`integrations/thprac.mjs`](thprac.mjs) 与 Product Catalog |
| 语言选项清单 | Site Manifest `languageOptions`；类型和校验在 [`src/contracts/site-manifest.mts`](../src/contracts/site-manifest.mts) |
| Runtime 发布能力证明 | [`scripts/package-runtime-release.mjs`](../scripts/package-runtime-release.mjs) |
| 语言安装与选择 | [`src/launcher/language-catalog.mts`](../src/launcher/language-catalog.mts) |
| Runtime 配置字段 | [`src/contracts/runtime-protocol.mts`](../src/contracts/runtime-protocol.mts) |
| 语言包编译与静态打包 | [`scripts/prepare-th06-language-pack.mjs`](../scripts/prepare-th06-language-pack.mjs)、Host 打包脚本 |

## 语言包

发布时，Host 先根据 `hostPreparation.languagePack` 编译已选补丁，生成带版本与哈希身份的静态语言资源；Site Manifest 仅公布已经准备好的 `languageOptions`。Launcher 校验并安装所选资源及字体后再启动 Runtime。

Runtime 只读取已挂载的本地化资源树和已选语言。翻译资源缺失时回退到原版资源。原文日文字体与翻译文字体按各自资源路径加载。

开发服务器的 `/api/thcrap/...` 由 `server/thcrap-service.mjs` 提供，用于开发时编译和检查补丁；正式 Runtime 不依赖此接口。

## thprac

### 配置与认证

仅当 Product Catalog 声明 `thprac` 且具体 Runtime Release 声明支持该能力时，Launcher 才启用该功能。

启动前，Launcher 通过 Runtime `configure` 命令发送：

| 字段 | 类型 | 含义 |
| --- | --- | --- |
| `options.thpracEnabled` | `boolean` | 是否启用 thprac |
| `options.thpracLocale` | `string` | 界面语言；当前适配值为 `zh-CN`、`ja-JP`、`en-US` |

Runtime 自己物化练习配置并持有运行中的练习状态。`Module.eaglerOptions` 是部分 Runtime 的内部实现，不属于 Host 协议。

### 触控面板

现有 `touch-controls` 命令可选携带两个布尔字段：

| 字段 | 可观察效果 |
| --- | --- |
| `thpracMobilePanelsEnabled` | 启用移动端练习面板入口 |
| `thpracMobilePanelsVisible` | 显示或收起面板 |

这些字段只控制 Runtime 原生 Cheat、Advanced、Information 面板的显示，不模拟 `Backspace`、`F12` 或 `Tab` 按键，也不自行打开作弊项。重新打开后面板回到折叠状态；只有展开的 Advanced 面板接管游戏触控。缺少字段时保留 Runtime 默认行为。

### 状态与 Replay

- 练习阶段、帧、资源、分数、Rank、重开/重试和 Replay 同步由 Runtime 负责，并跟随原生 Practice 生命周期。
- 已支持的练习参数直接写入原 Replay 的 `PRAC` trailer；没有 `.rpy.thprac.json` 旁车文件格式。
- TH06 Runtime 可导出 `_EaglerThpracSaveReplaySlot(slot)`，其中 `slot` 为 `1–99`；Host 调用后负责同步 IDBFS。
- 对外显示的练习能力以 [`integrations/thprac.mjs`](thprac.mjs) 对该产品的声明为准。未移植的上游功能不因启用 thprac 而自动可用。

## 验证

```sh
cd launcher
npm run test:thcrap-static-pack
npm run test:runtime-protocol
```

开发编译服务由 `npm start` 启动。发布前用 `npm run adapter:inspect` 核对产品声明与 Runtime Release；语言安装和资源回退见 `npm run test:language-fallback:browser`。
