004e1080 55                       push       ebp
004e1081 8bec                     mov        ebp, esp
004e1083 51                       push       ecx
004e1084 68c8205700               push       0x5720c8
004e1089 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004e108f e86cebffff               call       0x4dfc00
004e1094 8945fc                   mov        dword ptr [ebp - 4], eax
004e1097 837dfc00                 cmp        dword ptr [ebp - 4], 0
004e109b 7504                     jne        0x4e10a1
004e109d 33c0                     xor        eax, eax
004e109f eb37                     jmp        0x4e10d8
004e10a1 8b4508                   mov        eax, dword ptr [ebp + 8]
004e10a4 50                       push       eax
004e10a5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e10a8 e8d3fdffff               call       0x4e0e80
004e10ad 85c0                     test       eax, eax
004e10af 7404                     je         0x4e10b5
004e10b1 eb07                     jmp        0x4e10ba
004e10b3 eb05                     jmp        0x4e10ba
004e10b5 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e10b8 eb1e                     jmp        0x4e10d8
004e10ba 837dfc00                 cmp        dword ptr [ebp - 4], 0
004e10be 7416                     je         0x4e10d6
004e10c0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e10c3 51                       push       ecx
004e10c4 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004e10ca e8f1e6f3ff               call       0x41f7c0
004e10cf c745fc00000000           mov        dword ptr [ebp - 4], 0
004e10d6 33c0                     xor        eax, eax
004e10d8 8be5                     mov        esp, ebp
004e10da 5d                       pop        ebp
004e10db c3                       ret        
004e10dc cc                       int3       
004e10dd cc                       int3       
004e10de cc                       int3       
004e10df cc                       int3       
