# 网页入口回退发布

公共加载层为全部作品提供香港 → 日本 → 国内的网页与 Runtime 文件回退。浏览器保留 `touhouonline.com` 的地址与本地存储，独立部署配置随完整 App Shell 缓存保存。当前协议见[网页入口回退](../../docs/architecture/site-recovery.md)。

## 发布身份

- App Shell：`b304e351e0057509cfc9`。
- Release：`sha256-6a99ff46e58a102a3fee39751397c95d588546279f89925e535b68ea91f43afc`。
- 发布清单：6251 个所属文件。
- 香港主入口、日本备用入口、国内备用入口与 CF 的配置、Worker 和发布清单逐字节一致，见[公网文件核对](public-final.json)。

| 顺序 | 入口 | 发布方式 |
| --- | --- | --- |
| 主入口 | `https://touhouonline.com/` | 香港 VPS，灰云单 A |
| 第二入口 | `https://jp-site.touhouonline.com/` | 日本 VPS，直接返回文件 |
| 第三入口 | `https://cn-site.touhouonline.com:8443/` | 国内 VPS，完整校验后原子切换 |
| 海外边缘 | `https://ritosa.cyou/` | 同一发布源自动同步到 Static Assets 与既有私有 R2 |

首页与大厅文档保留在公共 Origin，海外静态文件继续使用边缘节点。下载发现接口返回既有资源节点，游戏 DATA、音乐和语言包继续按资源分流协议下载。WS 保留香港 → 国内的既有顺序。

## 浏览器验证

[线上浏览器记录](browser-live-report.json)使用南京腾讯云出站，在临时测试连接中依次屏蔽香港和日本入口，覆盖：

- 香港失联后从日本取得缺失封面，SHA-256 与发布文件一致。
- 香港和日本都失联后重开浏览器，从国内取得缺失封面，保留原 Origin 与 localStorage 标记。
- 国内入口提供 th09 的不可变 Runtime 文件，按清单校验完整内容。
- 国内下载发现接口返回 `game.touhou.best`。
- 离线刷新继续显示 14 张游戏卡片。

HTTPS 浏览器夹具还覆盖相对模块 URL、Runtime 文件、冷启动配置缓存与离线刷新。行为检查覆盖连接停顿、正文停顿、503、内容校验失败、访问限制、嵌套挂载与发布配置入缓存。App Shell 构建、注册和运行服务检查覆盖 24 个用例，原有 Runtime 代际与缓存交接检查继续运行。

## 自动发布与回退资料

日本发布源上的 `eagler-web-recovery.service` 在镜像导出和 CF 发布前完成独立配置应用。准备任务复制未启用的候选包，复用不可变 Runtime 文件，重建 App Shell 和发布身份，核对完整清单后切换。香港与国内镜像按最终清单同步；国内下载分流器独立读取该镜像的 Package Descriptor 与已校验资源库存。

当前节点状态见[日本](jp-after.json)、[香港](hk-after.json)和[国内](cn-after.json)。主域名保持单 A，备用入口使用各自域名与证书。地域数据随镜像同步，备用站沿用日本 IP 访问限制。

原始发布目录 `relay-entry-20261027`、此前的回退版本及 Nginx 配置副本保留。日本 `/root/site-mirror-20261028/` 保存切换前后目录和发布准备记录；恢复旧发布时同步调整发布准备任务与镜像任务。此次发布沿用运行中的 WS 与 TURN 服务。
