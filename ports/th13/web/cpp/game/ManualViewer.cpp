#include "ManualViewer.hpp"

namespace th13 {

ManualViewer::~ManualViewer() { close(); }

bool ManualViewer::fail(const std::string& why) {
    if (error.empty()) error = why.empty() ? "Manual animation failed" : why;
    return false;
}

bool ManualViewer::menu() {
    for (u32& handle : entries)
        animations.retire(handle);
    for (i32 i = 0; i < 8; ++i) {
        entries[u32(i)] = animations.create(bank, i, -1, 4, {128, 0, 0});
        if (!entries[u32(i)] || !animations.interrupt(entries[u32(i)], i == page ? 2 : 3, true))
            return fail(animations.error);
    }
    phase = 1;
    age = 0;
    return true;
}

bool ManualViewer::open() {
    close();
    error.clear();
    done = false;
    page = 0;
    return menu();
}

bool ManualViewer::show_page() {
    if (!load_page || !load_page(page)) return fail("Manual page unavailable");
    for (u32& handle : entries)
        animations.retire(handle);
    animations.retire(page_vm);
    page_vm = animations.create(bank, 9, -1, 4, {128, 0, 0});
    if (!page_vm) return fail(animations.error);
    phase = 2;
    age = 0;
    return true;
}

bool ManualViewer::update(u32 pressed, u32 repeated) {
    if (!error.empty()) return false;
    ++age;
    if (phase == 3) {
        if (age > 29) done = true;
        return true;
    }
    if (age <= 19) return true;
    if (phase == 1) {
        const i32 previous = page;
        const u32 input = pressed | repeated;
        if (input & 0x10) page = (page + 7) % 8;
        if (input & 0x20) page = (page + 1) % 8;
        if (page != previous) {
            if (!sound || !sound(10)) return fail("Manual navigation sound failed");
            for (i32 i = 0; i < 8; ++i)
                if (!animations.interrupt(entries[u32(i)], i == page ? 2 : 3, true)) return fail(animations.error);
        }
        if (pressed & 0x80001) {
            if (!sound || !sound(7)) return fail("Manual confirm sound failed");
            return show_page();
        }
        if (pressed & 0x102) {
            if (!sound || !sound(9)) return fail("Manual cancel sound failed");
            for (u32 handle : entries)
                if (!animations.interrupt(handle, 1)) return fail(animations.error);
            phase = 3;
            age = 0;
        }
    } else if (phase == 2) {
        const i32 previous = page;
        if ((pressed & 0x20) && page < 7)
            ++page;
        else if ((pressed & 0x10) && page > 0)
            --page;
        if (page != previous) {
            if (!sound || !sound(10)) return fail("Manual page sound failed");
            return show_page();
        }
        if (pressed & 0x80103) {
            if (!sound || !sound(9)) return fail("Manual page close sound failed");
            animations.retire(page_vm);
            return menu();
        }
    }
    return true;
}

void ManualViewer::close() {
    for (u32& handle : entries)
        animations.retire(handle);
    animations.retire(page_vm);
    phase = age = 0;
    done = false;
}

}
