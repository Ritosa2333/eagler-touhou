# 海惠堂单机文件导入

東方海恵堂位于 `ports/thkkd`，包含 C++ 游戏源码、浏览器后台、资源清单、固定依赖、构建脚本、测试与开发说明。

## 来源与范围

游戏工程来源为 [Renko2333/thkkd](https://github.com/Renko2333/thkkd)，原始 Windows 核心基线为 `64d48dc366f2d57210dbdac25be82e0e71b2641d`。网页适配加入 WebAssembly 构建、WebGL2 绘图、浏览器音频与输入、持久化存档和原版及扩展 Replay。`src/core` 与生成的 `src/render` 沿用已验证的核心与显示计算。

327 项原始素材保留在本机被 Git 忽略的 `assets`，来源哈希与时间清单保存在 `assets.metadata.json`。网页从玩家提供的原版 `dat.dxa`、`bgm.dxa` ZIP 生成浏览器缓存，原始资源保留原字节。资源使用方式见 [README](../../ports/thkkd/README.md)，依赖与字体来源见[第三方说明](../../ports/thkkd/THIRD_PARTY.md)。


## 开发与验证

[开发指南](../../ports/thkkd/docs/DEVELOPMENT.md)提供工具链、构建和本地启动入口。[适配表](../../ports/thkkd/ADAPTATION.md)记录公共接口检查；其中引用的历史验收归档注明各自构建、环境和范围。当前资源流程见[原版导入验证](../../archive/thkkd-original-import-20261126/README.md)，此前的运行检查见[接入验证](../../archive/thkkd-import-20261126/README.md)。
