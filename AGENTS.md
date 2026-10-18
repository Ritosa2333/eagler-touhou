# Candidate workspace

这是独立迁移候选。不要修改或切换 D:/workspace/eagler 下正在进行旁观工作的原仓库、分支、依赖和构建输出。

- 默认子代理只能使用 6 luna xhigh fast。只有明确要求时才创建工作树。
- c = commit，p = push，d = deploy。实际发布、推送或部署需要对应授权。
- 公开 GitHub 评论、PR 标题和正文先由用户过眼。提交说明使用简短中文，不加类别前缀。
- 开始前阅读根 WORKSPACE.md 和 launcher/docs/playbooks 下相关主 playbook，检查目标 git status/diff。
- launcher/config/workspace.json 是目录映射权威，runtime-builds.json 是构建配方权威。
- common/ 是唯一默认共享源码，不恢复各作 common gitlink 或固定 SHA。实际提交、目录树和脏输入记录在构建来源中。
- 保留已有普通版/MP 源码归属及 MP 的两种构建模式。普通版修改必须复核对应 MP 的语义影响。
- 旧 playbook 中的分支/工作区路径不用于在本候选中选择外部源码。新开发分支从本 monorepo 主线开始，旧导入 refs 只用于来源和历史查询。
- 根 .github/workflows 是有效 CI 入口。组件内部原工作流只保留来源记录，不作为新的维护入口。
- 在旁观改动刷新、源码归属差异和 Runtime 行为验收完成前，不把本候选标成正式切换完成。
