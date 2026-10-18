00529b40 55                       push       ebp
00529b41 8bec                     mov        ebp, esp
00529b43 83ec48                   sub        esp, 0x48
00529b46 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00529b4b 33c5                     xor        eax, ebp
00529b4d 8945fc                   mov        dword ptr [ebp - 4], eax
00529b50 894de8                   mov        dword ptr [ebp - 0x18], ecx
00529b53 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00529b56 8b4820                   mov        ecx, dword ptr [eax + 0x20]
00529b59 894de4                   mov        dword ptr [ebp - 0x1c], ecx
00529b5c 837de402                 cmp        dword ptr [ebp - 0x1c], 2
00529b60 740b                     je         0x529b6d
00529b62 837de403                 cmp        dword ptr [ebp - 0x1c], 3
00529b66 7405                     je         0x529b6d
00529b68 e9f9020000               jmp        0x529e66
00529b6d 51                       push       ecx
00529b6e 0f57c0                   xorps      xmm0, xmm0
00529b71 f30f110424               movss      dword ptr [esp], xmm0
00529b76 51                       push       ecx
00529b77 f30f100530fa5600         movss      xmm0, dword ptr [0x56fa30]
00529b7f f30f110424               movss      dword ptr [esp], xmm0
00529b84 51                       push       ecx
00529b85 f30f100530fa5600         movss      xmm0, dword ptr [0x56fa30]
00529b8d f30f110424               movss      dword ptr [esp], xmm0
00529b92 8d4df0                   lea        ecx, [ebp - 0x10]
00529b95 e83692efff               call       0x422dd0
00529b9a 6a07                     push       7
00529b9c 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00529ba2 e859eff5ff               call       0x488b00
00529ba7 90                       nop        
00529ba8 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
00529bab 81c254010000             add        edx, 0x154
00529bb1 8955e0                   mov        dword ptr [ebp - 0x20], edx
00529bb4 6a0a                     push       0xa
00529bb6 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00529bb9 e8029aefff               call       0x4235c0
00529bbe 0fb6c0                   movzx      eax, al
00529bc1 85c0                     test       eax, eax
00529bc3 750d                     jne        0x529bd2
00529bc5 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00529bc8 83792003                 cmp        dword ptr [ecx + 0x20], 3
00529bcc 0f856c020000             jne        0x529e3e
00529bd2 f30f100584565700         movss      xmm0, dword ptr [0x575684]
00529bda f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00529bdf f30f100568565700         movss      xmm0, dword ptr [0x575668]
00529be7 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00529bec c745ec01000000           mov        dword ptr [ebp - 0x14], 1
00529bf3 eb09                     jmp        0x529bfe
00529bf5 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00529bf8 83c201                   add        edx, 1
00529bfb 8955ec                   mov        dword ptr [ebp - 0x14], edx
00529bfe 837dec07                 cmp        dword ptr [ebp - 0x14], 7
00529c02 0f8d36020000             jge        0x529e3e
00529c08 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00529c0b 83c024                   add        eax, 0x24
00529c0e 8945dc                   mov        dword ptr [ebp - 0x24], eax
00529c11 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00529c14 83e901                   sub        ecx, 1
00529c17 51                       push       ecx
00529c18 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00529c1b e89015f4ff               call       0x46b1b0
00529c20 85c0                     test       eax, eax
00529c22 0f849f000000             je         0x529cc7
00529c28 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00529c2e e81d60feff               call       0x50fc50
00529c33 05f8760000               add        eax, 0x76f8
00529c38 8945d8                   mov        dword ptr [ebp - 0x28], eax
00529c3b b968a55b00               mov        ecx, 0x5ba568
00529c40 e85ba4f3ff               call       0x4640a0
00529c45 69d090000000             imul       edx, eax, 0x90
00529c4b 8955d4                   mov        dword ptr [ebp - 0x2c], edx
00529c4e 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00529c51 034dd4                   add        ecx, dword ptr [ebp - 0x2c]
00529c54 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00529c57 83e801                   sub        eax, 1
00529c5a c1e004                   shl        eax, 4
00529c5d 03c8                     add        ecx, eax
00529c5f e8cc2d0000               call       0x52ca30
00529c64 0fb6c8                   movzx      ecx, al
00529c67 85c9                     test       ecx, ecx
00529c69 7449                     je         0x529cb4
00529c6b 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
00529c6e 837a2003                 cmp        dword ptr [edx + 0x20], 3
00529c72 751a                     jne        0x529c8e
00529c74 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00529c77 0554010000               add        eax, 0x154
00529c7c 8945d0                   mov        dword ptr [ebp - 0x30], eax
00529c7f 6a04                     push       4
00529c81 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00529c84 e8b783f4ff               call       0x472040
00529c89 83f802                   cmp        eax, 2
00529c8c 7d13                     jge        0x529ca1
00529c8e 6800ffffff               push       0xffffff00
00529c93 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00529c99 e8326ef4ff               call       0x470ad0
00529c9e 90                       nop        
00529c9f eb11                     jmp        0x529cb2
00529ca1 68000000ff               push       0xff000000
00529ca6 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00529cac e81f6ef4ff               call       0x470ad0
00529cb1 90                       nop        
00529cb2 eb11                     jmp        0x529cc5
00529cb4 68dfdfdfff               push       0xffdfdfdf
00529cb9 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00529cbf e80c6ef4ff               call       0x470ad0
00529cc4 90                       nop        
00529cc5 eb11                     jmp        0x529cd8
00529cc7 68808080ff               push       0xff808080
00529ccc 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00529cd2 e8f96df4ff               call       0x470ad0
00529cd7 90                       nop        
00529cd8 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00529cde e86d5ffeff               call       0x50fc50
00529ce3 05f8760000               add        eax, 0x76f8
00529ce8 8945cc                   mov        dword ptr [ebp - 0x34], eax
00529ceb b968a55b00               mov        ecx, 0x5ba568
00529cf0 e8aba3f3ff               call       0x4640a0
00529cf5 69c890000000             imul       ecx, eax, 0x90
00529cfb 894dc8                   mov        dword ptr [ebp - 0x38], ecx
00529cfe 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00529d01 034dc8                   add        ecx, dword ptr [ebp - 0x38]
00529d04 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00529d07 83ea01                   sub        edx, 1
00529d0a c1e204                   shl        edx, 4
00529d0d 03ca                     add        ecx, edx
00529d0f e81c2d0000               call       0x52ca30
00529d14 0fb6c0                   movzx      eax, al
00529d17 85c0                     test       eax, eax
00529d19 7563                     jne        0x529d7e
00529d1b 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00529d1e 8b148da8ff5a00           mov        edx, dword ptr [ecx*4 + 0x5affa8]
00529d25 52                       push       edx
00529d26 68e4e05600               push       0x56e0e4
00529d2b 8d45f0                   lea        eax, [ebp - 0x10]
00529d2e 50                       push       eax
00529d2f 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00529d35 51                       push       ecx
00529d36 e8552cf4ff               call       0x46c990
00529d3b 83c410                   add        esp, 0x10
00529d3e f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00529d43 f30f5805d40a5700         addss      xmm0, dword ptr [0x570ad4]
00529d4b f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00529d50 6824515700               push       0x575124
00529d55 8d55f0                   lea        edx, [ebp - 0x10]
00529d58 52                       push       edx
00529d59 a198065c00               mov        eax, dword ptr [0x5c0698]
00529d5e 50                       push       eax
00529d5f e82c2cf4ff               call       0x46c990
00529d64 83c40c                   add        esp, 0xc
00529d67 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00529d6c f30f5c05d40a5700         subss      xmm0, dword ptr [0x570ad4]
00529d74 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00529d79 e9a9000000               jmp        0x529e27
00529d7e 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00529d81 8b148da8ff5a00           mov        edx, dword ptr [ecx*4 + 0x5affa8]
00529d88 52                       push       edx
00529d89 68e4e05600               push       0x56e0e4
00529d8e 8d45f0                   lea        eax, [ebp - 0x10]
00529d91 50                       push       eax
00529d92 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00529d98 51                       push       ecx
00529d99 e8f22bf4ff               call       0x46c990
00529d9e 83c410                   add        esp, 0x10
00529da1 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00529da6 f30f5805d40a5700         addss      xmm0, dword ptr [0x570ad4]
00529dae f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00529db3 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00529db9 e8925efeff               call       0x50fc50
00529dbe 05f8760000               add        eax, 0x76f8
00529dc3 8945c4                   mov        dword ptr [ebp - 0x3c], eax
00529dc6 b968a55b00               mov        ecx, 0x5ba568
00529dcb e8d0a2f3ff               call       0x4640a0
00529dd0 69d090000000             imul       edx, eax, 0x90
00529dd6 8955c0                   mov        dword ptr [ebp - 0x40], edx
00529dd9 8b45c4                   mov        eax, dword ptr [ebp - 0x3c]
00529ddc 0345c0                   add        eax, dword ptr [ebp - 0x40]
00529ddf 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00529de2 83e901                   sub        ecx, 1
00529de5 c1e104                   shl        ecx, 4
00529de8 8b1408                   mov        edx, dword ptr [eax + ecx]
00529deb 8b440804                 mov        eax, dword ptr [eax + ecx + 4]
00529def 8955b8                   mov        dword ptr [ebp - 0x48], edx
00529df2 8945bc                   mov        dword ptr [ebp - 0x44], eax
00529df5 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
00529df8 51                       push       ecx
00529df9 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
00529dfc 52                       push       edx
00529dfd 6830515700               push       0x575130
00529e02 8d45f0                   lea        eax, [ebp - 0x10]
00529e05 50                       push       eax
00529e06 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00529e0c 51                       push       ecx
00529e0d e87e2bf4ff               call       0x46c990
00529e12 83c414                   add        esp, 0x14
00529e15 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00529e1a f30f5c05d40a5700         subss      xmm0, dword ptr [0x570ad4]
00529e22 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00529e27 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00529e2c f30f580554565700         addss      xmm0, dword ptr [0x575654]
00529e34 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00529e39 e9b7fdffff               jmp        0x529bf5
00529e3e 6aff                     push       -1
00529e40 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00529e46 e8856cf4ff               call       0x470ad0
00529e4b 6a00                     push       0
00529e4d 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00529e53 e8c8cbfbff               call       0x4e6a20
00529e58 6a00                     push       0
00529e5a 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00529e60 e89becf5ff               call       0x488b00
00529e65 90                       nop        
00529e66 b801000000               mov        eax, 1
00529e6b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529e6e 33cd                     xor        ecx, ebp
00529e70 e8798b0100               call       0x5429ee
00529e75 8be5                     mov        esp, ebp
00529e77 5d                       pop        ebp
00529e78 c3                       ret        
00529e79 cc                       int3       
00529e7a cc                       int3       
00529e7b cc                       int3       
00529e7c cc                       int3       
00529e7d cc                       int3       
00529e7e cc                       int3       
00529e7f cc                       int3       
