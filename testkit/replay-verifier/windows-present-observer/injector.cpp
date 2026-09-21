#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdint>
#include <cwchar>

int wmain(int argc, wchar_t **argv) {
    if (argc != 4) return 2;
    const DWORD pid = wcstoul(argv[1], nullptr, 10);
    const std::uintptr_t readyAddress = wcstoul(argv[2], nullptr, 0);
    const wchar_t *dll = argv[3];
    HANDLE process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pid);
    if (!process) return 3;
    if (readyAddress) {
        std::uint32_t ready = 0; SIZE_T read = 0;
        for (int attempt = 0; attempt < 3000 && !ready; ++attempt) {
            ReadProcessMemory(process, reinterpret_cast<void *>(readyAddress), &ready, sizeof(ready), &read);
            if (!ready) Sleep(10);
        }
        if (!ready) { CloseHandle(process); return 4; }
    }
    const SIZE_T bytes = (wcslen(dll) + 1) * sizeof(wchar_t);
    void *remote = VirtualAllocEx(process, nullptr, bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remote || !WriteProcessMemory(process, remote, dll, bytes, nullptr)) { CloseHandle(process); return 5; }
    auto loadLibrary = reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW"));
    HANDLE thread = CreateRemoteThread(process, nullptr, 0, loadLibrary, remote, 0, nullptr);
    if (!thread) { VirtualFreeEx(process, remote, 0, MEM_RELEASE); CloseHandle(process); return 6; }
    const DWORD waited = WaitForSingleObject(thread, 10000);
    if (waited != WAIT_OBJECT_0) {
        // The remote thread may still be reading the argument. Do not free it
        // on timeout, and never interpret STILL_ACTIVE as a successful load.
        CloseHandle(thread); CloseHandle(process); return 8;
    }
    DWORD result = 0;
    const BOOL gotResult = GetExitCodeThread(thread, &result);
    CloseHandle(thread); VirtualFreeEx(process, remote, 0, MEM_RELEASE); CloseHandle(process);
    return gotResult && result && result != STILL_ACTIVE ? 0 : 7;
}
