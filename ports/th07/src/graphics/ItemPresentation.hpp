#pragma once

#include <cstdint>

namespace Graphics
{
// The item's indicator/appearance transition is a fixed-tick operation. Draw
// must not change this state: a rollback can run several ticks without drawing,
// and different peers may render different numbers of pictures for one tick.
template <typename Item, typename SetSprite>
void UpdateItemPresentation(Item &item, SetSprite setSprite)
{
    if (item.pos.y < -8.0f)
    {
        if (item.isOnscreen)
        {
            setSprite(item.sprite, item.itemType + 694);
            item.isOnscreen = 0;
            item.sprite.zWriteDisable = 1;
        }
        auto alpha = 255 - static_cast<std::int32_t>(
            (8.0f - item.pos.y) * 255.0f / 128.0f);
        if (alpha < 64) alpha = 64;
        item.sprite.color.color = (item.sprite.color.color & 0x00ffffffu) |
                                   (static_cast<std::uint32_t>(alpha) << 24);
    }
    else if (!item.isOnscreen)
    {
        setSprite(item.sprite, item.itemType + 684);
        item.isOnscreen = 1;
        item.sprite.color.color = 0xffffffffu;
        item.sprite.zWriteDisable = 1;
    }
}
} // namespace Graphics
