; lcg_step: original th20.exe code, SHA256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
; Recovered C++: native_recovered/native_core.hpp
; Called helpers remain visible in analysis/binary/disassembly.asm
00422cb0  55                               push     ebp
00422cb1  8bec                             mov      ebp, esp
00422cb3  83ec10                           sub      esp, 0x10
00422cb6  56                               push     esi
00422cb7  8b4508                           mov      eax, dword ptr [ebp + 8]
00422cba  b98fbc0000                       mov      ecx, 0xbc8f
00422cbf  f7e1                             mul      ecx
00422cc1  8945f8                           mov      dword ptr [ebp - 8], eax
00422cc4  8955fc                           mov      dword ptr [ebp - 4], edx
00422cc7  8b45f8                           mov      eax, dword ptr [ebp - 8]
00422cca  8b55fc                           mov      edx, dword ptr [ebp - 4]
00422ccd  b11f                             mov      cl, 0x1f
00422ccf  e8bc011200                       call     0x542e90
00422cd4  8b4df8                           mov      ecx, dword ptr [ebp - 8]
00422cd7  81e1ffffff7f                     and      ecx, 0x7fffffff
00422cdd  8b75fc                           mov      esi, dword ptr [ebp - 4]
00422ce0  83e600                           and      esi, 0
00422ce3  03c1                             add      eax, ecx
00422ce5  13d6                             adc      edx, esi
00422ce7  8945f8                           mov      dword ptr [ebp - 8], eax
00422cea  8955fc                           mov      dword ptr [ebp - 4], edx
00422ced  837dfc00                         cmp      dword ptr [ebp - 4], 0
00422cf1  7719                             ja       0x422d0c
00422cf3  7209                             jb       0x422cfe
00422cf5  817df8ffffff7f                   cmp      dword ptr [ebp - 8], 0x7fffffff
00422cfc  730e                             jae      0x422d0c
00422cfe  8b55f8                           mov      edx, dword ptr [ebp - 8]
00422d01  8b45fc                           mov      eax, dword ptr [ebp - 4]
00422d04  8955f0                           mov      dword ptr [ebp - 0x10], edx
00422d07  8945f4                           mov      dword ptr [ebp - 0xc], eax
00422d0a  eb15                             jmp      0x422d21
00422d0c  8b4df8                           mov      ecx, dword ptr [ebp - 8]
00422d0f  81e9ffffff7f                     sub      ecx, 0x7fffffff
00422d15  8b55fc                           mov      edx, dword ptr [ebp - 4]
00422d18  83da00                           sbb      edx, 0
00422d1b  894df0                           mov      dword ptr [ebp - 0x10], ecx
00422d1e  8955f4                           mov      dword ptr [ebp - 0xc], edx
00422d21  8b45f0                           mov      eax, dword ptr [ebp - 0x10]
00422d24  8b4df4                           mov      ecx, dword ptr [ebp - 0xc]
00422d27  8945f8                           mov      dword ptr [ebp - 8], eax
00422d2a  894dfc                           mov      dword ptr [ebp - 4], ecx
00422d2d  8b45f8                           mov      eax, dword ptr [ebp - 8]
00422d30  5e                               pop      esi
00422d31  8be5                             mov      esp, ebp
00422d33  5d                               pop      ebp
00422d34  c3                               ret      
