00534ab0 55                       push       ebp
00534ab1 8bec                     mov        ebp, esp
00534ab3 83ec78                   sub        esp, 0x78
00534ab6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00534abb 33c5                     xor        eax, ebp
00534abd 8945fc                   mov        dword ptr [ebp - 4], eax
00534ac0 894dac                   mov        dword ptr [ebp - 0x54], ecx
00534ac3 6a00                     push       0
00534ac5 e8b6f5f2ff               call       0x464080
00534aca 83c404                   add        esp, 4
00534acd 8bc8                     mov        ecx, eax
00534acf e8dc7feeff               call       0x41cab0
00534ad4 83f807                   cmp        eax, 7
00534ad7 7416                     je         0x534aef
00534ad9 6a00                     push       0
00534adb e850bdf2ff               call       0x460830
00534ae0 83c404                   add        esp, 4
00534ae3 8bc8                     mov        ecx, eax
00534ae5 e866bdf2ff               call       0x460850
00534aea 8945a0                   mov        dword ptr [ebp - 0x60], eax
00534aed eb16                     jmp        0x534b05
00534aef 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
00534af2 8b4828                   mov        ecx, dword ptr [eax + 0x28]
00534af5 8b55ac                   mov        edx, dword ptr [ebp - 0x54]
00534af8 8b01                     mov        eax, dword ptr [ecx]
00534afa 8b4a28                   mov        ecx, dword ptr [edx + 0x28]
00534afd 8b5060                   mov        edx, dword ptr [eax + 0x60]
00534b00 ffd2                     call       edx
00534b02 8945a0                   mov        dword ptr [ebp - 0x60], eax
00534b05 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
00534b08 89459c                   mov        dword ptr [ebp - 0x64], eax
00534b0b 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
00534b0e 8b11                     mov        edx, dword ptr [ecx]
00534b10 8955ec                   mov        dword ptr [ebp - 0x14], edx
00534b13 8b4104                   mov        eax, dword ptr [ecx + 4]
00534b16 8945f0                   mov        dword ptr [ebp - 0x10], eax
00534b19 8b4908                   mov        ecx, dword ptr [ecx + 8]
00534b1c 894df4                   mov        dword ptr [ebp - 0xc], ecx
00534b1f 8b55ac                   mov        edx, dword ptr [ebp - 0x54]
00534b22 8b4240                   mov        eax, dword ptr [edx + 0x40]
00534b25 50                       push       eax
00534b26 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00534b2c e8cfdcf6ff               call       0x4a2800
00534b31 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
00534b34 c7414000000000           mov        dword ptr [ecx + 0x40], 0
00534b3b 8b55ac                   mov        edx, dword ptr [ebp - 0x54]
00534b3e 8b427c                   mov        eax, dword ptr [edx + 0x7c]
00534b41 50                       push       eax
00534b42 6a00                     push       0
00534b44 6a0c                     push       0xc
00534b46 6a0c                     push       0xc
00534b48 6818635700               push       0x576318
00534b4d 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00534b53 e878c7f3ff               call       0x4712d0
00534b58 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
00534b5b 894140                   mov        dword ptr [ecx + 0x40], eax
00534b5e 6a00                     push       0
00534b60 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
00534b63 83c144                   add        ecx, 0x44
00534b66 e8b5e9eeff               call       0x423520
00534b6b 8b55ac                   mov        edx, dword ptr [ebp - 0x54]
00534b6e 8b4240                   mov        eax, dword ptr [edx + 0x40]
00534b71 894598                   mov        dword ptr [ebp - 0x68], eax
00534b74 6820275300               push       0x532720
00534b79 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00534b7c e8cf81ffff               call       0x52cd50
00534b81 90                       nop        
00534b82 8d4db4                   lea        ecx, [ebp - 0x4c]
00534b85 e8a66ef4ff               call       0x47ba30
00534b8a 51                       push       ecx
00534b8b 0f57c0                   xorps      xmm0, xmm0
00534b8e f30f110424               movss      dword ptr [esp], xmm0
00534b93 8d4df8                   lea        ecx, [ebp - 8]
00534b96 e87546efff               call       0x429210
00534b9b 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00534b9e 894db4                   mov        dword ptr [ebp - 0x4c], ecx
00534ba1 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00534ba4 8955b8                   mov        dword ptr [ebp - 0x48], edx
00534ba7 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00534baa 8945bc                   mov        dword ptr [ebp - 0x44], eax
00534bad 6a00                     push       0
00534baf e8ccf4f2ff               call       0x464080
00534bb4 83c404                   add        esp, 4
00534bb7 8bc8                     mov        ecx, eax
00534bb9 e8f27eeeff               call       0x41cab0
00534bbe 83f807                   cmp        eax, 7
00534bc1 7406                     je         0x534bc9
00534bc3 c645b301                 mov        byte ptr [ebp - 0x4d], 1
00534bc7 eb04                     jmp        0x534bcd
00534bc9 c645b300                 mov        byte ptr [ebp - 0x4d], 0
00534bcd 8a4db3                   mov        cl, byte ptr [ebp - 0x4d]
00534bd0 884ddc                   mov        byte ptr [ebp - 0x24], cl
00534bd3 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00534bdb f30f5e058cec5600         divss      xmm0, dword ptr [0x56ec8c]
00534be3 f30f1145d0               movss      dword ptr [ebp - 0x30], xmm0
00534be8 6880c0ffff               push       0xffffc080
00534bed 8d4dd4                   lea        ecx, [ebp - 0x2c]
00534bf0 e85b8af2ff               call       0x45d650
00534bf5 90                       nop        
00534bf6 c745a800000000           mov        dword ptr [ebp - 0x58], 0
00534bfd eb09                     jmp        0x534c08
00534bff 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
00534c02 83c201                   add        edx, 1
00534c05 8955a8                   mov        dword ptr [ebp - 0x58], edx
00534c08 837da818                 cmp        dword ptr [ebp - 0x58], 0x18
00534c0c 7d56                     jge        0x534c64
00534c0e 8d4df8                   lea        ecx, [ebp - 8]
00534c11 e8ca46efff               call       0x4292e0
00534c16 d95dcc                   fstp       dword ptr [ebp - 0x34]
00534c19 6a00                     push       0
00534c1b e80029f0ff               call       0x437520
00534c20 83c404                   add        esp, 4
00534c23 894594                   mov        dword ptr [ebp - 0x6c], eax
00534c26 6a00                     push       0
00534c28 8d45b4                   lea        eax, [ebp - 0x4c]
00534c2b 50                       push       eax
00534c2c 6a06                     push       6
00534c2e 8d4d8c                   lea        ecx, [ebp - 0x74]
00534c31 51                       push       ecx
00534c32 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00534c35 e8368ff6ff               call       0x49db70
00534c3a 90                       nop        
00534c3b f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00534c43 f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
00534c4b f30f5e05b4f75600         divss      xmm0, dword ptr [0x56f7b4]
00534c53 51                       push       ecx
00534c54 f30f110424               movss      dword ptr [esp], xmm0
00534c59 8d4df8                   lea        ecx, [ebp - 8]
00534c5c e85fe4f1ff               call       0x4530c0
00534c61 90                       nop        
00534c62 eb9b                     jmp        0x534bff
00534c64 f30f1005f8e05600         movss      xmm0, dword ptr [0x56e0f8]
00534c6c f30f5e058cec5600         divss      xmm0, dword ptr [0x56ec8c]
00534c74 f30f1145d0               movss      dword ptr [ebp - 0x30], xmm0
00534c79 c745a400000000           mov        dword ptr [ebp - 0x5c], 0
00534c80 eb09                     jmp        0x534c8b
00534c82 8b55a4                   mov        edx, dword ptr [ebp - 0x5c]
00534c85 83c201                   add        edx, 1
00534c88 8955a4                   mov        dword ptr [ebp - 0x5c], edx
00534c8b 837da418                 cmp        dword ptr [ebp - 0x5c], 0x18
00534c8f 7d56                     jge        0x534ce7
00534c91 8d4df8                   lea        ecx, [ebp - 8]
00534c94 e84746efff               call       0x4292e0
00534c99 d95dcc                   fstp       dword ptr [ebp - 0x34]
00534c9c 6a00                     push       0
00534c9e e87d28f0ff               call       0x437520
00534ca3 83c404                   add        esp, 4
00534ca6 894590                   mov        dword ptr [ebp - 0x70], eax
00534ca9 6a00                     push       0
00534cab 8d45b4                   lea        eax, [ebp - 0x4c]
00534cae 50                       push       eax
00534caf 6a06                     push       6
00534cb1 8d4d88                   lea        ecx, [ebp - 0x78]
00534cb4 51                       push       ecx
00534cb5 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00534cb8 e8b38ef6ff               call       0x49db70
00534cbd 90                       nop        
00534cbe f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00534cc6 f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
00534cce f30f5e05b4f75600         divss      xmm0, dword ptr [0x56f7b4]
00534cd6 51                       push       ecx
00534cd7 f30f110424               movss      dword ptr [esp], xmm0
00534cdc 8d4df8                   lea        ecx, [ebp - 8]
00534cdf e87cd3f3ff               call       0x472060
00534ce4 90                       nop        
00534ce5 eb9b                     jmp        0x534c82
00534ce7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00534cea 33cd                     xor        ecx, ebp
00534cec e8fddc0000               call       0x5429ee
00534cf1 8be5                     mov        esp, ebp
00534cf3 5d                       pop        ebp
00534cf4 c3                       ret        
00534cf5 cc                       int3       
00534cf6 cc                       int3       
00534cf7 cc                       int3       
00534cf8 cc                       int3       
00534cf9 cc                       int3       
00534cfa cc                       int3       
00534cfb cc                       int3       
00534cfc cc                       int3       
00534cfd cc                       int3       
00534cfe cc                       int3       
00534cff cc                       int3       
