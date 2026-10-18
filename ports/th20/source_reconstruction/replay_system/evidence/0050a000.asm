0050a000 55                       push       ebp
0050a001 8bec                     mov        ebp, esp
0050a003 833d28a85b0000           cmp        dword ptr [0x5ba828], 0
0050a00a 7416                     je         0x50a022
0050a00c 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
0050a012 e8c9baf7ff               call       0x485ae0
0050a017 85c0                     test       eax, eax
0050a019 7407                     je         0x50a022
0050a01b b801000000               mov        eax, 1
0050a020 eb08                     jmp        0x50a02a
0050a022 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0050a025 e836e3ffff               call       0x508360
0050a02a 5d                       pop        ebp
0050a02b c3                       ret        
0050a02c cc                       int3       
0050a02d cc                       int3       
0050a02e cc                       int3       
0050a02f cc                       int3       
