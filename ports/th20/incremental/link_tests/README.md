# Final PE linkage hardware oracle

This test reads the original verified EXE, the final rebuilt EXE, and the exact
distributed bridge DLL. It maps the two PE images into a 32-bit test process,
applies their HIGHLOW relocations, and resolves only the rebuilt EXE's seven
bridge imports. Each comparison calls the **patched entry RVA in the rebuilt
EXE**, traversing its `FF 25` indirect jump and IAT slot; it does not call the DLL
export directly as a substitute for checking the final link.

The original and rebuilt process entry points are never executed. Original
imports remain unresolved because the selected isolated routines do not use
them. This test does not validate Windows loader initialization or gameplay.

```powershell
cmake -S incremental/link_tests -B incremental/link_tests/build -G 'Visual Studio 16 2019' -A Win32
cmake --build incremental/link_tests/build --config Release --parallel
& '.\incremental\link_tests\build\Release\th20_playable_link_check.exe' `
  'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe' `
  '.\dist\playable\th20_rebuilt.exe' `
  '.\dist\playable\th20_native_bridge.dll' `
  '.\reports\playable_link_validation.json'
```

The command performs 162,784 comparisons across two rebuilt image bases. It
checks all seven patched entry/IAT pairs, complete object bytes and guards,
EAX return values (including Timer self returns), ESP cleanup, and EBX/ESI/EDI
preservation. Floating-point cases include signed zero, denormals, normal values,
infinities, NaNs, arbitrary bit patterns, initialized/uninitialized Timer flags,
and null/non-null clock pointers. The default SSE/x87 environment is restored
before each call; floating-point status flags are not compared.

The harness first tries to reserve the game's preferred `0x00400000` region.
Windows may already have mapped its NLS file there before the test starts. In
that case it records the preferred-base attempt as **skipped** with the mapped
file name and checks two different nonpreferred addresses instead; it never
unmaps the system view. The report identifies both bases and relocation counts.

During the test, the loaded bridge's exported `main_module_base` field is bound
to the mapped rebuilt image and restored before unloading. This is necessary
because `GetModuleHandleW(nullptr)` in this process identifies the test program.
All three input hashes are included in the report. Inputs are opened read-only.
