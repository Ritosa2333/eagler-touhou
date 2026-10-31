# TH12 主站性能更新部署

2026-10-29 已更新主站现有 TH12 单人及多人入口。用户确认更新主站多人入口；测试站保持 `/var/www/eagler-touhou-test/releases/classics-restored-20261029`。

## 发布范围

源码提交 `3d0c227ff569e08894f31471ddb1f51ddd964ccc`，同时包含上轮 `a2bd40233` 的 TH12 采集优化。每个 Runtime 替换 `animation.mjs`、`draw-scheduler.mjs`、`renderer.mjs` 三个模块，重建不可变 Runtime generation、站点清单、App Shell 及发布清单。保留当前下载修复版本的启动器及其他作品。

本次为 TH12 动画与渲染更新；`121fae5cc` 中启动器、产品策略及 relay 的零延迟入口更改没有包含在此发布中。TH12 的直接回滚原本已经使用 0 帧输入延迟。本次未重启 relay 或 TURN。

| 版本 | 原 generation | 新 generation |
| --- | --- | --- |
| 单人 | `10bcfb2d5c89bd07b0542cde36a3bb4c27f4c9b126e4f81ecfe975605ffcb82b` | `5f64412b52a03c42cddf9144a8c8fbed5e74b921a54ef848d3edc6e0b70e7cee` |
| 多人 | `57f4766e4f0971fae0af308ad0a2d9ffd069fd48a84dc6526fc14c655fd94dd3` | `17916a73deea24dd6348b128cd42ee8620a09c0565ca56a3c2058ef459eb18c4` |

日本及国内旧域源站候选均为 `/var/www/eagler-touhou/releases/th12-hud-3d0c227ff-20261029`，从当前 `download-370cd01f1-20261028` 复制并核对旧模块哈希后构造。日本在 2026-10-29 07:43:55 +08:00、国内在 07:46:02 +08:00 完成切换。国内新域镜像同步为 `th12-hud-3d0c227ff-20261029-mirror-ecb989faf10c`。

## 验证

- 两个源站发布清单及 Runtime 模块闭包验证通过。每个源站的 CDN 发布检查各通过 76 个文件及 3 个资源路由。
- `touhouonline.com`、`jp-site.touhouonline.com`、`cn-site.touhouonline.com:8443`、`touhou.vip` 各核对单人 57、多人 64 个文件的长度与 SHA-256，全部通过。
- `touhouonline.com`、`touhou.vip` 的真实 Chromium 验证通过：新模块及依赖导入、核心加载器、Wasm 编译、WebGL 初始化、站点 generation 指针；静态采样到第 60000 帧不再执行额外历史 tick，仍只保留一个当前帧。
- 核对其他 Runtime group、启动器元数据保持一致；Web 同步和镜像定时器运行中，测试站链接未变化。

[公网文件检查](public-verification.json)、[日本浏览器检查](browser-jp.json)、[国内浏览器检查](browser-cn.json)、[日本发布凭据](published-jp.json)、[国内发布凭据](published-cn129.json)。
