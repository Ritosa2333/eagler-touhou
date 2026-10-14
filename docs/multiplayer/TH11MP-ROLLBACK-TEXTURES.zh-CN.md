# TH11MP 动态文字纹理回滚组件记录

日期：2026-10-13。工作树：D:/workspace/eagler/worktrees/th11-rollback-smoothness。

## 当前状态与范围

本记录对应主机侧 live CPU 文字纹理 owner。生产接线、独立测试和 runner 已写入并 readback；root 已串行执行 texture-component-01，ordinary 1 组 / 115 checks、MP 8 组 / 600 checks 全部通过，receipt 的 passed、sourceStable、assets.stable 均为 true。Application 的预测调度、World 与 host 统一事务接线、SDL/GL 上传仍需独立证据；本组件没有启用预测或修改 canonical hash 门禁。

生产改动只有：

- 新增 th11_web/cpp/sdl/TextureRollback.hpp、TextureRollback.cpp。
- 修改 th11_web/cpp/sdl/GraphicsDevice.hpp、GraphicsDevice.cpp。
- 修改 th11_web/cpp/sdl/FontDevice.cpp。

测试为 portable/multiplayer/texture-rollback.cpp 和 texture-rollback.mjs。没有修改 game、World、ANM、普通版游戏逻辑或公共 journal。

## 为什么文字纹理有独立 owner

FontDevice 将文字写到 GraphicsDevice 中的 live TextureImage::pixels。ANM VM 的 flags 恢复正确，并不意味着这块 CPU atlas 已回到旧状态；GPU 也可能仍缓存未来文字。因此文字像素必须成为单独的回滚 owner。

旧文字请求不能替代旧像素前像。文字写入有矩形边界、不同字体与格式、较短替换、失败与早返回，未被改写的 atlas 区域仍有状态。新实现第一次写入前保存整个 CPU pixels allocation 的字节；同一 checkpoint 后续写同一个 handle 复用第一份前像。没有复制 std::vector 头，也没有把 GPU generation 放进 journal。

TextRaster::scratch 是每次 rasterize 重置的临时区域，不作为逻辑历史恢复。FontDevice 的 writes 为诊断计数，不恢复。vm.flags 仍属于 World/ANM 字节 owner；TextureRollback 不重复 Touch VM。

## 事务 API 与调用顺序

TextureRollback 是无 SDL/GL 依赖的组件。host 提供按 handle 查找当前 live TextureImage 的回调，以及将该 handle 标记 dirty 的回调。TextureRollbackFrame 不可复制，关联 owner identity、逻辑 frame、保存的纹理身份和共享 history lease。

正常 checkpoint 必须覆盖全部会写文字的逻辑更新与语义 Draw：

1. common journal.BeginFrame。
2. GraphicsDevice::BeginTextureRollbackFrame(journal, frame)，以及其他世界 owner Begin。
3. 执行完整逻辑窗口。FontDevice 在原 CP932 raster 和本地化 TTF raster 分支之前调用 BeforeTextureWrite。
4. 所有 owner End；EndTextureRollbackFrame 返回 bool，必须被检查。
5. journal.EndFrame 成功后才允许将完整 checkpoint 视作可回滚历史。任何 owner 或 journal 失败都不能提交预测结果。

恢复必须遵守以下顺序：

1. 停止全部 observer，保留将跨越的全部 typed frames。
2. 世界各 owner 与 ValidateTextureRollbackFrames 在 Undo 前完成 preflight。
3. PrepareTextureRollbackRestore 传入 common UndoTo 将跨越的全部纹理 frames，含没有写文字的 frame；该函数建立全部 handle 的去重并集，并调用 backend.flush。
4. common journal.UndoTo 恢复全部原字节。
5. 恢复 ANM、变形池、World 等全部 typed metadata。
6. FinishTextureRollbackRestore 再次检查 identity，然后对并集中的每个 handle 增加当前 revision。
7. 直到全部恢复完成才销毁未来 checkpoint。仅保留目标 frame 或提前清除未来 frames 都违反合同。

传入 frames 集合的完整性属于调度器职责；组件能验证 owner、frame 资格、重复、目标存在和 live identity，不能凭调用者提供的列表推导出被遗漏的 checkpoint。采用 checkpoint stride 时必须传入 common journal 实际将恢复的 checkpoint 起点，不能把请求帧号直接当作已确认的 restored_frame。

