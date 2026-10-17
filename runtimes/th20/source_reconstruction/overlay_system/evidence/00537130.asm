00537130 55                       push       ebp
00537131 8bec                     mov        ebp, esp
00537133 51                       push       ecx
00537134 894dfc                   mov        dword ptr [ebp - 4], ecx
00537137 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053713a e89189ffff               call       0x52fad0
0053713f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00537142 c70058655700             mov        dword ptr [eax], 0x576558
00537148 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053714b 83c138                   add        ecx, 0x38
0053714e e83dbceeff               call       0x422d90
00537153 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00537156 83c148                   add        ecx, 0x48
00537159 e832bceeff               call       0x422d90
0053715e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00537161 c7415800000000           mov        dword ptr [ecx + 0x58], 0
00537168 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053716b 8be5                     mov        esp, ebp
0053716d 5d                       pop        ebp
0053716e c3                       ret        
0053716f cc                       int3       
