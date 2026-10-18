#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdio>

int main()
{
    HMODULE module = LoadLibraryW(L"SDL3_ttf.dll");
    if (!module)
    {
        std::printf("LOAD FAIL error=%lu\n", GetLastError());
        return 1;
    }
    std::puts("SDL3_ttf native loader: PASS");
    FreeLibrary(module);
    return 0;
}
#else
int main()
{
    return 77;
}
#endif
