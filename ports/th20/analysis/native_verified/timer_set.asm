; timer_set: original th20.exe code, SHA256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
; Recovered C++: native_recovered/native_core.hpp
; Called helpers remain visible in analysis/binary/disassembly.asm
00423f80  55                               push     ebp
00423f81  8bec                             mov      ebp, esp
00423f83  51                               push     ecx
00423f84  894dfc                           mov      dword ptr [ebp - 4], ecx
00423f87  8b45fc                           mov      eax, dword ptr [ebp - 4]
00423f8a  8b480c                           mov      ecx, dword ptr [eax + 0xc]
00423f8d  83e101                           and      ecx, 1
00423f90  7521                             jne      0x423fb3
00423f92  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
00423f95  e8b6ffffff                       call     0x423f50
00423f9a  6a00                             push     0
00423f9c  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
00423f9f  e83c000000                       call     0x423fe0
00423fa4  8b55fc                           mov      edx, dword ptr [ebp - 4]
00423fa7  8b420c                           mov      eax, dword ptr [edx + 0xc]
00423faa  83c801                           or       eax, 1
00423fad  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
00423fb0  89410c                           mov      dword ptr [ecx + 0xc], eax
00423fb3  8b55fc                           mov      edx, dword ptr [ebp - 4]
00423fb6  8b4508                           mov      eax, dword ptr [ebp + 8]
00423fb9  894204                           mov      dword ptr [edx + 4], eax
00423fbc  f30f2a4508                       cvtsi2ss xmm0, dword ptr [ebp + 8]
00423fc1  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
00423fc4  f30f114108                       movss    dword ptr [ecx + 8], xmm0
00423fc9  8b5508                           mov      edx, dword ptr [ebp + 8]
00423fcc  83ea01                           sub      edx, 1
00423fcf  8b45fc                           mov      eax, dword ptr [ebp - 4]
00423fd2  8910                             mov      dword ptr [eax], edx
00423fd4  8be5                             mov      esp, ebp
00423fd6  5d                               pop      ebp
00423fd7  c20400                           ret      4
