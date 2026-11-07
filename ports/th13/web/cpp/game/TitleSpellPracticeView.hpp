// Original title.anm roots: 004560a0/00456650/00456b50/00457630.
#pragma once

#include "AnmManager.hpp"
#include "HudDrawData.hpp"
#include "TitleSpellPracticeMenu.hpp"

#include <array>
#include <functional>

namespace th13 {

class TitleSpellPracticeView {
public:
    using Rasterizer = std::function<bool(AnmVm&, u32, const std::string&)>;
    TitleSpellPracticeView(AnmManager& animations, i32 title_bank = 16)
        : animations_(animations), bank_(title_bank) {}
    ~TitleSpellPracticeView();
    bool update(const TitleSpellPracticeMenu&, const TitleSpellPracticePlatform&,
                const Rasterizer&);
    bool draw(const TitleSpellPracticeMenu&, const TitleSpellPracticePlatform&,
              HudDrawServices&);
    bool confirm(const TitleSpellPracticeMenu&);
    void release();
    std::string error;

private:
    AnmManager& animations_;
    i32 bank_;
    u32 stage_ = 0, cards_ = 0, character_ = 0, badge_ = 0, particles_ = 0;
    std::array<u32, 5> names_{};
    std::array<u32, 7> ranks_{};
    std::vector<u32> owned_;
    SpellPracticeLevel level_ = SpellPracticeLevel::Stage;
    i32 cursor_ = -1, character_index_ = -1, selected_stage_ = -1, slot_ = -1;
    bool opened_ = false;
    bool fail(const std::string&);
    bool create(u32&, i32 script);
    bool interrupt(u32, i32, bool immediate = false);
    bool select(u32, i32);
    bool rebuild_names(i32 stage, i32 family, i32 character,
                       const TitleSpellPracticePlatform&, const Rasterizer&);
};

} // namespace th13
