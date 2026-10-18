0051f3c0 55                       push       ebp
0051f3c1 8bec                     mov        ebp, esp
0051f3c3 83ec10                   sub        esp, 0x10
0051f3c6 894dfc                   mov        dword ptr [ebp - 4], ecx
0051f3c9 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051f3cc 50                       push       eax
0051f3cd 68e0c15200               push       0x52c1e0
0051f3d2 6a0b                     push       0xb
0051f3d4 e8372fefff               call       0x412310
0051f3d9 83c40c                   add        esp, 0xc
0051f3dc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051f3df 894108                   mov        dword ptr [ecx + 8], eax
0051f3e2 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051f3e5 52                       push       edx
0051f3e6 6810c25200               push       0x52c210
0051f3eb 6a59                     push       0x59
0051f3ed e8be2fefff               call       0x4123b0
0051f3f2 83c40c                   add        esp, 0xc
0051f3f5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051f3f8 89410c                   mov        dword ptr [ecx + 0xc], eax
0051f3fb 68ec4f5700               push       0x574fec
0051f400 6a0b                     push       0xb
0051f402 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0051f408 e843faf2ff               call       0x44ee50
0051f40d 8945f8                   mov        dword ptr [ebp - 8], eax
0051f410 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051f413 8b45f8                   mov        eax, dword ptr [ebp - 8]
0051f416 894210                   mov        dword ptr [edx + 0x10], eax
0051f419 837df800                 cmp        dword ptr [ebp - 8], 0
0051f41d 7517                     jne        0x51f436
0051f41f 6810f65600               push       0x56f610
0051f424 6878065c00               push       0x5c0678
0051f429 e8224df3ff               call       0x454150
0051f42e 83c408                   add        esp, 8
0051f431 83c8ff                   or         eax, 0xffffffff
0051f434 eb50                     jmp        0x51f486
0051f436 68f84f5700               push       0x574ff8
0051f43b 6a0c                     push       0xc
0051f43d 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0051f443 e808faf2ff               call       0x44ee50
0051f448 8945f4                   mov        dword ptr [ebp - 0xc], eax
0051f44b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051f44e 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0051f451 895114                   mov        dword ptr [ecx + 0x14], edx
0051f454 837df400                 cmp        dword ptr [ebp - 0xc], 0
0051f458 7517                     jne        0x51f471
0051f45a 6810f65600               push       0x56f610
0051f45f 6878065c00               push       0x5c0678
0051f464 e8e74cf3ff               call       0x454150
0051f469 83c408                   add        esp, 8
0051f46c 83c8ff                   or         eax, 0xffffffff
0051f46f eb15                     jmp        0x51f486
0051f471 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051f474 83c024                   add        eax, 0x24
0051f477 8945f0                   mov        dword ptr [ebp - 0x10], eax
0051f47a 6a01                     push       1
0051f47c 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0051f47f e85c07faff               call       0x4bfbe0
0051f484 33c0                     xor        eax, eax
0051f486 8be5                     mov        esp, ebp
0051f488 5d                       pop        ebp
0051f489 c3                       ret        
0051f48a cc                       int3       
0051f48b cc                       int3       
0051f48c cc                       int3       
0051f48d cc                       int3       
0051f48e cc                       int3       
0051f48f cc                       int3       
