00483d20 55                       push       ebp
00483d21 8bec                     mov        ebp, esp
00483d23 83ec4c                   sub        esp, 0x4c
00483d26 56                       push       esi
00483d27 894dfc                   mov        dword ptr [ebp - 4], ecx
00483d2a 8b45fc                   mov        eax, dword ptr [ebp - 4]
00483d2d 05a0000000               add        eax, 0xa0
00483d32 8945f8                   mov        dword ptr [ebp - 8], eax
00483d35 6a0a                     push       0xa
00483d37 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00483d3a e84189ffff               call       0x47c680
00483d3f 8945ec                   mov        dword ptr [ebp - 0x14], eax
00483d42 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483d45 81c1a0000000             add        ecx, 0xa0
00483d4b 894df4                   mov        dword ptr [ebp - 0xc], ecx
00483d4e 6a0a                     push       0xa
00483d50 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00483d53 e82889ffff               call       0x47c680
00483d58 8b5030                   mov        edx, dword ptr [eax + 0x30]
00483d5b 8955f0                   mov        dword ptr [ebp - 0x10], edx
00483d5e 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00483d61 50                       push       eax
00483d62 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00483d65 e856f8f9ff               call       0x4235c0
00483d6a 0fb6c8                   movzx      ecx, al
00483d6d 85c9                     test       ecx, ecx
00483d6f 7426                     je         0x483d97
00483d71 8b55fc                   mov        edx, dword ptr [ebp - 4]
00483d74 8b8290000000             mov        eax, dword ptr [edx + 0x90]
00483d7a 25ffffdfff               and        eax, 0xffdfffff
00483d7f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483d82 898190000000             mov        dword ptr [ecx + 0x90], eax
00483d88 b801000000               mov        eax, 1
00483d8d e962010000               jmp        0x483ef4
00483d92 e938010000               jmp        0x483ecf
00483d97 8b55fc                   mov        edx, dword ptr [ebp - 4]
00483d9a 81c2a0000000             add        edx, 0xa0
00483da0 8955e8                   mov        dword ptr [ebp - 0x18], edx
00483da3 6a0a                     push       0xa
00483da5 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00483da8 e8d388ffff               call       0x47c680
00483dad 8bf0                     mov        esi, eax
00483daf b9e4ef5a00               mov        ecx, 0x5aefe4
00483db4 e82755faff               call       0x4292e0
00483db9 d95de4                   fstp       dword ptr [ebp - 0x1c]
00483dbc f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
00483dc1 f30f594610               mulss      xmm0, dword ptr [esi + 0x10]
00483dc6 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00483dcb 8b45fc                   mov        eax, dword ptr [ebp - 4]
00483dce f30f104020               movss      xmm0, dword ptr [eax + 0x20]
00483dd3 f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
00483dd8 f30f1045e0               movss      xmm0, dword ptr [ebp - 0x20]
00483ddd f30f5845dc               addss      xmm0, dword ptr [ebp - 0x24]
00483de2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483de5 f30f114120               movss      dword ptr [ecx + 0x20], xmm0
00483dea 8b55fc                   mov        edx, dword ptr [ebp - 4]
00483ded 81c2a0000000             add        edx, 0xa0
00483df3 8955d8                   mov        dword ptr [ebp - 0x28], edx
00483df6 6a0a                     push       0xa
00483df8 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00483dfb e88088ffff               call       0x47c680
00483e00 83c024                   add        eax, 0x24
00483e03 8945d0                   mov        dword ptr [ebp - 0x30], eax
00483e06 b9e4ef5a00               mov        ecx, 0x5aefe4
00483e0b e8d054faff               call       0x4292e0
00483e10 d95dd4                   fstp       dword ptr [ebp - 0x2c]
00483e13 51                       push       ecx
00483e14 f30f1045d4               movss      xmm0, dword ptr [ebp - 0x2c]
00483e19 f30f110424               movss      dword ptr [esp], xmm0
00483e1e 8d45b4                   lea        eax, [ebp - 0x4c]
00483e21 50                       push       eax
00483e22 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00483e25 e8c654faff               call       0x4292f0
00483e2a 50                       push       eax
00483e2b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483e2e 83c170                   add        ecx, 0x70
00483e31 e8aa58faff               call       0x4296e0
00483e36 90                       nop        
00483e37 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483e3a 51                       push       ecx
00483e3b f30f104170               movss      xmm0, dword ptr [ecx + 0x70]
00483e40 f30f110424               movss      dword ptr [esp], xmm0
00483e45 e8e667ffff               call       0x47a630
00483e4a 83c404                   add        esp, 4
00483e4d d95dcc                   fstp       dword ptr [ebp - 0x34]
00483e50 f30f1045cc               movss      xmm0, dword ptr [ebp - 0x34]
00483e55 0f2f0530fd5600           comiss     xmm0, dword ptr [0x56fd30]
00483e5c 7727                     ja         0x483e85
00483e5e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00483e61 51                       push       ecx
00483e62 f30f104274               movss      xmm0, dword ptr [edx + 0x74]
00483e67 f30f110424               movss      dword ptr [esp], xmm0
00483e6c e8bf67ffff               call       0x47a630
00483e71 83c404                   add        esp, 4
00483e74 d95dc8                   fstp       dword ptr [ebp - 0x38]
00483e77 f30f1045c8               movss      xmm0, dword ptr [ebp - 0x38]
00483e7c 0f2f0530fd5600           comiss     xmm0, dword ptr [0x56fd30]
00483e83 764a                     jbe        0x483ecf
00483e85 8b45fc                   mov        eax, dword ptr [ebp - 4]
00483e88 51                       push       ecx
00483e89 f30f104070               movss      xmm0, dword ptr [eax + 0x70]
00483e8e f30f110424               movss      dword ptr [esp], xmm0
00483e93 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483e96 51                       push       ecx
00483e97 f30f104174               movss      xmm0, dword ptr [ecx + 0x74]
00483e9c f30f110424               movss      dword ptr [esp], xmm0
00483ea1 e8da53fdff               call       0x459280
00483ea6 83c404                   add        esp, 4
00483ea9 d91c24                   fstp       dword ptr [esp]
00483eac 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483eaf 81c184000000             add        ecx, 0x84
00483eb5 e866f0fcff               call       0x452f20
00483eba 8b55fc                   mov        edx, dword ptr [ebp - 4]
00483ebd 83c270                   add        edx, 0x70
00483ec0 52                       push       edx
00483ec1 e8da63ffff               call       0x47a2a0
00483ec6 83c404                   add        esp, 4
00483ec9 8b45fc                   mov        eax, dword ptr [ebp - 4]
00483ecc d95820                   fstp       dword ptr [eax + 0x20]
00483ecf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483ed2 81c1a0000000             add        ecx, 0xa0
00483ed8 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
00483edb 6a0a                     push       0xa
00483edd 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00483ee0 e89b87ffff               call       0x47c680
00483ee5 8945c0                   mov        dword ptr [ebp - 0x40], eax
00483ee8 6a00                     push       0
00483eea 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00483eed e84ef6f9ff               call       0x423540
00483ef2 33c0                     xor        eax, eax
00483ef4 5e                       pop        esi
00483ef5 8be5                     mov        esp, ebp
00483ef7 5d                       pop        ebp
00483ef8 c3                       ret        
00483ef9 cc                       int3       
00483efa cc                       int3       
00483efb cc                       int3       
00483efc cc                       int3       
00483efd cc                       int3       
00483efe cc                       int3       
00483eff cc                       int3       
