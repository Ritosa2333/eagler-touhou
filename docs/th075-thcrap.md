# th075 语言资源准备

th075 使用统一的静态语言链：解析补丁栈、校验下载 CRC、编译替换资源、发布语言目录与 ZIP 身份，再由启动器在运行前安装。Host 与 Package Descriptor 共用这套发布身份。

## 编译与发布

格式适配器位于 `launcher/server/th075-thcrap-compiler.mjs`，接受日文原版 `th075.dat`、`th075b.dat` 或已验证的 `th075.data`。图像、立绘和菜单按原生 Base/Patched 归属安装，输出包含替换图像、文本和经过许可核对的 GG75 字体。结束字幕补丁 `scenario.txt.jdiff` 按结束脚本顺序编译，普通 Story 对话保留原有位置映射。

从仓库根目录运行：

```powershell
node launcher/scripts/prepare-th06-language-pack.mjs --game th075 `
  --archive D:/private/th075.data --language lang_zh-hans `
  --font-file D:/fonts/unifont.otf --output D:/prepared/th075-languages
```

英文补丁栈使用 `--language lang_en`。默认字体许可为内置 Unifont OFL；指定其他字体时同时提供 `--font-license`。Python 依赖使用 Host 配置中的 Pillow 和 fonttools。下载缓存按预期 CRC 与源 URL 建立，每次命中重新核验，编译 ZIP 使用 SHA-256 标识。

`--th075-language-packs=...` 将目录交给共用服务端包发布器。源码开发通过 `EAGLER_TH075_LANGUAGE_PACKS` 指定已准备的语言目录。

## 语言覆盖

简体中文资源检查覆盖 101 个图像 bank、10 份结束脚本、Music Room 标题和注释，以及这些内容所需的全部字形。缺少替换项时采用日文原资源，生成的 options 文件记录每项实际覆盖范围。

英文使用 SHA-256 固定的 IaMP English Translation v1.1 `th075e.dat`，由 `launcher/scripts/prepare-th075-english-static.mjs` 生成 574 个图像替换和全部 11 名角色的译文脚本。编译器完成资源别名映射、7 个 512px 符卡名到原有 256px 槽位的适配，以及 6 组结束对话的分页。原生脚本继续管理控制流。

```sh
node launcher/scripts/prepare-th075-english-static.mjs /private/th075e.dat /private/th075.data /fonts/unifont.otf /prepared/th075-languages
```

选项帮助来自可执行文件中的文本，7 条描述编码到 `localization/options.json`，使用语言包字形 bank。英文沿用 v1.1 译文，简体中文采用开发包译文。Runtime 校验全部字节数组，并在字段缺失时采用日文内容。

### 中文剧情与通关资源

简体中文剧情通过 `--th075-story-resources D:/private/resources/chinese` 接入现有开发包的十一份 GBK 压缩中文脚本。编译器逐项核对对白位置与日文脚本的控制位置，提取译文，并保留 thpatch 结束字幕。繁简转换使用 `opencc-python-reimplemented==0.1.7` 的 `t2s` 配置。源脚本 SHA-256 和对白覆盖数量写入 `localization/options.json`，同时参与准备缓存身份；语言 ZIP 发布替换内容。

通关姓名登记读取 Base 版 `data/system/replay.dat`，回放列表和保存菜单读取 Patched 版。编译器为两个版本生成独立绑定尺寸与 CRC 的替换资源，复用图像前核对槽位几何。Runtime 保留原有资源选择与严格校验，存档及 Replay 格式保持一致。

生产启动入口同时接收 `/thcrap/th075/images/base/data/system/replay.dat.bin`。
Base 变体按这一确定路径授权，其余图像、文字、字体与元数据继续分别校验。
`test-th075-language-configure.mjs` 覆盖普通版及 MP 的 configure 路径准入；
`test-th075-language-upgrade-browser.mjs` 使用历史导入 ZIP、真实 Package Store
和生产 Runtime，核对导入、语言包更新、DATA 复用、刷新启动与中文对白呈现。

## 验证职责

- 编译器、静态包与补丁优先级检查负责资源内容和发布身份。
- 原生检查逐项验证替换资源与 11 名角色的脚本安装。
- 启动器浏览器检查覆盖日文、简体中文、英文的安装与实际呈现，并记录菜单进入延迟和堆内存。
- 类型检查和自托管依赖闭包检查验证构建与交付入口。
- th06–08 上游合同 L3 检查使用锁定提交的源码检出，按该检查的输入配置独立复现。

2026-10-20 回归检查：七项编译器测试通过；真实日文 DATA 的诊断浏览器检查覆盖全部替换解码、译文渲染、十一名角色，以及姓名登记与回放场景 10／5／7／10。诊断包与生产包分别构建，完整通关游玩作为独立验收项目。
