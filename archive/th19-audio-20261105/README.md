# TH19 重复音效验收

原生 `AudioEngine::play_sfx` 的即时输出分支现在逐次执行 stop、seek(0)、play，与 miniaudio 分支的每次从头播放行为一致。共享 WebSfx 的 play 保持恢复当前游标的语义。

[修复前](before.json)：同一原生菜单音第一次产生 PCM，随后三次峰值均为零；WebSfx 游标停在结束位置，未创建新的播放节点。

[修复后](after.json)：同一原生菜单音连续四次调用，四次均创建播放节点、游标为 0、音频 PCM 峰值大于 0.25。检测连接生产音效混音节点，直接采集实时波形；背景音乐关闭，避免把音乐误判为音效。

执行 `npm run test:sfx:browser -- --url=<本地游戏服务> --output=<证据目录>`，浏览器与 Puppeteer 路径通过环境变量提供。源码编译见 [日志](source-build.log)。

HTTP 场景使用既有 SHA-256 回退算法验证程序和 Runtime 资源，校验损坏文件的拒绝测试见 [回归结果](http-integrity-tests.log)。WebCrypto 可用时继续使用原生实现。

公开部署参数由本机运行包持有，游戏接入源码不包含机器 IP 或固定外网端口。
