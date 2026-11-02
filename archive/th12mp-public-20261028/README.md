# 星莲船正式发布

th12 普通版和 2–3 人多人版进入正式作品目录，主站、Cloudflare、香港入口、国内备用入口与 test 统一提供新版多人运行包。普通版和多人版均展示日语、简体中文、英语及高刷选项，游戏 DATA、语言包和共享字体沿现有资源分发链下载。

129 原域名 `touhou.vip` 的独立目录和 CF 发布目标见[原主站入口同步](cn129.md)。

## 版本身份

主线同步至 `0dde257ec92cb8cad888dcdfd012aba6a0f6a00a`，包含已合并的 th12 多人版与独立存储、历史 Replay 迁移及依赖锁定。公共产品目录移除 th12 的测试站限制，Launcher 与 Relay 共用相同产品契约。

| 发布项 | 身份 |
| --- | --- |
| 主站 Release | `sha256-7c564562be7ea5864c80520f597deedb276e347a373cda75def357807396a9d9` |
| 主站 App Shell | `cd0df3fadea89e3db219` |
| test Release | `sha256-c1e0869d72aad75251228fb6813802b1fa5b799ad6119a8c531454de22eb95ef` |
| test App Shell | `41504c3074116adc48c6` |
| th12 多人 Runtime | `b0e4d32065897df419584872ebc40deb7030f6a3f44e0181630414969d557496` |

多人运行包由当前主线源码构建，发布 64 个文件，WASM 为 807539 字节，声明 `product: th12mp`、`profile: multiplayer`、语言可用、thprac 关闭。多人存储使用独立的 `/savesth12mp` 数据库身份，游戏内部文件目录沿用 `/savesth12`，既有多人 Replay 由当前迁移规则接续。

主站普通版延用 `bfcf985060e4c1c9934e67b145f9967230449de26bc2e9b6ab1874e2a1bea262`；test 普通版延续其现有实验代次 `6a8277aa6b3dc86631881597558ec7396b90f6019b5728a77ac7f70f0240abac`。test 同时按原完整 SHA-256 补齐现有 Runtime 声明的 24 个字体文件，各实验作品的当前代次保持一致。

## 发布与服务

日本发布源先准备封闭的 Runtime 代次与前端，再由 Cloudflare 提前装入新资源，随后切换主站及 test。香港、国内备用入口从日本发布源导出并核对同一 Release。边缘预发布核对 70 个文件和 3 条资源路由，资源存储本轮上传量为 0；自动同步恢复使用 `/var/www/eagler-touhou/current`。

正式 Relay 使用 `relay-th12mp-20261028` 构建，支持 th12mp 的三人席位、RTC 信令和 `0x43` 旁观标记，继续监听原端口。进程 PID 为 19159，可信代理为 `127.0.0.1`，公网代理覆盖 `X-Real-IP`。日本 TLS 桥接 PID 29801、香港 CF TURN 代理 PID 1868 保持运行。

test 保留自己的实验 Relay 实现，TURN 凭据改为读取既有 `/etc/eagler-turn-monitor/active-turn.json`，新连接跟随当前 Best 与 1145 节点选择。配置使用统一的 `EAGLER_NETPLAY_*` 名称，对应旧别名从该服务配置中移除，私有凭据保存在服务器。该配置调整重新加载 test Relay，进程 PID 为 311663，TURN 服务沿原生命周期运行。

## 发布核对

- 主站完整发布验证覆盖 6553 个文件；test 核对全部 6259 个发布文件的长度与完整 SHA-256、前端身份、全部 Runtime 依赖闭包和新 th12mp 构建能力。
- 浏览器分别从正式站和 test 下载并校验全部 64 个多人 Runtime 文件，两站各由三名客户端建立完整的 TURN 连接，三个配对各完成 10 次回显，服务端选择 `rtc`。
- 主站、test、Cloudflare 的简体中文和英语语言包、两种共享字体完成全量 SHA-256 核对，DATA 通过 64 字节 Range 核对完整长度与分段响应。
- 真实页面的普通版、多人版选项均提供语言选择和高刷开关，前端页面与请求错误为空。
- 正式公网代理上的两个独立来源分别计量房间配额；一个来源创建两间房后收到限额响应，离开释放配额后可再次创建，另一来源同时独立创建成功。

## 证据与回退

- [`assembled-jp.json`](assembled-jp.json)、[`assembled-test.json`](assembled-test.json)、[`node-verification.json`](node-verification.json)：发布身份与各节点代次。
- [`targeted-verify-test.json`](targeted-verify-test.json)：test 的文件、前端和新运行包核对范围。
- [`browser-release.json`](browser-release.json)、[`browser-release-test.json`](browser-release-test.json)：两站运行包的全量下载与三人 TURN 数据通道。
- [`ui-resources.json`](ui-resources.json)、[`download-resources.json`](download-resources.json)：选项入口与三个入口的实际资源下载。
- [`relay-cutover.json`](relay-cutover.json)、[`test-turn-cutover.json`](test-turn-cutover.json)：正式服务构建与 test TURN 配置加载记录。
- [`public-quota.json`](public-quota.json)：正式公网入口的跨来源配额隔离与离开后释放。
- [`test-map-repair.json`](test-map-repair.json)：按既有声明恢复的字体文件。
- [`cutover-jp.json`](cutover-jp.json)、[`cutover-test.json`](cutover-test.json)：新旧发布目录与切换时间。

日本上一发布 `main-5db7ef555-20261028`、test 上一发布 `th12-story-clear-20261028-e7433209a462` 和上一边缘版本继续保留。正式 Relay 的 systemd 覆盖文件备份、test 原配置均保存在对应服务器 `/root/th12mp-update-20261028/`。源码差异与同步前备份存于本地发布资料。
