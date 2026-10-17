004bd390 55                       push       ebp
004bd391 8bec                     mov        ebp, esp
004bd393 51                       push       ecx
004bd394 894dfc                   mov        dword ptr [ebp - 4], ecx
004bd397 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bd39a 83782c00                 cmp        dword ptr [eax + 0x2c], 0
004bd39e 741c                     je         0x4bd3bc
004bd3a0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bd3a3 8b512c                   mov        edx, dword ptr [ecx + 0x2c]
004bd3a6 52                       push       edx
004bd3a7 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004bd3ad e80e24f6ff               call       0x41f7c0
004bd3b2 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bd3b5 c7402c00000000           mov        dword ptr [eax + 0x2c], 0
004bd3bc 8be5                     mov        esp, ebp
004bd3be 5d                       pop        ebp
004bd3bf c3                       ret        
