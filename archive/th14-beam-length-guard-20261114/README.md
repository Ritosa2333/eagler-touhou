# TH14 魔理沙激光回退长度保护

普通模式现在保留有界的上一帧长度，避免异常栈临时值将光束几何放大到非有限或约 10²² 的范围。保护使用现有增长上限 512+18，正常交点计算保持原路径。核心来源 85a60542；行为验收目标见 [THP 目标](https://github.com/AdingApkgg/thp/blob/6649cb53657e22bd8b56b17d5c9cfd4dd654b9f0/GOAL.md)。

网站可选补丁以 5eb4ac87 为基线，只更新 ShotCallbacksTh14.cpp 的补丁区段，其余段落保持原字节。普通模式启用相同保护；显式 thprac 激光记录／播放／恢复模式沿用原记录值及修正规则，保留 NaN、Infinity 等已声明值的语义。

## 验证范围

实际可选头文件和正常权限 C++ 组件通过 4,032 组原版有效输入、48 组边缘、30 组异常保护和 21 组记录／播放／恢复检查。Linux 直接读取基线 Git 文件后，整份新补丁检查及应用通过。初次模式 3 测试误将输入放入 playback，改用该模式实际读取的 recovered 后通过；游戏实现未因此改动。

普通核心对应模块在 Arch 私有预览中通过实际 WebGL2 魔理沙 A Normal 开局、射击、暂停及返回标题，约 60 FPS；OGG 流式模式音乐和音效均为 0。39 个运行响应经完整大小、SHA256 与线程隔离及 no-transform 核验。详见 [核心浏览器证据](https://github.com/AdingApkgg/thp/blob/6649cb53657e22bd8b56b17d5c9cfd4dd654b9f0/docs/porting/TH14_BEAM_BROWSER_PREVIEW_NOV14.md)。

可选完整运行包尚未重编，普通版短流程不代替可选版浏览器或完整游戏验收。生产已采用核心 85a60542／generation fd54b847；39 个公网运行文件完整响应、哈希与隔离检查通过，旧版本、18 首 Vorbis 和回滚目录保留，服务进程保持。相同运行版本的静音预览短流程通过；公网浏览器工具两次超时，公网画面操作复验继续。[机器记录](qualification.json)。[当前适配说明](../../docs/adaptation/th14.md)。
