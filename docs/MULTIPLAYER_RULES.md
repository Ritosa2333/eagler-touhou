# 多人阶段首个实现：规则握手

2026-11-17，第三阶段有限Web功能由可见练习846–854闭合后进入第四阶段。以下是MAIN在用户已授权范围内作的首版工程选择，可由用户后续调整；没有把这些选择冒称为已商定竞技规则。

首版采用两位玩家各自独立的同起点对局：同关卡、难度、seed、运行核心和资源指纹，保留原单人逻辑与输入历史，不新增共享碰撞/伤害或改RNG。最小范围沿用六路线第三面Normal与EX起点；所有多人局保持辅助/非正常通关资格。先形成可校验的协议规则，不公开未实现入口。

本次只实现不可变规则对象与SHA256握手：固定60Hz；route推导stage/rank，seed0..65535；固定两位玩家；inputDelay0..8，rollbackWindow0..120；coreWASM与resource包完整SHA、显式matchId和epoch。未知字段/缺字段/错误模式/不支持值拒绝，先复制后hash。接收端按完整规范化规则比对并拒绝差异。SHA绑定字节一致性，不是账号认证或对等方身份签名。

后续顺序：有界逐帧输入账本（来源player/epoch/frame、重复/乱序/迟到修正）→协议重开需双方同意并换epoch→Replay日志与旁观只读消费→实际页面/传输与native consumer。当前core112没有已证明完整World save/restore接口；不能把th128_world_state诊断JSON、opaque跨关carry或Score/Replay恢复冒称为全World rollback。可先实现纯调度/账本并用显式合成模拟器验证，再定位真实确定性重建/回滚策略；不重编成功core。

855缺规则模块10RED＋1条过宽拒绝断言PASS，856先收紧该断言再11GREEN；Node纯规则/crypto，不创建World/浏览器/服务器/GPU/持久数据。模块为site/runtime/multiplayer-rules.mjs，当前只有规则组件，尚无页面调用或包内接线。transport、实际两玩家游戏、回滚native状态、重开同步和旁观尚未实施。hosting/本地打包保持暂停。
