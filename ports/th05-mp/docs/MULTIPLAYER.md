# TH05 原生合作模式

两人、三人合作在 `ports/th05-mp` 独立维护，基于 uth05win 原生 C++ 关卡编译为 WebAssembly／WebGL。启动器负责房间、选人、准备和资源安装，Runtime 负责共享世界与逐帧输入。

## 合作规则

- 灵梦、魔理沙、魅魔、幽香自由混选；各席独立移动、射弹、生命、Bomb、Power 和拾取半点余量。
- 分数、点数、梦点、擦弹和结算为团队状态；公共世界每帧更新一次，自机射弹总预算为 288。
- 双人 Boss 有效血量 ×1.5，三人 ×2.2；伤害除数保留余数，击中得分保持原作倍率。
- 小 P 在双人时交替增加 1／2，三人增加 2；大 P 分别增加 15／20。满火力拾取者支援最低火力的存活队友，同火力按距离和席位确定。
- Bomb 道具奖励存活玩家；1UP 与每累计 100 个蓝点奖残作用于全队，并复活幽灵。
- 停火靠近队友，连续点按低速键 5 次，相邻两次间隔最多 24 个游戏帧，可转移一条生命。救活幽灵时，救援者 Bomb 减半，被救者得到一个 Bomb。
- 世界同一时刻运行一个 Bomb 演出，动画、图像与伤害始终绑定释放者。
- 暂停、重开和返回房间由确认输入同步。暂停菜单的触控点选先进入输入队列；等待网络时继续绘制界面以接收短按。
- 通关沿用原生结局、Staff、评价与祝贺流程，再返回房间。挑战模式禁用 Bomb，以死亡次数替代残机显示。

## 画面与显示配置

侧栏沿用 PR #143 的原生像素字形、分数位置、各席生命／Bomb／Power 资源行、公共点数／擦弹／梦槽及底部 Enemy 血条。两人和三人共用固定的 128 像素侧栏，游戏世界保留 384 像素宽度。

多人启动器的“画面”设置提供原版侧栏、红魔乡式顶部和环形三种血条，以及阶段刻线、剩余阶段星星、时间／轮数、梦槽数值和各席火力数值。偏好经 `th05MultiplayerDisplay` 配置传入，保持在各自的本地显示层；不同玩家选择不同样式也使用相同的世界输入与校验。血条切换自动约束阶段提示和时间显示，游戏内关闭 thprac 辅助入口。

## 时序与输入

采用确认帧同步，支持 0–9 帧手动延迟和开局自动测量，不启用预测或回滚。每 120 帧交换游戏状态校验；输入缺失时冻结世界并报告等待，状态不同立即结束本局。

直连／中继传输和双通道输入修复沿用 [PR #143](https://github.com/Ritosa2333/eagler-touhou/pull/143) 的 `managed-transport.mjs` 与 `input-transport.js`，来源提交为 `659e65658d4cd1bcfee1670f4c5cc63d79d2819f`。中继从已认证连接添加席位来源，客户端不能用载荷选择发送者。公网开局固定 Runtime generation；本机 `ws://127.0.0.1` 开发服务可使用源码构建入口。

滑动触控同步目标坐标，在执行帧由原生自机计算实际位移；限速模式继续按机体速度截断。摇杆同步速度。这样排入延迟队列的滑动不会重复补偿过去的位置。

单人保留 `/savesth05` 和原生 `.rpy`；多人使用 `/savesth05mp`。当前多人入口提供玩家席位，观战与多人录像工作区由后续协议接入。旧独立页保留在多人栏的“旧版联机入口”。

## 构建与验证

```powershell
node ports/th05-mp/scripts/build-eagler.mjs
node --test ports/th05-mp/tests/*.test.mjs
$env:TH05_BROWSER_TESTS='1'
node ports/th05-mp/scripts/build-eagler.mjs
node ports/th05-mp/tests/multiplayer-native.mjs
node ports/th05-mp/tests/multiplayer-browser.mjs
$env:TH05_MP_PLAYERS='3'
$env:TH05_MP_ROUTE='relay'
node ports/th05-mp/tests/multiplayer-browser.mjs
```

单人 `ports/th05` 与多人 `ports/th05-mp` 各自编译、维护和导出历史。`launcher/config/workspace.json`、Runtime 构建配置与 `docs/adaptation/port-sync.json` 登记目录和同步基线；`project-history.py --game th05` 刷新两侧投影与 MP 的普通版上游。
