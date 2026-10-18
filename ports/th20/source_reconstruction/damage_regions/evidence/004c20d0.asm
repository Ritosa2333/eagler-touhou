004c20d0 55                       push       ebp
004c20d1 8bec                     mov        ebp, esp
004c20d3 51                       push       ecx
004c20d4 68c40c5700               push       0x570cc4
004c20d9 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004c20df e8fcdcffff               call       0x4bfde0
004c20e4 8945fc                   mov        dword ptr [ebp - 4], eax
004c20e7 837dfc00                 cmp        dword ptr [ebp - 4], 0
004c20eb 7504                     jne        0x4c20f1
004c20ed 33c0                     xor        eax, eax
004c20ef eb37                     jmp        0x4c2128
004c20f1 8b4508                   mov        eax, dword ptr [ebp + 8]
004c20f4 50                       push       eax
004c20f5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c20f8 e833eaffff               call       0x4c0b30
004c20fd 85c0                     test       eax, eax
004c20ff 7404                     je         0x4c2105
004c2101 eb07                     jmp        0x4c210a
004c2103 eb05                     jmp        0x4c210a
004c2105 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c2108 eb1e                     jmp        0x4c2128
004c210a 837dfc00                 cmp        dword ptr [ebp - 4], 0
004c210e 7416                     je         0x4c2126
004c2110 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c2113 51                       push       ecx
004c2114 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004c211a e8a1d6f5ff               call       0x41f7c0
004c211f c745fc00000000           mov        dword ptr [ebp - 4], 0
004c2126 33c0                     xor        eax, eax
004c2128 8be5                     mov        esp, ebp
004c212a 5d                       pop        ebp
004c212b c3                       ret        
004c212c cc                       int3       
004c212d cc                       int3       
004c212e cc                       int3       
004c212f cc                       int3       
