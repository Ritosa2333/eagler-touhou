# TH13 保真地图

C++ 单元与原版 th13.exe（v1.00c，SHA256 `f12760ca…80d`）的对应关系。三部分内容各自有固定的来源：

- **子系统入口**：`web/cpp/rebuild/AddressMap.hpp` 的登记表，逐条带证据。
- **逐文件映射**：由 `tools/decomp/fidelity-map.mjs` 从源码与 Ghidra 导出的函数索引生成，不手写。
- **已知分歧与未决项**：对照反编译逐条核对的结论，写明地址。

## 证据链

保真由三层证据支撑，强度依次递增：

1. **地址引用真实**。源码引用的每个原版地址都逐个核对过类别：导出函数的入口、函数体内的指令边界、跳转表、回调入口或数据地址。`tools/decomp/address-check.py` 重跑这项核对，任何地址落在指令中间或映像之外都以非零退出。反编译索引用 `tools/decomp/export.sh` 在容器内导出（见 [DECOMP.md](DECOMP.md)），2291 个函数全部反编译成功。
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

<!-- fidelity-map:begin -->
源码共引用 1464 个不同的原版地址、4231 处引用，全部落在映像内：2613 处恰好是函数入口，1128 处落在函数体内（函数内的具体指令），388 处是数据地址，102 处在代码节内但不在导出的函数体内。下面每个文件列出它引用的原版函数（按引用次数，最多六个）。路径省略 `web/cpp/` 前缀。

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
<!-- fidelity-map:end -->

重新生成（容器内，需先导出反编译索引）：

```sh
ports/th13/tools/container.sh bash -lc 'node tools/decomp/fidelity-map.mjs /work/artifacts/decomp/jp --pe /work/games/th13/th13.exe --write'
```

## 已知分歧

下列各项都已对照反编译确认，按对玩家的影响排序。

| # | 位置 | 分歧 | 原版依据 |
|---|---|---|---|
| 1 | `game/StageBattle.cpp` `collide_player_bullet` | 自机死亡与无敌对碰撞门的映射是近似：`player_dead` 对应 state 4，`invulnerable > 0` 对应 state 2 与 `0x14688`。原版由 `0x444a10`、`0x444ca0` 写入完整状态机 | `0x444140:13` |
| 2 | 同上 | 圆形判定先把距离平方落成 f32、阈值用 double 比较；原版全程 x87 80 位。生产与原版同式（严格小于），但 x87 尾差未认证 | `0x444181`、`0x4441a1` |
| 3 | `game/StageBattle.hpp` 命中尺寸常量 | 判定盒与碰撞半径的定标值标注为待校准 | `0x446870`、`0x441dd0:70-84` |
| 4 | `game/StageSim.cpp` etEx | 弹的 etEx `0x400000` 未实现；绘制侧契约已就位，位一旦置位即生效 | `0x40e4d0:24-29` |
| 5 | `game/EndingScene.cpp` | 文本页用一份近似渲染缓冲代替原版 5 个文本 VM；结局 BGM 的第二参数 `0xf`/`0x10` 语义未定位 | `0x46e8f0`、`0x43ae40` |
| 6 | `game/PostGameFlow.cpp` | Extra 通关的结算B 覆盖层之后的存档写入未实现；六面通关转结局处 `0x414fc0` 的非激活降级未移植 | `0x441930`、`0x43d820`、`0x414fc0` |
| 7 | `sdl/FileStore.cpp` 存档镜像 | 过关时原版往练习行写入通过旗；该字节在原版里没有任何读取方，移植侧的镜像不写它。练习解锁用的游玩旗按原版语义落盘 | `0x42cd40:100-103`、读取方仅 `0x451cb0`、`0x452170` |
| 8 | `game/AnmVariables.cpp` | ANM 可写变量只开放原版写侧的子集 | `0x462380`、`0x4622c0` |
| 9 | `game/StageGvars.cpp` | ECL 全局变量供给是行为子集，待校准清单见 [event-contract.md](plans/event-contract.md) | `0x420380`、`0x420d00` |

游戏指令（`0x41b260` 分发）里未实现的指令不会静默跳过：`game/StageCommands.cpp` 的兜底分支把指令号写进错误并返回 −2。

## 未决项

读反编译定不了结论、需要对原版实机取证的事项。在拿到证据之前保持现状，不猜。

| # | 事项 | 已知事实 | 需要的证据 |
|---|---|---|---|
| 1 | Boss 血环在道中非符卡阶段是否显示、同屏双槽时是否出两条 | 原版 `0x4265a0:180-226` 对每个占用槽独立判定：存活、`flags & 0x31 == 0`、无敌计时 < 1 | 原版各阶段的画面取证 |
| 2 | x87 80 位中间量与 SSE2 的尾差 | 涉及双精度链与超越函数的路径只在少数边界帧可见 | Unicorn oracle 或实机状态采样 |
| 3 | 绘制清单与原版的首次分歧帧 511 / 124 | 登记在 `tools/replay-verifier/golden/baseline.json` | 原版 draw log（本机缺席） |
