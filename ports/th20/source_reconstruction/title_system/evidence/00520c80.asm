00520c80 55                       push       ebp
00520c81 8bec                     mov        ebp, esp
00520c83 83ec3c                   sub        esp, 0x3c
00520c86 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00520c8b 33c5                     xor        eax, ebp
00520c8d 8945fc                   mov        dword ptr [ebp - 4], eax
00520c90 894dec                   mov        dword ptr [ebp - 0x14], ecx
00520c93 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00520c96 8b4820                   mov        ecx, dword ptr [eax + 0x20]
00520c99 894ddc                   mov        dword ptr [ebp - 0x24], ecx
00520c9c 837ddc01                 cmp        dword ptr [ebp - 0x24], 1
00520ca0 740b                     je         0x520cad
00520ca2 837ddc02                 cmp        dword ptr [ebp - 0x24], 2
00520ca6 7405                     je         0x520cad
00520ca8 e9e9020000               jmp        0x520f96
00520cad 51                       push       ecx
00520cae 0f57c0                   xorps      xmm0, xmm0
00520cb1 f30f110424               movss      dword ptr [esp], xmm0
00520cb6 51                       push       ecx
00520cb7 f30f10057cfe5600         movss      xmm0, dword ptr [0x56fe7c]
00520cbf f30f110424               movss      dword ptr [esp], xmm0
00520cc4 51                       push       ecx
00520cc5 f30f100528fa5600         movss      xmm0, dword ptr [0x56fa28]
00520ccd f30f110424               movss      dword ptr [esp], xmm0
00520cd2 8d4df0                   lea        ecx, [ebp - 0x10]
00520cd5 e8f620f0ff               call       0x422dd0
00520cda 6aff                     push       -1
00520cdc 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00520ce2 e87954faff               call       0x4c6160
00520ce7 68000000ff               push       0xff000000
00520cec 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00520cf2 e8b9fdf4ff               call       0x470ab0
00520cf7 6a05                     push       5
00520cf9 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00520cff e87c54faff               call       0x4c6180
00520d04 90                       nop        
00520d05 c745e000000000           mov        dword ptr [ebp - 0x20], 0
00520d0c eb09                     jmp        0x520d17
00520d0e 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00520d11 83c201                   add        edx, 1
00520d14 8955e0                   mov        dword ptr [ebp - 0x20], edx
00520d17 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00520d1a 0554010000               add        eax, 0x154
00520d1f 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00520d22 6a0a                     push       0xa
00520d24 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00520d27 e84403f4ff               call       0x461070
00520d2c 0fb6c8                   movzx      ecx, al
00520d2f 85c9                     test       ecx, ecx
00520d31 7409                     je         0x520d3c
00520d33 c745d80a000000           mov        dword ptr [ebp - 0x28], 0xa
00520d3a eb11                     jmp        0x520d4d
00520d3c 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00520d3f 81c154010000             add        ecx, 0x154
00520d45 e846f2eeff               call       0x40ff90
00520d4a 8945d8                   mov        dword ptr [ebp - 0x28], eax
00520d4d 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00520d50 3b55d8                   cmp        edx, dword ptr [ebp - 0x28]
00520d53 0f8df1000000             jge        0x520e4a
00520d59 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00520d5c 8b88d0560000             mov        ecx, dword ptr [eax + 0x56d0]
00520d62 034de0                   add        ecx, dword ptr [ebp - 0x20]
00520d65 894de8                   mov        dword ptr [ebp - 0x18], ecx
00520d68 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00520d6b 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00520d6e 3b827c040000             cmp        eax, dword ptr [edx + 0x47c]
00520d74 7c05                     jl         0x520d7b
00520d76 e9cf000000               jmp        0x520e4a
00520d7b 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00520d7e 83c124                   add        ecx, 0x24
00520d81 894dd0                   mov        dword ptr [ebp - 0x30], ecx
00520d84 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
00520d87 52                       push       edx
00520d88 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00520d8b e820a4f4ff               call       0x46b1b0
00520d90 85c0                     test       eax, eax
00520d92 7413                     je         0x520da7
00520d94 6800ffffff               push       0xffffff00
00520d99 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00520d9f e82cfdf4ff               call       0x470ad0
00520da4 90                       nop        
00520da5 eb11                     jmp        0x520db8
00520da7 68b0b0b0ff               push       0xffb0b0b0
00520dac 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00520db2 e819fdf4ff               call       0x470ad0
00520db7 90                       nop        
00520db8 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00520dbe e81d33f4ff               call       0x4640e0
00520dc3 83c036                   add        eax, 0x36
00520dc6 8945cc                   mov        dword ptr [ebp - 0x34], eax
00520dc9 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00520dcc 50                       push       eax
00520dcd 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00520dd0 e83b0eefff               call       0x411c10
00520dd5 0fb608                   movzx      ecx, byte ptr [eax]
00520dd8 85c9                     test       ecx, ecx
00520dda 742e                     je         0x520e0a
00520ddc 83ec08                   sub        esp, 8
00520ddf 8bcc                     mov        ecx, esp
00520de1 6b55e842                 imul       edx, dword ptr [ebp - 0x18], 0x42
00520de5 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00520de8 8d9410900c0000           lea        edx, [eax + edx + 0xc90]
00520def 52                       push       edx
00520df0 e8dbcfeeff               call       0x40ddd0
00520df5 8d45f0                   lea        eax, [ebp - 0x10]
00520df8 50                       push       eax
00520df9 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00520dff 51                       push       ecx
00520e00 e80bb4f4ff               call       0x46c210
00520e05 83c410                   add        esp, 0x10
00520e08 eb29                     jmp        0x520e33
00520e0a 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
00520e0d 83c201                   add        edx, 1
00520e10 52                       push       edx
00520e11 83ec08                   sub        esp, 8
00520e14 8bcc                     mov        ecx, esp
00520e16 6808555700               push       0x575508
00520e1b e8b0cfeeff               call       0x40ddd0
00520e20 8d45f0                   lea        eax, [ebp - 0x10]
00520e23 50                       push       eax
00520e24 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00520e2a 51                       push       ecx
00520e2b e8e0b3f4ff               call       0x46c210
00520e30 83c414                   add        esp, 0x14
00520e33 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00520e38 f30f58058cec5600         addss      xmm0, dword ptr [0x56ec8c]
00520e40 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00520e45 e9c4feffff               jmp        0x520d0e
00520e4a 68000000ff               push       0xff000000
00520e4f 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00520e55 e856fcf4ff               call       0x470ab0
00520e5a 6aff                     push       -1
00520e5c 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00520e62 e869fcf4ff               call       0x470ad0
00520e67 f30f100590f05600         movss      xmm0, dword ptr [0x56f090]
00520e6f f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00520e74 f30f10050cf15600         movss      xmm0, dword ptr [0x56f10c]
00520e7c f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00520e81 6a04                     push       4
00520e83 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00520e89 e8f252faff               call       0x4c6180
00520e8e 90                       nop        
00520e8f c745e400000000           mov        dword ptr [ebp - 0x1c], 0
00520e96 eb09                     jmp        0x520ea1
00520e98 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
00520e9b 83c201                   add        edx, 1
00520e9e 8955e4                   mov        dword ptr [ebp - 0x1c], edx
00520ea1 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00520ea4 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00520ea7 3b8880040000             cmp        ecx, dword ptr [eax + 0x480]
00520ead 0f8db8000000             jge        0x520f6b
00520eb3 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00520eb9 e82232f4ff               call       0x4640e0
00520ebe 83c036                   add        eax, 0x36
00520ec1 8945c4                   mov        dword ptr [ebp - 0x3c], eax
00520ec4 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00520ec7 8b8284040000             mov        eax, dword ptr [edx + 0x484]
00520ecd 8945c8                   mov        dword ptr [ebp - 0x38], eax
00520ed0 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00520ed3 51                       push       ecx
00520ed4 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00520ed7 e8340defff               call       0x411c10
00520edc 0fb610                   movzx      edx, byte ptr [eax]
00520edf 85d2                     test       edx, edx
00520ee1 750c                     jne        0x520eef
00520ee3 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00520ee6 83b88c04000000           cmp        dword ptr [eax + 0x48c], 0
00520eed 753d                     jne        0x520f2c
00520eef 83ec08                   sub        esp, 8
00520ef2 8bcc                     mov        ecx, esp
00520ef4 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00520ef7 69828404000010020000     imul       eax, dword ptr [edx + 0x484], 0x210
00520f01 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00520f04 8d8402d0140000           lea        eax, [edx + eax + 0x14d0]
00520f0b 6b55e442                 imul       edx, dword ptr [ebp - 0x1c], 0x42
00520f0f 03c2                     add        eax, edx
00520f11 50                       push       eax
00520f12 e8b9ceeeff               call       0x40ddd0
00520f17 8d45f0                   lea        eax, [ebp - 0x10]
00520f1a 50                       push       eax
00520f1b 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00520f21 51                       push       ecx
00520f22 e8e9b2f4ff               call       0x46c210
00520f27 83c410                   add        esp, 0x10
00520f2a eb28                     jmp        0x520f54
00520f2c 83ec08                   sub        esp, 8
00520f2f 8bcc                     mov        ecx, esp
00520f31 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
00520f34 8b0495880a5b00           mov        eax, dword ptr [edx*4 + 0x5b0a88]
00520f3b 50                       push       eax
00520f3c e88fceeeff               call       0x40ddd0
00520f41 8d4df0                   lea        ecx, [ebp - 0x10]
00520f44 51                       push       ecx
00520f45 8b1598065c00             mov        edx, dword ptr [0x5c0698]
00520f4b 52                       push       edx
00520f4c e8bfb2f4ff               call       0x46c210
00520f51 83c410                   add        esp, 0x10
00520f54 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00520f59 f30f580544265700         addss      xmm0, dword ptr [0x572644]
00520f61 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00520f66 e92dffffff               jmp        0x520e98
00520f6b 6aff                     push       -1
00520f6d 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00520f73 e8e851faff               call       0x4c6160
00520f78 68000000ff               push       0xff000000
00520f7d 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00520f83 e828fbf4ff               call       0x470ab0
00520f88 6a02                     push       2
00520f8a 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00520f90 e8eb51faff               call       0x4c6180
00520f95 90                       nop        
00520f96 33c0                     xor        eax, eax
00520f98 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00520f9b 33cd                     xor        ecx, ebp
00520f9d e84c1a0200               call       0x5429ee
00520fa2 8be5                     mov        esp, ebp
00520fa4 5d                       pop        ebp
00520fa5 c3                       ret        
00520fa6 cc                       int3       
00520fa7 cc                       int3       
00520fa8 cc                       int3       
00520fa9 cc                       int3       
00520faa cc                       int3       
00520fab cc                       int3       
00520fac cc                       int3       
00520fad cc                       int3       
00520fae cc                       int3       
00520faf cc                       int3       
