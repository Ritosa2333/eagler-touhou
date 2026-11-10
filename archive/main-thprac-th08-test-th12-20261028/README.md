# 主站 THPrac／TH08 与测试站 TH12 多人发布

主站发布 TH06、TH07、TH08、TH10、TH11、TH15 的 Information 信息窗触控转交，TH08 普通版与多人版同时接收正式运行包。TH08 多人结局继续使用本地时钟、绘制与输入，结局后进入原生结果和 Replay 保存流程。TH12 普通版线上运行包已通过实际 ImGui 信息窗轻点与拖动转交检查。

测试站 TH12 多人故事通关在结局后进入姓名与排名，再进入原版 25 槽 Replay 保存页。Game Over 和 Extra 保留多人结果菜单。该运行代只更新 `title-scenes.mjs`，现有 WASM 与详细错误报告器继续使用。

[发布身份](deployment.json)保存两站原发布目录、新发布目录与各运行代的逐文件身份。两个站点各自从原发布目录建立新发布，逐文件替换已审阅内容，校验后原子切换。资源包、字体清单、前端产物和其他运行组保留原发布身份；App Shell 与发布清单随运行代更新。

## 正式运行包验收

[本地正式包启动](production-boot.json)与[公网实际启动](public-production-boot.json)均通过 7 个运行包：TH06、TH07、TH08、TH08 多人、TH10、TH11、TH15。检查使用正式入口、原游戏数据、共享字体、实际 SDL 绘制和触控／键盘桥。TH06、TH07 重新编译生成的数据包与线上资源包字节一致。

[公网校验](public-check.json)覆盖 51 个发布文件、两个启动器页面及测试站实际 TitleScenes 模块的排名与 Replay 选择。[App Shell 生效检查](public-service-worker.json)确认两站 Service Worker 激活后重新载入，读取的运行清单与新发布一致。测试站候选包的多人结局、故事通关、Game Over、Extra 和独立录像命名空间共 [7 项流程检查](test-ending-checks.json)通过。TH08 结局与原生 Replay 保存的诊断范围见[前次结局验收](../thprac-information-touch-20261027/th08-ending-proof.json)。

[服务器最终状态](remote-final.json)保存实际发布指针和回退入口。Nginx 配置检查通过，切换发布目录完成生效。

## 本次修复范围

[源码核对](history-cleanup.json)保存重建后的触控修复与结局测试提交。分支从当前 `upstream/main` 接收本次修改，普通版和多人版在各自源码目录维护。已发布运行包涉及的源码目录与原构建快照一致，历史整理保留正式包的构建与公网验收身份。

本次分支包含信息窗触控修复、TH08 结局 Replay 验收与发布记录。作品来源连接和同步基线继续使用上游记录。
