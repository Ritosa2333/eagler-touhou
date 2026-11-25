# TH13 保真地图

C++ 单元与原版 th13.exe（v1.00c，SHA256 `f12760ca…80d`）的对应关系。三部分内容各自有固定的来源：

- **子系统入口**：`web/cpp/rebuild/AddressMap.hpp` 的登记表，逐条带证据。
- **逐文件映射**：提交 `089c4066c` 的快照，由当时源码里的地址与 Ghidra 导出的函数索引生成；此后手工维护。
- **已对齐项、已知分歧与未决项**：对照反编译与直调探针逐条核对的结论，写明地址。

## 证据链

保真由三层证据支撑，强度依次递增：

1. **地址引用真实**。源码引用的每个原版地址都逐个核对过类别：导出函数的入口、函数体内的指令边界、跳转表、回调入口或数据地址。两个端口的分类表（1464 + 1467 个地址，全部归类）保存在仓库根的 `archive/th13-address-snapshot-20261124/`。直调探针 `tools/oracle/probes.py` 在容器内调用原版函数取证（用法见 [DEVELOPMENT.md](DEVELOPMENT.md)“原版直调探针”）。反编译索引用 `tools/decomp/export.sh` 在容器内导出（见 [DECOMP.md](DECOMP.md)），2291 个函数全部反编译成功。
2. **行为逐位锁定**。回归摘要门禁在 10 份录像、35 关、58 万帧上，每 60 帧对状态池、事件流与绘制清单取滚动摘要，与金标逐行比对（用法见 [DEVELOPMENT.md](DEVELOPMENT.md)“回归摘要门禁”）。任何改动模拟或绘制行为的提交都会在首个分歧帧被报出。
3. **原生套件**。`anm`、`ecl`、`ui`、`replay`、`items`、`score`、`audio`、`combat` 等套件按子系统用原版资产夹具断言行为，共 900 余项。

## 子系统入口

| 子系统 | 原版入口 | C++ 对应物 | 定位证据 |
|---|---|---|---|
| RNG | `0x47bba2` | `game/Rng.hpp` | 常量对在二进制中唯一出现；oracle 序列已对齐 |
| supervisor | `0x439ee0` | `sdl/ApplicationState::step` | 每帧调 `0x460dd0` 音乐、`0x46e510` ANM 资源装载、`0x43c1c0` 场景分发 |
| ANM 虚拟机 | `0x462450` | `game/AnmInterpreter.cpp` | 指令解释器主体，84 个调用者 |
| ECL 线程 | `0x4772a0` | `game/EclContext.cpp` | 线程 step；取参 `0x478e60`/`0x478ee0` 对过反汇编 |
| ECL 指令分发 | `0x41b260` | `game/StageCommands.cpp` | 17.6KB 的主分发，经 `0x41b250` 调用 |
| 敌机管理器 | `0x418e00` | `game/StageSim.cpp` | 遍历敌机链表，逐机 `0x41a8a0` |
| 弹管理器 | `0x40e360` | `game/StageSim.cpp` | 遍历弹表，逐发 `0x40db10` |
| 道具管理器 | `0x42e380` | `game/ItemManager.cpp` | 每帧序逐槽步进 0..0xa57 |
| 自机 | `0x443240` | `game/StageBattle.cpp` | 自机状态机，常规更新 `0x442960` |
| 神霊 | `0x438660` | `game/ItemManager.cpp` | 256 槽每帧序，逐槽 `0x438730`；灵界激活在 `0x405a20` |
| 录像 | `0x448c40` | `game/ReplayFile.cpp` | 装载 `0x448c40`、保存 `0x4484d0` |
| 对话 | `0x428ae0` | `game/MsgPlayer.cpp` | 装载 `0x424fd0`、文本解密 `0x42a340` |
| 关终了 | `0x42cd40` | `game/PostGameFlow.cpp` | 每关奖 `0x42abf0`、六面四式 `0x42ce96`–`0x42ced0` |
| 覆盖层 | `0x43dbc0` | `game/PostGameFlow.cpp` | 31 态分发：结算A `0x43e780`、GameOver `0x43fa90`、结算B `0x440960` |
| 结局 | `0x416130` / `0x416a10` | `game/EndingScene.cpp` | 初始化与脚本解释器，op 表 `0x4171e8` |
| 场景调度 | `0x43c1c0` | `sdl/ApplicationState::apply_destination` | supervisor 的 `0xf`/`0x10` 分支 |
| 存档 | `0x449e50` | `sdl/FileStore.cpp` | `scoreth13.dat` 序列化，`0x4723a0` 加密 |

supervisor 与敌机、道具、自机、神霊四个管理器的入口没有直接调用者，由调度器经函数指针每帧调用。

## 逐文件映射

快照时源码共引用 1464 个不同的原版地址、4231 处引用，全部落在映像内：2613 处恰好是函数入口，1128 处落在函数体内（函数内的具体指令），388 处是数据地址，102 处在代码节内但不在导出的函数体内。下面每个文件列出它引用的原版函数（按引用次数，最多六个）。路径省略 `web/cpp/` 前缀。

