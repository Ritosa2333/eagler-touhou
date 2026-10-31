# 联机自适应选路

公共 `BrowserPeerTransport` 在同一玩家会话下比较当前路径和 TURN 备用路径。游戏继续使用既有输入、确认和维修协议，路径切换保留原生帧号、会话身份与已采样输入。全部接入公共传输的多人作品共享这套实现。

## 设计依据

- [Valve GameNetworkingSockets](https://github.com/ValveSoftware/GameNetworkingSockets/blob/master/src/steamnetworkingsockets/clientlib/steamnetworkingsockets_p2p.cpp)：备用路径低频端到端探测，切换前补齐近期样本，控制端主导提议，稳定路径获得切换保护。
- [Network Next 路径管理](https://github.com/networknext/next/blob/main/sdk/source/next_route_manager.cpp)：待确认、当前与上一条路径分别持有身份，确认新路径后继续接收在途数据。
- [Network Next 探测历史](https://github.com/networknext/next/blob/main/sdk/include/next_ping_history.h)：序号回显使用本机时钟计算往返时间，统计窗口给在途探针保留接收时间。

这里复用设计原则，浏览器实现独立编写。选路模块源码为 `common/browser/adaptive-peer-route.mjs`；`node common/tools/sync-adaptive-peer-route.mjs` 将它嵌入公共 C++ 传输，`--check` 核对生成内容。Runtime 随构建获得完整实现，无需额外下载模块。

### 运行包浏览器桥更新

`launcher/scripts/update-peer-runtime.mjs` 可将同一源码的四个浏览器传输绑定写入既有 Runtime 的新脚本文件。工具检查绑定唯一性与原生参数数量，保留其他脚本内容，并记录输入、输出、源码和各绑定的摘要。游戏 WASM、玩法、原生协议和素材继续沿用对应发布代；更新脚本按新的 Runtime 内容身份发布。

```sh
node launcher/scripts/update-peer-runtime.mjs --input=previous.mjs --output=candidate.mjs --receipt=bridge-update.json
node launcher/tests/test-peer-runtime-update.mjs
```

这种更新适用于原生 ABI 相同的浏览器桥修改。完整源码构建直接使用相同的 `BrowserPeerTransport.cpp`，两种发布入口共用传输实现。

更新工具的独立检查使用显式绑定源，可在 Launcher 稀疏检出中执行。公共传输检查同时核对原始可靠通道和自适应可靠发送，维修流量共用已有的背压与容量边界。

## 路径与兼容

玩家对战默认使用 WebRTC，建立失败后返回等待室。启用旧 WebSocket 对战兼容时，服务端设置 `EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK=1`，客户端同时设置 `netplayAllowWebsocketFallback=true`。房间、信令与旁观继续由 Relay 承载。生命周期测试对兼容场景显式开启回退，默认关闭场景由服务端专项检查覆盖。

每对玩家保留当前 WebRTC 连接和至多一条 TURN 连接。备用连接强制 `iceTransportPolicy=relay`，使用信令服务签发的第一个 TURN 候选及其现有端口配置。初始连接已经走 TURN 或本机启用「优先使用 TURN」时，继续沿用该路径。

双方通过当前可靠控制通道交换 `etr1:` 能力和备用 SDP/ICE，不增加 Relay 消息种类。相同控制通道承载备用协商，双方确认支持后才启用选路。旧版客户端继续使用既有二进制输入与控制流。

备用路径协商、探测和切换按玩家对隔离。房间、旁观、Replay 与原生时序参数保持各自所有权。RTC 与 WebSocket 的开局选择继续由现有服务端屏障完成。

备用连接失败时双方交换退休代次，经过退避后由控制端建立新代次。失败的备用连接释放端口与定时状态，当前健康路径继续承载游戏。

## 测量与选择

当前路径每 250 ms 发送一次无重传输入通道回显，备用路径间隔 10 秒。疑似劣化后双方加密到 50 ms，观察 4 秒。探针在至少 1 秒接收窗后计入统计，乱序和重复回显按唯一序号处理。

六秒窗口至少有 12 个成熟样本后，连续丢失造成的高丢包或相对基线升高的 p95 会触发复核。控制端要求候选至少取得 12 个回显、丢包不超过 5%，且评分乘以 1.1 再加 5 后依然优于当前路径。评分为 p95 毫秒加丢包比例乘以 1000。切换后保持 60 秒保护时间；连接失效使用独立的有界恢复流程。

页面隐藏、校准暂停和超过 750 ms 的本地调度间隙会清空本轮样本并通知对端暂停比较，恢复后经过两秒稳定窗重新采样。每个定时回调最多发送一个探针，恢复页面后沿正常节奏继续。

质量选路在原生对局激活后运行。资源加载和 Adonis 测量启动阶段保留初始路径，时序协商继续绑定实际游戏通道。Runtime 的 `netplayAdaptiveTurn=false` 可供对照验证；手动「优先使用 TURN」继续固定中继策略。

## 会话交接

席位编号较小的一端控制该玩家对的路径提议。新路径先完成双向通道与回显确认，再经 `switch → accept → commit → committed` 交接。版本号和备用连接代次隔离过期请求；提交确认可重试，重复提交返回同一版本的确认。质量复核未取得收益时退避十秒再比较。

已接受的代次与已提交的确认跨页面暂停保留，恢复时继续交接同一代次；暂停只清除质量样本。控制端尚未提交的提议可以取消，双方继续沿当前路径收发。

可靠控制消息在支持选路的双方之间增加递增序号，接收端跨路径保持顺序、去重并返回累计确认。发送端保存至多 256 条、1 MiB 未确认消息，切换时在新可靠通道重发。接收端使用同样的消息与字节边界。游戏输入保持原生报文，重复、维修及帧确认继续由原生 `SessionChannel` 处理。

控制重发按发送缓冲分批推进，缓冲释放后继续发送剩余消息；新控制消息等待既有重发批次完成。输入通道独立收发，可靠控制的排序不为输入通道增加等待。

可靠控制包一旦进入发送队列，其序号与内容持续保留到累计确认。浏览器临时拒绝发送时，该包进入重发队列，由定时器在当前路径重试；备用路径就绪时沿既有交接流程重发。后续控制包等待这批重发完成，保持连续序号。

主路径回调绑定具体玩家实例与通道实例。已被替换的通道所排队的打开、关闭、错误和消息事件不能改写新通道状态；路径交接接收窗内的旧主路径消息仍由选路模块判定。崩溃报告保存最近八条当前通道关闭／错误事件、浏览器 SCTP 错误字段、连接状态及有界队列快照，信令描述与 TURN 凭据不进入该记录。

上一条路径保留五秒输入接收窗。备用协商和连接失败不会终止仍可用的当前路径。会话关闭同时释放探测定时器、备用连接、控制消息与统计历史。

## 验证职责

确定性测试覆盖双端能力、劣化选择、协商顺序、控制重放与去重、故障恢复、页面暂停、旧客户端、容量边界和完整释放。真实浏览器测试使用两端 DataChannel 和受控的输入丢包，核对切换前后收到的游戏字节及可靠控制顺序，并读取所选 ICE 候选。

```sh
node common/tests/adaptive-peer-route.test.mjs
node --test common/tests/browser-peer-channel-lifecycle.test.mjs
node common/tools/sync-adaptive-peer-route.mjs --check
python common/tests/adaptive-peer-route-browser.py --turn-origin https://your-launcher.example
```

浏览器脚本从诊断接口取得短期 TURN 凭据，在本机两个独立 frame 中执行生产 `EM_JS` 传输。房间信令由隔离 fixture 提供，初始路径使用真实直连，备用路径使用指定站点的实际 TURN。测试只丢弃直连输入通道的发送，并暂存一条可靠控制消息，以验证新路径重发与旧路径迟到去重。`--output` 可保存不含凭据的 JSON 结果。

### TH12 与 TH15 多人接入

TH12 和 TH15 多人 Runtime 通过 `eagler::netplay` 使用同一套 `BrowserPeerTransport`，选路直接嵌入运行脚本。游戏标签分别为 `th12` 和 `th15`，双人建立一组路径，三人对每个对端分别建立路径；各组路径独立探测、协商与释放。

```sh
python common/tests/adaptive-peer-route-browser.py --turn-origin https://your-launcher.example --game th12 --output th12-route.json
python common/tests/adaptive-peer-route-browser.py --turn-origin https://your-launcher.example --game th15 --output th15-route.json
```

这两个入口验证两作标签对应的真实直连、TURN 切换和可靠控制顺序。`browser-peer-turn-preference.test.mjs` 同时覆盖两作双人、三人配置及显式固定 TURN。

已有多人运行包可使用 `launcher/scripts/update-peer-runtime.mjs` 更新四个同签名浏览器绑定：TH12 的目标为 `core.mjs`，TH15 为 `th15.mjs`。工具校验绑定参数，保留 WASM 和其余脚本内容，并输出源文件与更新结果的 SHA-256 记录。正式发布时同步重建运行包清单与代际身份。完整源码构建继续使用两作的多人构建入口。

两作各自的多人开发分支承载此次公共传输接入。原生回滚窗口、输入身份和规则版本沿用各作当前实现。

### 实际游戏换路

`ports/th09/web/tests/multiplayer/adaptive-route-browser.mjs` 加载正式 th09 WASM 和原作素材，通过独立的生产 Relay 进程建立双人房间。测试从直连开始，建立健康基线后持续丢弃初始路径的输入发送，核对双方选择实际 TURN、已确认游戏帧继续推进，以及换路前后共同帧的世界 hash 一致。

`PC_BUILD` 指向正式编译目录，`TURN_ORIGIN` 指向提供短期诊断凭据的站点，`ROUTE_REPORT` 指定结果文件。`ROUTE_SCENARIO` 选择 `stable`、`bursts`、`outage`，分别验证健康路径保持、间歇性长丢包和持续断流；`ROUTE_BURST_MS` 控制每二十秒的连续丢包时长，`ROUTE_DURATION_MS` 控制观察时间。素材使用 th09 开发指南中的 `TH09_FIXTURE_DATA`、`TH09_FIXTURE_ASSETS`、`TH09_FIXTURE_MUSIC` 显式配置。短期凭据由内存和子进程环境传递，结果记录运行包身份、换路时长、候选类型和逐帧对照结果。

`ROUTE_RUNTIME_URL` 可指定公网候选的 `th09.mjs`。测试沿实际响应及重定向记录浏览器加载的脚本与 WASM，分别核对本地候选摘要，并将实际 URL、字节数与摘要写入结果。两类响应完整匹配后确认验收结果，将公网文件分发纳入同一次实际对局验证。该校验复用[浏览器验证工具](../../tools/browser/README.md)。

`ROUTE_CONTROL_FAULT=1` 同时暂存初始可靠通道的发送，恢复后按原顺序交付，用游戏状态对照验证控制重放与迟到去重。`ROUTE_STARTUP_ONLY=1` 固定使用健康路径场景，检查双端开局后持续推进、原路径保持和共同帧 hash；此模式关闭输入与控制故障注入，可重复运行核对开局。完整断流换路场景使用默认入口，保留成熟采样与恢复后的帧对照。`ROUTE_ADAPTIVE_DISABLED=1` 提供相同运行包的旧传输对照。

`ROUTE_RELAY_URL` 可指定公开 WSS 服务，测试先准备双端世界，再以随机房间号和独立成员身份创建私密双人房间，经该服务完成信令与路线屏障。结束时通过目录的当前会话凭据释放成员和传输。默认入口使用独立本地 Relay 进程。
