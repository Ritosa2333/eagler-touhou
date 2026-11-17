# TH13 MP 同步修复与提交交接（2026-11-12）

本批修复输入采样与收包唤醒、回滚世界及宿主恢复、合作规则、终局保存隔离、多人录像定位、换关后旁观恢复，以及 Launcher 返回房间。基线为 `6c0a4a67d`。本批是源码提交，完整正式联网通关验收尚未完成；没有推送或部署。

## 旁观修复

TH13 的 StageEntry 保存跨关携带资源，不是完整世界快照。Relay 现在为 v2/schema2 保留从 frame0 起的确认输入；后续关头是导航书签。旁观端通过既有 Run 模式从合法本局首关重建并重演输入，换关时保留真实 destination/load/present 顺序。重复定位清理上一世界和宿主过渡状态。

共享默认 108000 帧限制保持，TH13 使用原 16 MiB 字节预算导出的 279620 帧上限；字节门继续独立生效，计入事件后可能先到达。旧协议及其他作品的恢复方式保持，TH17 原 Run 模式保持。

- 真实资源、录制 Session 对旁观的定向检查：12656 项通过，2.375 秒；包含双人 Normal、三人 Lunatic、双人 Extra，晚入、跨关、倒退、暂停、回直播和693次逐输入帧完整状态比较。
- 真实旧适配函数使用同一探针检出39处失败；旧实现没有批次让出，实际执行12650项。比较口径没有通过删字段或改容差降低。
- 共享容量、Clear、保留前缀都有变异负控；Relay 单元48项通过。真实双人/三人 Relay 晚入与重入逐包一致，既有跨作品旁观回归通过。
- 多人录像定位2646项通过；既有 M9 7块1483断言通过。以上为片段验证，不替代整场浏览器验收。

## 其他实测结果

最终源码重新编译163个生产/测试单元后，M1–M9累计56/56项、69987断言通过，零skip，进程退出0，含构建共39.447秒。源身份清单逐文件与提交前工作区一致。旁观具名修复已由独立复核签署。

纯逻辑战斗路线速度为 Normal 24.763×、Extra 23.377×、三人 Lunatic 10.099×，保留真实资源、碰撞和敌方脚本。该轮 Session inactive，不能作为正式网络认证。后续自然 Session 的 Normal/Lunatic 结算、保存完整T13M与回本地标题有独立原始字节核验；回房另有历史真实RTC/Relay各三次生命周期证据。

终局控制器合法选择了空录像名，但原fixture要求八字符，child17/parent31原样保留。独立解析精确空名、全部输入、六章资源与完成标志通过。原长程审计另因驱动漏装载回调缺终态、部分尾写入不完整，退出-15保持失败。扩大Launcher UI回归1650通过、62失败、326未执行；因果尚未判定。定向Runtime/Room161项和真实Relay返回正负控在提交前重新通过。

## 部署接续边界

剩余验收为正式RTC/Relay×RTT0/50/100完整Normal/Extra、三浏览器Lunatic、持续真实丢包修复与墙钟输入时延，以及完整逐确认帧审计8×。GPU按用户要求跳过，不能认证最终画面。旁观缺陷已完成上述定向修复，仍须部署环境的浏览器连通性验证。

正式MP构建已退出0，WASM SHA256为 `5038d1aa8c459322bade844ceca572469dbca0f640b264d9aeaec60d1f8e9c1d`。构建目录和网页产物JS/WASM哈希一致。生产部署需要同时更新Runtime与Relay，以提供完整旁观前缀；当前尚未执行部署。

## 可重复入口

在容器内进入 `ports/th13-mp`：

```sh
node portable/build-eagler.mjs --multiplayer
python3 tests/multiplayer/spectator-prefix-replay.py --objects /work/artifacts/mp-sync-20261112/spectator-prefix/current-v1/build.json --out /work/artifacts/唯一新旁观目录
python3 tests/multiplayer/replay-prefix-seek.py --objects /work/artifacts/mp-sync-20261112/spectator-prefix/current-v1/build.json --out /work/artifacts/唯一新录像目录
```

原生对象来源变化时加旁观入口的 `--rebuild`，不能复用失配对象。公共测试使用 `common/tests/CMakeLists.txt`；Relay定向入口为 `node services/relay/tests/test-th13-spectator-prefix-relay.mjs`（仓根）。新增原生回归在 [workflow-cases.json](../../ports/th13-mp/tests/multiplayer/workflow-cases.json) 登记。

机器范围及本地ignored证据哈希见 [verification.json](verification.json)。原版资产、录像与大体积捕获留在 `/work/artifacts/mp-sync-20261112/`，不入Git。普通版菜单原有六个文件和外层探针保留；本批未覆盖它们。

## 提交与同步身份

通用源 `dc9a747ee`、世界与合作规则 `ec57e3c91`、Launcher返回房间 `2a93ac2d2`、共享旁观与Relay `01abb4ae5`、MP运行时与回归 `3da0ed775` 分项提交。普通版25文件中17个blob相同，8个保留MP适配，详见[sync.json](sync.json)。

在临时克隆保存自动合并冲突后，以已人工迁入并通过回归的MP树作明确接收决定，生成投影合并 `154f31df4`，双亲写回 `724446c00`。写回前后整个monorepo树相同，原有六个菜单文件状态及内容未变。刷新投影后普通版为MP祖先，普通版待接收提交为0。同步处理记录与Git祖先分别保存。

源码来源检查 `python3 tools/ports/check-history.py --base 6c0a4a67d --changed-subtrees` 退出0，真实 `git subtree split` 导出 `154f31df4` 并核对树及祖先。容器原Git 2.39.5的subtree不支持检查器使用的`--quiet`；在ignored工具目录安装官方Git v2.51.0 subtree并以`GIT_EXEC_PATH`指定后通过，未修改仓库检查器。原失败收据和新工具SHA见机器记录。