| C++ 文件 | 原版函数数 | 主要入口 |
|---|---:|---|
| portable/sdl/FrameRateMeter.hpp | 1 | 0x424990×1 |
| game/AnmCamera.cpp | 2 | 0x4681b0×7 0x4977c0×1 |
| game/AnmCoordinates.cpp | 1 | 0x467e80×3 |
| game/AnmDrawSchedule.cpp | 2 | 0x43bbc0×1 0x467350×1 |
| game/AnmDrawSchedule.hpp | 2 | 0x43b900×1 0x4679a0×1 |
| game/AnmEnvironment.hpp | 3 | 0x406680×2 0x401730×1 0x4243e0×1 |
| game/AnmInterpreter.cpp | 3 | 0x462450×5 0x461a60×1 0x4732c0×1 |
| game/AnmManager.cpp | 13 | 0x462450×3 0x46f820×3 0x46fab0×3 0x46f240×2 0x46f990×2 0x402ff0×1 |
| game/AnmManager.hpp | 13 | 0x46f820×4 0x403800×3 0x438730×2 0x4418b0×2 0x45bf10×2 0x46f170×2 |
| game/AnmOverlay.hpp | 2 | 0x402ff0×1 0x42b990×1 |
| game/AnmRegistry.cpp | 7 | 0x46f240×2 0x46f3f0×2 0x46f690×2 0x46f990×2 0x46f170×1 0x46fca0×1 |
| game/AnmRegistry.hpp | 8 | 0x46f170×3 0x46f3f0×2 0x42e380×1 0x462450×1 0x46f240×1 0x46f690×1 |
| game/AnmRenderer.cpp | 7 | 0x43b900×2 0x467350×2 0x406d90×1 0x407310×1 0x45d2f0×1 0x46ef60×1 |
| game/AnmRenderer.hpp | 6 | 0x43b900×1 0x43bbc0×1 0x467350×1 0x467960×1 0x4679a0×1 0x467a80×1 |
| game/AnmResource.hpp | 2 | 0x46e0b0×1 0x46e230×1 |
| game/AnmVariables.hpp | 5 | 0x461e80×1 0x4621b0×1 0x4622c0×1 0x462380×1 0x472c90×1 |
| game/AnmVisualState.cpp | 1 | 0x472f30×1 |
| game/AnmWindTrail.cpp | 1 | 0x401730×1 |
| game/AsciiFrame.hpp | 2 | 0x403930×2 0x424a90×1 |
| game/AsciiText.cpp | 2 | 0x4042a0×1 0x424a90×1 |
| game/AsciiText.hpp | 1 | 0x4042a0×1 |
| game/BossPresentation.cpp | 13 | 0x4265a0×46 0x42b510×2 0x42b530×2 0x472fc0×2 0x403800×1 0x412890×1 |
| game/BossPresentation.hpp | 13 | 0x4265a0×18 0x425170×4 0x427910×4 0x46fc30×3 0x42b510×2 0x42b530×2 |
| game/BulletAppearance.hpp | 1 | 0x40f030×2 |
| game/DialogueText.cpp | 11 | 0x428ae0×9 0x45aef0×5 0x46fc30×4 0x4282b0×3 0x424ba0×1 0x429d00×1 |
| game/DialogueText.hpp | 8 | 0x428ae0×21 0x4282b0×6 0x46e8f0×3 0x45aef0×2 0x46fc30×2 0x429d00×1 |
| game/DrawCapture.cpp | 1 | 0x4679a0×1 |
| game/DrawCapture.hpp | 2 | 0x45d2f0×1 0x46f380×1 |
| game/EclContext.cpp | 9 | 0x478e60×4 0x478ee0×4 0x477030×2 0x479a20×2 0x479a60×2 0x420380×1 |
| game/EclContext.hpp | 13 | 0x4793e0×2 0x41b250×1 0x41b260×1 0x420380×1 0x420b80×1 0x420d00×1 |
| game/EclFile.hpp | 1 | 0x4793e0×1 |
| game/EclInterpreter.cpp | 6 | 0x4772a0×3 0x472e10×1 0x479480×1 0x4794d0×1 0x479540×1 0x4977aa×1 |
| game/EclResource.cpp | 1 | 0x4795b0×1 |
| game/EclResource.hpp | 2 | 0x4795b0×1 0x4797d0×1 |
| game/EclThreads.cpp | 5 | 0x478db0×1 0x4793e0×1 0x479480×1 0x4794b0×1 0x4797d0×1 |
| game/EclThreads.hpp | 3 | 0x478db0×1 0x4793e0×1 0x4797d0×1 |
| game/EndingScene.cpp | 21 | 0x416a10×30 0x416130×7 0x416610×4 0x46fc30×3 0x4016c0×2 0x4163b0×2 |
| game/EndingScene.hpp | 23 | 0x416a10×16 0x416130×6 0x416610×4 0x4163b0×3 0x416570×3 0x4165f0×2 |
| game/EnemyFog.cpp | 1 | 0x41a3d0×1 |
| game/GameHud.cpp | 19 | 0x425170×8 0x42b030×5 0x42b990×3 0x439ce0×3 0x439730×2 0x4398e0×2 |
| game/GameHud.hpp | 10 | 0x42b030×4 0x424f00×2 0x425170×2 0x4265a0×2 0x46e620×2 0x413270×1 |
| game/GameHudDraw.cpp | 8 | 0x427910×27 0x4038a0×2 0x403da0×2 0x42a370×2 0x4042a0×1 0x42b030×1 |
| game/GameHudFrame.cpp | 3 | 0x4265a0×6 0x424f00×2 0x42c550×1 |
| game/GameKeyboard.hpp | 1 | 0x4525b0×1 |
| game/GameRandom.hpp | 1 | 0x472c90×1 |
| game/Interpolation.cpp | 1 | 0x473a40×8 |
| game/Interpolation.hpp | 3 | 0x466180×3 0x417450×1 0x4772a0×1 |
| game/ItemManager.cpp | 34 | 0x42e380×20 0x438ec0×9 0x443f60×9 0x438730×7 0x413bf0×4 0x42f020×3 |
| game/ItemManager.hpp | 27 | 0x438730×18 0x438ec0×16 0x42e380×15 0x42f020×5 0x43d210×4 0x43d480×4 |
| game/LaserPresentation.cpp | 3 | 0x403800×1 0x433260×1 0x461cb0×1 |
| game/ManualViewer.hpp | 3 | 0x42d740×1 0x42d940×1 0x42da10×1 |
| game/MsgPlayer.cpp | 12 | 0x428ae0×30 0x460bc0×2 0x46fc30×2 0x4016c0×1 0x424ba0×1 0x4265a0×1 |
| game/MsgPlayer.hpp | 16 | 0x428ae0×25 0x42b3f0×4 0x4265a0×3 0x428820×3 0x43af70×3 0x42cd40×2 |
| game/MsgPresentation.cpp | 7 | 0x428ae0×10 0x46fc30×5 0x46f3f0×2 0x429d00×1 0x429f10×1 0x46f4a0×1 |
| game/MsgPresentation.hpp | 7 | 0x428ae0×20 0x46fc30×3 0x46fca0×2 0x424310×1 0x429d00×1 0x429f10×1 |
| game/PauseMenu.cpp | 3 | 0x43fa90×14 0x43e780×10 0x440960×1 |
| game/PauseMenu.hpp | 2 | 0x440960×5 0x43f570×1 |
| game/PauseState.hpp | 6 | 0x440960×2 0x43e780×1 0x43f570×1 0x43f8f0×1 0x454050×1 0x4549d0×1 |
| game/PostGameFlow.cpp | 14 | 0x42cd40×5 0x42c550×3 0x43d820×2 0x43e5a0×2 0x43f570×2 0x43f720×2 |
| game/PostGameFlow.hpp | 12 | 0x42abf0×2 0x42cd40×2 0x43dbc0×2 0x43f570×2 0x428820×1 0x42c550×1 |
| game/ReplayEntityState.hpp | 2 | 0x419950×1 0x419a90×1 |
| game/ReplayFile.cpp | 13 | 0x458c30×12 0x459160×5 0x4484d0×3 0x448c40×3 0x458f30×3 0x4593f0×2 |
| game/ReplayFile.hpp | 11 | 0x448200×5 0x4484d0×3 0x448c40×3 0x458f30×3 0x4480a0×2 0x458c30×2 |
| game/ReplayRecorder.cpp | 7 | 0x4484d0×31 0x4549d0×2 0x424990×1 0x43fa90×1 0x447900×1 0x4480a0×1 |
| game/ReplayRecorder.hpp | 7 | 0x4484d0×12 0x4549d0×4 0x43e780×3 0x43fa90×3 0x424990×1 0x440960×1 |
| game/ReplaySession.cpp | 2 | 0x449070×2 0x447900×1 |
| game/ReplaySession.hpp | 11 | 0x447900×2 0x449070×2 0x401000×1 0x40e840×1 0x40f030×1 0x417b10×1 |
| game/Rng.hpp | 5 | 0x472e10×2 0x47bba2×2 0x449070×1 0x472c20×1 0x472c90×1 |
| game/RunPause.cpp | 14 | 0x43de80×7 0x43e3f0×4 0x43e700×3 0x429d00×2 0x429f10×2 0x43d6b0×2 |
| game/RunPause.hpp | 10 | 0x43e3f0×3 0x43e700×2 0x43f570×2 0x415030×1 0x429d00×1 0x429f10×1 |
| game/RunScene.cpp | 56 | 0x428ae0×22 0x4265a0×12 0x438730×9 0x424f00×6 0x445900×4 0x41b260×3 |
| game/RunScene.hpp | 19 | 0x428ae0×5 0x424f00×4 0x415650×3 0x4282b0×2 0x42cd40×2 0x4265a0×1 |
| game/RunTransitionEffect.hpp | 6 | 0x4035e0×4 0x42c550×3 0x402ff0×2 0x403260×1 0x415650×1 0x46fc30×1 |
| game/SceneMatrices.cpp | 3 | 0x43ba20×6 0x404140×1 0x43bbc0×1 |
| game/ScoreBoard.hpp | 2 | 0x42cd40×10 0x42abf0×2 |
| game/ScoreDat.cpp | 1 | 0x449cd0×1 |
| game/ScorePopups.cpp | 5 | 0x44aa20×2 0x405010×1 0x44a970×1 0x44ad60×1 0x467350×1 |
| game/ScorePopups.hpp | 3 | 0x44a970×1 0x44aa20×1 0x44ad60×1 |
| game/ScreenViews.cpp | 8 | 0x43b900×4 0x43cf40×1 0x45cef0×1 0x45d2f0×1 0x45d570×1 0x45dc80×1 |
| game/SessionState.hpp | 34 | 0x42b990×21 0x42d3c0×6 0x405b80×4 0x42b030×3 0x42cd40×3 0x43d530×3 |
| game/SpellElapsedTime.hpp | 2 | 0x412ee0×3 0x412760×1 |
| game/SpellPresentation.cpp | 17 | 0x427910×16 0x413270×11 0x413a60×7 0x413080×5 0x42a430×4 0x43d530×3 |
| game/SpellPresentation.hpp | 10 | 0x413a60×5 0x42a430×5 0x43d530×5 0x413270×3 0x427910×3 0x403800×1 |
| game/StageBattle.cpp | 59 | 0x419a90×20 0x444140×9 0x405a20×8 0x405b80×7 0x441dd0×7 0x443240×7 |
| game/StageBattle.hpp | 39 | 0x42e380×5 0x438730×5 0x446000×4 0x4057f0×3 0x405a20×3 0x419a90×3 |
| game/StageCommands.cpp | 35 | 0x41b260×59 0x41fe80×7 0x4243e0×5 0x419450×4 0x41fcc0×4 0x4772a0×4 |
| game/StageGvars.cpp | 10 | 0x420380×6 0x420d00×4 0x472e10×4 0x401190×1 0x401310×1 0x401730×1 |
| game/StageMath.hpp | 7 | 0x473a40×25 0x472f30×4 0x472e90×3 0x473780×3 0x41b260×2 0x474f00×2 |
| game/StagePresentation.cpp | 51 | 0x419450×27 0x442960×22 0x438ec0×16 0x4464d0×12 0x445c40×10 0x46f170×10 |
| game/StagePresentation.hpp | 30 | 0x438730×5 0x4464d0×5 0x42b6d0×4 0x438ec0×4 0x441b00×4 0x442960×4 |
| game/StageScenery.cpp | 7 | 0x4076c0×14 0x406120×5 0x4080b0×4 0x406d90×1 0x407310×1 0x43b900×1 |
| game/StageScenery.hpp | 3 | 0x406d90×1 0x407310×1 0x4076c0×1 |
| game/StageScript.cpp | 1 | 0x408390×2 |
| game/StageSim.cpp | 69 | 0x40f030×31 0x40e840×22 0x422f80×14 0x4234b0×11 0x40db10×10 0x4183f0×8 |
| game/StageSim.hpp | 63 | 0x419450×16 0x40f030×13 0x41b260×13 0x40db10×11 0x41fe80×8 0x40e840×6 |
| game/TextureImage.cpp | 6 | 0x43e5a0×1 0x4418b0×1 0x46cf10×1 0x46d600×1 0x46d8f0×1 0x46dc10×1 |
| game/TitleFrame.cpp | 15 | 0x44c570×11 0x454050×2 0x4554f0×2 0x43c1c0×1 0x44dd00×1 0x44fcc0×1 |
| game/TitleFrame.hpp | 3 | 0x452c30×1 0x4554f0×1 0x4560a0×1 |
| game/TitleKeyConfigMenu.hpp | 2 | 0x44c570×2 0x44fcc0×1 |
| game/TitleMainMenu.cpp | 16 | 0x44cd10×17 0x44c570×9 0x44bb80×2 0x450fb0×2 0x415030×1 0x416130×1 |
| game/TitleMainMenu.hpp | 6 | 0x44cd10×3 0x44c570×2 0x416130×1 0x450fb0×1 0x451450×1 0x4525b0×1 |
| game/TitleManualMenu.cpp | 2 | 0x42d860×1 0x455330×1 |
| game/TitleManualMenu.hpp | 5 | 0x42d740×4 0x42d940×4 0x42d860×2 0x42da10×2 0x44c570×2 |
| game/TitleMusicRoomMenu.cpp | 1 | 0x4554f0×9 |
| game/TitleMusicRoomMenu.hpp | 6 | 0x4554f0×10 0x43adc0×4 0x461830×4 0x44c570×2 0x43ae40×1 0x4724c0×1 |
| game/TitleOptionsMenu.cpp | 4 | 0x44ed70×5 0x44dd00×2 0x462450×2 0x46fca0×1 |
| game/TitleOptionsMenu.hpp | 4 | 0x44c570×2 0x44ed70×2 0x4613c0×1 0x476230×1 |
| game/TitlePlayerDataMenu.cpp | 1 | 0x453170×1 |
| game/TitlePlayerDataMenu.hpp | 1 | 0x44c570×1 |
| game/TitleReplayMenu.cpp | 12 | 0x4525b0×31 0x452c30×30 0x417370×2 0x447900×2 0x4172e0×1 0x42c550×1 |
| game/TitleReplayMenu.hpp | 7 | 0x452c30×5 0x4525b0×3 0x417370×1 0x42c550×1 0x4417c0×1 0x44c570×1 |
| game/TitleResultEntry.cpp | 6 | 0x454050×15 0x4549d0×11 0x454690×2 0x4484d0×1 0x44bb80×1 0x453db0×1 |
| game/TitleResultEntry.hpp | 6 | 0x454050×6 0x4549d0×6 0x44bb80×1 0x44c570×1 0x454690×1 0x454f90×1 |
| game/TitleResultView.hpp | 4 | 0x454050×2 0x43d6b0×1 0x453db0×1 0x4549d0×1 |
| game/TitleScene.cpp | 18 | 0x44c570×11 0x453170×3 0x454050×2 0x4549d0×2 0x4560a0×2 0x4576c0×2 |
| game/TitleScene.hpp | 6 | 0x454050×5 0x4549d0×3 0x43f8f0×1 0x44c570×1 0x44fcc0×1 0x4576c0×1 |
| game/TitleSelectionMenu.cpp | 17 | 0x451450×31 0x450fb0×22 0x46fc30×7 0x46f4a0×4 0x46fca0×3 0x44bac0×1 |
| game/TitleSelectionMenu.hpp | 6 | 0x451450×2 0x451cb0×2 0x416130×1 0x44c570×1 0x46fc30×1 0x46fca0×1 |
| game/TitleSpellPracticeMenu.cpp | 2 | 0x456af0×1 0x4576c0×1 |
| game/TitleSpellPracticeMenu.hpp | 6 | 0x456650×3 0x4576c0×3 0x4560a0×2 0x456af0×2 0x44c570×1 0x4564c0×1 |
| game/TitleSpellPracticeView.cpp | 4 | 0x456650×16 0x4560a0×7 0x4576c0×4 0x46fca0×1 |
| game/TitleState.hpp | 16 | 0x44c570×9 0x454050×5 0x4549d0×3 0x4525b0×2 0x453170×2 0x44dd00×1 |
| game/Types.hpp | 3 | 0x4971f0×2 0x477030×1 0x478e60×1 |
| game/YoumuBomb.cpp | 5 | 0x40cb50×9 0x4016c0×2 0x40c910×2 0x40cf70×1 0x46fb90×1 |
| game/YoumuBomb.hpp | 7 | 0x40cb50×6 0x4457e0×4 0x4016c0×2 0x40c910×2 0x4119c0×2 0x472e10×2 |
| rebuild/AddressMap.hpp | 44 | 0x42cd40×4 0x416130×3 0x416570×2 0x43c1c0×2 0x405a20×1 0x40db10×1 |
| sdl/Application.cpp | 66 | 0x42b990×13 0x42c550×11 0x42bfa0×10 0x449070×4 0x4525b0×4 0x46fc30×4 |
| sdl/ApplicationGame.cpp | 35 | 0x42b990×4 0x412ee0×3 0x43d6b0×3 0x4549d0×3 0x424990×2 0x42d3c0×2 |
| sdl/ApplicationState.hpp | 60 | 0x453170×5 0x42bfa0×4 0x4560a0×4 0x42b990×3 0x42c550×3 0x416570×2 |
| sdl/ApplicationTitle.cpp | 23 | 0x42b990×6 0x42da10×3 0x451450×3 0x42d3c0×2 0x43adc0×2 0x415650×1 |
| sdl/AudioDevice.cpp | 6 | 0x4613c0×7 0x460bc0×4 0x460c50×4 0x460dd0×4 0x4971f0×2 0x460d40×1 |
| sdl/AudioDevice.hpp | 8 | 0x4613c0×3 0x4460d0×2 0x460dd0×2 0x44dd00×1 0x44ed70×1 0x460c50×1 |
| sdl/BrowserRuntime.cpp | 18 | 0x42cd40×3 0x461e80×3 0x46f170×3 0x439ce0×2 0x40e840×1 0x40f030×1 |
| sdl/FileStore.cpp | 7 | 0x42bfa0×1 0x4399e0×1 0x43a040×1 0x449a30×1 0x449b00×1 0x449cd0×1 |
| sdl/FileStore.hpp | 10 | 0x416130×1 0x43a040×1 0x449a30×1 0x449b00×1 0x449cd0×1 0x44bec0×1 |
| sdl/FontDevice.cpp | 4 | 0x45aef0×3 0x4282b0×1 0x428ae0×1 0x46e8f0×1 |
| sdl/FontDevice.hpp | 5 | 0x45aef0×2 0x4282b0×1 0x428ae0×1 0x45ac90×1 0x46e8f0×1 |
| sdl/GraphicsDevice.cpp | 5 | 0x43e3f0×1 0x4418b0×1 0x45bf10×1 0x45d7d0×1 0x4692d0×1 |
| sdl/GraphicsDevice.hpp | 3 | 0x4418b0×1 0x45bf10×1 0x45d7d0×1 |
| sdl/SceneDisplay.cpp | 3 | 0x43c4b0×3 0x45bf10×1 0x45d7d0×1 |
| sdl/SceneDisplay.hpp | 1 | 0x45bf10×1 |
| sdl/StageAssetStager.hpp | 1 | 0x42b990×2 |
| sdl/StageTransitionClock.hpp | 5 | 0x42b990×7 0x409270×3 0x42c550×3 0x45f430×2 0x409380×1 |
这一节是提交 `089c4066c` 的快照：两个端口当时引用的 1464 + 1467 个原版地址逐个归类并通过核查，分类表见 `archive/th13-address-snapshot-20261124/`。此后源码不写注释，新增或移动的地址对应关系在这一节和提交说明里登记。

