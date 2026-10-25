# 第六关多人 Replay 一致性

分关 Replay 独立恢复第六关，背景演出沿自己的原生时钟推进。背景变形使用呈现随机流，敌机、弹幕与道具共享战斗随机流，连续切关与中途恢复保持相同的战斗演算。

## 行为检查

三位独立玩家从第五关进入第六关，一位独立旁观者消费全席位确认输入。驱动持续按住射击与对话快进，1P 同时按住低速，逐帧比较关头之后的 RNG、玩家、敌机、ECL、弹幕、道具与激光状态。

| 场景 | 核对内容 | 结果 |
| --- | --- | --- |
| 第六关连续旁观 | 第 886 至 10885 帧，三端来源与独立观众 | 10000 帧逐帧一致，期间无时间线操作 |
| 背景变形 | 原生 STD、变形网格与独立随机流 | 普通模式 24 项、多人模式 200 项检查通过 |
| 规则版本 5 | 双人／三人，0／2／9 帧延迟，三种时序与原生录像元数据 | 原生协议与浏览器解析共同通过 |

持续旁观使用当前生产对象链接私有观测入口，并在所有实例上维持相同的无敌状态以覆盖完整 Boss 演算。观测构建与生产 WASM 分别记录身份，发布使用生产构建。

## 运行入口

构建 `portable/multiplayer/build-spectator-stage.mjs --production --stage=5`，设置 `TH11_TEST_PLAYER_COUNT=3`、`TH11_TEST_STAGE_ONE_ONLY=1`、`TH11_TEST_UNINTERRUPTED_FRAMES=10000`，运行 `portable/multiplayer/spectator-relay-browser.mjs --transition-first --hold-input --playback-only`。

`stage-rollback.mjs --lane all` 检查普通与多人背景归属及多人回滚恢复。`network-test.mjs` 同时验证原生录像编码、浏览器版本检查和席位元数据。
