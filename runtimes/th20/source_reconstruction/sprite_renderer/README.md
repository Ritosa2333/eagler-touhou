# 动画、贴图与绘制源码（继续恢复中）

这里是重新编译的 C++，不在生产代码中执行原游戏机器码。原始 ANM 数据作为资源读取；`analysis/` 及 `evidence/` 仅提供恢复依据。

- `animation.*`、`binding.*`：对象构造/重置、脚本/精灵绑定、模板复制、UV 与父子关联。
- `anm_vm.*`：全部已知 ANM 指令控制流、插值、运动和几何数据；详见 `anm_vm_README.md` 与对应 CPU 报告。
- `pool*`、`dispatch.*`、`named_spawn.*`、`controller.*`：真实 0x7d40e94 大小管理器、动画池/堆对象、句柄、链表、更新/绘制回调。池最后一项按原构造不加入空闲表。构造注册 2 项更新和 50 项绘制回调；表由静态反汇编证据核对，源码中只有 C++ 函数指针。
- `animation_file.*`、`file_lifecycle.cpp`、`postload*`、`file_tasks*`：归档预载、逐条贴图后载、脚本模板初始化、清理及帧内时间预算。73 个资源文件的 667 条记录、5,447 个精灵、2,795 个脚本与独立 DSL 核对。
- `texture_load.*`、`texture_edges.*`：真实 D3DX9 贴图创建、PNG 裁剪/缩放、透明边缘修复。2,181 组原 CPU/真实 D3D9 对照涵盖全部资源记录的三种缩放和外部 PNG 分支，469,066,224 个像素一致；未比较未初始化的动态/渲染目标像素。
- `buffers.cpp`、`vertex_buffer.*`、`render_state.*`、`quad.*`、`draw.*` 等：缓冲、渲染状态、四角计算及绘制模式持续恢复。顶点结构按 FVF 区分 XYZUV、XYZ/color/UV 和屏幕 RHW 顶点。

正常路径依据原指令顺序，尤其保留浮点舍入、计时器、RNG 消耗和帧内资源阶段。错误域有明确差异：越界文件/格式索引在源码抛异常；预载失败销毁对象后清除原程序残留的悬空槽位；原 44dfb0 卸载分支在 44c430 清槽后再写空指针 +0x60，源码在该处抛异常。没有以成功返回掩盖这些错误。

`controller_cpu_validation.json` 比较整个 131,337,876 字节管理器、两个共享顶点数组、52 个回调注册和 SetVertexShader(null)，零值与 A5 初始填充均一致；析构同时覆盖空池与 32 个活跃池对象。该测试运行未修改的原构造/析构，仅初始化其 Win32 导入和堆/锁全局，不替换其游戏函数；未执行帧回调或活跃资源/工作线程销毁。

`cpu_validation.json`、`anm_vm_cpu_validation.json`、`pool_cpu_validation.json`、`file_preload_validation.json`、`texture_cpu_validation.json` 各自记录覆盖范围。部分测试使用 COM 记录器比较调用序列，这与真实显卡贴图测试不同；所有模式尚未完成全游戏逐帧验证。未恢复的实体、文本和绘制依赖保留为明确的外部接口。
