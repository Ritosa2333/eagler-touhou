#pragma once

// Declarations for extensions defined by the bundled thprac fork.
namespace ImGui {
bool ComboAlt(const char*, const char*, int*, bool(*)(void*, int, const char**), void*, int, int);
void SetItemDefaultFocusAlt();
bool IsItemFocusedAlt(const char*);
void SetItemFocusAlt(const char*, bool);
bool IsItemActiveAlt(const char*);
void EraseColumn(const char*, int);
}
