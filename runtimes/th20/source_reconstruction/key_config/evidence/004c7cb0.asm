004c7cb0 55                       push       ebp
004c7cb1 8bec                     mov        ebp, esp
004c7cb3 51                       push       ecx
004c7cb4 68100e5700               push       0x570e10
004c7cb9 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004c7cbf e84cd7ffff               call       0x4c5410
004c7cc4 8945fc                   mov        dword ptr [ebp - 4], eax
004c7cc7 837dfc00                 cmp        dword ptr [ebp - 4], 0
004c7ccb 7504                     jne        0x4c7cd1
004c7ccd 33c0                     xor        eax, eax
004c7ccf eb37                     jmp        0x4c7d08
004c7cd1 8b4508                   mov        eax, dword ptr [ebp + 8]
004c7cd4 50                       push       eax
004c7cd5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c7cd8 e8f3dbffff               call       0x4c58d0
004c7cdd 85c0                     test       eax, eax
004c7cdf 7404                     je         0x4c7ce5
004c7ce1 eb07                     jmp        0x4c7cea
004c7ce3 eb05                     jmp        0x4c7cea
004c7ce5 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c7ce8 eb1e                     jmp        0x4c7d08
004c7cea 837dfc00                 cmp        dword ptr [ebp - 4], 0
004c7cee 7416                     je         0x4c7d06
004c7cf0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c7cf3 51                       push       ecx
004c7cf4 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004c7cfa e8c17af5ff               call       0x41f7c0
004c7cff c745fc00000000           mov        dword ptr [ebp - 4], 0
004c7d06 33c0                     xor        eax, eax
004c7d08 8be5                     mov        esp, ebp
004c7d0a 5d                       pop        ebp
004c7d0b c3                       ret        
004c7d0c cc                       int3       
004c7d0d cc                       int3       
004c7d0e cc                       int3       
004c7d0f cc                       int3       
