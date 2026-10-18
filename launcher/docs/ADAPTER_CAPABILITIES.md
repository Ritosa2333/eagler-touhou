# Adapter 能力参考

`adapter-capabilities.mts` 是能力分类与验收 owner 的机器权威；产品声明由 [`common/contracts/product-catalog.mjs`](../../common/contracts/product-catalog.mjs) 提供。新增产品前先生成实际契约：

```sh
cd launcher
npm run adapter:inspect -- --game=th20
```

## 分类

| 类别 | 何时需要 | Owner |
| --- | --- | --- |
| `required` | 每个正式 Runtime 都必须满足 | Launcher 与 Runtime |
| `inherited` | Launcher 已提供，Runtime 使用其协议 | Launcher |
| `profile-required` | 仅在产品声明相应可选 profile 后成为必需 | 对应 Host、Launcher 或 Runtime |
| `optional` | 产品差异，按 Catalog 明确声明 | Product |
| `implementation-detail` | 现有 Runtime 的内部实现方式 | Runtime |
| `format-adapter` | 读取特定原版文件格式 | Format owner |
| `compatibility` | 已发布旧格式或旧字段的有限迁移 | Legacy owner |

## 所有产品必需

| Capability ID | 可观察义务 |
| --- | --- |
| `authoritative-gameplay-fidelity` | 保留原作玩法、碰撞、RNG、计时器和菜单状态机，除非明确产品规则改变它 |
| `runtime-lifecycle` | 完成 `ready`、配置、启动、首帧、退出与错误协议 |
| `keyboard-input`、`gamepad-input` | 映射结果同步进入原作逻辑输入边界，正确清理按键与设备状态 |
| `touch-controls` | 触控、摇杆、取消、焦点、炸弹及灵敏度遵守共享输入语义 |
| `restart-action` | 共享 Restart 的 R 键经暂停菜单重启当前局 |
| `always-hitbox` | 始终显示原生判定点视觉，不改变实际碰撞或焦点状态 |
| `visibility-input-cleanup` | blur、隐藏页面和 pagehide 后没有卡住的输入 |
| `runtime-health-diagnostics` | 提供 Runtime 身份、帧与音频健康状态 |
| `durable-storage`、`replay-file-management` | 存档、配置、Replay 可列出、读写、删除、同步，并在重启后保留 |
| `replay-determinism-verification` | 游戏仓库提供固定步 Demo 与长 Replay 对照门禁和只读 Oracle 路径 |
| `normal-music-ogg` | OGG 与无音乐模式可用 |
| `presentation-cadence` | 高刷额外绘制不推进玩法、Replay、RNG、定时器或存档 |
| `package-offline-launch` | 安装后可重载并离线启动已安装游戏 |

详细步骤：[Build](../../docs/adaptation/build.md)、[Runtime](../../docs/adaptation/runtime.md)、[输入](../../docs/adaptation/input.md)、[展示与音频](../../docs/adaptation/presentation.md)、[保存](../../docs/adaptation/storage.md)、[验证](../../docs/adaptation/validation.md)。

## 开启 profile 后的义务

- `multiplayer`：独立 Runtime 变体、同步配置与开局、席位输入归属、旁观者隔离与已确认历史、限额与故障隔离、多人 Replay、Pause/Restart 和离房生命周期。
- `thprac`：Runtime 与发布物一致声明；启动前传入启用状态和 locale；Runtime 拥有 Practice 生命周期与 Replay 参数；共享触控使用原生指向和函数键桥。
- `languages`：日文原版始终可用；包在发布和启动前校验；缺包安全回退；翻译资源与字体覆盖所需字符。
- `midi-music`：仅在声明且发布有效 MIDI 时显示；实际播放并遵守标题暂停、切歌和退出生命周期。OGG 仍是必需路径。

可选声明位于 `PRODUCT_GAMES`：`musicCapabilities.midi`、`features.thprac`、`features.languages`、`multiplayer`、`features.focusHitbox`、`display.faithBar`、`storage.hintFiles`、`package.rawDataImport`、`support.adaptationNotice`。仅已声明的产品差异可控制共享 UI。

## 证据与命令

Capability 表为每项义务指定 `verification`。标记含义：`repository` 是 Launcher 仓库测试，`workspace` 检查多工作区接线，`runtime` 是游戏仓库门禁，`browser` 是浏览器验证，`relay` 与 `python` 是对应服务或格式测试。

```sh
cd launcher
npm run check
npm run check:workspace
npm run test:runtime-protocol
npm run test:storage
npm run test:touch
npm run test:replay-determinism
```

浏览器设备路径按能力表另行运行，例如 `npm run test:always-hitbox:browser`、`npm run test:music-selection:browser`。各标题还需提交其自身的构建、Replay、输入与呈现证据。
