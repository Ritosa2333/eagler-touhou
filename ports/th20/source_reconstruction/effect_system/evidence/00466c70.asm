00466c70 55                       push       ebp
00466c71 8bec                     mov        ebp, esp
00466c73 83ec50                   sub        esp, 0x50
00466c76 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00466c7b 33c5                     xor        eax, ebp
00466c7d 8945fc                   mov        dword ptr [ebp - 4], eax
00466c80 894df0                   mov        dword ptr [ebp - 0x10], ecx
00466c83 6a01                     push       1
00466c85 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00466c88 81c1082d0000             add        ecx, 0x2d08
00466c8e e88dc8fbff               call       0x423520
00466c93 90                       nop        
00466c94 8b4508                   mov        eax, dword ptr [ebp + 8]
00466c97 51                       push       ecx
00466c98 f30f104018               movss      xmm0, dword ptr [eax + 0x18]
00466c9d f30f110424               movss      dword ptr [esp], xmm0
00466ca2 8d4df8                   lea        ecx, [ebp - 8]
00466ca5 e86625fcff               call       0x429210
00466caa 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00466cad 51                       push       ecx
00466cae f30f10411c               movss      xmm0, dword ptr [ecx + 0x1c]
00466cb3 f30f110424               movss      dword ptr [esp], xmm0
00466cb8 8d4df4                   lea        ecx, [ebp - 0xc]
00466cbb e85025fcff               call       0x429210
00466cc0 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00466cc3 83c208                   add        edx, 8
00466cc6 8955e8                   mov        dword ptr [ebp - 0x18], edx
00466cc9 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00466ccc e8cf50faff               call       0x40bda0
00466cd1 8945ec                   mov        dword ptr [ebp - 0x14], eax
00466cd4 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00466cd7 e8f4020000               call       0x466fd0
00466cdc 8945e0                   mov        dword ptr [ebp - 0x20], eax
00466cdf eb0b                     jmp        0x466cec
00466ce1 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00466ce4 0568010000               add        eax, 0x168
00466ce9 8945ec                   mov        dword ptr [ebp - 0x14], eax
00466cec 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00466cef 3b4de0                   cmp        ecx, dword ptr [ebp - 0x20]
00466cf2 0f84e0000000             je         0x466dd8
00466cf8 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00466cfb 8955c4                   mov        dword ptr [ebp - 0x3c], edx
00466cfe 8b4508                   mov        eax, dword ptr [ebp + 8]
00466d01 8b4820                   mov        ecx, dword ptr [eax + 0x20]
00466d04 894ddc                   mov        dword ptr [ebp - 0x24], ecx
00466d07 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
00466d0a 8955d8                   mov        dword ptr [ebp - 0x28], edx
00466d0d 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
00466d10 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00466d13 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00466d16 f30f104124               movss      xmm0, dword ptr [ecx + 0x24]
00466d1b f30f1145d0               movss      dword ptr [ebp - 0x30], xmm0
00466d20 8d4df4                   lea        ecx, [ebp - 0xc]
00466d23 e8b825fcff               call       0x4292e0
00466d28 d95dcc                   fstp       dword ptr [ebp - 0x34]
00466d2b 8d4df8                   lea        ecx, [ebp - 8]
00466d2e e8ad25fcff               call       0x4292e0
00466d33 d95dc8                   fstp       dword ptr [ebp - 0x38]
00466d36 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
00466d39 52                       push       edx
00466d3a 51                       push       ecx
00466d3b f30f1045d0               movss      xmm0, dword ptr [ebp - 0x30]
00466d40 f30f110424               movss      dword ptr [esp], xmm0
00466d45 51                       push       ecx
00466d46 f30f1045cc               movss      xmm0, dword ptr [ebp - 0x34]
00466d4b f30f110424               movss      dword ptr [esp], xmm0
00466d50 51                       push       ecx
00466d51 f30f1045c8               movss      xmm0, dword ptr [ebp - 0x38]
00466d56 f30f110424               movss      dword ptr [esp], xmm0
00466d5b 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00466d5e e8bd000000               call       0x466e20
00466d63 90                       nop        
00466d64 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00466d6c f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
00466d74 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00466d77 83c108                   add        ecx, 8
00466d7a f30f1145c0               movss      dword ptr [ebp - 0x40], xmm0
00466d7f e86c020000               call       0x466ff0
00466d84 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00466d87 f20f2a45e4               cvtsi2sd   xmm0, dword ptr [ebp - 0x1c]
00466d8c 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
00466d8f c1e81f                   shr        eax, 0x1f
00466d92 f20f5804c5d0cd5600       addsd      xmm0, qword ptr [eax*8 + 0x56cdd0]
00466d9b f20f1145b0               movsd      qword ptr [ebp - 0x50], xmm0
00466da0 f20f5a45b0               cvtsd2ss   xmm0, qword ptr [ebp - 0x50]
00466da5 f30f104dc0               movss      xmm1, dword ptr [ebp - 0x40]
00466daa f30f5ec8                 divss      xmm1, xmm0
00466dae 51                       push       ecx
00466daf f30f110c24               movss      dword ptr [esp], xmm1
00466db4 8d4df8                   lea        ecx, [ebp - 8]
00466db7 e804c3feff               call       0x4530c0
00466dbc 51                       push       ecx
00466dbd f30f1005f4e05600         movss      xmm0, dword ptr [0x56e0f4]
00466dc5 f30f110424               movss      dword ptr [esp], xmm0
00466dca 8d4df4                   lea        ecx, [ebp - 0xc]
00466dcd e84efcffff               call       0x466a20
00466dd2 90                       nop        
00466dd3 e909ffffff               jmp        0x466ce1
00466dd8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00466ddb 8b5104                   mov        edx, dword ptr [ecx + 4]
00466dde 8955bc                   mov        dword ptr [ebp - 0x44], edx
00466de1 8b4508                   mov        eax, dword ptr [ebp + 8]
00466de4 50                       push       eax
00466de5 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
00466de8 e8736fffff               call       0x45dd60
00466ded 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00466df0 8b5104                   mov        edx, dword ptr [ecx + 4]
00466df3 8955b8                   mov        dword ptr [ebp - 0x48], edx
00466df6 6a05                     push       5
00466df8 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00466dfb e85095feff               call       0x450350
00466e00 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00466e03 8b4804                   mov        ecx, dword ptr [eax + 4]
00466e06 e88577ffff               call       0x45e590
00466e0b 33c0                     xor        eax, eax
00466e0d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00466e10 33cd                     xor        ecx, ebp
00466e12 e8d7bb0d00               call       0x5429ee
00466e17 8be5                     mov        esp, ebp
00466e19 5d                       pop        ebp
00466e1a c20800                   ret        8
00466e1d cc                       int3       
00466e1e cc                       int3       
00466e1f cc                       int3       
