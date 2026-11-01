# 東方怪綺談 · uth05win for Eagler

uth05win 的原生 C++ 游戏逻辑直接运行于 WebAssembly／WebGL，接入当前 Eagler 启动器的资源包、存档、输入、语言和 Replay 协议。

- 原版与 thprac 使用同一程序，由启动器的 thprac 按钮开启和关闭。
- 日文／简体中文游戏语言，以及日文／英文／中文练习界面。
- PR #143 的独立 FM 音效、原版 BGM 与“幺乐团专辑版”，关闭音乐保留音效。
- 原生 Replay 和触控位移记录，支持启动器的录像工作区。

[构建与开发](docs/DEVELOPMENT.md) · [适配表](ADAPTATION.md) · [上游说明](README-UPSTREAM.md) · [练习功能](PRACTICE.md)

源码来源为 [Goan114/uth05win-vibe](https://github.com/Goan114/uth05win-vibe)，音频与资源分离方案参考 [PR #143](https://github.com/Ritosa2333/eagler-touhou/pull/143)。GPL-3.0，详见 [LICENSE](LICENSE)。原版资源由使用者提供，制备结果与下载缓存写入被忽略的 `artifacts/`。
