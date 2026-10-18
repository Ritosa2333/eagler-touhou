# 语言包与字体

Host 将 thcrap 补丁编译为静态语言 ZIP。启动器安装所选语言，再通过 `configure` 交给 Runtime。Runtime 从 `/thcrap/<game>/` 读取覆盖内容，缺少的文件或条目回到原文。

## 1. 找到修改位置

| 内容 | 源码 |
| --- | --- |
| thcrap 仓库、依赖与下载顺序 | [`thcrap.mjs`](thcrap.mjs) |
| MSG、结局、字符串与替换图编译 | [`thcrap-compiler.mjs`](../server/thcrap-compiler.mjs) |
| 字符串与 ASCII 映射 | [`thcrap-string-contract.mjs`](../server/thcrap-string-contract.mjs)、[`thcrap-ascii-contract.mjs`](../server/thcrap-ascii-contract.mjs) |
| THTK 调用 | [`thtk-runner.mjs`](../server/thtk-runner.mjs) |
| ZIP 与描述符 | [`thcrap-static-pack.mjs`](../server/thcrap-static-pack.mjs) |
| 字体子集 | [`subset-font.py`](../scripts/subset-font.py) |
| 各作资源声明 | [`content-definition.mjs`](../lib/content-definition.mjs) |
| 原文字体栅格化 | [`RuntimeFontRaster.hpp`](../../common/include/eagler/text/RuntimeFontRaster.hpp) |

## 2. 生成语言包

准备日文原始档案、THTK 的 `thdat`/`thmsg`、允许相应用途的 Unicode 字体，以及 Python 字体处理依赖（见[自托管](../docs/SELF_HOSTING.md)）。下面从 `launcher/` 生成 TH11 简体中文包，路径均替换为本地文件：

```sh
node scripts/prepare-th06-language-pack.mjs \
  --game th11 \
  --language lang_zh-hans \
  --archive /games/th11/th11.dat \
  --thdat /tools/thtk/thdat \
  --thmsg /tools/thtk/thmsg \
  --font-file /fonts/unifont.otf \
  --output /prepared/th11-language
```

`--game` 的作品范围由资源声明决定。TH06 多个档案用 `--archives`，路径以分号连接并整体加引号。Windows 可用 `--temporary-root` 指定临时目录。

把语言改为 `lang_en`，再次运行到同一作品的输出目录。产物为 `catalog.json` 与 `language/lang_*.zip`。ZIP 包含编译补丁和它所需的字体子集，`localization/options.json` 指定字体。

## 3. 接入站点与离线包

按[Host 配置参考](../docs/SELF_HOSTING_REFERENCE.md)提供语言目录；底层 `scripts/package-server.mjs` 使用 `--th11-language-packs` 等对应作品参数。发布清单从语言 catalog 生成语言选项与文件哈希。

需要把已组装站点的资源做成可导入 ZIP 时，在 `launcher/` 运行：

```sh
node scripts/package-offline-game.mjs /assembled/site th11 /packages/th11.zip
node scripts/verify-offline-game-package.mjs /packages/th11.zip th11
```

修改补丁后重新生成语言包、描述符与包含它的站点或离线包。不同语言通过同一套依赖到叶子补丁的覆盖顺序合并；字体提供者优先级只用于字体选项。

## 4. 各作格式

| 作品 | 编译与运行时关注点 |
| --- | --- |
| TH06/07/08 | 按资源声明读取本作档案，保留原生文字时序和字形度量；音乐、符卡与界面字符串分别核对 |
| TH09 | `thdat/thmsg` 使用作品号 9；故事 `plNN.msg`、对战 `plNN_match.msg` 和 `endNN.end` 分开处理 |
| TH10 | 自动对话的无参数 7/8/10 指令结束当前文本框，保留原始 timecode 与等待规则 |
| TH11 | 用日文 `th11.dat`；故事走 `MSG_TH11`，结局走 `END_TH10`，对话 opcode 25 按 OP_DELETE 处理；7/8/9/11 的无参数结束指令要识别 |
| TH15 | MSG、结局与 Ruby 标注分别测量；运行时文字宽度、气泡布局、注音位置使用相同度量接口 |
| TH20 | 语言能力按产品与构建配方声明启用；实现状态与验证项见[适配表](../../ports/th20/ADAPTATION.md) |

TH11 对话保留 tabstop、对齐与临时字形样式：`<t$Suika >` 显示并定义宽度，后续 `<l$>` 使用该 tabstop。日文原文与有语言包的 UTF-8 路径都要核对场景服务到实际文字绘制的连接。

练习界面跟随当前游戏语言：`ja` → `ja-JP`、`lang_zh-hans` → `zh-CN`、其余 → `en-US`。是否启用 thprac 由该变体的构建配方决定。

## 5. 验证

在 `launcher/` 先运行 `node tests/test-thcrap-compiler.mjs`、`node tests/test-thcrap-static-pack.mjs` 和 `node tests/test-thcrap-patch-precedence.mjs`。

修改 TH10/TH11 自动对话时，用本地原始档案再运行：

```sh
node tests/test-thcrap-timed-dialogue.mjs th11 /games/th11/th11.dat /prepared/th11-language /tools/thtk /validation/th11-dialogue
```

随后打开真实 Runtime，逐项检查标题、菜单、剧情、结局、Music Room、符卡、长行与 Ruby。离线导入检查同时覆盖字体与语言资源。记录所用语言包哈希、场景和截图，填写[语言与练习适配表](../../docs/adaptation/localization.md)。
