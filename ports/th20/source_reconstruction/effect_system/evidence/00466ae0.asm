00466ae0 55                       push       ebp
00466ae1 8bec                     mov        ebp, esp
00466ae3 83ec3c                   sub        esp, 0x3c
00466ae6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00466aeb 33c5                     xor        eax, ebp
00466aed 8945fc                   mov        dword ptr [ebp - 4], eax
00466af0 894dec                   mov        dword ptr [ebp - 0x14], ecx
00466af3 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00466af6 8b4804                   mov        ecx, dword ptr [eax + 4]
00466af9 51                       push       ecx
00466afa 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00466b00 e8bbf0fdff               call       0x445bc0
00466b05 8d4df0                   lea        ecx, [ebp - 0x10]
00466b08 e803c3fbff               call       0x422e10
00466b0d 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00466b10 8b4204                   mov        eax, dword ptr [edx + 4]
00466b13 8945d8                   mov        dword ptr [ebp - 0x28], eax
00466b16 8d4df0                   lea        ecx, [ebp - 0x10]
00466b19 51                       push       ecx
00466b1a 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00466b1d e8be0bfdff               call       0x4376e0
00466b22 90                       nop        
00466b23 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00466b26 83c208                   add        edx, 8
00466b29 8955e4                   mov        dword ptr [ebp - 0x1c], edx
00466b2c 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00466b2f e86c52faff               call       0x40bda0
00466b34 8945e8                   mov        dword ptr [ebp - 0x18], eax
00466b37 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00466b3a e891040000               call       0x466fd0
00466b3f 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00466b42 eb0b                     jmp        0x466b4f
00466b44 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00466b47 0568010000               add        eax, 0x168
00466b4c 8945e8                   mov        dword ptr [ebp - 0x18], eax
00466b4f 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00466b52 3b4dd4                   cmp        ecx, dword ptr [ebp - 0x2c]
00466b55 0f84a5000000             je         0x466c00
00466b5b 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
00466b5e 8955c4                   mov        dword ptr [ebp - 0x3c], edx
00466b61 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00466b64 05082d0000               add        eax, 0x2d08
00466b69 8945d0                   mov        dword ptr [ebp - 0x30], eax
00466b6c 6a1e                     push       0x1e
00466b6e 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00466b71 e8eac9fbff               call       0x423560
00466b76 0fb6c8                   movzx      ecx, al
00466b79 85c9                     test       ecx, ecx
00466b7b 7413                     je         0x466b90
00466b7d 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00466b80 81c1082d0000             add        ecx, 0x2d08
00466b86 e80594faff               call       0x40ff90
00466b8b 8945e0                   mov        dword ptr [ebp - 0x20], eax
00466b8e eb18                     jmp        0x466ba8
00466b90 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00466b93 81c1082d0000             add        ecx, 0x2d08
00466b99 e8f293faff               call       0x40ff90
00466b9e ba3c000000               mov        edx, 0x3c
00466ba3 2bd0                     sub        edx, eax
00466ba5 8955e0                   mov        dword ptr [ebp - 0x20], edx
00466ba8 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00466bab 05082d0000               add        eax, 0x2d08
00466bb0 8945cc                   mov        dword ptr [ebp - 0x34], eax
00466bb3 6a1e                     push       0x1e
00466bb5 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00466bb8 e8a3c9fbff               call       0x423560
00466bbd 0fb6c8                   movzx      ecx, al
00466bc0 85c9                     test       ecx, ecx
00466bc2 7409                     je         0x466bcd
00466bc4 c745dc00000000           mov        dword ptr [ebp - 0x24], 0
00466bcb eb19                     jmp        0x466be6
00466bcd 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00466bd0 81c2082d0000             add        edx, 0x2d08
00466bd6 8955c8                   mov        dword ptr [ebp - 0x38], edx
00466bd9 6a1e                     push       0x1e
00466bdb 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00466bde e81dfeffff               call       0x466a00
00466be3 8945dc                   mov        dword ptr [ebp - 0x24], eax
00466be6 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00466be9 50                       push       eax
00466bea 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00466bed 51                       push       ecx
00466bee 8d55f0                   lea        edx, [ebp - 0x10]
00466bf1 52                       push       edx
00466bf2 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00466bf5 e816000000               call       0x466c10
00466bfa 90                       nop        
00466bfb e944ffffff               jmp        0x466b44
00466c00 33c0                     xor        eax, eax
00466c02 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00466c05 33cd                     xor        ecx, ebp
00466c07 e8e2bd0d00               call       0x5429ee
00466c0c 8be5                     mov        esp, ebp
00466c0e 5d                       pop        ebp
00466c0f c3                       ret        
