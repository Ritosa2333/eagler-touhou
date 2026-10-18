#include "graphics/ItemPresentation.hpp"
#include "ItemManager.hpp"
#include <cassert>
#include <cstdio>
#include <random>
#include <cstring>

// The production constructor is empty. Link this pure appearance test without
// pulling in the game's singleton managers, graphics, audio and asset loader.
Item::Item() {}

static void LegacyDrawAppearance(Item &item)
{
    if (item.pos.y < -8.0f)
    {
        if (item.isOnscreen)
        {
            item.sprite.activeSpriteIdx = item.itemType + 694;
            item.isOnscreen = 0;
            item.sprite.zWriteDisable = 1;
        }
        auto alpha = 255 - static_cast<int>((8.0f - item.pos.y) * 255.0f / 128.0f);
        if (alpha < 64) alpha = 64;
        item.sprite.color.color = (item.sprite.color.color & 0xffffffu) | (unsigned(alpha) << 24);
    }
    else if (!item.isOnscreen)
    {
        item.sprite.activeSpriteIdx = item.itemType + 684;
        item.isOnscreen = 1;
        item.sprite.color.color = 0xffffffffu;
        item.sprite.zWriteDisable = 1;
    }
}

int main()
{
    std::mt19937 rng(717191);
    for (unsigned run = 0; run < 40000; ++run)
    {
        Item reference{}, actual{};
        std::memset(&reference, 0, sizeof(reference));
        std::memset(&actual, 0, sizeof(actual));
        reference.itemType = rng() % 12;
        reference.isOnscreen = rng() % 2;
        reference.sprite.color.color = rng();
        reference.pos.y = run % 4 == 0 ? -8.0f : float(int(rng() % 6000) - 4000) / 16;
        actual = reference;
        LegacyDrawAppearance(reference);
        Graphics::UpdateItemPresentation(actual, [](AnmVm &vm, int sprite) { vm.activeSpriteIdx = sprite; });
        assert(actual.isOnscreen == reference.isOnscreen);
        assert(actual.sprite.color.color == reference.sprite.color.color);
        assert(actual.sprite.activeSpriteIdx == reference.sprite.activeSpriteIdx);
        assert(actual.sprite.zWriteDisable == reference.sprite.zWriteDisable);
        // Multiple frames with unchanged position do not toggle/reset appearance.
        Graphics::UpdateItemPresentation(actual, [](AnmVm &vm, int sprite) { vm.activeSpriteIdx = sprite; });
        assert(actual.isOnscreen == reference.isOnscreen);
        assert(actual.sprite.color.color == reference.sprite.color.color);
    }
    std::puts("item presentation: PASS 40000 legacy-appearance and repeat-state cases");
}
