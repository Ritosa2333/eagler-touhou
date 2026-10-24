# TH15 无欠检查点与完整存档导出

对应 [Issue #113](https://github.com/YomotsuHisami/eagler-touhou/issues/113)：无欠模式退出重进时动画树读档报错，以及导出后导入缺少章节进度。

## 动画树恢复

原版 4899f0 格式会把宽子节点列表展开为嵌套链。检查点写入保留这个拓扑，读写使用显式遍历栈，按文件长度和节点数量检查边界；几何粒子嵌套继续单独限制。已有文件格式和角色／难度槽位保持一致。

`anm-checkpoint-wide-tree.test.mjs` 使用原版 front.anm 构造 96 个直接子节点，覆盖保存、读回、恢复、再次捕获和保存；截断输入验证节点分配回滚。该用例在原实现触发 Issue 同名报错，修复后通过。

`checkpoint-persistence-browser.test.mjs` 使用真实日文 DAT、共享字体、新页面与新 Wasm 检查普通六面的 IDBFS 冷启动恢复，六面均通过。受控跨面用于检查存储边界，自然通关游玩另行验收。

## 完整存档备份

TH15 普通版和 MP 的“导出存档”生成 `scoreth15.zip`，包含 `scoreth15.dat` 和现有 `autosave/save0_0.dat` 至 `save3_4.dat`。导入接受 ZIP 及历史单独 DAT；ZIP 保留章节目录，原生 Runtime 负责验证与写入，重新打开后逐文件核对内容。

文件服务 20 项测试通过，覆盖全部 20 个章节槽位的往返、历史 score-only ZIP、DAT、路径穿越和重复路径拒绝。缺少 autosave 的旧备份不包含章节进度；Issue 提供的 ZIP 实际只有 scoreth15.dat。排查已有坏档仍需保留原浏览器中的 autosave 文件。
