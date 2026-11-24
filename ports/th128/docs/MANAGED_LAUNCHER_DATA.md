# 当前页面的 managed DATA 接线

当前入口支持显式 `managedData=1&gameGeneration=…&runtimeEpoch=…`。父窗口必须同源，提供原协议回调 `__eaglerPrepareManagedRuntimeDataV1({game:'th128',generation,epoch})`。入口检查窗口、文档、URL、回调和取消信号，等待回调返回原DATA缓冲区及fileId/bytes，再按已验证包的私有SHA收据校验并交给原THA1解码器。

依赖来源保持明确：完整的已验证DAT+OGG包提供描述符、贴图、语言和全部音乐；父窗口提供原DAT。成功adopt前不发布ready，失败不替换已有消费者。原DAT替换后清掉旧raw/resource缓存，后续读取真正走父数据的自有副本。包来自当前自动下载或已有资源缓存，独立于父窗口的post-ready资源消息，因此没有ready等resources、父窗口又等ready的循环。

resources只验证描述符中已经挂载的相同文件。只接受同源HTTP/HTTPS/Blob URL、精确路径、长度及摘要；冲突、新挂载和未实现runtimePack明确失败。已运行时资源验证使用独立忙标志，键盘和退出仍能处理；退出取消在途验证，禁止迟到成功回执。文件、配置和Replay桥仍未实现，页面继续报告partialAdapter:true、fileBridge:false。当前完整包的12个文件目标与771个内部资源目标不是同一个命名空间。

810先观察到当前入口缺managed接线的15项失败、2项控制通过；811同17项全通过。812复现网站组装漏新模块，813增加实际依赖并完成13项组装检查。814用原32,336,111字节DAT的跨realm缓冲区走生产owner和解码器，重新核对113份未翻译raw资源，10份OGG同字节收据经真实临时HTTP验证。815受影响入口、协议、导入和组装回归97项全通过。

816在真实同源父窗口和iframe中完成17项检查，覆盖跨realm DATA、真实父Blob URL、配置、首帧通知界限、运行中资源验证不阻断键盘，以及退出取消；29.008秒后同一工作流关闭临时标签和服务器。此浏览器目的用小型自编THA1素材，存储、play/native输入和呈现为接缝；没有原游戏画面、WASM工厂、World、GPU或真实IDB验收。814的原资源与816的真实浏览器是两项独立证据，不能合并成完整游戏验收。

当前仍需要完整兼容资源包，不支持任意上游资源更新、所有选项、MIDI/无音乐或可变文件。30秒回调超时有实现但尚未做超时等待验收；回调退出取消已覆盖。具有不同编码Content-Length的资源响应会拒绝，不能宣称适配任意CDN压缩设置。

本地v7、资源v2和旧772网站目录保持原字节；新源代码尚未公开部署。详细指纹和边界见[810–816证据](managed-caller810-816.json)。下一步连接原生存档、配置和Replay的严格list/read/write/remove/sync收据，继续第三阶段；多人第四阶段尚未实施。
