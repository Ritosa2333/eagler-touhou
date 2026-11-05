#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <string>

namespace
{
HWND g_Target = nullptr;

std::string Utf8(const wchar_t *text)
{
    if (!text || !*text)
        return {};
    const int bytes = WideCharToMultiByte(CP_UTF8, 0, text, -1, nullptr, 0, nullptr, nullptr);
    if (bytes <= 1)
        return {};
    std::string result(static_cast<std::size_t>(bytes - 1), '\0');
    WideCharToMultiByte(CP_UTF8, 0, text, -1, result.data(), bytes, nullptr, nullptr);
    return result;
}

BOOL CALLBACK DumpVisibleWindow(HWND hwnd, LPARAM)
{
    wchar_t title[512] = {};
    if (IsWindowVisible(hwnd) && GetWindowTextW(hwnd, title, 511) > 0)
    {
        const std::string titleUtf8 = Utf8(title);
        std::printf("visible window: %s\n", titleUtf8.c_str());
        if (std::wcsstr(title, L"th07-netplay-probe.exe") != nullptr)
        {
            EnumChildWindows(
                hwnd,
                [](HWND child, LPARAM) -> BOOL {
                    wchar_t text[1024] = {};
                    if (GetWindowTextW(child, text, 1023) > 0)
                    {
                        const std::string childUtf8 = Utf8(text);
                        std::printf("  child: %s\n", childUtf8.c_str());
                    }
                    return TRUE;
                },
                0);
        }
    }
    return TRUE;
}

BOOL CALLBACK FindGameWindow(HWND hwnd, LPARAM)
{
    wchar_t title[512] = {};
    if (!IsWindowVisible(hwnd) || GetWindowTextW(hwnd, title, 511) <= 0)
        return TRUE;
    if (std::wcsstr(title, L"Perfect Cherry Blossom") != nullptr)
    {
        g_Target = hwnd;
        return FALSE;
    }
    return TRUE;
}

void Tap(WORD virtualKey, DWORD holdMs)
{
    INPUT input[2] = {};
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = virtualKey;
    input[1] = input[0];
    input[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input[0], sizeof(INPUT));
    Sleep(holdMs);
    SendInput(1, &input[1], sizeof(INPUT));
}

WORD ParseKey(const char *name)
{
    if (!name || !*name)
        return 'Z';
    if (_stricmp(name, "z") == 0)
        return 'Z';
    if (_stricmp(name, "x") == 0)
        return 'X';
    if (_stricmp(name, "enter") == 0)
        return VK_RETURN;
    if (_stricmp(name, "up") == 0)
        return VK_UP;
    if (_stricmp(name, "down") == 0)
        return VK_DOWN;
    if (_stricmp(name, "left") == 0)
        return VK_LEFT;
    if (_stricmp(name, "right") == 0)
        return VK_RIGHT;
    return 0;
}
} // namespace

int main(int argc, char **argv)
{
    const WORD key = ParseKey(argc > 1 ? argv[1] : "z");
    const int count = argc > 2 ? std::atoi(argv[2]) : 1;
    const int gapMs = argc > 3 ? std::atoi(argv[3]) : 350;
    if (!key || count <= 0)
        return 2;

    EnumWindows(FindGameWindow, 0);
    if (!g_Target)
    {
        std::puts("TH07 native input: window not found");
        EnumWindows(DumpVisibleWindow, 0);
        return 3;
    }

    ShowWindow(g_Target, SW_RESTORE);
    SetForegroundWindow(g_Target);
    Sleep(100);
    for (int i = 0; i < count; ++i)
    {
        Tap(key, 40);
        if (i + 1 < count)
            Sleep(gapMs);
    }
    std::printf("TH07 native input: sent %d tap(s)\n", count);
    return 0;
}
#else
int main()
{
    return 77;
}
#endif
