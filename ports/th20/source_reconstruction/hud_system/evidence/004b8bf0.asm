004b8bf0 55                       push       ebp
004b8bf1 8bec                     mov        ebp, esp
004b8bf3 83ec3c                   sub        esp, 0x3c
004b8bf6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004b8bfb 33c5                     xor        eax, ebp
004b8bfd 8945fc                   mov        dword ptr [ebp - 4], eax
004b8c00 894de8                   mov        dword ptr [ebp - 0x18], ecx
004b8c03 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004b8c06 83c050                   add        eax, 0x50
004b8c09 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004b8c0c 6a00                     push       0
004b8c0e 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004b8c11 e86ab9f5ff               call       0x414580
004b8c16 833800                   cmp        dword ptr [eax], 0
004b8c19 7505                     jne        0x4b8c20
004b8c1b e95c010000               jmp        0x4b8d7c
004b8c20 51                       push       ecx
004b8c21 0f57c0                   xorps      xmm0, xmm0
004b8c24 f30f110424               movss      dword ptr [esp], xmm0
004b8c29 51                       push       ecx
004b8c2a 0f57c0                   xorps      xmm0, xmm0
004b8c2d f30f110424               movss      dword ptr [esp], xmm0
004b8c32 51                       push       ecx
004b8c33 0f57c0                   xorps      xmm0, xmm0
004b8c36 f30f110424               movss      dword ptr [esp], xmm0
004b8c3b 8d4df0                   lea        ecx, [ebp - 0x10]
004b8c3e e88da1f6ff               call       0x422dd0
004b8c43 b907000000               mov        ecx, 7
004b8c48 2b4d10                   sub        ecx, dword ptr [ebp + 0x10]
004b8c4b f30f2ac1                 cvtsi2ss   xmm0, ecx
004b8c4f f30f590548fd5600         mulss      xmm0, dword ptr [0x56fd48]
004b8c57 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004b8c5c c745ec00000000           mov        dword ptr [ebp - 0x14], 0
004b8c63 eb09                     jmp        0x4b8c6e
004b8c65 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004b8c68 83c201                   add        edx, 1
004b8c6b 8955ec                   mov        dword ptr [ebp - 0x14], edx
004b8c6e 837dec07                 cmp        dword ptr [ebp - 0x14], 7
004b8c72 7329                     jae        0x4b8c9d
004b8c74 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004b8c77 83c050                   add        eax, 0x50
004b8c7a 8945e0                   mov        dword ptr [ebp - 0x20], eax
004b8c7d 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004b8c80 51                       push       ecx
004b8c81 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004b8c84 e8f7b8f5ff               call       0x414580
004b8c89 8b10                     mov        edx, dword ptr [eax]
004b8c8b 8955dc                   mov        dword ptr [ebp - 0x24], edx
004b8c8e 8d45f0                   lea        eax, [ebp - 0x10]
004b8c91 50                       push       eax
004b8c92 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004b8c95 e8c650faff               call       0x45dd60
004b8c9a 90                       nop        
004b8c9b ebc8                     jmp        0x4b8c65
004b8c9d c745ec00000000           mov        dword ptr [ebp - 0x14], 0
004b8ca4 eb09                     jmp        0x4b8caf
004b8ca6 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004b8ca9 83c101                   add        ecx, 1
004b8cac 894dec                   mov        dword ptr [ebp - 0x14], ecx
004b8caf 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004b8cb2 3b5508                   cmp        edx, dword ptr [ebp + 8]
004b8cb5 7d1f                     jge        0x4b8cd6
004b8cb7 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004b8cba 83c050                   add        eax, 0x50
004b8cbd 8945d8                   mov        dword ptr [ebp - 0x28], eax
004b8cc0 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004b8cc3 51                       push       ecx
004b8cc4 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004b8cc7 e8b4b8f5ff               call       0x414580
004b8ccc 8b08                     mov        ecx, dword ptr [eax]
004b8cce e85dd6fcff               call       0x486330
004b8cd3 90                       nop        
004b8cd4 ebd0                     jmp        0x4b8ca6
004b8cd6 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004b8cd9 3b5510                   cmp        edx, dword ptr [ebp + 0x10]
004b8cdc 7d3c                     jge        0x4b8d1a
004b8cde 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004b8ce1 83c050                   add        eax, 0x50
004b8ce4 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004b8ce7 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004b8cea 51                       push       ecx
004b8ceb 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004b8cee e88db8f5ff               call       0x414580
004b8cf3 8b10                     mov        edx, dword ptr [eax]
004b8cf5 8955cc                   mov        dword ptr [ebp - 0x34], edx
004b8cf8 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004b8cfb 8b0c8580075700           mov        ecx, dword ptr [eax*4 + 0x570780]
004b8d02 894dd0                   mov        dword ptr [ebp - 0x30], ecx
004b8d05 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
004b8d08 52                       push       edx
004b8d09 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004b8d0c e88ff8ffff               call       0x4b85a0
004b8d11 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004b8d14 83c001                   add        eax, 1
004b8d17 8945ec                   mov        dword ptr [ebp - 0x14], eax
004b8d1a eb09                     jmp        0x4b8d25
004b8d1c 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004b8d1f 83c101                   add        ecx, 1
004b8d22 894dec                   mov        dword ptr [ebp - 0x14], ecx
004b8d25 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004b8d28 3b5510                   cmp        edx, dword ptr [ebp + 0x10]
004b8d2b 7d1f                     jge        0x4b8d4c
004b8d2d 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004b8d30 83c050                   add        eax, 0x50
004b8d33 8945c8                   mov        dword ptr [ebp - 0x38], eax
004b8d36 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004b8d39 51                       push       ecx
004b8d3a 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004b8d3d e83eb8f5ff               call       0x414580
004b8d42 8b08                     mov        ecx, dword ptr [eax]
004b8d44 e827d6fcff               call       0x486370
004b8d49 90                       nop        
004b8d4a ebd0                     jmp        0x4b8d1c
004b8d4c eb09                     jmp        0x4b8d57
004b8d4e 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004b8d51 83c201                   add        edx, 1
004b8d54 8955ec                   mov        dword ptr [ebp - 0x14], edx
004b8d57 837dec07                 cmp        dword ptr [ebp - 0x14], 7
004b8d5b 731f                     jae        0x4b8d7c
004b8d5d 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004b8d60 83c050                   add        eax, 0x50
004b8d63 8945c4                   mov        dword ptr [ebp - 0x3c], eax
004b8d66 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004b8d69 51                       push       ecx
004b8d6a 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004b8d6d e80eb8f5ff               call       0x414580
004b8d72 8b08                     mov        ecx, dword ptr [eax]
004b8d74 e807f8ffff               call       0x4b8580
004b8d79 90                       nop        
004b8d7a ebd2                     jmp        0x4b8d4e
004b8d7c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b8d7f 33cd                     xor        ecx, ebp
004b8d81 e8689c0800               call       0x5429ee
004b8d86 8be5                     mov        esp, ebp
004b8d88 5d                       pop        ebp
004b8d89 c20c00                   ret        0xc
004b8d8c cc                       int3       
004b8d8d cc                       int3       
004b8d8e cc                       int3       
004b8d8f cc                       int3       
