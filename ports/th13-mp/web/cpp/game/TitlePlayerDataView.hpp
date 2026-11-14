#pragma once
#include "TitlePlayerDataMenu.hpp"
#include "AnmManager.hpp"
#include "AnmText.hpp"
#include "HudDrawData.hpp"

namespace th13 {

// Original 00453170 ANM roots and 00453db0 summary/00453b30 card row output.
// The menu is the sole selection/page/data state; the view owns VM handles.
class TitlePlayerDataView {
public:
    struct Banks { i32 title = 16, ascii = 2, text = 0; };
    using Rasterizer = std::function<bool(AnmVm&, u32 color, const std::string&,
                                          AnmTextAlignment)>;
    explicit TitlePlayerDataView(AnmManager& manager) : TitlePlayerDataView(manager, Banks{}) {}
    TitlePlayerDataView(AnmManager& manager, Banks banks) : manager_(manager), banks_(banks) {}
    bool update(const TitlePlayerDataMenu&, const Rasterizer&);
    bool draw(const TitlePlayerDataMenu&, HudDrawServices&);
    void release();
    u32 character_handle() const { return character_; }
    u32 difficulty_handle() const { return difficulty_; }
    u32 row_handle(i32 row) const { return row >= 0 && row < 10 ? rows_[size_t(row)] : 0; }
    std::string error;

private:
    AnmManager& manager_;
    Banks banks_;
    std::vector<u32> roots_;
    std::array<u32, 10> rows_{};
    u32 character_ = 0, difficulty_ = 0;
    i32 old_character_ = -1, old_difficulty_ = -1, old_page_ = -1;
    bool opened_ = false, closing_ = false;
    bool fail(const std::string&);
    bool create(u32& handle, i32 bank, i32 script);
    bool leave();
};
} // namespace th13