GPU revision 不回退，只在当前值上增加。否则旧 GPU generation 可能与恢复后的 CPU pixels 看似一致，导致纹理上传被跳过。dirty 必须包含仅在更晚 frame 出现的 handle，不能只看目标 checkpoint。恢复前 flush 的作用是先消费引用未来像素的已排队绘制，避免 Undo 在提交过程中改变其数据来源。

## live identity 与生命周期

每条前像记录 handle、TextureImage 对象地址、pixels.data、pixels.size、width、height、pitch、packed_format 和 PixelFormat。Undo 前先通过 handle 查找当前 live 对象，再比较保存的身份；不直接解引用可能已释放的旧对象。Prepare 后到 Finish 之间禁止文字写入和资源改变，Finish 还会复验全部保存身份。

history lease 是资源边界锁，不是 TextureImage 的内存所有权。正常 host 必须在 frame 存在时保持对应资源和 allocation 原址存活，且 frame 对象必须比 active observer 活得更久。validate 拒绝非法替换并不意味着允许先释放再 Undo。错误身份测试保留原对象/原 allocation，验证被拒绝后先恢复合法身份，再进行真实 Undo，避免用 UB 证明安全。

即使某 frame 没有写入文字，也持有 history lease。资源身份在整个可回滚区间都需要稳定。owner 析构后，保存 frame 的 weak identity 失效；这不能替代销毁运行会话前正确清理 journal 的责任。

## 资源屏障及失败合同

普通非 MP 编译不包含 TextureRollback，不改变原 new/load/write 路径。MP 在没有历史、没有 observer、没有 Prepare 时允许资源改变。

已有资源的 preload 查询直接返回 true，不重复分配或写入，在历史存在时仍可使用。真正新 preload、已存在资源 unload、GraphicsDevice 初始化、startup texture 的新建/释放都会在任何资源修改之前检查全局纹理边界。被拒绝时设置 GraphicsDevice.error 和 TextureRollbackBoundaryRequested latch。bool API 返回 false；既有 void unload/startup API 通过 latch 明确上报拒绝。

调度器必须检查并处理 latch，进入全局确认边界后清除完整历史、重试资源操作，确认成功后才 ClearTextureRollbackBoundaryRequest。不能把失败当成“无纹理可继续”，不能静默推进依赖缺失纹理的游戏帧。

BeforeTextureWrite 失败同样设置 host boundary latch，且不执行后续 raster。存在历史却没有 active observer 的文字写入会被拒绝，以暴露漏接事务窗口；无历史时仍保留正常 MP 写入能力。

容量不足会使 common journal 失败，并令纹理 frame 无效。其他 owner 使 journal 失败时，EndTextureRollbackFrame 也返回 false 并令纹理 frame 无效。非法 live identity 等本地失败可能只使纹理 frame 无效，故不能只检查 journal.EndFrame。调用者必须把所有 owner 结果合并为完整事务结果。

Prepare 失败发生在 Undo 之前。Finish 若发现后续身份改变，拒绝 dirty 并保留 prepared 状态；Cancel 仅清理 prepared 标记，不自动重建世界、不执行 Undo，也不修复非法资源身份。发生已执行一部分世界恢复的错误时，应按统一失败恢复策略处理，不能继续普通游戏。

## Renderer、暂停截图与资源销毁的只读审计

Renderer::issue 每次真实绘制提交都会查询 surface(handle)。surface 比较 GPU version 与当前 CPU revision，发现不同时上传整块 live CPU pixels；因此恢复后脚本即使没有再次发出文字请求，跨帧 union 的 revision 增量也能使下一次真实绘制进入上传路径。这是源码链确认，不能代替实际 WebGL 上传验证。

capture_pause_frame 使用 GPU resample，然后 graphics.changed 将 CPU revision 推到 GPU output.version 相同的值，保护刚生成的 GPU 截图；CPU pixels 没有获得截图前像。暂停截图必须在 driver 的完整确认屏障后执行，不能放入本 CPU 回滚事务。若同一 atlas 混用 GPU 截图与 CPU 文本，后续恢复仍必须遵守其 confirmed-only 生命周期，不能假定 CPU 像素含有 GPU 结果。

Application 的 release(StageResources) 和 release_animation 是 void 回调。GraphicsDevice 拒绝 unload 并置 latch，只能阻止 GPU/CPU 纹理删除，无法阻止更外层立刻销毁 AnmResource、script vector 或其他 world owner。driver 必须在可能触发 begin/release/切阶段的世界动作之前完成确认并清除完整历史，不能先执行 Update、销毁资源，再看到 latch 后试图重试。任何资源屏障都要覆盖世界资源和纹理资源，而不仅是 GraphicsDevice map。

## 独立组件门禁设计

