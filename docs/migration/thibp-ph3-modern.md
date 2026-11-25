# 东方鬼葬剑 Web 运行时的源码来源

本作将 [ph3-modern 的固定提交](https://github.com/Ritosa2333/ph3-modern/commit/12ac7141b5cdcd9cabd4479ef76e6453f19fcdf3)作为构建依赖。构建脚本校验源码归档的 SHA-256，再应用包含近邻缩放、输入、退出、高刷新率和 FPS 显示改动的 Web 补丁。引擎的许可由固定版本的 LICENSE 提供。[运行时补丁](../../tools/ports/thibp/engine.patch) · [构建入口](../../tools/ports/thibp/build_runtime.py)

发行版 1.10b 提供游戏内容，本机导入流程负责挂载。Web 适配登记为 `thibp` 试验项目。构建产物列出源码和运行时文件哈希，存档与录像使用本作的独立命名空间。[本地联调](../../tools/ports/thibp/README.md)
