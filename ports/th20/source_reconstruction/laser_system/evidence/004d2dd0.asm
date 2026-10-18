004d2dd0 55                       push       ebp
004d2dd1 8bec                     mov        ebp, esp
004d2dd3 81ec80000000             sub        esp, 0x80
004d2dd9 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004d2dde 33c5                     xor        eax, ebp
004d2de0 8945fc                   mov        dword ptr [ebp - 4], eax
004d2de3 894dc8                   mov        dword ptr [ebp - 0x38], ecx
004d2de6 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004d2de9 8b88d8060000             mov        ecx, dword ptr [eax + 0x6d8]
004d2def 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
004d2df2 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004d2df5 81c24c070000             add        edx, 0x74c
004d2dfb 8955b8                   mov        dword ptr [ebp - 0x48], edx
004d2dfe 6a00                     push       0
004d2e00 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004d2e03 e8b835fbff               call       0x4863c0
004d2e08 90                       nop        
004d2e09 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004d2e0c 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004d2e0f c1e906                   shr        ecx, 6
004d2e12 83e101                   and        ecx, 1
004d2e15 7520                     jne        0x4d2e37
004d2e17 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004d2e1a 8b02                     mov        eax, dword ptr [edx]
004d2e1c 8b4810                   mov        ecx, dword ptr [eax + 0x10]
004d2e1f 894db4                   mov        dword ptr [ebp - 0x4c], ecx
004d2e22 6a00                     push       0
004d2e24 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004d2e27 ff55b4                   call       dword ptr [ebp - 0x4c]
004d2e2a 85c0                     test       eax, eax
004d2e2c 7507                     jne        0x4d2e35
004d2e2e 33c0                     xor        eax, eax
004d2e30 e9bc020000               jmp        0x4d30f1
004d2e35 eb15                     jmp        0x4d2e4c
004d2e37 8b55c4                   mov        edx, dword ptr [ebp - 0x3c]
004d2e3a 8b4238                   mov        eax, dword ptr [edx + 0x38]
004d2e3d c1e803                   shr        eax, 3
004d2e40 83e001                   and        eax, 1
004d2e43 7507                     jne        0x4d2e4c
004d2e45 33c0                     xor        eax, eax
004d2e47 e9a5020000               jmp        0x4d30f1
004d2e4c 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004d2e4f f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004d2e57 f30f118184000000         movss      dword ptr [ecx + 0x84], xmm0
004d2e5f 8d4df0                   lea        ecx, [ebp - 0x10]
004d2e62 e8a9fff4ff               call       0x422e10
004d2e67 8d4de4                   lea        ecx, [ebp - 0x1c]
004d2e6a e8a1fff4ff               call       0x422e10
004d2e6f c745a000000000           mov        dword ptr [ebp - 0x60], 0
004d2e76 8b55c4                   mov        edx, dword ptr [ebp - 0x3c]
004d2e79 8b02                     mov        eax, dword ptr [edx]
004d2e7b 8945f0                   mov        dword ptr [ebp - 0x10], eax
004d2e7e 8b4a04                   mov        ecx, dword ptr [edx + 4]
004d2e81 894df4                   mov        dword ptr [ebp - 0xc], ecx
004d2e84 8b5208                   mov        edx, dword ptr [edx + 8]
004d2e87 8955f8                   mov        dword ptr [ebp - 8], edx
004d2e8a 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004d2e8d 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004d2e93 e8f8d0f3ff               call       0x40ff90
004d2e98 8945a4                   mov        dword ptr [ebp - 0x5c], eax
004d2e9b 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004d2e9e f30f104118               movss      xmm0, dword ptr [ecx + 0x18]
004d2ea3 f30f1145b0               movss      dword ptr [ebp - 0x50], xmm0
004d2ea8 8b55c4                   mov        edx, dword ptr [ebp - 0x3c]
004d2eab f30f10421c               movss      xmm0, dword ptr [edx + 0x1c]
004d2eb0 f30f1145ac               movss      dword ptr [ebp - 0x54], xmm0
004d2eb5 8b45c4                   mov        eax, dword ptr [ebp - 0x3c]
004d2eb8 f30f104024               movss      xmm0, dword ptr [eax + 0x24]
004d2ebd f30f1145a8               movss      dword ptr [ebp - 0x58], xmm0
004d2ec2 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004d2ec5 51                       push       ecx
004d2ec6 51                       push       ecx
004d2ec7 f30f1045b0               movss      xmm0, dword ptr [ebp - 0x50]
004d2ecc f30f110424               movss      dword ptr [esp], xmm0
004d2ed1 51                       push       ecx
004d2ed2 f30f1045ac               movss      xmm0, dword ptr [ebp - 0x54]
004d2ed7 f30f110424               movss      dword ptr [esp], xmm0
004d2edc 51                       push       ecx
004d2edd f30f1045a8               movss      xmm0, dword ptr [ebp - 0x58]
004d2ee2 f30f110424               movss      dword ptr [esp], xmm0
004d2ee7 8d55f0                   lea        edx, [ebp - 0x10]
004d2eea 52                       push       edx
004d2eeb 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
004d2eee e8dd620200               call       0x4f91d0
004d2ef3 8945c0                   mov        dword ptr [ebp - 0x40], eax
004d2ef6 837dc002                 cmp        dword ptr [ebp - 0x40], 2
004d2efa 7528                     jne        0x4d2f24
004d2efc 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004d2eff 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004d2f05 e886d0f3ff               call       0x40ff90
004d2f0a 8bc8                     mov        ecx, eax
004d2f0c e83fd9f8ff               call       0x460850
004d2f11 8b08                     mov        ecx, dword ptr [eax]
004d2f13 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004d2f16 8b5004                   mov        edx, dword ptr [eax + 4]
004d2f19 8955e8                   mov        dword ptr [ebp - 0x18], edx
004d2f1c 8b4008                   mov        eax, dword ptr [eax + 8]
004d2f1f 8945ec                   mov        dword ptr [ebp - 0x14], eax
004d2f22 eb13                     jmp        0x4d2f37
004d2f24 837dc000                 cmp        dword ptr [ebp - 0x40], 0
004d2f28 750d                     jne        0x4d2f37
004d2f2a 837da000                 cmp        dword ptr [ebp - 0x60], 0
004d2f2e 7407                     je         0x4d2f37
004d2f30 c745c002000000           mov        dword ptr [ebp - 0x40], 2
004d2f37 837dc001                 cmp        dword ptr [ebp - 0x40], 1
004d2f3b 7571                     jne        0x4d2fae
004d2f3d 51                       push       ecx
004d2f3e 0f57c0                   xorps      xmm0, xmm0
004d2f41 f30f110424               movss      dword ptr [esp], xmm0
004d2f46 51                       push       ecx
004d2f47 f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
004d2f4f f30f110424               movss      dword ptr [esp], xmm0
004d2f54 51                       push       ecx
004d2f55 f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
004d2f5d f30f110424               movss      dword ptr [esp], xmm0
004d2f62 8d4dcc                   lea        ecx, [ebp - 0x34]
004d2f65 e866fef4ff               call       0x422dd0
004d2f6a 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004d2f6d 8b11                     mov        edx, dword ptr [ecx]
004d2f6f 8b422c                   mov        eax, dword ptr [edx + 0x2c]
004d2f72 894598                   mov        dword ptr [ebp - 0x68], eax
004d2f75 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004d2f78 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004d2f7e e80dd0f3ff               call       0x40ff90
004d2f83 8bc8                     mov        ecx, eax
004d2f85 e8c6d8f8ff               call       0x460850
004d2f8a 89459c                   mov        dword ptr [ebp - 0x64], eax
004d2f8d 6a01                     push       1
004d2f8f 6a00                     push       0
004d2f91 51                       push       ecx
004d2f92 0f57c0                   xorps      xmm0, xmm0
004d2f95 f30f110424               movss      dword ptr [esp], xmm0
004d2f9a 8d55cc                   lea        edx, [ebp - 0x34]
004d2f9d 52                       push       edx
004d2f9e 8b459c                   mov        eax, dword ptr [ebp - 0x64]
004d2fa1 50                       push       eax
004d2fa2 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004d2fa5 ff5598                   call       dword ptr [ebp - 0x68]
004d2fa8 90                       nop        
004d2fa9 e941010000               jmp        0x4d30ef
004d2fae 837dc002                 cmp        dword ptr [ebp - 0x40], 2
004d2fb2 0f852a010000             jne        0x4d30e2
004d2fb8 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004d2fbb 8b91d4060000             mov        edx, dword ptr [ecx + 0x6d4]
004d2fc1 83c201                   add        edx, 1
004d2fc4 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004d2fc7 8990d4060000             mov        dword ptr [eax + 0x6d4], edx
004d2fcd 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004d2fd0 83c138                   add        ecx, 0x38
004d2fd3 894d94                   mov        dword ptr [ebp - 0x6c], ecx
004d2fd6 6a08                     push       8
004d2fd8 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
004d2fdb e860f0f9ff               call       0x472040
004d2fe0 85c0                     test       eax, eax
004d2fe2 7568                     jne        0x4d304c
004d2fe4 8d4dd8                   lea        ecx, [ebp - 0x28]
004d2fe7 e824fef4ff               call       0x422e10
004d2fec 8d55d8                   lea        edx, [ebp - 0x28]
004d2fef 52                       push       edx
004d2ff0 8d45e4                   lea        eax, [ebp - 0x1c]
004d2ff3 50                       push       eax
004d2ff4 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004d2ff7 51                       push       ecx
004d2ff8 f30f104124               movss      xmm0, dword ptr [ecx + 0x24]
004d2ffd f30f110424               movss      dword ptr [esp], xmm0
004d3002 8b55c4                   mov        edx, dword ptr [ebp - 0x3c]
004d3005 51                       push       ecx
004d3006 f30f104218               movss      xmm0, dword ptr [edx + 0x18]
004d300b f30f110424               movss      dword ptr [esp], xmm0
004d3010 8d45f0                   lea        eax, [ebp - 0x10]
004d3013 50                       push       eax
004d3014 e8671df8ff               call       0x454d80
004d3019 ddd8                     fstp       st(0)
004d301b 83c414                   add        esp, 0x14
004d301e 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004d3021 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004d3027 e864cff3ff               call       0x40ff90
004d302c 89458c                   mov        dword ptr [ebp - 0x74], eax
004d302f 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004d3032 8b82d0060000             mov        eax, dword ptr [edx + 0x6d0]
004d3038 894590                   mov        dword ptr [ebp - 0x70], eax
004d303b 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
004d303e 51                       push       ecx
004d303f 8d55d8                   lea        edx, [ebp - 0x28]
004d3042 52                       push       edx
004d3043 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004d3046 e8455b0200               call       0x4f8b90
004d304b 90                       nop        
004d304c 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004d304f 8b889c060000             mov        ecx, dword ptr [eax + 0x69c]
004d3055 d1e9                     shr        ecx, 1
004d3057 83e101                   and        ecx, 1
004d305a 7570                     jne        0x4d30cc
004d305c 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004d305f 8b82d4060000             mov        eax, dword ptr [edx + 0x6d4]
004d3065 d1e0                     shl        eax, 1
004d3067 b9d0000000               mov        ecx, 0xd0
004d306c 2bc8                     sub        ecx, eax
004d306e 894dbc                   mov        dword ptr [ebp - 0x44], ecx
004d3071 817dbcf0000000           cmp        dword ptr [ebp - 0x44], 0xf0
004d3078 7e07                     jle        0x4d3081
004d307a c745bcf0000000           mov        dword ptr [ebp - 0x44], 0xf0
004d3081 837dbc60                 cmp        dword ptr [ebp - 0x44], 0x60
004d3085 7d07                     jge        0x4d308e
004d3087 c745bc60000000           mov        dword ptr [ebp - 0x44], 0x60
004d308e 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004d3091 81c24c070000             add        edx, 0x74c
004d3097 895588                   mov        dword ptr [ebp - 0x78], edx
004d309a 6a01                     push       1
004d309c 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
004d309f e81c33fbff               call       0x4863c0
004d30a4 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004d30a7 054c070000               add        eax, 0x74c
004d30ac 894584                   mov        dword ptr [ebp - 0x7c], eax
004d30af 68ff000000               push       0xff
004d30b4 6880000000               push       0x80
004d30b9 0fb64dbc                 movzx      ecx, byte ptr [ebp - 0x44]
004d30bd 51                       push       ecx
004d30be 68ff000000               push       0xff
004d30c3 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
004d30c6 e8e533fbff               call       0x4864b0
004d30cb 90                       nop        
004d30cc 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004d30cf 83c238                   add        edx, 0x38
004d30d2 895580                   mov        dword ptr [ebp - 0x80], edx
004d30d5 6a00                     push       0
004d30d7 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
004d30da e86104f5ff               call       0x423540
004d30df 90                       nop        
004d30e0 eb0d                     jmp        0x4d30ef
004d30e2 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004d30e5 c780d406000000000000     mov        dword ptr [eax + 0x6d4], 0
004d30ef 33c0                     xor        eax, eax
004d30f1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d30f4 33cd                     xor        ecx, ebp
004d30f6 e8f3f80600               call       0x5429ee
004d30fb 8be5                     mov        esp, ebp
004d30fd 5d                       pop        ebp
004d30fe c20400                   ret        4
004d3101 cc                       int3       
004d3102 cc                       int3       
004d3103 cc                       int3       
004d3104 cc                       int3       
004d3105 cc                       int3       
004d3106 cc                       int3       
004d3107 cc                       int3       
004d3108 cc                       int3       
004d3109 cc                       int3       
004d310a cc                       int3       
004d310b cc                       int3       
004d310c cc                       int3       
004d310d cc                       int3       
004d310e cc                       int3       
004d310f cc                       int3       
