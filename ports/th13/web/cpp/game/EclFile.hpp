// th13 ECL 线程快照落盘/恢复（oracle 对照用）。
// 块大小取原版线程分配 0x11C4（0x4793e0），字段序按 §2.1：
//   +0x000 time；+0x004 routine（游标文件侧代理，替代原版 Q 指针）；
//   +0x008..+0x1007 栈区；+0x1008 SP；+0x100C FP；+0x1010 线程 id；
//   +0x1018 线程状态；+0x101C 难度；+0x1020..+0x119F 8×0x30 插值通道；
//   +0x11A0 主游标指令偏移；+0x11A4..+0x11B3 通道目标（u32 = routine<<18 | offset/2，
//   指令 4 对齐故偏移折半存储）；+0x11B4..+0x11BF 保留 0；+0x11C0 线程标志。
// 进程指针（宿主 +0x1014、通道挂接指令指针）一律以 routine/offset 代理，恢复时重绑。
#pragma once

#include "EclThreads.hpp"

#include <vector>

namespace th13 {

class EclFile {
public:
    using Block = std::array<u8, 0x11c4>;
    std::string error;
    bool write(const EclContext&, Block&);
    bool read(const u8* data, u32 size, EclContext&, EclProgram&);
    // 线程组容器格式：owner 区 = 16 字节节点头 + 主线程块（0x11D4），
    // 子线程存在标志在 owner+0x11D8（主块尾节点 +8）；子线程紧随，每项 0x11D4
    // （块 + 16 字节节点），块尾 +4 处 u32 链继续标志。两偏移均相对缓冲首字节。
    bool write_threads(const EclThreadsSnapshot&, std::vector<u8>&, u32 owner_offset, u32 child_offset);
    bool read_threads(const u8* data, u32 size, EclThreadsSnapshot&, EclProgram&, u32 owner_offset, u32 child_offset,
                      u32& consumed);
};

}  // namespace th13
