#include <windows.h>
#include <cstdio>
#include <cstring>

int main(int argc, char** argv) {
  const auto module = LoadLibraryW(L"device_lifetime.dll");
  if (!module) return 2;
  const auto create = reinterpret_cast<HRESULT (*)()>(GetProcAddress(module, "create_device"));
  const auto dispose = reinterpret_cast<void (*)()>(GetProcAddress(module, "dispose_device"));
  if (!create || !dispose) return 3;
  const auto hr = create();
  std::printf("create HRESULT=0x%08lx\n", static_cast<unsigned long>(hr));
  std::fflush(stdout);
  if (FAILED(hr)) return 4;
  if (argc > 1 && std::strcmp(argv[1], "scoped") == 0) dispose();
  std::puts("returning from main");
  std::fflush(stdout);
  return 0;
}
