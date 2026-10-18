; lcg_next: original th20.exe code, SHA256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
; Recovered C++: native_recovered/native_core.hpp
; Called helpers remain visible in analysis/binary/disassembly.asm
004235f0  55                               push     ebp
004235f1  8bec                             mov      ebp, esp
004235f3  51                               push     ecx
004235f4  894dfc                           mov      dword ptr [ebp - 4], ecx
004235f7  8b45fc                           mov      eax, dword ptr [ebp - 4]
004235fa  8b08                             mov      ecx, dword ptr [eax]
004235fc  51                               push     ecx
004235fd  e8aef6ffff                       call     0x422cb0
00423602  83c404                           add      esp, 4
00423605  8b55fc                           mov      edx, dword ptr [ebp - 4]
00423608  8902                             mov      dword ptr [edx], eax
0042360a  8b45fc                           mov      eax, dword ptr [ebp - 4]
0042360d  8b00                             mov      eax, dword ptr [eax]
0042360f  8be5                             mov      esp, ebp
00423611  5d                               pop      ebp
00423612  c3                               ret      
