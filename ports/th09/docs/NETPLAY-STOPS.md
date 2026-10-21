# th09 联机技能停顿

技能插入画面和咲夜时停的开始、结束及恢复移动第一帧使用已确认的输入历史。角色坐标、碰撞与原生 Draw 共同推进，停顿边界确认后再提交画面。收到的回滚修正在停顿画面提交前完成。

这层调度由确认帧蓄力显示和本地优先蓄力两种方案共用。蓄力方案各自负责进度规则与录像轨道，技能停顿层负责世界状态恢复和画面提交。

每个逻辑帧采集一次物理输入，等待和回滚复用同一份记录。普通移动继续使用回滚预测；停顿边界等待输入确认，不增加固定输入延迟。原生 Draw 的状态变更仍纳入检查点。确认停顿边界前撤回候选世界，重试时复用已经接受的本机蓄力记录。

## 验证入口

在 `ports/th09/web/` 构建开发版并运行浏览器验证。需要 Chromium 或 Edge、`puppeteer-core`、本地游戏数据与字体和音乐目录：

```powershell
$env:TH09_EMSDK='C:\tools\emsdk'
$env:TH09_OUTPUT='artifacts/skill-stop-candidate'
node scripts/build-sdl.mjs
$env:BROWSER_EXECUTABLE='C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe'
$env:BROWSER_TEST_PACKAGE='C:\project\launcher\package.json'
$env:TH09_FIXTURE_DATA='C:\Games\th09\th09.dat'
$env:TH09_FIXTURE_ASSETS='C:\local-content\th09-fonts'
$env:TH09_FIXTURE_MUSIC='C:\local-content\th09-music'
$env:PC_BUILD=(Resolve-Path 'artifacts/skill-stop-candidate').Path
$env:RUN_LABEL='skill-stop-candidate-001'
$env:FRAMES='9600'
$env:PACKET_DELAY='3'
$env:PACKET_JITTER='4'
$env:SKILL_WARMUP='0'
$env:CHAINED_SKILLS='1'
npm run test:skill-stop
```

`LOCAL_CHARGE=1` 对照本地优先方案的双方已接受蓄力指令。默认对照原有蓄力逻辑，可验证确认帧显示方案。报告绑定 WASM 和脚本哈希，保留双方的状态、绘制坐标、完整输入重算结果和首个差异。

停顿验收比较实际提交的每一帧技能插入画面、时停状态切换和首次恢复移动的画面。咲夜在时停期间可以正常移动，状态切换之间沿用普通回滚预测。尚未收到对方释放指令时的预测差异独立记录为 `unseenStopFrames` 和 `unseenMaxPose`，与已呈现停顿的坐标核对分别统计。浏览器使用合成网络延迟，报告提供确定性和画面提交证据；设备性能由实机测量。

WASI 组件验证继续使用 `npm run test:rollback`。正式构建核对开发观察接口已移除，普通与多人包使用同一份游戏 WASM。
