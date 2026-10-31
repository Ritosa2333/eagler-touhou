# 东方星莲船多人合作版

[开发指南](docs/DEVELOPMENT.md) · [适配与验证](ADAPTATION.md) · [工作区](../../WORKSPACE.md)

[SDL3 / GLES 3 应用](portable/sdl/README.md)拥有正式多人入口的画面、菜单、声音、换面及结局／成绩流程，链接本目录多人核心。浏览器边界保留启动器 RPC、WebRTC、安装资源和存档；输入确认、回滚和已确认录像继续由 C++ Session 管理。

`ports/th12-mp` 是星莲船的独立多人源码，支持双人、三人合作。普通版保留在 `ports/th12`；多人版沿普通版来源历史维护，并使用共用的联机、回滚、观战和输入录像组件。

## 合作规则

- 六种机体独立持有 P 点、残机、Bomb、擦弹和碰撞状态；得分、Rank、飞碟槽及残机／Bomb 碎片共享。
- 20 像素内连续五次射击，赠送一个 P 档位给相对火力最低的队友；自动收取的 P 优先分给附近未满火力的玩家。
- 20 像素内连续五次低速，赠送残机或救起幽灵队友。关底跨关复活、180 帧全灭判定、16 帧决死和团队 Boss 伤害规则沿用 TH10／TH11 多人实现。
- 玩家死亡保留共享飞碟槽；各席自机、子机、射击及 Bomb 使用独立所有者。

## 房间与回放

进入房间时预加载资源并显示动态标题；标题不进入 Demo，开局复用已加载的核心和资源。联机支持手动输入延迟、开局自动实测及固定 12 帧回滚上限。旁观复用共用的阶段初始化与时间轴，可中途接入、暂停和回看。

暂停、确认、成绩排名、姓名输入和 25 个 Replay 槽位复用星莲船原版界面。多人录像记录各席已确认输入及构建身份，使用 `th12_mp_XX.rpy` 槽位；自动保存另留时间戳录像。

多人版使用独立 IndexedDB `/savesth12mp`，原生文件路径保持 `/savesth12/`，启动器按对应数据库读取多人录像。首次运行自动复制旧共享库中的 `th12_mp_*.rpy`，既有普通版文件继续由普通版管理。

## 构建

在 monorepo 根目录使用已配置的 Emscripten 工具链：

```sh
npm run build:runtime -- --game=th12 --variant=multiplayer --emsdk=工具链目录
```

直接构建多人代码包：

```sh
node ports/th12-mp/scripts/build-eagler.mjs
```

输出 `ports/th12-mp/build-eagler-multiplayer/`。普通版和多人版共用合法导入的 `th12.data`；代码包不包含原版素材。

## 维护

初始分支 `feature/th12-multiplayer` 从当前 TH12 普通版独立派生。来源登记见 `docs/migration/th12mp-eagler-20261024.json`；同步基线见 `docs/adaptation/port-sync.json`。规则适配位于 `src/game/Multiplayer.*`，会话与回滚位于 `src/multiplayer/`，正式网页协议入口为 `site/game-sdl.mjs` 与 `src/multiplayer.mjs`；`site/game.mjs` 通过 `build-legacy-eagler.mjs` 单独构建。

原版游戏、角色、贴图、文字及音乐版权属于上海爱丽丝幻乐团。手写移植代码及原版内容的许可范围见 [NOTICE](NOTICE.md)。
