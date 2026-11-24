# 妖精大战争 · Web 多人联机

多人模式以独立的合作 Runtime 与作者 Launcher 的联机大厅对接。双人、三人共享同一个原生战场，机体、干劲、Z 蓄力和完美冻结各自独立。四档主线、A/B/C 路线和 Extra 使用原作关卡；Extra 无需单机解锁。房主选择后续路线，挑战模式独立设置。多人入口不加载 thprac。

## 房间与游戏规则

- **冻结干劲**默认 ×1.0，按单机原版的实际冰伤结算；可选 ×2.5、×2.0、×0.75、×0.5。实际单次冻结面积达到 5% 时，施放者与每位队友各获得同一份倍率后的奖励；不足 5% 时只有施放者获得。普通命中与冰星另计。累计冻结面积与冻结技能奖励仍使用既有 2P ×0.8、3P ×0.75 系数。
- 原版标题、对话、关卡提示、暂停、全队团灭、结局与录像界面接入同一合作流程。录像、冰伤倍率和染色保持本局设定。房间的独立观战者可以补齐已确认输入后观看，也能独立暂停、回到直播和保存录像，不控制玩家。
- 对局和观战保留同一局的 216000 个已确认帧，相当于 60 FPS 下 60 分钟的游戏时间。计时随战场推进，到达上限后各端结算并保留完整录像。中途加入的观战者可以先看到已确认的画面，再补算到直播位置。
- 机体耗尽干劲后为半透明幽灵，仍获得共享奖励、不受攻击，也不因干劲增长自动复活。下关或队友送命复活。高速停火靠近队友短按 Shift 五次送 100% 干劲、短点 Z 五次送 100% 完美冻结。
- 每人最多存 200% 完美冻结。按 X 施放扣除施放者 100%，技能期间全队存活角色无敌。同一帧按座位顺序接受第一个满足条件的施放，期间按键需要松开后重按。
- 房间启用回滚时，对局从第一个到期帧起允许预测，最多预测两帧。联机音效、观战和录像使用已确认的输入。
- 随机染色为可选房间规则。P1 原色，P2/P3 在本局固定且配色互异。六套 `CRP1` 记录只含用户绘制的差异像素；运行时对各端本地导入的机体贴图应用编辑。完整原版纹理、游戏数据、音乐、录像和个人档案不入库。

## 在本机复测

此审阅入口需显式配置；不启用现有公共大厅，不改作者服务器环境。不同浏览器配置分别导入同一份本地资源包后，在作者页面入座、准备、开局。可再用一个配置中途加入观战。单机保留自己的程序和保存区。

1. 按 [单机开发说明](../th128/docs/DEVELOPMENT.md) 配置 Node 与 Emscripten 4.0.22。正常构建 `ports/th128/tools/build-runtime.mjs`，用 `ports/th128/tools/package-launcher-runtime.mjs --assets=<核心站点目录> --output=<normal>` 得到标准程序目录。不把 DAT／OGG 放进程序目录。
2. 从仓库根目录运行 · `node ports/th128-mp/tools/build-coop-core.mjs <native输出目录>`、`node ports/th128-mp/tools/build-coop-peer.mjs <传输输出目录>`。取两条命令输出中带回执的 `dst`。以仓库 Launcher 依赖中的 `acorn` 校验打包闭包：`node ports/th128-mp/tools/package-managed-runtime.mjs --normal <normal> --transport <peer-dst> --coop <core-dst> --output <现存空目录>`。
3. 以普通版 `runtime-files.json` 的 SHA-256 及上步返回的目录、代次运行 `node ports/th128-mp/tools/project-browser-programs.mjs --normal <normal> --normal-inventory-sha256 <sha256> --multiplayer <mp目录> --multiplayer-generation <代次> --output <新投影目录>`。将投影的 `multiplayer` 传入 `node ports/th128-mp/tools/stage-coop-launcher.mjs <multiplayer> <新Launcher目录>`，从现有工作区依赖按锁文件安装后，在输出的 `launcher` 内运行 `node scripts/build-launcher.mjs --force`。
4. 运行 `node ports/th128-mp/tools/stage-coop-palette-relay.mjs --source . <新Relay目录>`，在输出的 `services/relay` 运行 `npm ci --omit=dev`。用 `node ports/th128-mp/tools/prepare-local-handoff.mjs <新Launcher目录> <新投影目录> <新Relay目录> <作者单机导入站点清单> <封面目录> <新部署目录> <HTTP端口> <Relay端口>` 生成独立本机部署。可运行输出的 `Start-Coop.cmd` 或 `node serve-local-coop.mjs local.json`，用 `Stop-Coop.cmd` 或同一命令加 `--stop` 退出本部署。

## 测试入口

- `node --test ports/th128-mp/tests/*.test.mjs`：双端确认、回滚、规则、准备、观战、录像、显示、染色和 UI。
- `tests/coop-freeze-threshold.integration.mjs`、`tests/coop-perfect.integration.mjs` 和 `tests/coop-route-choice.integration.mjs`：使用本地原版数据验证五档、5% 门槛、普通与完美冻结、跨关和房主地图选择。
- `tests/coop-skill-transfer.integration.mjs` 使用独立的原生测试核心，验证双人、三人的技能上限、全队无敌、互斥施放、迟到输入回滚和赠送。
- `tests/coop-managed-package.integration.mjs` 和 `tests/coop-launcher-browser.integration.mjs`：使用本地作者资源包验证五难度、双人／三人开局、晚入观战、录像保存与复演。测试使用独立浏览器和回环 Relay，不访问生产服务。

普通版共享代码与多人专属补丁分开：`native/world-coop.patch` 只应用于多人编译目录，不改普通版原生文件；独立的输入录像由 `th128-coop-replay/1` 记录。旧 `parallel-stage` 录像保留在各自协议和构建，不当作同场合作。
