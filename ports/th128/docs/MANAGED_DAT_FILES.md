# 分文件原DAT资源消费者

资源消费者增加 `createDatOggProviderFromFiles(descriptorBytes, filesById, options)`，供后续启动器按原包文件ID交付数据。它与现有完整ZIP导入共用原DAT解密、索引校验和资源读取路径；原资源包不需要改版。

descriptor必须是UTF8 JSON字节，filesById为Map，值为Uint8Array。文件ID、数量、长度、SHA256和挂载布局必须与描述符一致。构造会在第一次异步操作前复制整个文件表及缓冲区，调用方随后修改或转移原缓冲区不能改变消费者。仍然读取真实THA1 DAT，ZIP改扩展名不能冒充DAT。

806先观察到缺工厂的20项失败，现有ZIP格式控制通过；807实现后21项全通过，覆盖所有权、错误资源、摘要、布局、异步取消及读取隔离。808独立走分文件来源，读原32,336,111字节DAT、10份OGG和中文ZIP，共12份文件，逐项读取771个目标并核对完整收据，其中包含33份ECL的原始includes和provenance。旧ZIP导入/重开消费者另用809的4项检查通过；存储明确使用内存接缝。没有运行浏览器、游戏World或GPU。

现有 `createDatOggProvider(zipBytes, options)` 接口保持可用；导入器和新分文件接口调用同一验证核心。原DAT、93MB资源ZIP、本地v7及已成功的核心没有重建。

806–809交付时入口尚未接上游managed DATA；后续810–816已连接同源父原DATA和已挂载文件的同字节resources校验，见[managed接线](MANAGED_LAUNCHER_DATA.md)。严格可变文件桥仍未实现，继续报告partialAdapter和fileBridge:false。本项完成的是该桥要使用的真实资源消费者，第三阶段仍在推进。后续需要把{game,generation,epoch}、原DATA、描述符和resources收据绑定到同一生命周期，再连接原存档、配置和Replay消费者。

边界：此消费者验证描述符与文件相互一致；可信发布来源仍由外层持有。返回字节保持复制隔离。相同目标的在途读取共享缓存，不承诺两个并发请求各自取消时完全隔离；失败后的新请求重试已覆盖。无帧率、实际游戏画面、物理音频或存档持久化验收声明。详细记录见[806–809证据](managed-files806-809.json)。
