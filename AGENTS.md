# 协作约定

在当前 monorepo 工作区开发。D:/workspace/eagler 下原有的旁观工作、分支、依赖和构建输出保持独立。

- 默认子代理使用 6 luna xhigh fast。明确要求后再创建工作树。
- c = commit，p = push，d = deploy。提交、推送和部署分别取得对应授权。
- 公开 GitHub 评论、PR 标题和正文先由用户过眼。提交说明使用简短中文，不加类别前缀。
- 开始前阅读 [WORKSPACE.md](WORKSPACE.md) 和相关[适配分项](docs/adaptation/README.md)，检查目标的 git status/diff。
- 每次修改同步对应文档和适配记录。文档遵守发布会原则，围绕用途、解决思路和可复核的结果组织，语言简短直接，禁止防御性写作。文风要求集中在本文件，不写入其他项目文档。
- `launcher/config/workspace.json` 管目录映射，`runtime-builds.json` 管构建配方。
- `common/` 是唯一默认共享源码。构建来源记录实际提交、目录树和未提交输入；各作直接使用同仓库 common。
- 保留普通版与 MP 的源码归属及两种构建模式。普通版改动同时复核对应 MP 的语义影响。
- 新开发分支从 monorepo 主线开始；旧导入 refs 用于来源与历史查询。
- 根 `.github/workflows/` 是有效 CI 入口。组件内部原工作流属于来源记录。
- 涉及源码迁移时，完成旁观改动同步、源码归属核对和 Runtime 行为验收后，再记录切换完成。
- 首屏中英文 README（根目录及 launcher 入口）和 `docs/TOUHOU_PROGRESS.md` 及其兼容入口保持现状。
- 设计和审计以工程化、跨平台、长期维护为原则。功能状态、目录保留和删除均提供实际代码、构建或测试依据；不把猜测写成需求，也不因迁就现状保留不合理设计。
