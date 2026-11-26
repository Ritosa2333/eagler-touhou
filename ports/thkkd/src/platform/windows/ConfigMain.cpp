#include "DisplaySettings.hpp"
#include <windows.h>
int WINAPI wWinMain(HINSTANCE,HINSTANCE,PWSTR,int) {
    wchar_t executable[32768]{};GetModuleFileNameW(nullptr,executable,32768);
    return kaikeidou::show_display_settings(std::filesystem::path(executable).parent_path()/"userdata");
}
