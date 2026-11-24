# 原生暂停与录像保存的重置后重新打开（628–629）

2026-11-16。实际使用首次成功后封存的 private102，以及继承自619的前端。此次只增加测试与证据，没有产品源码修改、核心重编或公开部署。

629通过18项生命周期检查。一个真实 WASM 实例先后创建两个 A1 World，各执行3次真实逻辑 tick 并记录3帧；真实原版暂停菜单共执行130个菜单 tick，两次完成 ReplaySave 选择和确认。菜单期间 World 帧与游戏 RNG 均保持不变。

每次通过完整生产 `originalPauseConsumer`、`OriginalPauseController` 和 `MenuAudioGate` 打开实际原生录像保存对象，再将姓名存储读取显式挂起。第一次读取保持挂起时，测试关闭前端控制器并调用实际 World reset；随后创建第二个 World 和第二个实际保存对象，最后才释放旧读取。旧回调没有新增任何被监测的原生调用，也没有改变后继的控制器、存储引用、姓名、槽位、完整 World 状态或暂停快照。

实际观察到前端 dispose 后原生暂停仍然存在。World reset 清除了原生暂停、保存对象及录制状态；重置后的无会话 pause tick、save open 和 pause commit 均拒绝，之后新会话重新打开成功。这些拒绝是无会话检查，不是合法旧保存收据的跨代 ACK 测试。

628首次运行在第二次 World 配置时遇到 `Stage music already started`：测试沿用了已启动的真实 StageMusic 对象。该首份失败完整保留（36144字节，SHA256 `933f3873d78f39aabb64682ba338ffe9e6ef61d9b2184cf2d5580f5e7ffb8221`）。629只在第二次配置前销毁并重新创建 StageMusic；它仍使用模拟 AudioContext，没有修改产品音乐实现。

边界：World、暂停、录制及原版 ANM 资源真实执行；音频设备、录像/姓名存储、声音收据和页面环境为模拟边界。未执行完整姓名/录像保存事务、浏览器耐久存储、完整 Title/Main 或无重置的同 World 重新打开，也未验证浏览器完整终局。此项不改变617原版23707帧回放仍在4906帧提前GameOver的结论。

本地首失败与成功目录分别为 `case628-private102-pause-replay-reset-reopen`、`case629-private102-pause-replay-reset-reopen-green`。两者都经过完整输入摘要（142/144项）、实际清单、带引号Windows命令、sourcecheck0、fresh MAIN工程准入、exclusive once和ROOT释放。成功629不得重复运行以获取另一个PASS。

精确执行源码保存在 `tests/native-pause-reset/check.cjs` 与 `audio-fixture.cjs`；它们依赖未随源码仓库分发的本地原作资源、请求文件和已构建核心。未来新目的运行须使用新编号、完整资源清单和独立准入。机器可读结果见 `native-pause-reset628-629.json`。
