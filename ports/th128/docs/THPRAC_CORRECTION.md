# 纠正：TH128练习适配必须以thprac为目标

2026-11-17，用户明确“练习是thprac”，并指出TH128已有对应模块。MAIN此前实现的是7项简易选关，不是该要求的thprac；据此宣布第三阶段完成，判断错误。现撤回第三阶段完成结论，优先补实际TH128 thprac适配。六路线＋EX原单人功能基线与其他已验证Web能力保留；多人代码仅为协议原型，后续推进置后。

已定位上游touhouworldcup/thprac的thprac/src/thprac_th128.cpp。只读参考固定commit b5beb37a91e322d985c8f89bb6377d55bcf07734，文件80569B/SHA91d9a8b8312a4d4bfc9b7df36726983df2864966aba8b66be0cbb145a15b81c3。其THPracParam覆盖mode/stage/section/phase/score/motivation/perfect_freeze/ice_power/ice_area/dlg，另有专用菜单、场景跳转/ECL分支、Replay和高级选项。不是只有Normal第三面和EX七项。

Eagler参考固定commit bfc70b275b49a28a3a77140c9ef373b61348b16c，common/docs/thprac.md及launcher/integrations/thprac.mjs已完整读取。共用输入、统计、速度、导航/移动面板与各作参数、ECL、Replay及生命周期分层。当前TH128源码没有与这些约定闭合的真实thprac consumer；仅现有辅助setter或网页选关不能冒完成。支持声明必须与真实产品能力一致，未移植字段不得假ACK或只改JSON标签。

下一步先逐项建立TH128专用菜单/参数→实际World/ECL/Replay消费者矩阵，复用原TH128模块的真实语义与本工程可证明接口，确定native桥缺口后TDD实现。不是在浏览器运行或注入Windows THPrac EXE，不新增JA/GDI/原EXE/D3D/GPU授权。若确需新native功能，须源/组件/实际consumer RED和新号工程准入；不重编已成功core112，也不为声明支持盲加新API。

临时按钮已改名“简易选关（不是thprac）”，这是避免误导，不算完成thprac。旧846–854测试与原raw保持原范围；本轮纠正文档/标签不重跑成功case。旧并行独立场景多人规则是MAIN原型选择，不是用户已接受最终多人设计。

来源：Ritosa2333/eagler-touhou的common/docs/thprac.md、launcher/integrations/thprac.mjs；touhouworldcup/thprac的thprac/src/thprac_th128.cpp。原始固定版本及MIT许可只读保存在W/thprac-reference-20261117，完整sha与原准备期LICENSE→LICENCE路径纠正记录保留，不把失败时重复的stale响应当有效许可。
