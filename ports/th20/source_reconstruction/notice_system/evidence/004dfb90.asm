004dfb90 55                       push       ebp
004dfb91 8bec                     mov        ebp, esp
004dfb93 51                       push       ecx
004dfb94 68a81f5700               push       0x571fa8
004dfb99 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004dfb9f e86ceeffff               call       0x4dea10
004dfba4 8945fc                   mov        dword ptr [ebp - 4], eax
004dfba7 837dfc00                 cmp        dword ptr [ebp - 4], 0
004dfbab 7504                     jne        0x4dfbb1
004dfbad 33c0                     xor        eax, eax
004dfbaf eb3f                     jmp        0x4dfbf0
004dfbb1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004dfbb4 e8b7faffff               call       0x4df670
004dfbb9 85c0                     test       eax, eax
004dfbbb 7404                     je         0x4dfbc1
004dfbbd eb13                     jmp        0x4dfbd2
004dfbbf eb11                     jmp        0x4dfbd2
004dfbc1 8b4508                   mov        eax, dword ptr [ebp + 8]
004dfbc4 50                       push       eax
004dfbc5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004dfbc8 e8c3c8fbff               call       0x49c490
004dfbcd 8b45fc                   mov        eax, dword ptr [ebp - 4]
004dfbd0 eb1e                     jmp        0x4dfbf0
004dfbd2 837dfc00                 cmp        dword ptr [ebp - 4], 0
004dfbd6 7416                     je         0x4dfbee
004dfbd8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004dfbdb 51                       push       ecx
004dfbdc 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004dfbe2 e8d9fbf3ff               call       0x41f7c0
004dfbe7 c745fc00000000           mov        dword ptr [ebp - 4], 0
004dfbee 33c0                     xor        eax, eax
004dfbf0 8be5                     mov        esp, ebp
004dfbf2 5d                       pop        ebp
004dfbf3 c3                       ret        
004dfbf4 cc                       int3       
004dfbf5 cc                       int3       
004dfbf6 cc                       int3       
004dfbf7 cc                       int3       
004dfbf8 cc                       int3       
004dfbf9 cc                       int3       
004dfbfa cc                       int3       
004dfbfb cc                       int3       
004dfbfc cc                       int3       
004dfbfd cc                       int3       
004dfbfe cc                       int3       
004dfbff cc                       int3       
