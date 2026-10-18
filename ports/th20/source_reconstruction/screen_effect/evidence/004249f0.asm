004249f0 55                       push       ebp
004249f1 8bec                     mov        ebp, esp
004249f3 83ec28                   sub        esp, 0x28
004249f6 833d18a55b0000           cmp        dword ptr [0x5ba518], 0
004249fd 740a                     je         0x424a09
004249ff b807000000               mov        eax, 7
00424a04 e9f3030000               jmp        0x424dfc
00424a09 8b4508                   mov        eax, dword ptr [ebp + 8]
00424a0c 83c030                   add        eax, 0x30
00424a0f 8945e0                   mov        dword ptr [ebp - 0x20], eax
00424a12 6a00                     push       0
00424a14 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00424a17 e824ebffff               call       0x423540
00424a1c 90                       nop        
00424a1d 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424a20 83c130                   add        ecx, 0x30
00424a23 894ddc                   mov        dword ptr [ebp - 0x24], ecx
00424a26 8b5508                   mov        edx, dword ptr [ebp + 8]
00424a29 8b421c                   mov        eax, dword ptr [edx + 0x1c]
00424a2c 50                       push       eax
00424a2d 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00424a30 e88bebffff               call       0x4235c0
00424a35 0fb6c8                   movzx      ecx, al
00424a38 85c9                     test       ecx, ecx
00424a3a 740a                     je         0x424a46
00424a3c b807000000               mov        eax, 7
00424a41 e9b6030000               jmp        0x424dfc
00424a46 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424a49 83c130                   add        ecx, 0x30
00424a4c e87ff1ffff               call       0x423bd0
00424a51 d95dd8                   fstp       dword ptr [ebp - 0x28]
00424a54 f30f1045d8               movss      xmm0, dword ptr [ebp - 0x28]
00424a59 8b5508                   mov        edx, dword ptr [ebp + 8]
00424a5c 8b4508                   mov        eax, dword ptr [ebp + 8]
00424a5f 8b4a24                   mov        ecx, dword ptr [edx + 0x24]
00424a62 2b4820                   sub        ecx, dword ptr [eax + 0x20]
00424a65 f30f2ac9                 cvtsi2ss   xmm1, ecx
00424a69 f30f59c1                 mulss      xmm0, xmm1
00424a6d f30f1145fc               movss      dword ptr [ebp - 4], xmm0
00424a72 8b5508                   mov        edx, dword ptr [ebp + 8]
00424a75 f30f2a421c               cvtsi2ss   xmm0, dword ptr [edx + 0x1c]
00424a7a f30f104dfc               movss      xmm1, dword ptr [ebp - 4]
00424a7f f30f5ec8                 divss      xmm1, xmm0
00424a83 f30f114dfc               movss      dword ptr [ebp - 4], xmm1
00424a88 8b4508                   mov        eax, dword ptr [ebp + 8]
00424a8b f30f2a4020               cvtsi2ss   xmm0, dword ptr [eax + 0x20]
00424a90 f30f5845fc               addss      xmm0, dword ptr [ebp - 4]
00424a95 f30f1145fc               movss      dword ptr [ebp - 4], xmm0
00424a9a 6a03                     push       3
00424a9c b9c4a45b00               mov        ecx, 0x5ba4c4
00424aa1 e8faf3ffff               call       0x423ea0
00424aa6 8945f8                   mov        dword ptr [ebp - 8], eax
00424aa9 837df800                 cmp        dword ptr [ebp - 8], 0
00424aad 7410                     je         0x424abf
00424aaf 837df801                 cmp        dword ptr [ebp - 8], 1
00424ab3 7479                     je         0x424b2e
00424ab5 837df802                 cmp        dword ptr [ebp - 8], 2
00424ab9 0f84f8000000             je         0x424bb7
00424abf b96c010000               mov        ecx, 0x16c
00424ac4 6bd103                   imul       edx, ecx, 3
00424ac7 b808000000               mov        eax, 8
00424acc 6bc800                   imul       ecx, eax, 0
00424acf 0f57c0                   xorps      xmm0, xmm0
00424ad2 f30f11840ae4505c00       movss      dword ptr [edx + ecx + 0x5c50e4], xmm0
00424adb ba6c010000               mov        edx, 0x16c
00424ae0 c1e200                   shl        edx, 0
00424ae3 8b4508                   mov        eax, dword ptr [ebp + 8]
00424ae6 8b4840                   mov        ecx, dword ptr [eax + 0x40]
00424ae9 0f57c0                   xorps      xmm0, xmm0
00424aec f30f1184cae4505c00       movss      dword ptr [edx + ecx*8 + 0x5c50e4], xmm0
00424af5 ba6c010000               mov        edx, 0x16c
00424afa 6bc200                   imul       eax, edx, 0
00424afd 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424b00 8b5140                   mov        edx, dword ptr [ecx + 0x40]
00424b03 0f57c0                   xorps      xmm0, xmm0
00424b06 f30f1184d0e4505c00       movss      dword ptr [eax + edx*8 + 0x5c50e4], xmm0
00424b0f b86c010000               mov        eax, 0x16c
00424b14 6bc805                   imul       ecx, eax, 5
00424b17 8b5508                   mov        edx, dword ptr [ebp + 8]
00424b1a 8b4240                   mov        eax, dword ptr [edx + 0x40]
00424b1d 0f57c0                   xorps      xmm0, xmm0
00424b20 f30f1184c1e4505c00       movss      dword ptr [ecx + eax*8 + 0x5c50e4], xmm0
00424b29 e91b010000               jmp        0x424c49
00424b2e b96c010000               mov        ecx, 0x16c
00424b33 6bd103                   imul       edx, ecx, 3
00424b36 b808000000               mov        eax, 8
00424b3b 6bc800                   imul       ecx, eax, 0
00424b3e f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
00424b43 f30f11840ae4505c00       movss      dword ptr [edx + ecx + 0x5c50e4], xmm0
00424b4c f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
00424b51 f30f590518885b00         mulss      xmm0, dword ptr [0x5b8818]
00424b59 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00424b5e ba6c010000               mov        edx, 0x16c
00424b63 c1e200                   shl        edx, 0
00424b66 8b4508                   mov        eax, dword ptr [ebp + 8]
00424b69 8b4840                   mov        ecx, dword ptr [eax + 0x40]
00424b6c f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00424b71 f30f1184cae4505c00       movss      dword ptr [edx + ecx*8 + 0x5c50e4], xmm0
00424b7a ba6c010000               mov        edx, 0x16c
00424b7f 6bc200                   imul       eax, edx, 0
00424b82 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424b85 8b5140                   mov        edx, dword ptr [ecx + 0x40]
00424b88 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00424b8d f30f1184d0e4505c00       movss      dword ptr [eax + edx*8 + 0x5c50e4], xmm0
00424b96 b86c010000               mov        eax, 0x16c
00424b9b 6bc805                   imul       ecx, eax, 5
00424b9e 8b5508                   mov        edx, dword ptr [ebp + 8]
00424ba1 8b4240                   mov        eax, dword ptr [edx + 0x40]
00424ba4 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00424ba9 f30f1184c1e4505c00       movss      dword ptr [ecx + eax*8 + 0x5c50e4], xmm0
00424bb2 e992000000               jmp        0x424c49
00424bb7 f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
00424bbc 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00424bc3 b96c010000               mov        ecx, 0x16c
00424bc8 6bd103                   imul       edx, ecx, 3
00424bcb b808000000               mov        eax, 8
00424bd0 6bc800                   imul       ecx, eax, 0
00424bd3 f30f11840ae4505c00       movss      dword ptr [edx + ecx + 0x5c50e4], xmm0
00424bdc f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
00424be1 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00424be8 f30f590518885b00         mulss      xmm0, dword ptr [0x5b8818]
00424bf0 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00424bf5 ba6c010000               mov        edx, 0x16c
00424bfa c1e200                   shl        edx, 0
00424bfd 8b4508                   mov        eax, dword ptr [ebp + 8]
00424c00 8b4840                   mov        ecx, dword ptr [eax + 0x40]
00424c03 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00424c08 f30f1184cae4505c00       movss      dword ptr [edx + ecx*8 + 0x5c50e4], xmm0
00424c11 ba6c010000               mov        edx, 0x16c
00424c16 6bc200                   imul       eax, edx, 0
00424c19 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424c1c 8b5140                   mov        edx, dword ptr [ecx + 0x40]
00424c1f f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00424c24 f30f1184d0e4505c00       movss      dword ptr [eax + edx*8 + 0x5c50e4], xmm0
00424c2d b86c010000               mov        eax, 0x16c
00424c32 6bc805                   imul       ecx, eax, 5
00424c35 8b5508                   mov        edx, dword ptr [ebp + 8]
00424c38 8b4240                   mov        eax, dword ptr [edx + 0x40]
00424c3b f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00424c40 f30f1184c1e4505c00       movss      dword ptr [ecx + eax*8 + 0x5c50e4], xmm0
00424c49 6a03                     push       3
00424c4b b9c4a45b00               mov        ecx, 0x5ba4c4
00424c50 e84bf2ffff               call       0x423ea0
00424c55 8945ec                   mov        dword ptr [ebp - 0x14], eax
00424c58 837dec00                 cmp        dword ptr [ebp - 0x14], 0
00424c5c 7410                     je         0x424c6e
00424c5e 837dec01                 cmp        dword ptr [ebp - 0x14], 1
00424c62 7478                     je         0x424cdc
00424c64 837dec02                 cmp        dword ptr [ebp - 0x14], 2
00424c68 0f84f7000000             je         0x424d65
00424c6e b96c010000               mov        ecx, 0x16c
00424c73 c1e100                   shl        ecx, 0
00424c76 8b5508                   mov        edx, dword ptr [ebp + 8]
00424c79 8b4240                   mov        eax, dword ptr [edx + 0x40]
00424c7c 0f57c0                   xorps      xmm0, xmm0
00424c7f f30f1184c1e8505c00       movss      dword ptr [ecx + eax*8 + 0x5c50e8], xmm0
00424c88 b96c010000               mov        ecx, 0x16c
00424c8d 6bd103                   imul       edx, ecx, 3
00424c90 b808000000               mov        eax, 8
00424c95 d1e0                     shl        eax, 1
00424c97 0f57c0                   xorps      xmm0, xmm0
00424c9a f30f118402e8505c00       movss      dword ptr [edx + eax + 0x5c50e8], xmm0
00424ca3 b96c010000               mov        ecx, 0x16c
00424ca8 6bd100                   imul       edx, ecx, 0
00424cab 8b4508                   mov        eax, dword ptr [ebp + 8]
00424cae 8b4840                   mov        ecx, dword ptr [eax + 0x40]
00424cb1 0f57c0                   xorps      xmm0, xmm0
00424cb4 f30f1184cae8505c00       movss      dword ptr [edx + ecx*8 + 0x5c50e8], xmm0
00424cbd ba6c010000               mov        edx, 0x16c
00424cc2 6bc205                   imul       eax, edx, 5
00424cc5 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424cc8 8b5140                   mov        edx, dword ptr [ecx + 0x40]
00424ccb 0f57c0                   xorps      xmm0, xmm0
00424cce f30f1184d0e8505c00       movss      dword ptr [eax + edx*8 + 0x5c50e8], xmm0
00424cd7 e91b010000               jmp        0x424df7
00424cdc b86c010000               mov        eax, 0x16c
00424ce1 6bc803                   imul       ecx, eax, 3
00424ce4 ba08000000               mov        edx, 8
00424ce9 6bc200                   imul       eax, edx, 0
00424cec f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
00424cf1 f30f118401e8505c00       movss      dword ptr [ecx + eax + 0x5c50e8], xmm0
00424cfa f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
00424cff f30f590518885b00         mulss      xmm0, dword ptr [0x5b8818]
00424d07 f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
00424d0c b96c010000               mov        ecx, 0x16c
00424d11 c1e100                   shl        ecx, 0
00424d14 8b5508                   mov        edx, dword ptr [ebp + 8]
00424d17 8b4240                   mov        eax, dword ptr [edx + 0x40]
00424d1a f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
00424d1f f30f1184c1e8505c00       movss      dword ptr [ecx + eax*8 + 0x5c50e8], xmm0
00424d28 b96c010000               mov        ecx, 0x16c
00424d2d 6bd100                   imul       edx, ecx, 0
00424d30 8b4508                   mov        eax, dword ptr [ebp + 8]
00424d33 8b4840                   mov        ecx, dword ptr [eax + 0x40]
00424d36 f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
00424d3b f30f1184cae8505c00       movss      dword ptr [edx + ecx*8 + 0x5c50e8], xmm0
00424d44 ba6c010000               mov        edx, 0x16c
00424d49 6bc205                   imul       eax, edx, 5
00424d4c 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424d4f 8b5140                   mov        edx, dword ptr [ecx + 0x40]
00424d52 f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
00424d57 f30f1184d0e8505c00       movss      dword ptr [eax + edx*8 + 0x5c50e8], xmm0
00424d60 e992000000               jmp        0x424df7
00424d65 f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
00424d6a 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00424d71 b86c010000               mov        eax, 0x16c
00424d76 6bc803                   imul       ecx, eax, 3
00424d79 ba08000000               mov        edx, 8
00424d7e 6bc200                   imul       eax, edx, 0
00424d81 f30f118401e8505c00       movss      dword ptr [ecx + eax + 0x5c50e8], xmm0
00424d8a f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
00424d8f 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00424d96 f30f590518885b00         mulss      xmm0, dword ptr [0x5b8818]
00424d9e f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
00424da3 b96c010000               mov        ecx, 0x16c
00424da8 c1e100                   shl        ecx, 0
00424dab 8b5508                   mov        edx, dword ptr [ebp + 8]
00424dae 8b4240                   mov        eax, dword ptr [edx + 0x40]
00424db1 f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
00424db6 f30f1184c1e8505c00       movss      dword ptr [ecx + eax*8 + 0x5c50e8], xmm0
00424dbf b96c010000               mov        ecx, 0x16c
00424dc4 6bd100                   imul       edx, ecx, 0
00424dc7 8b4508                   mov        eax, dword ptr [ebp + 8]
00424dca 8b4840                   mov        ecx, dword ptr [eax + 0x40]
00424dcd f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
00424dd2 f30f1184cae8505c00       movss      dword ptr [edx + ecx*8 + 0x5c50e8], xmm0
00424ddb ba6c010000               mov        edx, 0x16c
00424de0 6bc205                   imul       eax, edx, 5
00424de3 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424de6 8b5140                   mov        edx, dword ptr [ecx + 0x40]
00424de9 f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
00424dee f30f1184d0e8505c00       movss      dword ptr [eax + edx*8 + 0x5c50e8], xmm0
00424df7 b801000000               mov        eax, 1
00424dfc 8be5                     mov        esp, ebp
00424dfe 5d                       pop        ebp
00424dff c3                       ret        
