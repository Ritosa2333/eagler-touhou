// th16 畸变网格（原版 0x1c 字节匿名对象，0x418c70 构造）：把场地渲染目标 RT1 以一张 17 列 × N 行的网格重新贴到
// RT2 上，宿主（Boss 畸变光环 ECL 629 / 炸弹 0x40b3b0 case 0x11）每帧改写网格顶点位置与颜色，形成"透镜"扭曲。
//
// 结构（原版 dword 下标）：
//   0 columns（固定 0x11；渲染目标未就绪 DAT_004c127c == 0 时全零）   1 rows（每列节点数）
//   2 母 VM id（text.anm 0x3b、层 0x22，mode 清 0，draw_callback = 4 → 0x418c60 绘制前把网格抄进条带）
//   3 条带 VM id 表（columns-1 条；0x43d8b0：text.anm 0x3b、mode 0xc、$I0 = rows、宿主顶点缓冲 rows×2 个
//     XYZRHW|DIFFUSE|TEX1，blend 位清零、层组位清零）    4 条带 VM 指针表
//   5 网格顶点（columns × rows，列主序）    6 网格节点位置（float3，未畸变坐标；UV 由它算出）
//
// 帧序：宿主 0x418df0 以 (x, y, 宽, 高) 重建网格并抄进条带 → 宿主自行位移 / 着色网格顶点 → 绘制链 0x43d2f0
// 画层 0x22 时母 VM 的绘制钩子再抄一次 → 各条带经 0x468490 case 0xc → 0x4681f0 画 TRIANGLESTRIP。
// 析构 0x409550：标删母 VM（0x46f1c0）与各条带子树（0x46f220 形），释放四块缓冲。
#pragma once

#include "AnmManager.hpp"
#include "RenderDevice.hpp"
#include "Types.hpp"
#include <vector>

namespace th16 {

struct AnmGeometry;

class DistortionGrid {
public:
    static constexpr i32 kColumns = 0x11;   // 0x418c70：列数固定 17

    // 0x418c70：rows 为每列节点数；render_targets_ready 为 DAT_004c127c，为假时对象保持空壳
    DistortionGrid(AnmManager& animations, AnmFile& text, bool render_targets_ready, i32 rows);
    ~DistortionGrid();   // 0x409550
    DistortionGrid(const DistortionGrid&) = delete;
    DistortionGrid& operator=(const DistortionGrid&) = delete;

    // 0x418df0：以场地偏移 (DAT_004d9d50, DAT_004d9d54) 为原点，从 (x, y) 起铺 width × height 的均匀网格，
    // 顶点 z = 0、rhw = 1、颜色 0xffffffff，UV = 节点坐标 / 渲染目标尺寸（负值截 0），最后抄进条带
    void fill(float x, float y, float width, float height, const AnmGeometry& geometry, u32 target_width,
              u32 target_height);
    // 0x418f40：条带 s 的第 r 对顶点 = 网格 (s, r)、(s+1, r)
    void copy_to_strips();

    i32 columns() const noexcept { return columns_; }
    i32 rows() const noexcept { return rows_; }
    // 网格顶点 / 节点（列主序：index = column × rows + row）；宿主每帧直接改写
    std::vector<ScreenColorUvVertex>& vertices() noexcept { return vertices_; }
    std::vector<Vec3>& nodes() noexcept { return nodes_; }

private:
    // 0x43d8b0：生成一条条带 VM，返回 id
    static u32 make_strip_vm(AnmManager& animations, AnmFile& text, i32 rows);

    AnmManager* animations_ = nullptr;
    i32 columns_ = 0;                          // +0
    i32 rows_ = 0;                             // +4
    u32 master_id_ = 0;                        // +8
    std::vector<u32> strip_ids_;               // +0xc
    std::vector<AnmVm*> strip_vms_;            // +0x10
    std::vector<ScreenColorUvVertex> vertices_;// +0x14
    std::vector<Vec3> nodes_;                  // +0x18
};

// 0x418c60（绘制钩子表 DAT_00491b2c[4]）：母 VM 绘制前把网格抄进条带
void distortion_draw_callback(AnmVm& vm);

}  // namespace th16
