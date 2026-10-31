# Launcher 接入

本作品源码位于 `ports/th12-mp`。在仓库根目录构建对应模式：

```sh
node launcher/scripts/build-runtime.mjs --game=th12 --variant=multiplayer --emsdk=/path/to/emsdk
npm run ports:check -- --game=th12
```

正式输出为 `ports/th12-mp/build-eagler-multiplayer/`，入口为 `th12.html`。SDL3 应用、WebGL 2、字体和音频接入见 [开发指南](DEVELOPMENT.md)，构建和测试命令见 [BUILD](BUILD.md)。

## 资源与存档

Launcher 通过 Package Store 管理 DATA、安装代次及内容身份，通过现有资源协议提供音乐、共享字体和语言包。Runtime 在启动前挂载资源，SDL 回调负责逻辑、画面和音频。OGG 在同一 Wasm 播放器中选择流式或整曲解码。

用户文件沿用 `/savesth12`，列出、读写、删除和同步受存档白名单约束。Replay 使用共享管理界面，具体格式和播放由本作原生代码处理。失焦清理输入，页面隐藏按原生暂停及音频生命周期处理。

多人 Runtime 发布独立产品身份，练习开关为关闭状态。房间配置、席位与旁观角色通过现有多人选项传入。普通版与 MP 分别发布自己的 Wasm、清单和文件哈希。

## 组装本地站点

先完成两个模式的正式构建，再按 [内容准备](CONTENT_PREPARATION.md)生成本地素材：

```text
CONTENT/
  th12.data
  music/
  artwork/th12-card.webp
  shared/msgothic.ttc
  shared/unifont.otf
  language/catalog.json
  language/lang_zh-hans.zip
  language/lang_en.zip
```

站点组装入口统一由普通版目录维护。在仓库根目录运行：

```sh
node ports/th12/scripts/package-launcher.mjs --workspace=. --content=/private/th12-content
node launcher/scripts/serve-static.mjs ports/th12/artifacts/launcher-site 8130
```

默认普通版来自 `ports/th12/build-eagler`，MP 来自 `ports/th12-mp/build-eagler-multiplayer`。`--runtime=PATH`、`--multiplayer-runtime=PATH` 分别覆盖两者，`--output=PATH` 指定忽略的 `artifacts` 子目录。MP 目录中的同名脚本转交这个入口，并提供本目录的多人 Runtime 路径。

默认选择 `ja,lang_zh-hans,lang_en`。`--language-packs=PATH` 指定已校验的语言目录，`--artwork=PATH` 指定封面目录。缺少所选内容时报告错误，日文场景可显式传入 `--languages=ja`。

输出包含真实 Launcher、两个 Runtime、Package Descriptor、字体和本地内容。构建工具与公开程序、私有游戏素材分别按发布清单打包。

## 验收与维护

共有生命周期、房间和旁观检查见 [验证指南](../../../docs/adaptation/validation.md)，本作 SDL 自动检查见 [BUILD](BUILD.md#自动检查)。运行记录登记构建身份、输入和实际场景。

旧网页前端通过 `build-legacy-eagler.mjs` 构建，诊断核心和原版 Replay 对照见 [录像验证器](../tools/replay-verifier/README.md)。源码来源及独立目录历史见 [迁移登记](../../../docs/migration/th12-eagler-20261020.json)和[维护流程](../../../docs/adaptation/port-sync.md)。
