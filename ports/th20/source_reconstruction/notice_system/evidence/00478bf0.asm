00478bf0 55                       push       ebp
00478bf1 8bec                     mov        ebp, esp
00478bf3 51                       push       ecx
00478bf4 894dfc                   mov        dword ptr [ebp - 4], ecx
00478bf7 b801000000               mov        eax, 1
00478bfc 8be5                     mov        esp, ebp
00478bfe 5d                       pop        ebp
00478bff c3                       ret        
