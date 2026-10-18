004c9ab0 55                       push       ebp
004c9ab1 8bec                     mov        ebp, esp
004c9ab3 81ec80000000             sub        esp, 0x80
004c9ab9 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c9abe 33c5                     xor        eax, ebp
004c9ac0 8945fc                   mov        dword ptr [ebp - 4], eax
004c9ac3 894dc8                   mov        dword ptr [ebp - 0x38], ecx
004c9ac6 837d1800                 cmp        dword ptr [ebp + 0x18], 0
004c9aca 7413                     je         0x4c9adf
004c9acc 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004c9acf 83b8cc06000000           cmp        dword ptr [eax + 0x6cc], 0
004c9ad6 7407                     je         0x4c9adf
004c9ad8 33c0                     xor        eax, eax
004c9ada e9a5020000               jmp        0x4c9d84
004c9adf c745bc00000000           mov        dword ptr [ebp - 0x44], 0
004c9ae6 c745c400000000           mov        dword ptr [ebp - 0x3c], 0
004c9aed 8d4dd8                   lea        ecx, [ebp - 0x28]
004c9af0 e81b93f5ff               call       0x422e10
004c9af5 8d4df0                   lea        ecx, [ebp - 0x10]
004c9af8 e81393f5ff               call       0x422e10
004c9afd 8d4de4                   lea        ecx, [ebp - 0x1c]
004c9b00 e80b93f5ff               call       0x422e10
004c9b05 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004c9b08 83c158                   add        ecx, 0x58
004c9b0b 8b11                     mov        edx, dword ptr [ecx]
004c9b0d 8955cc                   mov        dword ptr [ebp - 0x34], edx
004c9b10 8b4104                   mov        eax, dword ptr [ecx + 4]
004c9b13 8945d0                   mov        dword ptr [ebp - 0x30], eax
004c9b16 8b4908                   mov        ecx, dword ptr [ecx + 8]
004c9b19 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
004c9b1c 6848175700               push       0x571748
004c9b21 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004c9b24 8b8218070000             mov        eax, dword ptr [edx + 0x718]
004c9b2a 50                       push       eax
004c9b2b 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004c9b31 e8da5af5ff               call       0x41f610
004c9b36 8945c0                   mov        dword ptr [ebp - 0x40], eax
004c9b39 837dc000                 cmp        dword ptr [ebp - 0x40], 0
004c9b3d 7507                     jne        0x4c9b46
004c9b3f 33c0                     xor        eax, eax
004c9b41 e93e020000               jmp        0x4c9d84
004c9b46 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004c9b49 8b9118070000             mov        edx, dword ptr [ecx + 0x718]
004c9b4f 52                       push       edx
004c9b50 6a00                     push       0
004c9b52 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
004c9b55 50                       push       eax
004c9b56 e805ac0700               call       0x544760
004c9b5b 83c40c                   add        esp, 0xc
004c9b5e 51                       push       ecx
004c9b5f f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
004c9b67 f30f110424               movss      dword ptr [esp], xmm0
004c9b6c 8d4d98                   lea        ecx, [ebp - 0x68]
004c9b6f 51                       push       ecx
004c9b70 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004c9b73 e87894f8ff               call       0x452ff0
004c9b78 8b10                     mov        edx, dword ptr [eax]
004c9b7a 8955f0                   mov        dword ptr [ebp - 0x10], edx
004c9b7d 8b4804                   mov        ecx, dword ptr [eax + 4]
004c9b80 894df4                   mov        dword ptr [ebp - 0xc], ecx
004c9b83 8b5008                   mov        edx, dword ptr [eax + 8]
004c9b86 8955f8                   mov        dword ptr [ebp - 8], edx
004c9b89 8d45f0                   lea        eax, [ebp - 0x10]
004c9b8c 50                       push       eax
004c9b8d 8d4d8c                   lea        ecx, [ebp - 0x74]
004c9b90 51                       push       ecx
004c9b91 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c9b94 e8a7f8f5ff               call       0x429440
004c9b99 8b10                     mov        edx, dword ptr [eax]
004c9b9b 8955d8                   mov        dword ptr [ebp - 0x28], edx
004c9b9e 8b4804                   mov        ecx, dword ptr [eax + 4]
004c9ba1 894ddc                   mov        dword ptr [ebp - 0x24], ecx
004c9ba4 8b5008                   mov        edx, dword ptr [eax + 8]
004c9ba7 8955e0                   mov        dword ptr [ebp - 0x20], edx
004c9baa 8d45f0                   lea        eax, [ebp - 0x10]
004c9bad 50                       push       eax
004c9bae 8d4d80                   lea        ecx, [ebp - 0x80]
004c9bb1 51                       push       ecx
004c9bb2 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c9bb5 e8b6f9f5ff               call       0x429570
004c9bba 8b10                     mov        edx, dword ptr [eax]
004c9bbc 8955f0                   mov        dword ptr [ebp - 0x10], edx
004c9bbf 8b4804                   mov        ecx, dword ptr [eax + 4]
004c9bc2 894df4                   mov        dword ptr [ebp - 0xc], ecx
004c9bc5 8b5008                   mov        edx, dword ptr [eax + 8]
004c9bc8 8955f8                   mov        dword ptr [ebp - 8], edx
004c9bcb 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004c9bce 8b8828130000             mov        ecx, dword ptr [eax + 0x1328]
004c9bd4 894db8                   mov        dword ptr [ebp - 0x48], ecx
004c9bd7 c745c400000000           mov        dword ptr [ebp - 0x3c], 0
004c9bde eb09                     jmp        0x4c9be9
004c9be0 8b55c4                   mov        edx, dword ptr [ebp - 0x3c]
004c9be3 83c201                   add        edx, 1
004c9be6 8955c4                   mov        dword ptr [ebp - 0x3c], edx
004c9be9 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004c9bec 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004c9bef 3b8818070000             cmp        ecx, dword ptr [eax + 0x718]
004c9bf5 0f8d28010000             jge        0x4c9d23
004c9bfb 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
004c9bfe 8b02                     mov        eax, dword ptr [edx]
004c9c00 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004c9c03 8b4a04                   mov        ecx, dword ptr [edx + 4]
004c9c06 894de8                   mov        dword ptr [ebp - 0x18], ecx
004c9c09 8b5208                   mov        edx, dword ptr [edx + 8]
004c9c0c 8955ec                   mov        dword ptr [ebp - 0x14], edx
004c9c0f 51                       push       ecx
004c9c10 f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
004c9c15 f30f110424               movss      dword ptr [esp], xmm0
004c9c1a 51                       push       ecx
004c9c1b f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
004c9c20 f30f110424               movss      dword ptr [esp], xmm0
004c9c25 51                       push       ecx
004c9c26 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004c9c2b f30f110424               movss      dword ptr [esp], xmm0
004c9c30 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004c9c33 51                       push       ecx
004c9c34 f30f104004               movss      xmm0, dword ptr [eax + 4]
004c9c39 f30f110424               movss      dword ptr [esp], xmm0
004c9c3e 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004c9c41 51                       push       ecx
004c9c42 f30f1001                 movss      xmm0, dword ptr [ecx]
004c9c46 f30f110424               movss      dword ptr [esp], xmm0
004c9c4b 8b5508                   mov        edx, dword ptr [ebp + 8]
004c9c4e 51                       push       ecx
004c9c4f f30f104204               movss      xmm0, dword ptr [edx + 4]
004c9c54 f30f110424               movss      dword ptr [esp], xmm0
004c9c59 8b4508                   mov        eax, dword ptr [ebp + 8]
004c9c5c 51                       push       ecx
004c9c5d f30f1000                 movss      xmm0, dword ptr [eax]
004c9c61 f30f110424               movss      dword ptr [esp], xmm0
004c9c66 e825f2f8ff               call       0x458e90
004c9c6b 83c41c                   add        esp, 0x1c
004c9c6e 0fb6c8                   movzx      ecx, al
004c9c71 85c9                     test       ecx, ecx
004c9c73 750a                     jne        0x4c9c7f
004c9c75 e99b000000               jmp        0x4c9d15
004c9c7a e996000000               jmp        0x4c9d15
004c9c7f 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
004c9c82 0355c4                   add        edx, dword ptr [ebp - 0x3c]
004c9c85 c60201                   mov        byte ptr [edx], 1
004c9c88 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
004c9c8b 83c001                   add        eax, 1
004c9c8e 8945bc                   mov        dword ptr [ebp - 0x44], eax
004c9c91 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004c9c94 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004c9c9a e86126f4ff               call       0x40c300
004c9c9f 05886d2800               add        eax, 0x286d88
004c9ca4 8945b4                   mov        dword ptr [ebp - 0x4c], eax
004c9ca7 8b55b4                   mov        edx, dword ptr [ebp - 0x4c]
004c9caa 8b02                     mov        eax, dword ptr [edx]
004c9cac 83c001                   add        eax, 1
004c9caf 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
004c9cb2 8901                     mov        dword ptr [ecx], eax
004c9cb4 8b45c4                   mov        eax, dword ptr [ebp - 0x3c]
004c9cb7 99                       cdq        
004c9cb8 b914000000               mov        ecx, 0x14
004c9cbd f7f9                     idiv       ecx
004c9cbf 85d2                     test       edx, edx
004c9cc1 7552                     jne        0x4c9d15
004c9cc3 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004c9cc6 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004c9ccc e82f26f4ff               call       0x40c300
004c9cd1 8bc8                     mov        ecx, eax
004c9cd3 e8d8bcfbff               call       0x4859b0
004c9cd8 8945ac                   mov        dword ptr [ebp - 0x54], eax
004c9cdb 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004c9cde 8b8814070000             mov        ecx, dword ptr [eax + 0x714]
004c9ce4 8d9409d4000000           lea        edx, [ecx + ecx + 0xd4]
004c9ceb 8955b0                   mov        dword ptr [ebp - 0x50], edx
004c9cee 6a00                     push       0
004c9cf0 6aff                     push       -1
004c9cf2 51                       push       ecx
004c9cf3 0f57c0                   xorps      xmm0, xmm0
004c9cf6 f30f110424               movss      dword ptr [esp], xmm0
004c9cfb 8d45e4                   lea        eax, [ebp - 0x1c]
004c9cfe 50                       push       eax
004c9cff 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
004c9d02 51                       push       ecx
004c9d03 6814fd5600               push       0x56fd14
004c9d08 8d55a4                   lea        edx, [ebp - 0x5c]
004c9d0b 52                       push       edx
004c9d0c 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
004c9d0f e8ccf3faff               call       0x4790e0
004c9d14 90                       nop        
004c9d15 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
004c9d18 83c020                   add        eax, 0x20
004c9d1b 8945b8                   mov        dword ptr [ebp - 0x48], eax
004c9d1e e9bdfeffff               jmp        0x4c9be0
004c9d23 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004c9d26 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004c9d29 3b9118070000             cmp        edx, dword ptr [ecx + 0x718]
004c9d2f 7c14                     jl         0x4c9d45
004c9d31 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004c9d34 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004c9d37 83e1f9                   and        ecx, 0xfffffff9
004c9d3a 83c902                   or         ecx, 2
004c9d3d 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004c9d40 894a1c                   mov        dword ptr [edx + 0x1c], ecx
004c9d43 eb20                     jmp        0x4c9d65
004c9d45 837dbc00                 cmp        dword ptr [ebp - 0x44], 0
004c9d49 7e1a                     jle        0x4c9d65
004c9d4b 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004c9d4e 8b08                     mov        ecx, dword ptr [eax]
004c9d50 8b5138                   mov        edx, dword ptr [ecx + 0x38]
004c9d53 8955a8                   mov        dword ptr [ebp - 0x58], edx
004c9d56 8b45c4                   mov        eax, dword ptr [ebp - 0x3c]
004c9d59 50                       push       eax
004c9d5a 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004c9d5d 51                       push       ecx
004c9d5e 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004c9d61 ff55a8                   call       dword ptr [ebp - 0x58]
004c9d64 90                       nop        
004c9d65 837dc000                 cmp        dword ptr [ebp - 0x40], 0
004c9d69 7416                     je         0x4c9d81
004c9d6b 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
004c9d6e 52                       push       edx
004c9d6f 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004c9d75 e8f658f5ff               call       0x41f670
004c9d7a c745c000000000           mov        dword ptr [ebp - 0x40], 0
004c9d81 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
004c9d84 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c9d87 33cd                     xor        ecx, ebp
004c9d89 e8608c0700               call       0x5429ee
004c9d8e 8be5                     mov        esp, ebp
004c9d90 5d                       pop        ebp
004c9d91 c21400                   ret        0x14
004c9d94 cc                       int3       
004c9d95 cc                       int3       
004c9d96 cc                       int3       
004c9d97 cc                       int3       
004c9d98 cc                       int3       
004c9d99 cc                       int3       
004c9d9a cc                       int3       
004c9d9b cc                       int3       
004c9d9c cc                       int3       
004c9d9d cc                       int3       
004c9d9e cc                       int3       
004c9d9f cc                       int3       
