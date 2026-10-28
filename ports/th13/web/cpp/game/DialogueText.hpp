// text.anm 对话动态纹理提交（原版 0x4282b0 / 0x428ae0 / 0x46e8f0 的 port 接线）。
//
// 原版证据：
//   - 0x4282b0 在对话对象 +0x4c/+0x50/+0x54/+0x58 建四个 ANM VM；
//     0x428ae0 用同样四槽（+0x13..+0x16 为对应 handle）作正文两行与注音两行。
//   - text.anm entry0（Blank "@"，512x512，format5=ARGB4444）sprite0..3 =
//     (0,1/24/47/70,512,20)，script0..3（0x304 层 26）各选其一。
//   - 0x428ae0 正文把解密 CP932 交给 0x46e8f0（样式 (flags>>1)&1），
//     0x45aef0 在 1024x129 GDI 暂存面 2x 坐标绘制后 D3DX 缩入 sprite 条带；
//     生产路径用同一 1024x48 暂存面 + 2:1 点采样写入 texture0 并 changed()。
//   - text.anm script 以 ins_4 隐藏并等待中断；每次正文写入后原版以
//     0x46fc30(handle, 2)（EBX=2）设置 VM 待处理 label，ANM tick 跳到
//     ins_5(2) 显示分支（0x428c1d/0x428c7f/0x428e96/0x42901b/0x4290be/
//     0x429233）。块首清四槽用 label 3（0x428dd3），opcode 6 释放用 label 1
//     （0x429610）。port 必须在每次真实纹理写入后发同一 label，否则 VM 永远
//     停在 ins_4 的隐藏等待，AnmRenderer 因 flags bit0=0 跳过绘制。
//   - ruby 第二数值参数不是 y：0x428ae0:464/529 把它送到 0x46e8f0 第 5 参，
//     最终成为 0x45aef0 param_9（逐字 TextOut 步进，0=连续排版）。
//   - 0x428ae0:485-553 每条正文行调 0042a120/0042a280：以 front.anm
//     script 224+param（param=side+2*((+0x178>>2)&0xf)，0x46f820 建根）
//     重建对话框，并把 child 176+param/208+param 的 variables.floats[0]
//     （VM+0x4bc = 10004）写为当前块宽度。文本 VM 创建位为 0，正文位置
//     左侧 box_x、右侧 box_x-(width+30)（0x428ae0:499/564）。
//
// 本类只拥有这四行 VM、front.anm 气泡根与 texture0 条带写入；不建立平行
// 对话状态。
#pragma once

#include "AnmManager.hpp"
#include "MsgPlayer.hpp"
#include "TextureImage.hpp"

#include <array>
#include <functional>
#include <string>

namespace th13 {

// 生产设备边界：GraphicsDevice 实现，单测用内存桩。
struct DialogueTextureSink {
    virtual ~DialogueTextureSink() = default;
    virtual u32 texture(const AnmResource& resource, u32 index) = 0;
    virtual TextureImage* pixels(u32 handle) = 0;
    virtual void changed(u32 handle, const std::array<u32, 4>& region) = 0;
};

// 字形栅格回调：把 cp932 文本画进 image 的 (x,y)。ruby=false 为正文样式。
using DialogueRasterizer =
    std::function<bool(TextureImage& image, i32 x, i32 y, bool ruby, u32 color,
                       const std::string& cp932_text)>;

class DialogueText {
public:
    static constexpr i32 kLineCount = 2;   // 0x428ae0 正文两行
    static constexpr i32 kVmCount = 4;     // 正文 2 + ruby 2（0x4282b0 四槽）

    // row ∈ [0,2)：0 → script0，1 → script1。box_x/box_y 是对话对象
    // +0x194/+0x198（0x4282b0 默认或 opcode 28 写入）。
    bool submit(AnmManager& manager, DialogueTextureSink& sink, i32 row, const MsgLine& line,
                float box_x, float box_y, const DialogueRasterizer& rasterizer,
                std::string& error);
    // 行块清空（0x428ae0 opcode 11/6 路径）：四条纹理条带清零，保留 VM。
    bool clear(DialogueTextureSink& sink, std::string& error);
    // opcode 6/18（0x42960c/0x429250）：只释放对话框根 VM 并对四条文本 VM
    // 发 label（6→1 / 18→3）。原版 0x46fc30 不发像素清零；label1 8 帧 alpha
    // 淡出后 op1 回收，label3 3 帧 alpha 淡出后循环保留 VM/像素。
    bool fade(i32 label, std::string& error);
    bool set_anchor_y(AnmManager& manager, float value, std::string& error);
    // 对话销毁（0x4282b0 对象的 VM 回收）：retire 四条 VM。
    void release(AnmManager& manager) noexcept;
    bool active() const noexcept { return handles_[0] != 0; }
    // front.anm bank（生产 3；0x428ae0 对话框脚本 224+variant 所在文件）。
    // 负数 = 禁用气泡（单测无 front 资源时的显式降级）。
    void bind_bubble_bank(i32 bank) noexcept { bubble_bank_ = bank; }
    u32 bubble_handle() const noexcept { return box_; }

private:
    struct Region {
        i32 x = 0, y = 0, width = 0, height = 0;
    };
    bool ensure(AnmManager& manager, DialogueTextureSink& sink, std::string& error);
    bool blank(DialogueTextureSink& sink, const Region& region, std::string& error);
    bool write_row(AnmManager& manager, DialogueTextureSink& sink, i32 row, const MsgLine& line,
                   const DialogueRasterizer& rasterizer, std::string& error);
    // 0x428ae0 的各写点中断 label（false = 延迟到本帧 ANM update 消费）。
    bool request_label(AnmManager& manager, u32 handle, i32 label, std::string& error);
    bool blank_all(DialogueTextureSink& sink, std::string& error, i32 label);
    bool copy_strip(DialogueTextureSink& sink, const Region& region, std::string& error);
    bool rasterize_ruby(const MsgLine& line, const DialogueRasterizer& rasterizer,
                        std::string& error);
    // 0x428ae0:485-553：每条正文行重建 front.anm 对话框（script 224+variant），
    // variant = side + 2*((text_attr>>2)&0xf)；子脚本 176+v/208+v 的
    // variables.floats[0] = 宽度（0x4bc = 10004 写槽）。
    bool update_bubble(AnmManager& manager, i32 row, const MsgLine& line, float box_x, float box_y,
                       std::string& error);
    bool retire_bubble(AnmManager& manager, std::string& error);
    void place_rows(AnmManager& manager, float box_x, float box_y);
    static float row_x(const MsgLine& line, float box_x) noexcept {
        return line.side > 0 ? float(box_x - (line.bubble_width + 30.0f)) : box_x;
    }

    std::array<u32, kVmCount> handles_{};
    std::array<Region, kVmCount> regions_{};
    std::array<std::string, kLineCount> cached_text_{};
    std::array<u32, kLineCount> cached_block_{};
    std::array<bool, kLineCount> cached_{};
    std::array<MsgLine, kLineCount> cached_line_{};
    AnmManager* manager_ = nullptr;  // ensure() 绑定；供 deferred label / clear
    i32 bubble_bank_ = 3;            // 生产 front.anm bank（Application kFrontBank）
    u32 box_ = 0;                    // 对话框根 VM（script 224+）
    u32 texture_ = 0;
    float anchor_y_ = 0;
    bool anchor_y_set_ = false;
    TextureImage scratch_;
};

}  // namespace th13
