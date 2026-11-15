# 来源、许可与分发范围

原作《妖精大戦争 ～ 東方三月精》的程序、角色、美术、文字、音乐及其他资源的权利归各原权利人。本源码仓库不随附原版程序、资源或个人数据，不授予这些内容的再分发权。

本工程没有取得 TH128 官方源码，也没有复制 TH12 的完整游戏流程。它包含独立的行为还原实现、适配的通用组件、格式研究和验证工具。行为验收范围见 README 和 docs/STATUS.md。

## 适配组件

- Eagler Touhou / Tweet-META：参考 `Ritosa2333/eagler-touhou` 固定提交 `654717ac6d7b0f9e28239e230d5c965025493434` 的 `ports/th12`。通用 ANM VM/Scene、ECL 格式/栈/调度与部分渲染方式有适配或参考。原 MIT 许可保留在 `src/animation/UPSTREAM_LICENSE.txt`、`src/ecl/UPSTREAM_LICENSE.txt` 及 `licenses/Tweet-META-MIT.txt`。本归属也覆盖参考上游 renderer 的相应部分。TH12 的注释或已知行为不自动成为 TH128 行为证明。
- TH128 replay fixer：参考 `kikones34/th128-replay-fixer` 固定提交 `595d1451edbf4a55e121d45174b877f6eb0446d5`。录像编解码研究说明位于 `docs/research/replay.md` 和 `docs/research/replay-browser.md`，MIT 许可保留在 `docs/research/replay-license.txt` 与 `licenses/kikones34-MIT.txt`。

## 外部研究工具

- Touhou Toolkit：参考 `thpatch/thtk`；原研究环境使用 Windows release 12。工具二进制没有随本仓库分发。格式研究参考提交 `917a3d3858850f01276b7fb5b9327e2c24d27d5b`，不将它与 release 12 声称为同一版本。
- THPrac：早期线索参考 `touhouworldcup/thprac` 提交 `33c78d989cca8891af07b513b468d88323c4fc1e`。本轮六线与 EX 验收使用 WASM 核心原有的辅助接口，没有启动或注入 Windows thprac。

原作资源转换工具仅用于用户自己的本地资源。源码中用于解释行为的常量、地址和结构记录，不代表原版素材已获再分发许可。

## 项目许可状态

上述第三方部分继续适用各自保留的许可。当前尚未为项目自有代码选择整体许可证；不要将第三方 MIT 文本解释为对原作资源或整个仓库自动授予 MIT 许可。
