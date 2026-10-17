; timer_mode: original th20.exe code, SHA256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
; Recovered C++: native_recovered/native_core.hpp
; Called helpers remain visible in analysis/binary/disassembly.asm
00423fe0  55                               push     ebp
00423fe1  8bec                             mov      ebp, esp
00423fe3  51                               push     ecx
00423fe4  894dfc                           mov      dword ptr [ebp - 4], ecx
00423fe7  8b4508                           mov      eax, dword ptr [ebp + 8]
00423fea  83e003                           and      eax, 3
00423fed  d1e0                             shl      eax, 1
00423fef  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
00423ff2  8b510c                           mov      edx, dword ptr [ecx + 0xc]
00423ff5  83e2f9                           and      edx, 0xfffffff9
00423ff8  0bd0                             or       edx, eax
00423ffa  8b45fc                           mov      eax, dword ptr [ebp - 4]
00423ffd  89500c                           mov      dword ptr [eax + 0xc], edx
00424000  8be5                             mov      esp, ebp
00424002  5d                               pop      ebp
00424003  c20400                           ret      4