runner 使用 WASI32、-O2、TH11_DEVELOPMENT_HARNESS=0，ordinary lane 不定义 TH11_MULTIPLAYER；MP lane 编译真实公共 RollbackJournal。编译依赖是 TextureRollback.cpp、TextRaster.cpp、GlyphAtlas.cpp、ImageResample.cpp、TextureImage.cpp 和专属 C++ oracle。GraphicsDevice/FontDevice 的源码与头纳入 source manifest，但没有编译/执行这些 SDL host 类。

字体默认来自 D:/workspace/eagler/th11-eagler/build-eagler/fonts，可用 --font-dir 指定同格式真实导出字体。六个文件为 font0.bin 至 font3.bin、cp932.bin、blend4444.bin。runner 记录每个字体 SHA-256、大小与 aggregate digest，并在构建前后与结束时检查未变化。没有字体时失败，不降为伪造字体或源码字符串检查。

ordinary 1 组设计覆盖真实字库装载、六种 packed texture format、CP932 栅格化/重采样、矩形外原字节保持、非法矩形和原作小高度早返回。MP 在此基础上追加 7 组：

| 组 | 验证内容 |
| --- | --- |
| 首次前像 | 六种 packed formats，多次/不同矩形/短文字替换只占一个整 atlas journal block；恢复所有字节及原 allocation |
| 多 checkpoint | A、B、C 在不同 frame 写入，跨帧 Undo 后全部 dirty，A 不重复 dirty；再验证分级 Undo 的中间文字状态 |
| admission | closed journal、重复 Begin、错误 owner/frame、重复列表、已使用 frame、active observer 拒绝 |
| identity | 缺失 handle、另一对象、另一 allocation、size 与全部 shape 字段改变均在 Undo 前拒绝 |
| 失败事务 | byte/block 容量不足防止真实 raster 写入；其他 owner 的失败也使 End 和 frame 失效 |
| 资源与 Prepare | 空 frame lease、prepared write/resource lock、Cancel、Finish 前身份漂移、owner 失效 |
| raster 边界 | 真实 scratch 高度失败、小高度 no-op、后续成功写入仍恢复第一份原字节 |

每组比较实际完整像素，不以 API 返回 true 或源码字符串作为恢复 oracle。host dirty 回调仅验证通知并集和当前 revision 递增，明确不冒充真实 GPU 上传。

## 精确执行入口与验收边界

由 root 在工作树根目录串行运行：

    node portable/multiplayer/texture-rollback.mjs --output artifacts/rollback-smoothness/texture-component-01

runner 拒绝覆盖已有 receipt，成功与失败都保存命令、开始/结束 UTC、stdout/stderr、工具链 hash、git identity、源码 manifest/hash、字体 manifest/hash、WASM hash、lane 结果。新一轮运行必须更换 output。

实际收据：artifacts/rollback-smoothness/texture-component-01/receipt.json，已通过 nig 读取。执行命令 ID：_q4S-sGxYY1QNXCpkXhYL9Jj，exit 0。UTC 开始 2026-10-13T08:03:26.512Z，结束 2026-10-13T08:03:37.150Z。

| 身份 | SHA-256 / 结果 |
| --- | --- |
| sourceDigest | bf12cb673aa2ac0e0f41b71fdfa44e8e0b9bad0da68b506857d8f5fd0c0fd8b6 |
| 字体 aggregate digest | fd6f68d4c7b30e10312d33f54018cfa44ec17754c9e536ccc26869e2199d7cbc |
| ordinary WASM | 436e6cee9df85bd12d39dba08861bcad8937bc4299f16862a3e8538e279508b6，750284 bytes，1 组 / 115 checks |
| MP WASM | 6f082d911a1f64806d27a4b783e385a5af9ae0f4734bf2fe2783df6b94cabcca，892381 bytes，8 组 / 600 checks |

源码 manifest 没有 Application 路径；Application 仍需独立构建和事务接线测试。上述通过仅对应收据里的组件源与字体身份，不自动适用于之后任意改动。

后续仅需围绕实际缺口继续：接入 host 与 World 同一完整事务窗口、使资源 boundary latch 真正到达调度器、构建正式 MP/ordinary SDL host、验证 Undo 前 flush 与恢复后 revision 能触发真实 GPU 重新上传、覆盖本地化 TTF 分支。GPU render target 内容与 renderer state 是不同 owner，不由本 CPU 文字纹理组件代证；其恢复/重绘约束须由完整渲染架构验证。本组件不提供帧率、4× CPU、移动触控或端到端网络性能承诺。

