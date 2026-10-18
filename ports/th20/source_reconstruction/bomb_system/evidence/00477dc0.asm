00477dc0 55                       push       ebp
00477dc1 8bec                     mov        ebp, esp
00477dc3 51                       push       ecx
00477dc4 894dfc                   mov        dword ptr [ebp - 4], ecx
00477dc7 8b45fc                   mov        eax, dword ptr [ebp - 4]
00477dca 50                       push       eax
00477dcb 68b07f4700               push       0x477fb0
00477dd0 6a21                     push       0x21
00477dd2 e8e9a4f9ff               call       0x4122c0
00477dd7 83c40c                   add        esp, 0xc
00477dda 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477ddd 894108                   mov        dword ptr [ecx + 8], eax
00477de0 8b55fc                   mov        edx, dword ptr [ebp - 4]
00477de3 52                       push       edx
00477de4 68c07f4700               push       0x477fc0
00477de9 6a2c                     push       0x2c
00477deb e870a5f9ff               call       0x412360
00477df0 83c40c                   add        esp, 0xc
00477df3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477df6 89410c                   mov        dword ptr [ecx + 0xc], eax
00477df9 6a00                     push       0
00477dfb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477dfe 83c11c                   add        ecx, 0x1c
00477e01 e81ab7faff               call       0x423520
00477e06 8b55fc                   mov        edx, dword ptr [ebp - 4]
00477e09 c7421400000000           mov        dword ptr [edx + 0x14], 0
00477e10 8b4508                   mov        eax, dword ptr [ebp + 8]
00477e13 50                       push       eax
00477e14 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477e17 e8b4040000               call       0x4782d0
00477e1c 33c0                     xor        eax, eax
00477e1e 8be5                     mov        esp, ebp
00477e20 5d                       pop        ebp
00477e21 c20400                   ret        4
00477e24 cc                       int3       
00477e25 cc                       int3       
00477e26 cc                       int3       
00477e27 cc                       int3       
00477e28 cc                       int3       
00477e29 cc                       int3       
00477e2a cc                       int3       
00477e2b cc                       int3       
00477e2c cc                       int3       
00477e2d cc                       int3       
00477e2e cc                       int3       
00477e2f cc                       int3       