## 已对齐项

原「已知分歧」表里的条目消除后登记于此：每条写明原版依据、移植实现与验收。

| # | 范围 | 原版依据 | 移植实现 | 验收 |
|---|---|---|---|---|
| A | 自机受击碰撞门、圆判定数值语义与判定尺寸（原分歧 1–3） | 四条碰撞包装 `0x444140`（圆弹）、`0x444260`（盒弹）、`0x443fd0`（激光）、`0x44750`（敌机本体）；状态写入 `0x444a10`、`0x444ca0`；`0x441dd0:70-84`；`DAT_004c0ba8…bd8` | 接触后的结算门与原版同构：死亡、入场、受击待决（`+0x65c` 的 2、3、4）与对话中返回 0——不调受击处理、不消弹；其余接触在 `+0x14688<1` 时结算受击，并无条件消弹。灵界位对圆弹与盒弹统一前置，擦弹分支不带阶段门。距离平方与两条阈值经 `rebuild/X87.hpp` 按 80 位（64 位有效数字）各舍入一次后比较。判定尺寸 `kPlayerHitSize` 与 `DAT_004c0ba8` 逐值一致（2、3、3、3） | Unicorn 直调 `0x444140` 的 18 格状态×无敌矩阵与三组距离扫描（[collision.json](../../../archive/th13-fidelity-20261125/collision.json)）；`tests/combat/player_collision_oracle_test.cpp` 锁定边界值（接触 2.23/2.24、擦弹 42.011/42.012）、64 位舍入点，并在 x86 宿主上以 20 万组固定种子与 x87 `long double` 逐位对照 |
| B | ANM 变量表：读值、随机流与写目的地（原分歧 5） | 浮点读 `0x461e80`（跳转表 `0x462120`，id−10000 索引）、整型读 `0x4621b0`（字节表 `0x4622a0`）、浮点写目的地 `0x4622c0`（字节表 `0x462358`，id−10004 索引）、整型写目的地 `0x462380`（字节表 `0x4623ec`）；随机函数 `0x472de0`（[0,1)）与 `0x472e10`（[−1,1)）各取一次 `0x472c90`；摄像机块 `0x4dcaf4`–`0x4dcb44`；角度归一 `0x472f30` | `game/AnmVariables.cpp` 的读表与原版逐项相同：10000–10009 本地变量、10010–10012 视觉流、10013–10015 位置、10022 取模随机、10023–10025 旋转、10027–10029 随机参数、10030–10032 游戏流；`AnmInterpreter.cpp` 处理 10016–10021（摄像机块）与 10026（父旋转求和后归一）。浮点写目的地：10004–10007→`floats`、10013–10015→`position`、10023–10025→`rotation`、10027→`random_scale`、10028→`random_angle`，其余落指令参数槽；整型写目的地：10000–10003、10008/10009、10029 | Unicorn 直调四个函数的 37 个 id 全表与每条随机流消费后的种子、计数（[anm-variables.json](../../../archive/th13-fidelity-20261126/anm-variables.json)）；`tests/anm/anm_variables_oracle_test.cpp` 逐位锁定读值、随机流与写目的地 |
| C | etEx `0x400000` 缩放补间与插值终值的 x87 单次舍入（原分歧 1、未决 2 的插值部分） | `0x40e4d0:24-29`：置 behavior 位 `0x400000`，`+0x12f4/+0x12f8` 存起止值，`+0x131c/+0x1320` 存原始帧数与缓动，`flags(+0x20) \|= 0x40`；弹更新在 `duration==0` 清位；插值 `0x4120f0` 默认路径 `(end-start)*weight+start` 在 x87 扩展精度里只舍入一次（游戏内 `0x472de0/0x472e10` 的 FINIT 把 FPCW 置 0x37F，PC=64） | `game/StageSim.cpp`：`StageBullet::scale_tween`（`FloatInterpolation<1>`）在 `case 0x400000` 置位、起止与原始帧数，`step_bullet_behaviors` 以 `step(rate)` 推进 `hit_scale`，`duration==0` 清位、终值 1.0 再清 `0x40`。`Interpolation.hpp`/`.cpp` 的非角路径改为 `(float)((long double)(end-start)*weight+start)`，终值与 `0x4120f0` 逐位相同 | `tests/ecl/ecl_bullet_scale_test.cpp` 锁定置位、帧数、逐帧缩放与清位；`tests/anm/anm_interpolation_oracle_test.cpp` 以 `0x4120f0` 直调数值（线性 30 帧、平方 20 帧、正弦 10 帧、1 帧）同时锁定 `FloatInterpolation<1>` 与 `EclInterpolation`（[interpolation.json](../../../archive/th13-fidelity-20261126/interpolation.json)） |
| D | 结局 BGM 第二参数与 5 个文本 VM（原分歧 2） | op 10（`0x416a10:210-237`）：`0x43adc0(0, name)` 播放后把音乐名与 `0x4aaaf4` 的 `bgm/th13_18` 比较，调 `0x43ae40(0, 0xf)` 或 `0x43ae40(0, 0x10)`；`0x43ae40` 向 BGM 通道发请求并置 `save[0x1b27a + idx] = 1`（音乐室已听标记）。op 3（`0x416a10:51-92`）：5 个文本 VM 在 `+0x40…+0x4c`，行号 `+0x78` 到 5 回绕，行号为 0 时 5 个 VM 先写白色空格；`0x46e8f0` → `0x45aef0` 以 GDI `TextOutA` 写字（正文色 `+0x7c` 在 (0,2)，描边色 0 在 (±2,0)/(±2,4)），`0x45a3c0` 反相、`0x45a080` 取邻域均值后上传贴图 | `EndingScene::op_music` 以同一比较取 0xf/0x10，经 `record_music_unlocked` 写 `heard_mask`；`ApplicationState` 建 5 个文本 VM（脚本 68–72），`ending_page_line` 以 MS Gothic 栅格化后 2 倍渲染、0.5 倍重采样进 VM 的 512×20 精灵区并 `interrupt(2)`，`ending_page_reset` 写白色空格并 `interrupt(3)`；`op_page_line` 实现同一行号复位与回绕 | `tests/ui/ending_scene_test.cpp`、`ending_fast_forward_test.cpp`、`ending_staff_cue_test.cpp` 锁定 op 分发、行号回绕与 VM 复位；字形由平台字体引擎栅格化（原版 GDI，移植运行时栅格），不在逐位锁定范围 |
| E | 结算B 的 BClear 落档与 `0x414fc0` 降级（原分歧 3） | `0x43d820` 唯一调用点在结算B 态 `0x1f`（`0x440960:549-560`），该态要求 `DAT_004be830 >> 4 & 3 == 1` 且 `GameInf+0x74 == 0`；`.text` 对 `0x4be830` 的 90 处引用里写 `0x30` 位的只有回放装载（`0x447cd2` 置 `0x20`、`0x447ce5` 清 `0x30`）与标题回放入口（`0x44cd80` 清 `0x30`），`0x10` 位没有任何写入方；`+0x74` 在对局场景为 0、练习/回放/重试为 1，模式位非零的运行恒为 1，全部被 `0x42cd40` 送往结算A。`0x414fc0` 在 `+0x5bc & 0x2000` 时把请求改写为 2，而 `0x4dcc5c` 与 `+0x5bc` 都没有置 `0x2000` 的指令（`0x439e10` 只清位） | 两处都是不可达路径，移植侧不实现即对齐：`PostGameFlow` 按请求值直通，`0x43d820` 的分数落档没有对应实现 | 二进制扫描：`.text` 内对 `0x4be830` 的 90 处引用与对 `0x4dcc5c`/`+0x5bc` 的全部写入逐条核对（见归档记录） |
| F | 存档镜像的练习行坐标与通过旗（原分歧 4） | `0x42b990` 写游玩旗 `score+0x7fd+8*stage+64*d`，`0x42cd40` 写通过旗 `score+0x7fc+8*stage+64*d`（stage 取 1 起）；直调 `0x449e50` 生成镜像后两个字节落在列内 `0x7fd/0x7fc`，关卡分数在 `0x7f8`（[practice-rows.json](../../../archive/th13-fidelity-20261126/practice-rows.json)、[save-image.json](../../../archive/th13-fidelity-20261126/save-image.json)） | `ScoreDat::kProgressRowsOff = 0x7F8`，`set_progress_row_flag(column, d, stage−1, 0/1)` 分别落通过旗与游玩旗；`FileStore::practice_cleared[16]`，`RunScene::msg_stage_clear` → `record_stage_passed`（普通版直接落盘，多人版 `PendingFileEvent::StagePass`）；`TH13SAVE` v8 的 `CLR1` 段 | `tests/score/filestore_score_mapping_test.cpp`、`tests/score/filestore_score_test.cpp` 锁定坐标、v8 往返与 v7 升级；`tests/ui/run_scene_test.cpp` 锁定过关写入、回放与演示位不写、失败回传 |
| G | ECL 全局变量供给（原分歧 5） | 整型读 `0x420380`（跳转表 `0x420a04`，94 项）、浮点读 `0x420d00`（跳转表 `0x4214f8`，超范围返回 0.0f）、整型写槽 `0x420b80`、浮点写槽 `0x421670`；整型结果全部过 `0x4971f0` 截断。敌机侧：`+0x1230` 主位置、`+0x1264/+0x1268` 本帧位移、`+0x1274` vel 子位置、`+0x128c/+0x1290/+0x1294` vel 的 speed/angle/radius、`+0x12b8` pos 子位置、`+0x12d0/+0x12d4/+0x12d8` pos 的 speed/angle/radius、`+0x1474..+0x1480` 自变量 int、`+0x1484..+0x14a0` 自变量 float、`+0x14a4` 相位时钟 Timer（−9988）、`+0x5138` life（−9954）、`+0x521c` bit25（−9986，超时结束时为 1）、`+0x11e4/+0x11e8` 敌机 id/创建者 id。管理器：`+0x5c` Boss 槽 0、`+0xb8` 链表数、`+0xc0` 最近创建 id、`+0x10/0x14/0x18` 可读写 int、`+0x2c..+0x38` 共享 int、`+0x3c..+0x58` 共享 float；`0x443f60` 自机方位角、`0x423e20` 距离、`0x417e90` 存活计数、`0x46ff10` 主 ANM 的 `+0x4aa` 脚本号、`0x4be7e8` 残机数、`0x4be7d4` 符卡号、`0x4be7b8`/`0x4be7bc` 关号与难度 | `game/StageGvars.cpp` 的 `integer`/`floating` 按两张跳转表逐 id 供给同一来源（当前敌机、Boss 槽 0、自机三者各自可区分）；写槽按 `0x420b80`/`0x421670`：−9949..−9947 读写同槽、−9943..−9940 与 −9939..−9936 有 Boss 写 Boss 否则写当前敌机、−9914 只读、−9995..−9992 可写子位置；`StageSim::player_lives`（−9930）由 `StageBattle` 在装载、受击与残机变化时同步 | `tests/ecl/ecl_stage_gvars_oracle_test.cpp`（两侧同文件）以 [stage-gvars.json](../../../archive/th13-fidelity-20261126/stage-gvars.json) 逐位锁定 94 个整型读、94 个浮点读与随机流抽取后的种子、计数 |
| H | 插值角路径与 Bezier 的中间量（未决 2 的剩余部分） | 角度插值 `0x4662e0`：mode 7 `wrap(start + ctrl2)`；mode 17 `wrap(start + ctrl2)` 后 `wrap(end + ctrl2)`；mode 8 的四个基函数在 x87 里算好后转 float，乘控制点、转 float、`wrap`，按 end、start、ctrl1、ctrl2 累加，每次累加转 float 再 `wrap`；默认缓动 `wrap(float(wrap(float(w × d)) + start))`，`d` 为 `0x438220` 的角差经 `wrap`；`0x472f30` 的归一是 33 次上限的 2π 循环、每圈转 float。`0x4120f0` mode 8：四个基函数各转 float，四项在 x87 累加后一次舍入 | `Interpolation.hpp` 的 `retail_wrap` 复刻 `0x472f30`；`FloatInterpolation<N, Angle>::evaluate` 与 `EclInterpolation::evaluate` 的 mode 7/17/8 与默认缓动按同一序列计算，`duration==0` 返回 `end`（原版完成分支返回 `+0x4`） | `tests/anm/anm_interpolation_oracle_test.cpp`（两侧同文件）以 [interpolation-angle.json](../../../archive/th13-fidelity-20261126/interpolation-angle.json) 锁定 mode 7、17、8、0、1、18、22 的逐帧值，`scalar_bezier_matches_retail_oracle` 锁定 `0x4120f0` 的 mode 8 |
| I | Boss 血环的槽循环（原未决 1） | 血环段入口的五个门（`0x426694`–`0x4266cc`）：计时 ≥ 0、槽内有 Boss、`mgr+0x7c & 1 == 0`、`HUD+0x198 == 0`、管理器非空；槽循环从 `0x426910` 的计数 0 起，`0x42732f`–`0x42733b` 取槽、`INC EAX`、`CMP EAX,1; JL 0x426918` 只回到第一条，每帧只画一个槽 | `game/BossPresentation.cpp` 的单环即原版行为 | 二进制核对：`0x426910` 置 0 与 `0x42732f`–`0x42733b` 的单次回边 |

## 已知分歧

「已知分歧」表的 5 项全部消除：自机受击（A）、ANM 变量表（B）、etEx 缩放（C）、结局 BGM 与文本 VM（D）、结算B 死代码（E）各自对齐；存档练习行（F）、ECL 全局变量（G）随两轮取证一并登记；未决 1 的 Boss 血环（I）与未决 2 的 x87 中间量（C、H）同样关闭。

游戏指令（`0x41b260` 分发）里未实现的指令不会静默跳过：`game/StageCommands.cpp` 的兜底分支把指令号写进错误并返回 −2。

## 未决项

读反编译定不了结论、需要对原版实机取证的事项。在拿到证据之前保持现状，不猜。

| # | 事项 | 已知事实 | 需要的证据 |
|---|---|---|---|
| 1 | 绘制清单与原版的首次分歧帧 511 / 124 | 登记在 `tools/replay-verifier/golden/baseline.json` | 原版 draw log（本机缺席） |
