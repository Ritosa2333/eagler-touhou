# TH128 thprac：完整选段与原生消费者

固定上游 b5beb37a91e322d985c8f89bb6377d55bcf07734 的 THPatch 与 Boss/3Boss/MBoss/Card helpers 已作为一个功能批次移植：156 个 section、两种对话状态、EX 最后符卡四阶段，共318组上游写入记录。枚举和中英日标签取同版 games_def.json / loc_json.cpp，不套其他作品。MIT许可保留在 licenses/THPRAC-MIT.txt。

Program 使用不可变解析指令区域覆盖，StageWorld 在提交练习 owner 前核验原/现有中文资源的实际可达闭包，事务式构建 root 与 boss provider。保留原始资产字节、未改子程序、普通启动和 reset/退休隔离。支持原版及现有中文55个固定身份；中文坐标按子程序名称与指令序号映射。不是 Windows 地址注入或原始文件热补。

上游 BossCard 的字符串调用声明28字节但实际字符串带到32字节；移植器只把虚拟解析调用规范为32字节。原opcode0字段补丁规范为零操作数NOP。上游覆盖可延伸过原Boss尾部，所以仅该子程序的虚拟codeSize可延伸；原文件/parser不放宽，不把虚拟指令写回ECL。

892验证全部156选段的原/中文 preparation 和实际A1 Boss入口。897通过22599断言、4次实际StageWorld start：对话门保持/释放、原/中文首符卡HP1200与Normal spell id1、三Boss按time0/1生成；其余dlg/EX phase参数组都能prepare。EX四阶段另用真实Vm执行原Boss prelude，在HP写入点hold，读回7000/4000/1600/7000；资源host为显式no-op接缝，不冒EX完整World。898 reader/2000mutations/完整Vm/held-host受影响三组编译与运行全部0。成功892/897/898不重复刷PASS。

889引用记录器缺字符串重载编译失败，890真实缺section消费者RED，891仅pregrant被替代。893–896是时间、geometry host、辅助敌人计数和Normal符卡ID夹具错误；所有首FAIL/raw和修正裁定保留。产品从892至897未变。本批11次组件compiler、176 TU调用、15次实际World start；0全核心/Factory/browser/GPU。

**仍未完成完整thprac或第三阶段。** 尚须真实Web参数/菜单/launcher、练习Replay元数据与playLock、overlay热键/高级速度。完整表可prepare不等于156段均已玩通。原五字段真实WorldExports消费者保留888证据；本批没有新WASM API或新core，也没有公开部署/Runtime包。

历史读取5653引用：5178当前精确、474精确档案、1项历史外部工具未解。case772原bundled Python二进制被外部更新（相同长度而SHA变更），owned histories没有旧字节；原失败报告不改，也不冒0未解。当前组件均使用独立固定Emsdk工具链，8项核心/资源/公开不可变指纹仍精确。详见 thprac128-sections889-898.json。
