# Runtime116 v8 本地交付（2026-11-17）

第三步是协议、高刷新率、触控、多语言、thprac、Replay六项，已在[约定范围](WEB_FUNCTIONAL_SCOPE.md)内完成。最终1006/1008闭合此前真实thprac缺口，其他五项和六路线/EX基线保持。用户“第三步如果都完成了 就可以打包了”授权本地打包；公开发布仍暂停。

| 文件 | 字节 | SHA256 |
|---|---:|---|
| th128-runtime116-dat-ogg-v8.zip | 3591694 | 044745547e771bf46e2d69d160c91840c1a22e169d6c8e414a620de379c1794b |
| th128-dat-ogg-5e3bcbd5ebb8b171.zip（沿用） | 93317333 | f8eec6698f3ecdc94f6d64ce3413800a7558201d365e468e9c27ac61fc2504a5 |

Runtime在owned目录Web-delivery-20261117，ZIP旁有runtime116-dat-ogg-v8解压目录。61文件完整清单在包内manifest.json及SHA256SUMS.txt。原资源ZIP保留在Web-delivery-20261116/resource-package-dat-ogg-v2，没有重新生成或改字节。

解压Runtime并运行Start.ps1，需要Python3.9+及支持WebGL2的浏览器。打开窗口显示地址，默认http://127.0.0.1:8140/play.html，首次选择原资源ZIP，校验后点“开始游戏”。新8140与旧8138使用独立浏览器存储，无需停止或刷新旧页。Start.ps1 -Port 8148可选其他端口。

包内49份当前production网页文件和成功core116原字节配套，含全部6个thprac模块、协议/输入/语言/Replay依赖及许可。没有夹具入口、内部验收控制栏、个人数据或多人原型代码。本次没有改游戏原生/C++/ABI或重编核心。

1009通过：61ZIP条目CRC/完整读回、74模块依赖、44JS语法、52静态文件HTTP完整字节、MIME/HEAD/Range206/路径拒绝与服务退出。启动器读取真实core116/generation，旧v7和资源ZIP的完整SHA保持。

1010新包入口实测：原样import-entry/play，无函数抽取或入口替换；真实文件选择器导入93317333字节资源ZIP和隔离IndexedDB，原生Title→难度/路线自动thprac→原游戏、Backspace/F12，6项通过/24.198秒/console0。正常WebGL2/字形纹理/音频没有作者替身。最终整页截图按DOM文档坐标离线裁出F12，已核真实游戏背景和无测试栏。实际Factory/World/tick次数未插桩，不报精确次数；申请上界12/4/30000。ownTab34、服务及ROOT已关闭。

本地交付不等于公网部署、多人完成、所有硬件FPS或617旧录像全兼容；额外兼容项仍单列维护。MAIN作者单独自审，无独立worker审批声明。完整凭据见[runtime116-v8-local-delivery.json](runtime116-v8-local-delivery.json)。
