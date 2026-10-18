004a1010 55                       push       ebp
004a1011 8bec                     mov        ebp, esp
004a1013 51                       push       ecx
004a1014 68c0025700               push       0x5702c0
004a1019 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004a101f e80cd9ffff               call       0x49e930
004a1024 8945fc                   mov        dword ptr [ebp - 4], eax
004a1027 837dfc00                 cmp        dword ptr [ebp - 4], 0
004a102b 7504                     jne        0x4a1031
004a102d 33c0                     xor        eax, eax
004a102f eb33                     jmp        0x4a1064
004a1031 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a1034 e8c7f5ffff               call       0x4a0600
004a1039 85c0                     test       eax, eax
004a103b 7404                     je         0x4a1041
004a103d eb07                     jmp        0x4a1046
004a103f eb05                     jmp        0x4a1046
004a1041 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a1044 eb1e                     jmp        0x4a1064
004a1046 837dfc00                 cmp        dword ptr [ebp - 4], 0
004a104a 7416                     je         0x4a1062
004a104c 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a104f 50                       push       eax
004a1050 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004a1056 e865e7f7ff               call       0x41f7c0
004a105b c745fc00000000           mov        dword ptr [ebp - 4], 0
004a1062 33c0                     xor        eax, eax
004a1064 8be5                     mov        esp, ebp
004a1066 5d                       pop        ebp
004a1067 c3                       ret        
004a1068 cc                       int3       
004a1069 cc                       int3       
004a106a cc                       int3       
004a106b cc                       int3       
004a106c cc                       int3       
004a106d cc                       int3       
004a106e cc                       int3       
004a106f cc                       int3       
