# TH11 / TH15 普通版 common 依赖补齐

用户要求更新普通版依赖，双人版由另一对话继续处理。目录结构保持现状。

`RuntimeFontRaster.hpp` 目前在共享 `eagler-common` 工作区内，尚未成为可引用的新提交。此次将该文件原样同步到 TH11、TH15 的 `third_party/eagler-common/include/eagler/text/`，不以邻居目录覆盖默认依赖。三份文件的 SHA-256 均为 `e541f6f0478bf9f214be421618ef5f01d6f8c28a438e4d440b2e7d9af6282fcb`。

保留已有依赖 HEAD：TH11 为 `6fe1e24ee12257128e9e2255a73edad18e002be1`，TH15 为 `08974f6d8ee78cf1fbfcbb8bd77103f19fb00d5c`。保留各依赖目录中已有的 audio/replay 文件和共享库内其他任务的未提交改动。

这是本地依赖源码补齐，不代表新的不可变依赖版本已经固定。后续提交时，需要先让共享字体实现进入 common 提交，再更新作品的 gitlink，并验证全新检出。TH11 当前 common 分支不是共享 main 的祖先，更新时需保留它已有的练习/触屏修复，不能直接用 main 替换。

本机证据目录：`D:/workspace/eagler/artifacts/common-dependency-update-20261017/`。`dependencies.json` 记录文件哈希与现有版本，`default-plans.json` 确认两作默认解析各自的依赖目录。

字体组件检查通过，覆盖 CP932、日文字形、多字号、重复绘制、裁剪和缓存上限。15 个变体的合成构建计划检查及共享字体安装检查通过。TH11、TH15 普通版均使用各自默认依赖完成真实 release 编译，开启汉化和练习支持，没有手动指定 `EAGLER_COMMON_ROOT`。两份构建的字体输入哈希与共享原文件一致，实际 WASM 哈希与构建记录一致。结果见同目录的 `build-results.json`、`th11-build.log`、`th15-build.log` 和各构建目录的 `build.json`。

本次没有提交、推送或部署，也没有更改双人版专用依赖。

## 提交与版本固定

随后按用户“提交”的要求收口普通版依赖：

- common 字体实现、组件检查及运行入口：`8203331b11deac12f6530c987b00b8b7cdd4cfe9`。
- common 最终固定版本：`edcd08eb441e3175583c504e1a0dd63636a8a5d7`。
- TH11 更新 gitlink：`51c371d8a33e5c79f3e23464f82458a8a8d088ba`。
- TH15 更新 gitlink：`91091c8a5b01ec0af5bafb93a07e74c6bf4206f5`。

固定版本同时包含已提交的音效接口 `WebSfx.hpp`，两作原先未跟踪的该文件与共享库内容一致。两次历史合并保留原固定版本 `069367c`、`295e54e` 和本地已更新版本 `6fe1e24`、`08974f6`，四者均为最终提交的祖先。合并结果与字体提交的源码树完全相同，没有把联机进行中的改动纳入提交。

共享库其他未提交路径与操作前清单一致。作品提交仅包含 `third_party/eagler-common`，其余源码改动、Replay 工作区接口及 TH15 的双人版专用 common-netplay 继续保留，不在本次提交内。

干净源码验证使用上述三个提交生成 Git archive 快照，并按 gitlink 填入 common，不读取游戏工作区的未跟踪文件。TH11、TH15 普通版均以默认依赖完成 release 编译，开启汉化和练习支持。字体、音效和练习触屏组件检查通过；实际 WASM 字节与构建记录的长度、SHA-256 一致。汇总见 `committed-validation.json`，快照和原始输出位于本机证据目录的 `commit-validation-edcd08e/`、`committed-*-test.log` 及 `*-committed-build.log`。

这些提交尚未推送或部署。远端全新克隆的验收仍需在推送后进行。
