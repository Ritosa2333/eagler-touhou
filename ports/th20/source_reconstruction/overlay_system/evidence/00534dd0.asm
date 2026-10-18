00534dd0 55                       push       ebp
00534dd1 8bec                     mov        ebp, esp
00534dd3 51                       push       ecx
00534dd4 6878625700               push       0x576278
00534dd9 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00534ddf e8ccd9ffff               call       0x5327b0
00534de4 8945fc                   mov        dword ptr [ebp - 4], eax
00534de7 8b4508                   mov        eax, dword ptr [ebp + 8]
00534dea 50                       push       eax
00534deb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00534dee e8bde1ffff               call       0x532fb0
00534df3 85c0                     test       eax, eax
00534df5 7404                     je         0x534dfb
00534df7 eb07                     jmp        0x534e00
00534df9 eb05                     jmp        0x534e00
00534dfb 8b45fc                   mov        eax, dword ptr [ebp - 4]
00534dfe eb1e                     jmp        0x534e1e
00534e00 837dfc00                 cmp        dword ptr [ebp - 4], 0
00534e04 7416                     je         0x534e1c
00534e06 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00534e09 51                       push       ecx
00534e0a 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00534e10 e8aba9eeff               call       0x41f7c0
00534e15 c745fc00000000           mov        dword ptr [ebp - 4], 0
00534e1c 33c0                     xor        eax, eax
00534e1e 8be5                     mov        esp, ebp
00534e20 5d                       pop        ebp
00534e21 c3                       ret        
00534e22 cc                       int3       
00534e23 cc                       int3       
00534e24 cc                       int3       
00534e25 cc                       int3       
00534e26 cc                       int3       
00534e27 cc                       int3       
00534e28 cc                       int3       
00534e29 cc                       int3       
00534e2a cc                       int3       
00534e2b cc                       int3       
00534e2c cc                       int3       
00534e2d cc                       int3       
00534e2e cc                       int3       
00534e2f cc                       int3       
