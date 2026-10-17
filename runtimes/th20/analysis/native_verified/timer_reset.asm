; timer_reset: original th20.exe code, SHA256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
; Recovered C++: native_recovered/native_core.hpp
; Called helpers remain visible in analysis/binary/disassembly.asm
00423f50  55                               push     ebp
00423f51  8bec                             mov      ebp, esp
00423f53  51                               push     ecx
00423f54  894dfc                           mov      dword ptr [ebp - 4], ecx
00423f57  8b45fc                           mov      eax, dword ptr [ebp - 4]
00423f5a  c7400400000000                   mov      dword ptr [eax + 4], 0
00423f61  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
00423f64  c701c1bdf0ff                     mov      dword ptr [ecx], 0xfff0bdc1
00423f6a  8b55fc                           mov      edx, dword ptr [ebp - 4]
00423f6d  0f57c0                           xorps    xmm0, xmm0
00423f70  f30f114208                       movss    dword ptr [edx + 8], xmm0
00423f75  8be5                             mov      esp, ebp
00423f77  5d                               pop      ebp
00423f78  c3                               ret      
