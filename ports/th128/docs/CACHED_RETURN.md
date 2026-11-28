# 网页历史返回与新会话842–845

页面关闭/离开时仍按原流程退休输入、运行context、资源provider和存储连接。若浏览器直接复用该已退休的缓存文档，新的pageshow处理只在persisted=true且旧页面已关闭时请求一次完整刷新，建立新会话；不会重新激活旧World或输入历史，也不清资源缓存。普通首次pageshow、未退出的活owner和非缓存返回不会触发循环刷新。

842使用完整当前入口、session和page context复现3项失败、9个控制项通过。843新增四行守卫后同12项全部通过，包括运行后/资源就绪后返回、初次显示不循环、清理抛错仍能请求新文档。这些persisted事件和reload是明确CPU边界，不能称为实际浏览器BFCache命中。

## 真实浏览器844

通过真实链接离开后调用浏览器返回，12项检查、32个输入指纹通过，29.850秒内关闭临时标签、服务器并释放ROOT；控制台错误0。实际记录的父页和子页pageshow/pagehide都为persisted=false，说明该环境走重新加载的历史回退。验证了旧runtime实际退出、新child identity、原DAT目标读取、none配置、新context和资源IDB保持，只下载一次自编5285字节资源包。

此处没有实际命中persisted=true，因此没有对缓存文档正向恢复作设备证明。测试没有更改浏览器策略或注入伪事件。游戏运行/呈现/声音仍是接缝，0World/GPU，不证明物理声音或FPS。

![隔离历史返回结果](D:/Codex-th128-owned-20261108/menu-lifecycle-fixes-20261113/case844-browser-cached-history-return/result.jpg)

845受影响110项全部通过，0失败/跳过，44个输入指纹，退出码0。成功843/844/845不重跑。历史770–845共1622个输入引用，1345当前精确、277精确归档，0未解。第一次历史核对缺旧ONLINE_LAUNCH文档，已从完整旧读回保留精确2927字节、SHA `bb56c621986b5188d75ed84c905166aa9f1e25159d1e06b7a693e5e364ec6f77`；首次未解报告保留，新v2报告闭合，没有改旧请求或结果。

源码复核对“CPU零覆盖”的结论读错了测试路径；实际改变的是launcher-session/current-entry.test.mjs，842/843包含新分支RED/GREEN。复核正确指出844没有实际BFCache命中，MAIN保留该限制。reload失败后的提示/重试仍是小项待办。

Web功能范围已有限列出，现还剩一个具体用户入口：已有选关和taint函数，但现控件隐藏于开发工具，不能算可用练习界面。详见[有限Web功能范围](WEB_FUNCTIONAL_SCOPE.md)。第三阶段尚未声明完成，多人尚未实施，公开上线和本地Runtime打包保持后置。
