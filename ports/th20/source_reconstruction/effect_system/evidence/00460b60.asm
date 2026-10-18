00460b60 55                       push       ebp
00460b61 8bec                     mov        ebp, esp
00460b63 81ec80000000             sub        esp, 0x80
00460b69 53                       push       ebx
00460b6a 894df8                   mov        dword ptr [ebp - 8], ecx
00460b6d 8b45f8                   mov        eax, dword ptr [ebp - 8]
00460b70 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
00460b73 8988e0030000             mov        dword ptr [eax + 0x3e0], ecx
00460b79 8b55f8                   mov        edx, dword ptr [ebp - 8]
00460b7c 83c208                   add        edx, 8
00460b7f 8955e8                   mov        dword ptr [ebp - 0x18], edx
00460b82 6a00                     push       0
00460b84 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00460b87 e8e4caffff               call       0x45d670
00460b8c 0f57c0                   xorps      xmm0, xmm0
00460b8f f30f1100                 movss      dword ptr [eax], xmm0
00460b93 8b45f8                   mov        eax, dword ptr [ebp - 8]
00460b96 83c008                   add        eax, 8
00460b99 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00460b9c 6a00                     push       0
00460b9e 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00460ba1 e8cacaffff               call       0x45d670
00460ba6 0f57c0                   xorps      xmm0, xmm0
00460ba9 f30f114004               movss      dword ptr [eax + 4], xmm0
00460bae 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00460bb1 8b5508                   mov        edx, dword ptr [ebp + 8]
00460bb4 8b421c                   mov        eax, dword ptr [edx + 0x1c]
00460bb7 8981cc030000             mov        dword ptr [ecx + 0x3cc], eax
00460bbd 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00460bc0 81c1e4030000             add        ecx, 0x3e4
00460bc6 8b5508                   mov        edx, dword ptr [ebp + 8]
00460bc9 8b02                     mov        eax, dword ptr [edx]
00460bcb 8901                     mov        dword ptr [ecx], eax
00460bcd 8b4204                   mov        eax, dword ptr [edx + 4]
00460bd0 894104                   mov        dword ptr [ecx + 4], eax
00460bd3 8b5208                   mov        edx, dword ptr [edx + 8]
00460bd6 895108                   mov        dword ptr [ecx + 8], edx
00460bd9 8b45f8                   mov        eax, dword ptr [ebp - 8]
00460bdc 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00460bdf 8a5128                   mov        dl, byte ptr [ecx + 0x28]
00460be2 8890f0030000             mov        byte ptr [eax + 0x3f0], dl
00460be8 8b45f8                   mov        eax, dword ptr [ebp - 8]
00460beb 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00460bee 8b5118                   mov        edx, dword ptr [ecx + 0x18]
00460bf1 8990c8030000             mov        dword ptr [eax + 0x3c8], edx
00460bf7 6a01                     push       1
00460bf9 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00460bfc 81c1d0030000             add        ecx, 0x3d0
00460c02 e81929fcff               call       0x423520
00460c07 c745ec00000000           mov        dword ptr [ebp - 0x14], 0
00460c0e c745f000000000           mov        dword ptr [ebp - 0x10], 0
00460c15 eb09                     jmp        0x460c20
00460c17 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00460c1a 83c001                   add        eax, 1
00460c1d 8945f0                   mov        dword ptr [ebp - 0x10], eax
00460c20 837df050                 cmp        dword ptr [ebp - 0x10], 0x50
00460c24 0f8dad020000             jge        0x460ed7
00460c2a b950000000               mov        ecx, 0x50
00460c2f 2b4df0                   sub        ecx, dword ptr [ebp - 0x10]
00460c32 83e901                   sub        ecx, 1
00460c35 894df4                   mov        dword ptr [ebp - 0xc], ecx
00460c38 8b55f8                   mov        edx, dword ptr [ebp - 8]
00460c3b 81c288020000             add        edx, 0x288
00460c41 8955e0                   mov        dword ptr [ebp - 0x20], edx
00460c44 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00460c47 50                       push       eax
00460c48 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00460c4b e83039fbff               call       0x414580
00460c50 c64002ff                 mov        byte ptr [eax + 2], 0xff
00460c54 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00460c57 81c188020000             add        ecx, 0x288
00460c5d 894ddc                   mov        dword ptr [ebp - 0x24], ecx
00460c60 ba50000000               mov        edx, 0x50
00460c65 2b55f0                   sub        edx, dword ptr [ebp - 0x10]
00460c68 6bda03                   imul       ebx, edx, 3
00460c6b 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00460c6e 50                       push       eax
00460c6f 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00460c72 e80939fbff               call       0x414580
00460c77 8818                     mov        byte ptr [eax], bl
00460c79 b9c4a45b00               mov        ecx, 0x5ba4c4
00460c7e e89d90feff               call       0x449d20
00460c83 8845ff                   mov        byte ptr [ebp - 1], al
00460c86 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00460c89 81c188020000             add        ecx, 0x288
00460c8f 894dd8                   mov        dword ptr [ebp - 0x28], ecx
00460c92 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00460c95 52                       push       edx
00460c96 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00460c99 e8e238fbff               call       0x414580
00460c9e 8a4dff                   mov        cl, byte ptr [ebp - 1]
00460ca1 884801                   mov        byte ptr [eax + 1], cl
00460ca4 8b55f8                   mov        edx, dword ptr [ebp - 8]
00460ca7 81c288020000             add        edx, 0x288
00460cad 8955d4                   mov        dword ptr [ebp - 0x2c], edx
00460cb0 b850000000               mov        eax, 0x50
00460cb5 2b45f0                   sub        eax, dword ptr [ebp - 0x10]
00460cb8 6bd803                   imul       ebx, eax, 3
00460cbb 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00460cbe 51                       push       ecx
00460cbf 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00460cc2 e8b938fbff               call       0x414580
00460cc7 885803                   mov        byte ptr [eax + 3], bl
00460cca 837df428                 cmp        dword ptr [ebp - 0xc], 0x28
00460cce 7f31                     jg         0x460d01
00460cd0 8b55f8                   mov        edx, dword ptr [ebp - 8]
00460cd3 81c288020000             add        edx, 0x288
00460cd9 8955d0                   mov        dword ptr [ebp - 0x30], edx
00460cdc 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00460cdf c1e005                   shl        eax, 5
00460ce2 bbff000000               mov        ebx, 0xff
00460ce7 2bd8                     sub        ebx, eax
00460ce9 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00460cec 51                       push       ecx
00460ced 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00460cf0 e88b38fbff               call       0x414580
00460cf5 885803                   mov        byte ptr [eax + 3], bl
00460cf8 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00460cfb 83c201                   add        edx, 1
00460cfe 8955ec                   mov        dword ptr [ebp - 0x14], edx
00460d01 837df408                 cmp        dword ptr [ebp - 0xc], 8
00460d05 7d20                     jge        0x460d27
00460d07 8b45f8                   mov        eax, dword ptr [ebp - 8]
00460d0a 0588020000               add        eax, 0x288
00460d0f 8945cc                   mov        dword ptr [ebp - 0x34], eax
00460d12 8b5df0                   mov        ebx, dword ptr [ebp - 0x10]
00460d15 c1e305                   shl        ebx, 5
00460d18 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00460d1b 51                       push       ecx
00460d1c 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00460d1f e85c38fbff               call       0x414580
00460d24 885803                   mov        byte ptr [eax + 3], bl
00460d27 837df43f                 cmp        dword ptr [ebp - 0xc], 0x3f
00460d2b 0f8e85000000             jle        0x460db6
00460d31 8b55f8                   mov        edx, dword ptr [ebp - 8]
00460d34 f30f1082c8030000         movss      xmm0, dword ptr [edx + 0x3c8]
00460d3c f30f1145c4               movss      dword ptr [ebp - 0x3c], xmm0
00460d41 8b45f8                   mov        eax, dword ptr [ebp - 8]
00460d44 83c008                   add        eax, 8
00460d47 8945c8                   mov        dword ptr [ebp - 0x38], eax
00460d4a 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00460d4d 51                       push       ecx
00460d4e 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00460d51 e81ac9ffff               call       0x45d670
00460d56 8945c0                   mov        dword ptr [ebp - 0x40], eax
00460d59 51                       push       ecx
00460d5a f30f1005bcd75600         movss      xmm0, dword ptr [0x56d7bc]
00460d62 f30f110424               movss      dword ptr [esp], xmm0
00460d67 51                       push       ecx
00460d68 f30f1045c4               movss      xmm0, dword ptr [ebp - 0x3c]
00460d6d f30f110424               movss      dword ptr [esp], xmm0
00460d72 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
00460d75 52                       push       edx
00460d76 e8b585fdff               call       0x439330
00460d7b 83c40c                   add        esp, 0xc
00460d7e 8b45f8                   mov        eax, dword ptr [ebp - 8]
00460d81 51                       push       ecx
00460d82 f30f1080cc030000         movss      xmm0, dword ptr [eax + 0x3cc]
00460d8a f30f110424               movss      dword ptr [esp], xmm0
00460d8f 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00460d92 51                       push       ecx
00460d93 f30f1081c8030000         movss      xmm0, dword ptr [ecx + 0x3c8]
00460d9b f30f110424               movss      dword ptr [esp], xmm0
00460da0 e85b78fdff               call       0x438600
00460da5 83c408                   add        esp, 8
00460da8 8b55f8                   mov        edx, dword ptr [ebp - 8]
00460dab d99ac8030000             fstp       dword ptr [edx + 0x3c8]
00460db1 e9d6000000               jmp        0x460e8c
00460db6 b9c4a45b00               mov        ecx, 0x5ba4c4
00460dbb e8708afcff               call       0x429830
00460dc0 d95dbc                   fstp       dword ptr [ebp - 0x44]
00460dc3 f30f1045bc               movss      xmm0, dword ptr [ebp - 0x44]
00460dc8 f30f5905ccc85600         mulss      xmm0, dword ptr [0x56c8cc]
00460dd0 f30f5805e0c85600         addss      xmm0, dword ptr [0x56c8e0]
00460dd8 f30f1145b4               movss      dword ptr [ebp - 0x4c], xmm0
00460ddd 8b45f8                   mov        eax, dword ptr [ebp - 8]
00460de0 f30f1080c8030000         movss      xmm0, dword ptr [eax + 0x3c8]
00460de8 f30f1145b0               movss      dword ptr [ebp - 0x50], xmm0
00460ded 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00460df0 83c108                   add        ecx, 8
00460df3 894db8                   mov        dword ptr [ebp - 0x48], ecx
00460df6 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00460df9 52                       push       edx
00460dfa 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00460dfd e86ec8ffff               call       0x45d670
00460e02 8945ac                   mov        dword ptr [ebp - 0x54], eax
00460e05 51                       push       ecx
00460e06 f30f1045b4               movss      xmm0, dword ptr [ebp - 0x4c]
00460e0b f30f110424               movss      dword ptr [esp], xmm0
00460e10 51                       push       ecx
00460e11 f30f1045b0               movss      xmm0, dword ptr [ebp - 0x50]
00460e16 f30f110424               movss      dword ptr [esp], xmm0
00460e1b 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
00460e1e 50                       push       eax
00460e1f e80c85fdff               call       0x439330
00460e24 83c40c                   add        esp, 0xc
00460e27 b9c4a45b00               mov        ecx, 0x5ba4c4
00460e2c e89f89fcff               call       0x4297d0
00460e31 d95da8                   fstp       dword ptr [ebp - 0x58]
00460e34 f30f1045a8               movss      xmm0, dword ptr [ebp - 0x58]
00460e39 f30f590528ef5600         mulss      xmm0, dword ptr [0x56ef28]
00460e41 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00460e44 f30f1089cc030000         movss      xmm1, dword ptr [ecx + 0x3cc]
00460e4c f30f5cc8                 subss      xmm1, xmm0
00460e50 f30f114da4               movss      dword ptr [ebp - 0x5c], xmm1
00460e55 8b55f8                   mov        edx, dword ptr [ebp - 8]
00460e58 f30f1082c8030000         movss      xmm0, dword ptr [edx + 0x3c8]
00460e60 f30f1145a0               movss      dword ptr [ebp - 0x60], xmm0
00460e65 51                       push       ecx
00460e66 f30f1045a4               movss      xmm0, dword ptr [ebp - 0x5c]
00460e6b f30f110424               movss      dword ptr [esp], xmm0
00460e70 51                       push       ecx
00460e71 f30f1045a0               movss      xmm0, dword ptr [ebp - 0x60]
00460e76 f30f110424               movss      dword ptr [esp], xmm0
00460e7b e88077fdff               call       0x438600
00460e80 83c408                   add        esp, 8
00460e83 8b45f8                   mov        eax, dword ptr [ebp - 8]
00460e86 d998c8030000             fstp       dword ptr [eax + 0x3c8]
00460e8c 837df44f                 cmp        dword ptr [ebp - 0xc], 0x4f
00460e90 7d40                     jge        0x460ed2
00460e92 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00460e95 83c108                   add        ecx, 8
00460e98 894d9c                   mov        dword ptr [ebp - 0x64], ecx
00460e9b 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00460e9e 83c201                   add        edx, 1
00460ea1 52                       push       edx
00460ea2 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
00460ea5 e8c6c7ffff               call       0x45d670
00460eaa 894598                   mov        dword ptr [ebp - 0x68], eax
00460ead 8b45f8                   mov        eax, dword ptr [ebp - 8]
00460eb0 83c008                   add        eax, 8
00460eb3 894594                   mov        dword ptr [ebp - 0x6c], eax
00460eb6 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00460eb9 51                       push       ecx
00460eba 8d5580                   lea        edx, [ebp - 0x80]
00460ebd 52                       push       edx
00460ebe 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00460ec1 50                       push       eax
00460ec2 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00460ec5 e8a6c7ffff               call       0x45d670
00460eca 8bc8                     mov        ecx, eax
00460ecc e88f8afdff               call       0x439960
00460ed1 90                       nop        
00460ed2 e940fdffff               jmp        0x460c17
00460ed7 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00460eda 8b5104                   mov        edx, dword ptr [ecx + 4]
00460edd 895590                   mov        dword ptr [ebp - 0x70], edx
00460ee0 8b4508                   mov        eax, dword ptr [ebp + 8]
00460ee3 50                       push       eax
00460ee4 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00460ee7 e874ceffff               call       0x45dd60
00460eec 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00460eef 8b5104                   mov        edx, dword ptr [ecx + 4]
00460ef2 89558c                   mov        dword ptr [ebp - 0x74], edx
00460ef5 6a05                     push       5
00460ef7 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00460efa e851f4feff               call       0x450350
00460eff 8b45f8                   mov        eax, dword ptr [ebp - 8]
00460f02 8b4804                   mov        ecx, dword ptr [eax + 4]
00460f05 894d88                   mov        dword ptr [ebp - 0x78], ecx
00460f08 6a00                     push       0
00460f0a 68ff000000               push       0xff
00460f0f 6a00                     push       0
00460f11 6a50                     push       0x50
00460f13 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
00460f16 e8457dfdff               call       0x438c60
00460f1b 33c0                     xor        eax, eax
00460f1d 5b                       pop        ebx
00460f1e 8be5                     mov        esp, ebp
00460f20 5d                       pop        ebp
00460f21 c20800                   ret        8
00460f24 cc                       int3       
00460f25 cc                       int3       
00460f26 cc                       int3       
00460f27 cc                       int3       
00460f28 cc                       int3       
00460f29 cc                       int3       
00460f2a cc                       int3       
00460f2b cc                       int3       
00460f2c cc                       int3       
00460f2d cc                       int3       
00460f2e cc                       int3       
00460f2f cc                       int3       
