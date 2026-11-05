#include "MenuCursor.hpp"
namespace th10 {
// 0x40ad20. A zero count is used while configuring a cursor.
i32 MenuCursor::select(i32 value) noexcept {
    selected=!item_count?value:item_count<=value?item_count-1:value<0?0:value;
    return selected;
}
// 0x44be20. A full stack writes the final slot again, then saturates.
void MenuCursor::push() noexcept {
    selection_stack[stack_depth] = selected;
    count_stack[stack_depth] = item_count;
    if (++stack_depth > 15) stack_depth = 15;
    disabled_count = 0;
}
// 0x44be70. Popping an empty stack restores slot zero.
void MenuCursor::pop() noexcept {
    if (--stack_depth < 0) stack_depth = 0;
    selected = selection_stack[stack_depth];
    item_count = count_stack[stack_depth];
    disabled_count = 0;
}
// 0x44bea0. Menus supply a nonzero direction and at least one selectable
// entry. Preserve the original repeated-step handling of disabled entries.
i32 MenuCursor::move(i32 delta) noexcept {
    if (item_count <= 0) return selected;
    for (;;) {
        selected = wrapping_add(selected, delta);
        while (selected >= item_count) selected = wrap ? selected - item_count : item_count - 1;
        while (selected < 0) selected = wrap ? selected + item_count : 0;
        bool disabled = false;
        for (i32 index = 0; index < disabled_count; ++index) {
            if (disabled_items[index] == selected) { disabled = true; break; }
        }
        if (!disabled) return selected;
    }
}
}
