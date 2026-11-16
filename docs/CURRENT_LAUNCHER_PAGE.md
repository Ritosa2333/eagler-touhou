# 当前页面的启动器接线（有限适配）

当前import-entry与play已经实际使用launcher-session及page-launch-context。仅当存在合法runtimeEpoch和iframe parent/referrer时启用；独立打开保持本地导入或单网址自动下载流程。入口先准备并验证已有DAT＋OGG包，等待父窗口configure/launch后才导入游戏模块；资源就绪不会当作游戏首帧。

当前支持已安装资源模式、OGG、资源目录已有的语言、limitPresentationTo60和touchEnabled布尔选项。外部DATA映射/非空resources/sharedResources/runtimePack、可变文件命令、MIDI、无音乐和其他选项仍明确拒绝。runtime-info声明partialAdapter:true、fileBridge:false；此ready仅代表这个有限启动页可处理所列命令，不能声称完整上游文件/DATA桥已兼容。上游托管DATA/存档桥另待实施。

play在initialize中绑定实际runtime方法；输入通过独立Symbol来源，清理不释放物理键盘/触摸来源。loadVariant后检查实际module/renderer/active才完成启动等待，模块import成功本身不返回成功launch ACK。首次呈现仅从真实标题renderer.render非空记录后的serial变化通知，保原draw返回false的FPS语义；测试渲染出口为接缝，不冒实际GPU或完整设备画面。

退出会取消加载ticket/controller、结局、输入、renderer及音频，阻止后续rAF和键盘再进旧runtime。入口finally清理provider/store；即使runtime清理抛错仍清其余owner，退出回执失败。已失败模块加载不能重用页面并假回成功，要求重新打开，避免重复owner。

## 本轮验证

795缺context/接线10RED→796当前入口/context10GREEN。797缺play hooks6RED；798首次抽取fixture漏新增getPageLaunchContext依赖6FAIL，原raw保；799只补该依赖后6GREEN。800完整回归144PASS/1产品UI失败：启动import失败后错误显示无效重试按钮；801 guard修复后146GREEN且完整模块syntax通过。802新增失败启动重试假ACK、抛错退出/关页清理3RED/10controls；803最小修复13GREEN；804补完整origin并严格核真实catalog URL后最终149GREEN/40pins。源/原生输入/loader/DOM/parent/renderer均按各测试明确接缝，0实际浏览器、0World/Factory/编译/GPU。失败原始源/输入/输出与成功记录都保留。

本地v7、资源v2、旧772静态目录未改。新网页源码需后续单独部署整合，暂无公开URL。上游DATA/可变文件桥、其他必需选项及第三阶段剩余项继续推进，多人尚未实施。

## 真实 iframe 通信805

当前完整入口、context与session模块未改，放入真实父页面和不同端口的iframe中验证，25项检查全部通过。父来源与epoch隔离、请求字符串关联、配置、等待真实运行hook就绪后的launch ACK、键盘及清理、重复请求回执、退出和加载中取消均实际经过postMessage。未实现的音乐、外部资源和文件能力仍明确拒绝。运行20.861秒；一个临时标签和两个临时服务器在同一工作流清理，ROOT释放，控制台错误0。

资源/provider/store、play/native输入和呈现是明确接缝；没有创建游戏World、WASM工厂或画布。此处first-frame仅验证显式呈现接缝的通知，不证明游戏可见像素、帧率、资源/IDB或存档持久化。上游DATA回调要求同源，与这里的跨源消息测试是两项不同能力。当前仍是partialAdapter，尚未完成第三阶段。

详细检查及原始证据指纹见[805记录](launcher-iframe805.json)。下一步保留原DAT解密消费者，接上游真实DATA、资源及严格可变文件收据。

## managed父数据810–816的当前范围

当前入口已接同源父窗口原DATA回调，依赖仍由已完整校验包提供，父DATA须匹配该包的私有收据并真正进入原解码器。非空resources现在可以验证已挂载文件的相同字节，运行中验证不阻断键盘，退出取消迟到回执；新挂载、变更内容和未知runtimePack仍拒绝。这取代上文旧795–804阶段“所有非空外部资源都拒绝”的当前结论。partialAdapter及fileBridge:false保留，严格可变文件和其余必需模式继续推进。

811当前入口17项、813网站源依赖13项、814原32MB DAT/113raw/10OGG临时HTTP及815受影响回归97项通过。816另在真实同源iframe用自编小素材完成17项DATA/Blob/退出检查，存储、游戏运行和呈现仍是接缝。不是完整原游戏浏览器验收，未公开发布。见[当前managed接线](MANAGED_LAUNCHER_DATA.md)。

## Replay文件817–825的当前范围

当前页面新增实际25槽Replay的list/read/write/remove/sync，路径为adapter-owned相对名replay/th128_00.rpy..24.rpy；完整文件摘要、原codec、目标槽CAS、strict事务和完整回读接游戏原存储。写删仅启动前，运行中sync只核已提交Replay记录，明确storeOnly与非运行游戏flush。完整fileBridge:false保持；上文“文件命令尚未接”的描述仅代表810–816历史阶段。

821受影响115项及825范围修正后的20项通过，822真实隔离浏览器IDB14项和跨文档重开通过；823原354KB已生成RPY在新桥中逐字节保持并接回游戏Replay读取者，无新World或录像执行。完整分数和原配置桥继续推进，见[Replay文件桥](REPLAY_FILE_BRIDGE.md)。

## 原生分数只读导出826–831

当前文件接口已新增adapter-owned `scoreth128.dat` 的list/read，从真实OwnedNativeTitleRecordStore验证通过的既有档案生成T821容器及完整SHA；缺失返回errno44，不创建默认档案。该名字不是已证明的原版文件名，score写入/删除和原binary配置仍拒绝，fileBridge:false保持。

827格式15项、829当前codec/导出24项、830生产C++双向差分及831受影响144项通过。830只编译一个组件、未重编core112；当前入口存储/页面为明确模拟，没有新浏览器或游戏运行。详见[分数导出说明](NATIVE_SCORE_FILE.md)。
