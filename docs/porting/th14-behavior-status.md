# TH14 行为适配状态

TH14 使用独立 C++／Wasm 核心，目标为原作可观察行为级对齐。当前覆盖普通游戏、录像、原生菜单、存档、图形和静音音频运行路径；内部地址、内存布局及逐位 RNG 一致不作为独立完成指标。整作状态为进行中。

## 当前交付

生产入口为 wth.saop.cc，运行代 `12219d17`、Wasm `9d405b73`、源码 `440d512a`。资源取消信号贯通父页交接、归档、字体／音乐请求及写入检查；旧源码取消反例与修正检查配套保存。静音流式 OGG 普通灵梦 A Normal 开局、暂停、返回标题及 Quit 通过，源站／公网各 32 个运行文件和两个前端模块通过完整哈希与隔离头检查。源码合并检出独立于已交付运行目录。

## 验收入口

| 功能 | 构建与证据 |
| --- | --- |
| 加载与取消 | [12219d17 资源安装取消](../../archive/th14-behavior-acceptance-20261117/TH14_RESOURCE_ABORT_NOV17.md)，归档 32 项、资源取消 14 项及生命周期 6 项 |
| 原生菜单 | [七页说明与暂停恢复](../../archive/th14-behavior-acceptance-20261117/TH14_NATIVE_MANUAL_NOV17.md)、[Option 与跨页保存](../../archive/th14-behavior-acceptance-20261117/TH14_NATIVE_OPTIONS_NOV17.md) |
| 音乐室与成绩保护 | [0516c1a9 Music Room](../../archive/th14-behavior-acceptance-20261117/TH14_NATIVE_MUSIC_ROOM_NOV17.md)，前后全部 21,640 字节成绩保持 |
| 音频 | [当前与上游切曲增益比较](../../archive/th14-behavior-acceptance-20261117/TH14_TRACK_VOLUME_NOV17.md)，两实现各 48 组、12,582,912 帧内存 PCM；实际 OGG UI 全程零输出 |
| 录像与普通练习 | [终点显示](../../archive/th14-behavior-acceptance-20261117/TH14_REPLAY_END_PRESENTATION_NOV15.md)、[进入关卡解锁](../../archive/th14-behavior-acceptance-20261117/TH14_PRACTICE_ENTRY_UNLOCK_NOV15.md)，按报告构建复用 |

## 后续验收

继续真实浏览器中途取消、隐藏／恢复、其他音频转场和公网实际操作，以及已有普通游戏／录像／图形报告明确保留的分支。预览与测试的音乐和音效总输出保持 0。有限样本、静态响应或单项修复分别承担其验证范围，整作完成以全部所需实现与验收为准。

## 当前合并候选

已接续上游 `822d4759`，通过目录历史将公共修复同步到 TH14 多人版。普通／多人核心编译链接及资源取消、生命周期、音频竞态检查通过，普通版待接收历史为 0。网站四项构建检查通过；广泛测试有失败并停止，未标全绿。新合并核心未部署，继续普通版实际行为验收。[构建身份与边界](../../archive/th14-behavior-acceptance-20261117/TH14_UPSTREAM_RECONCILE_NOV17.md)。
