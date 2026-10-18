00478e00 55                       push       ebp
00478e01 8bec                     mov        ebp, esp
00478e03 83ec08                   sub        esp, 8
00478e06 894dfc                   mov        dword ptr [ebp - 4], ecx
00478e09 8b45fc                   mov        eax, dword ptr [ebp - 4]
00478e0c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00478e0f 8b5004                   mov        edx, dword ptr [eax + 4]
00478e12 3b11                     cmp        edx, dword ptr [ecx]
00478e14 7417                     je         0x478e2d
00478e16 8b45fc                   mov        eax, dword ptr [ebp - 4]
00478e19 8b4004                   mov        eax, dword ptr [eax + 4]
00478e1c 99                       cdq        
00478e1d f77d08                   idiv       dword ptr [ebp + 8]
00478e20 85d2                     test       edx, edx
00478e22 7509                     jne        0x478e2d
00478e24 c745f801000000           mov        dword ptr [ebp - 8], 1
00478e2b eb07                     jmp        0x478e34
00478e2d c745f800000000           mov        dword ptr [ebp - 8], 0
00478e34 8b45f8                   mov        eax, dword ptr [ebp - 8]
00478e37 8be5                     mov        esp, ebp
00478e39 5d                       pop        ebp
00478e3a c20400                   ret        4
00478e3d cc                       int3       
00478e3e cc                       int3       
00478e3f cc                       int3       
