# 东方神灵庙 · th13

《东方神灵庙 ～ Ten Desires.》v1.00c 的 C++ / SDL3 / Emscripten 网页移植，面向 [eagler-touhou](https://github.com/Ritosa2333/eagler-touhou) Launcher 接入。

本仓库根目录与 monorepo 的 `ports/th13/` 保持相同源码树。原版游戏数据、音乐、字体和录像由使用者合法提供，不随源码分发。

## 开发入口

- [开发与构建](docs/DEVELOPMENT.md)
- [适配表与验证范围](ADAPTATION.md)
- [资源格式](docs/ASSETS.md)
- [源码与第三方许可](NOTICE.md)

实现包含标题菜单、角色与难度选择、练习与符卡练习、Player Data、Replay、ECL / ANM / STD、战斗、HUD、对话、音效与音乐。实现范围与通过认证的范围分别记录在适配表，不以功能存在推定全游戏保真。

## 两种源码布局

推荐在 monorepo 中构建，复用其 `common/`、`tools/build/` 和 Launcher：

```sh
git clone https://github.com/AgentMystia/eagler-touhou.git
cd eagler-touhou
git switch feat/th13-port-20261028
# 按 docs/DEVELOPMENT.md 在容器中执行以下命令
node ports/th13/portable/build.mjs
```

独立仓库用于维护作品源码与 Git 子树往返。可将此仓库检出到 monorepo 的 `ports/th13/` 位置，或通过 Git subtree 更新该目录；不需要复制共享库或 Launcher 到本仓库。WASI harness 是局部诊断目标，不替代实际网页验证。

## 独立网页

`web/sdl-runtime/th13.html` 使用构建产物 `web/artifacts/sdl3/th13-sdl.mjs`，资源由 `prepare-assets.mjs` 从本地解包目录准备。键位为方向键、Z、X、Shift、Esc、Ctrl、Enter。部署应优先使用 Launcher 的正式资源与存档协议。

## 来源历史与合并

该仓首次发布为当前开发源码的干净导出，排除本地分析工作区与编译产物。上游通过标准 `git subtree add --prefix=ports/th13` 保留本仓提交父链；后续可用 `git subtree split --prefix=ports/th13` 校验并回推。

作品目录 PR 使用 **Create a merge commit**，不 squash、不 rebase，以保留来源历史。具体导入点及验证记录见上游 `docs/migration/`。

## 许可

实现源码采用 MIT，第三方文件保留各自许可。原版作品及素材归 Team Shanghai Alice / ZUN 及相关权利人所有；本项目为非官方移植。
