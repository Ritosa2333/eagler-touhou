# TH12 原版逐帧检查

使用Windows的64位Python运行下面的命令，打开 <http://127.0.0.1:8113/>。
此工具只用于本地还原检查；不加入普通网页版的运行入口。

共享工具需在本地准备并固定版本；仓库不包含原版安装和第三方参考副本：

```sh
git clone https://github.com/YomotsuHisami/eagler-common.git ../reference/eagler-common
git -C ../reference/eagler-common checkout cedb7101696b6f8f200a45814c594ec0f1052761
python tools/frame-inspector/server.py --original ../th12 --common ../reference/eagler-common
```

网页版需先构建核心并准备自己的原版资源，见 [构建说明](../../docs/BUILD.md)。

1. 关闭已有原版游戏，在检查页点击「启动检查副本」。原版有单实例限制。
2. 在新打开的原版窗口正常选择录像与关卡；检查页填写相同录像和关卡，点击「网页版加载录像」。
3. 点击「原版停帧」，再点击「网页对齐原版当前帧」。双方使用录像输入游标对齐，不使用画面帧数或近似秒数。
4. 「两边前进一帧」将原版推进一次完整 Update/Draw/Present，再让网页消费到原版的真实录像游标。
5. 可填写目标帧点击「原版运行到此帧」。时间只是输入辅助，最终目标是整数录像帧。原版不能倒退；倒退需解除工具停帧并在原版重播该关。
6. 停帧自动保存原版窗口画面和同帧数据，也可点击「保存当前帧」。请保持原版窗口可见、不被覆盖；GDI 截图不是 GPU 回读，遮挡情况下的画面不作为渲染证明。

不要用原版 Esc 菜单代替工具停帧。游戏菜单暂停仍会提交画面，但不会消费录像输入。切换窗口后若原版自动进入暂停菜单，解除游戏暂停，工具的目标帧仍然生效。

## 适配范围

共享来源固定为 `YomotsuHisami/eagler-common` 的
`cedb7101696b6f8f200a45814c594ec0f1052761`，文件内容哈希见 `common-pin.json`。
直接使用该版本 `windows-process-reader.py` 的读取实现；TH12 布局和停帧适配在本目录。
默认共享源码路径为工作区 `reference/eagler-common`，可用 `--common` 指定同一固定版本。

上游 Presentation Lab 的网页工作台支持独立绘制采样；本适配器的原版使用
Windows 硬件调试断点，不实现原版 draw-only alpha sweep，也不宣称完整 Lab 合约支持。
上游 D3D8 原版采集器目前是 TH06/TH07，TH12 使用它自己的 D3D9 调度边界。

EXE 必须是 TH12 1.00b，SHA-256：
`99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417`。
启动副本位于 `local/frame-inspector/retail`。EXE、DAT、RPY 不改变；设置和分数存档
属于独立副本。副本配置启用窗口和不跳帧，避免一个画面包含多次更新。

`0x44f8af` 是主循环三个 Update/Draw 分支汇合后的原版指令位置，完整的 Draw/
Present 已返回。只在主线程 DR0 设置执行断点。Windows 调试事件停住所有线程；
恢复时设置 CPU resume flag，让原指令执行一次。没有 WriteProcessMemory、INT3
补丁、游戏数据写入、键盘输入合成或随机数补偿。支持 64 位 Python 调试 WOW64
原版，正确处理 WOW64 的初始断点事件。

内部采集覆盖：原始分数、显示分数及滚动步长、游戏 RNG seed/calls、最大得点、
擦弹、灵力、自机固定坐标/状态/无敌时间、符卡编号/状态/年龄和真实录像游标。
未覆盖敌人、弹幕、激光、道具等全部对象；字段一致只能证明已采集字段一致。
不能据此判定整局所有行为或像素已经一致。

输出在 `artifacts/frame-inspector/`：`frames.jsonl`、`latest.json`、`latest.png`。
记录包含菜单画面和游戏画面；读取时检查 `replayActive` 和 `replayCursor`。
此流不是上游完整 `eagler/replay-trace/v1` golden，缺乏全对象覆盖和完整段落
生命周期，不可拿来替代整局 verifier 验收。

## 验证

```
python tools/frame-inspector/test_profile.py
node --test tests/frame-inspector-bridge.test.mjs tests/replay-seek.test.mjs
```

另需实际 Windows 原版检查：停帧期间画面帧/RNG 保持不变，单步画面帧只加一，
游戏未进入 Esc 暂停时录像游标也只加一；同帧截图可见并与原版窗口一致。
服务只绑定 127.0.0.1，写操作检查 Host 与 Origin；网页适配桥只在显式
`?frameInspector=1` 下加载，且仅接受同源父页面的串行检查请求。
