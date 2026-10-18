004818e0 55                       push       ebp
004818e1 8bec                     mov        ebp, esp
004818e3 6aff                     push       -1
004818e5 688d915600               push       0x56918d
004818ea 64a100000000             mov        eax, dword ptr fs:[0]
004818f0 50                       push       eax
004818f1 81eccc000000             sub        esp, 0xcc
004818f7 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004818fc 33c5                     xor        eax, ebp
004818fe 8945f0                   mov        dword ptr [ebp - 0x10], eax
00481901 50                       push       eax
00481902 8d45f4                   lea        eax, [ebp - 0xc]
00481905 64a300000000             mov        dword ptr fs:[0], eax
0048190b 894dd0                   mov        dword ptr [ebp - 0x30], ecx
0048190e c745fc00000000           mov        dword ptr [ebp - 4], 0
00481915 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00481918 81c14c6d2800             add        ecx, 0x286d4c
0048191e e8cd400000               call       0x4859f0
00481923 8945cc                   mov        dword ptr [ebp - 0x34], eax
00481926 837dcc00                 cmp        dword ptr [ebp - 0x34], 0
0048192a 7524                     jne        0x481950
0048192c c78538ffffff01000000     mov        dword ptr [ebp - 0xc8], 1
00481936 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0048193d 8d4d0c                   lea        ecx, [ebp + 0xc]
00481940 e8bba9ffff               call       0x47c300
00481945 8b8538ffffff             mov        eax, dword ptr [ebp - 0xc8]
0048194b e90c0e0000               jmp        0x48275c
00481950 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00481953 e8a8a9f8ff               call       0x40c300
00481958 8945e0                   mov        dword ptr [ebp - 0x20], eax
0048195b 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0048195e e87d03f9ff               call       0x411ce0
00481963 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00481966 05646d2800               add        eax, 0x286d64
0048196b 8945a8                   mov        dword ptr [ebp - 0x58], eax
0048196e 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00481971 51                       push       ecx
00481972 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00481975 e8368dfcff               call       0x44a6b0
0048197a 8d4d0c                   lea        ecx, [ebp + 0xc]
0048197d e87ea9f8ff               call       0x40c300
00481982 8945d8                   mov        dword ptr [ebp - 0x28], eax
00481985 8d550c                   lea        edx, [ebp + 0xc]
00481988 52                       push       edx
00481989 e8f298f8ff               call       0x40b280
0048198e 83c404                   add        esp, 4
00481991 50                       push       eax
00481992 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00481995 81c198000000             add        ecx, 0x98
0048199b e890abffff               call       0x47c530
004819a0 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
004819a3 8b889c6d2800             mov        ecx, dword ptr [eax + 0x286d9c]
004819a9 51                       push       ecx
004819aa 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004819ad e8be4b0000               call       0x486570
004819b2 0f57c0                   xorps      xmm0, xmm0
004819b5 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
004819ba 51                       push       ecx
004819bb f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004819c3 f30f110424               movss      dword ptr [esp], xmm0
004819c8 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004819cb 81c1d8040000             add        ecx, 0x4d8
004819d1 e81aacffff               call       0x47c5f0
004819d6 51                       push       ecx
004819d7 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004819df f30f110424               movss      dword ptr [esp], xmm0
004819e4 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004819e7 81c1e8040000             add        ecx, 0x4e8
004819ed e8feabffff               call       0x47c5f0
004819f2 8b5508                   mov        edx, dword ptr [ebp + 8]
004819f5 0fbf4226                 movsx      eax, word ptr [edx + 0x26]
004819f9 83f801                   cmp        eax, 1
004819fc 7e43                     jle        0x481a41
004819fe 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00481a01 8b5508                   mov        edx, dword ptr [ebp + 8]
00481a04 f30f10411c               movss      xmm0, dword ptr [ecx + 0x1c]
00481a09 f30f5c4220               subss      xmm0, dword ptr [edx + 0x20]
00481a0e f30f2a4d18               cvtsi2ss   xmm1, dword ptr [ebp + 0x18]
00481a13 f30f59c1                 mulss      xmm0, xmm1
00481a17 8b4508                   mov        eax, dword ptr [ebp + 8]
00481a1a 0fbf4826                 movsx      ecx, word ptr [eax + 0x26]
00481a1e f30f2ac9                 cvtsi2ss   xmm1, ecx
00481a22 f30f5c0dccc85600         subss      xmm1, dword ptr [0x56c8cc]
00481a2a f30f5ec1                 divss      xmm0, xmm1
00481a2e 8b5508                   mov        edx, dword ptr [ebp + 8]
00481a31 f30f104a1c               movss      xmm1, dword ptr [edx + 0x1c]
00481a36 f30f5cc8                 subss      xmm1, xmm0
00481a3a f30f114dc8               movss      dword ptr [ebp - 0x38], xmm1
00481a3f eb0d                     jmp        0x481a4e
00481a41 8b4508                   mov        eax, dword ptr [ebp + 8]
00481a44 f30f10401c               movss      xmm0, dword ptr [eax + 0x1c]
00481a49 f30f1145c8               movss      dword ptr [ebp - 0x38], xmm0
00481a4e f30f1045c8               movss      xmm0, dword ptr [ebp - 0x38]
00481a53 f30f1145d4               movss      dword ptr [ebp - 0x2c], xmm0
00481a58 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00481a5b f30f1045d4               movss      xmm0, dword ptr [ebp - 0x2c]
00481a60 f30f114124               movss      dword ptr [ecx + 0x24], xmm0
00481a65 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
00481a68 0fb74238                 movzx      eax, word ptr [edx + 0x38]
00481a6c 8945c4                   mov        dword ptr [ebp - 0x3c], eax
00481a6f 837dc40c                 cmp        dword ptr [ebp - 0x3c], 0xc
00481a73 0f87af050000             ja         0x482028
00481a79 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00481a7c ff248d78274800           jmp        dword ptr [ecx*4 + 0x482778]
00481a83 8b5508                   mov        edx, dword ptr [ebp + 8]
00481a86 0fbf4224                 movsx      eax, word ptr [edx + 0x24]
00481a8a 83e001                   and        eax, 1
00481a8d 741e                     je         0x481aad
00481a8f 8b4514                   mov        eax, dword ptr [ebp + 0x14]
00481a92 83c001                   add        eax, 1
00481a95 99                       cdq        
00481a96 2bc2                     sub        eax, edx
00481a98 d1f8                     sar        eax, 1
00481a9a f30f2ac0                 cvtsi2ss   xmm0, eax
00481a9e 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00481aa1 f30f594118               mulss      xmm0, dword ptr [ecx + 0x18]
00481aa6 f30f1145c0               movss      dword ptr [ebp - 0x40], xmm0
00481aab eb2d                     jmp        0x481ada
00481aad 8b4514                   mov        eax, dword ptr [ebp + 0x14]
00481ab0 99                       cdq        
00481ab1 2bc2                     sub        eax, edx
00481ab3 d1f8                     sar        eax, 1
00481ab5 f30f2ac0                 cvtsi2ss   xmm0, eax
00481ab9 8b5508                   mov        edx, dword ptr [ebp + 8]
00481abc f30f594218               mulss      xmm0, dword ptr [edx + 0x18]
00481ac1 8b4508                   mov        eax, dword ptr [ebp + 8]
00481ac4 f30f104818               movss      xmm1, dword ptr [eax + 0x18]
00481ac9 f30f590dece05600         mulss      xmm1, dword ptr [0x56e0ec]
00481ad1 f30f58c1                 addss      xmm0, xmm1
00481ad5 f30f1145c0               movss      dword ptr [ebp - 0x40], xmm0
00481ada f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00481adf f30f5845c0               addss      xmm0, dword ptr [ebp - 0x40]
00481ae4 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481ae9 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
00481aec 83e101                   and        ecx, 1
00481aef 7412                     je         0x481b03
00481af1 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00481af6 f30f5905f4e05600         mulss      xmm0, dword ptr [0x56e0f4]
00481afe f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481b03 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
00481b06 0fb74238                 movzx      eax, word ptr [edx + 0x38]
00481b0a 85c0                     test       eax, eax
00481b0c 750f                     jne        0x481b1d
00481b0e f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00481b13 f30f58451c               addss      xmm0, dword ptr [ebp + 0x1c]
00481b18 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481b1d 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00481b20 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00481b25 f30f584114               addss      xmm0, dword ptr [ecx + 0x14]
00481b2a f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481b2f e9f4040000               jmp        0x482028
00481b34 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00481b39 f30f58451c               addss      xmm0, dword ptr [ebp + 0x1c]
00481b3e f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481b43 f30f2a4514               cvtsi2ss   xmm0, dword ptr [ebp + 0x14]
00481b48 f30f100df0e05600         movss      xmm1, dword ptr [0x56e0f0]
00481b50 f30f590dd0c85600         mulss      xmm1, dword ptr [0x56c8d0]
00481b58 f30f59c1                 mulss      xmm0, xmm1
00481b5c 8b5508                   mov        edx, dword ptr [ebp + 8]
00481b5f 0fbf4224                 movsx      eax, word ptr [edx + 0x24]
00481b63 f30f2ac8                 cvtsi2ss   xmm1, eax
00481b67 f30f5ec1                 divss      xmm0, xmm1
00481b6b f30f5845dc               addss      xmm0, dword ptr [ebp - 0x24]
00481b70 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481b75 f30f2a4518               cvtsi2ss   xmm0, dword ptr [ebp + 0x18]
00481b7a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00481b7d f30f594118               mulss      xmm0, dword ptr [ecx + 0x18]
00481b82 8b5508                   mov        edx, dword ptr [ebp + 8]
00481b85 f30f584214               addss      xmm0, dword ptr [edx + 0x14]
00481b8a f30f5845dc               addss      xmm0, dword ptr [ebp - 0x24]
00481b8f f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481b94 e98f040000               jmp        0x482028
00481b99 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00481b9e f30f58451c               addss      xmm0, dword ptr [ebp + 0x1c]
00481ba3 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481ba8 8b4508                   mov        eax, dword ptr [ebp + 8]
00481bab 0fbf4824                 movsx      ecx, word ptr [eax + 0x24]
00481baf f30f2ac1                 cvtsi2ss   xmm0, ecx
00481bb3 f30f100df0e05600         movss      xmm1, dword ptr [0x56e0f0]
00481bbb f30f5ec8                 divss      xmm1, xmm0
00481bbf f30f584ddc               addss      xmm1, dword ptr [ebp - 0x24]
00481bc4 f30f114ddc               movss      dword ptr [ebp - 0x24], xmm1
00481bc9 f30f2a4514               cvtsi2ss   xmm0, dword ptr [ebp + 0x14]
00481bce f30f100df0e05600         movss      xmm1, dword ptr [0x56e0f0]
00481bd6 f30f590dd0c85600         mulss      xmm1, dword ptr [0x56c8d0]
00481bde f30f59c1                 mulss      xmm0, xmm1
00481be2 8b5508                   mov        edx, dword ptr [ebp + 8]
00481be5 0fbf4224                 movsx      eax, word ptr [edx + 0x24]
00481be9 f30f2ac8                 cvtsi2ss   xmm1, eax
00481bed f30f5ec1                 divss      xmm0, xmm1
00481bf1 f30f5845dc               addss      xmm0, dword ptr [ebp - 0x24]
00481bf6 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481bfb f30f2a4518               cvtsi2ss   xmm0, dword ptr [ebp + 0x18]
00481c00 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00481c03 f30f594118               mulss      xmm0, dword ptr [ecx + 0x18]
00481c08 8b5508                   mov        edx, dword ptr [ebp + 8]
00481c0b f30f584214               addss      xmm0, dword ptr [edx + 0x14]
00481c10 f30f5845dc               addss      xmm0, dword ptr [ebp - 0x24]
00481c15 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481c1a e909040000               jmp        0x482028
00481c1f 8b4508                   mov        eax, dword ptr [ebp + 8]
00481c22 51                       push       ecx
00481c23 f30f104018               movss      xmm0, dword ptr [eax + 0x18]
00481c28 f30f110424               movss      dword ptr [esp], xmm0
00481c2d b9a8a45b00               mov        ecx, 0x5ba4a8
00481c32 e8797cfaff               call       0x4298b0
00481c37 d95da4                   fstp       dword ptr [ebp - 0x5c]
00481c3a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00481c3d f30f1045a4               movss      xmm0, dword ptr [ebp - 0x5c]
00481c42 f30f584114               addss      xmm0, dword ptr [ecx + 0x14]
00481c47 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481c4c e9d7030000               jmp        0x482028
00481c51 8b5508                   mov        edx, dword ptr [ebp + 8]
00481c54 51                       push       ecx
00481c55 f30f104220               movss      xmm0, dword ptr [edx + 0x20]
00481c5a f30f110424               movss      dword ptr [esp], xmm0
00481c5f b9a8a45b00               mov        ecx, 0x5ba4a8
00481c64 e8977bfaff               call       0x429800
00481c69 d95da0                   fstp       dword ptr [ebp - 0x60]
00481c6c 8b4508                   mov        eax, dword ptr [ebp + 8]
00481c6f f30f1045a0               movss      xmm0, dword ptr [ebp - 0x60]
00481c74 f30f58401c               addss      xmm0, dword ptr [eax + 0x1c]
00481c79 f30f1145d4               movss      dword ptr [ebp - 0x2c], xmm0
00481c7e f30f2a4514               cvtsi2ss   xmm0, dword ptr [ebp + 0x14]
00481c83 f30f100df0e05600         movss      xmm1, dword ptr [0x56e0f0]
00481c8b f30f590dd0c85600         mulss      xmm1, dword ptr [0x56c8d0]
00481c93 f30f59c1                 mulss      xmm0, xmm1
00481c97 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00481c9a 0fbf5124                 movsx      edx, word ptr [ecx + 0x24]
00481c9e f30f2aca                 cvtsi2ss   xmm1, edx
00481ca2 f30f5ec1                 divss      xmm0, xmm1
00481ca6 f30f5845dc               addss      xmm0, dword ptr [ebp - 0x24]
00481cab f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481cb0 f30f2a4518               cvtsi2ss   xmm0, dword ptr [ebp + 0x18]
00481cb5 8b4508                   mov        eax, dword ptr [ebp + 8]
00481cb8 f30f594018               mulss      xmm0, dword ptr [eax + 0x18]
00481cbd 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00481cc0 f30f584114               addss      xmm0, dword ptr [ecx + 0x14]
00481cc5 f30f5845dc               addss      xmm0, dword ptr [ebp - 0x24]
00481cca f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481ccf e954030000               jmp        0x482028
00481cd4 8b5508                   mov        edx, dword ptr [ebp + 8]
00481cd7 51                       push       ecx
00481cd8 f30f104218               movss      xmm0, dword ptr [edx + 0x18]
00481cdd f30f110424               movss      dword ptr [esp], xmm0
00481ce2 b9a8a45b00               mov        ecx, 0x5ba4a8
00481ce7 e8c47bfaff               call       0x4298b0
00481cec d95d9c                   fstp       dword ptr [ebp - 0x64]
00481cef 8b4508                   mov        eax, dword ptr [ebp + 8]
00481cf2 f30f10459c               movss      xmm0, dword ptr [ebp - 0x64]
00481cf7 f30f584014               addss      xmm0, dword ptr [eax + 0x14]
00481cfc f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481d01 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00481d04 51                       push       ecx
00481d05 f30f104120               movss      xmm0, dword ptr [ecx + 0x20]
00481d0a f30f110424               movss      dword ptr [esp], xmm0
00481d0f b9a8a45b00               mov        ecx, 0x5ba4a8
00481d14 e8e77afaff               call       0x429800
00481d19 d95d98                   fstp       dword ptr [ebp - 0x68]
00481d1c 8b5508                   mov        edx, dword ptr [ebp + 8]
00481d1f f30f104598               movss      xmm0, dword ptr [ebp - 0x68]
00481d24 f30f58421c               addss      xmm0, dword ptr [edx + 0x1c]
00481d29 f30f1145d4               movss      dword ptr [ebp - 0x2c], xmm0
00481d2e e9f5020000               jmp        0x482028
00481d33 f30f2a4514               cvtsi2ss   xmm0, dword ptr [ebp + 0x14]
00481d38 f30f100df0e05600         movss      xmm1, dword ptr [0x56e0f0]
00481d40 f30f590dd0c85600         mulss      xmm1, dword ptr [0x56c8d0]
00481d48 f30f59c1                 mulss      xmm0, xmm1
00481d4c 8b4508                   mov        eax, dword ptr [ebp + 8]
00481d4f 0fbf4824                 movsx      ecx, word ptr [eax + 0x24]
00481d53 f30f2ac9                 cvtsi2ss   xmm1, ecx
00481d57 f30f5ec1                 divss      xmm0, xmm1
00481d5b f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481d60 8b5508                   mov        edx, dword ptr [ebp + 8]
00481d63 0fbf4226                 movsx      eax, word ptr [edx + 0x26]
00481d67 83e001                   and        eax, 1
00481d6a 7472                     je         0x481dde
00481d6c 8b4518                   mov        eax, dword ptr [ebp + 0x18]
00481d6f 83c001                   add        eax, 1
00481d72 99                       cdq        
00481d73 2bc2                     sub        eax, edx
00481d75 d1f8                     sar        eax, 1
00481d77 f30f2ac0                 cvtsi2ss   xmm0, eax
00481d7b 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00481d7e f30f594118               mulss      xmm0, dword ptr [ecx + 0x18]
00481d83 f30f5845dc               addss      xmm0, dword ptr [ebp - 0x24]
00481d88 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481d8d 8b5508                   mov        edx, dword ptr [ebp + 8]
00481d90 0fbf4226                 movsx      eax, word ptr [edx + 0x26]
00481d94 83f801                   cmp        eax, 1
00481d97 7e43                     jle        0x481ddc
00481d99 8b4d18                   mov        ecx, dword ptr [ebp + 0x18]
00481d9c 83c101                   add        ecx, 1
00481d9f 81e1feff0000             and        ecx, 0xfffe
00481da5 f30f2ac1                 cvtsi2ss   xmm0, ecx
00481da9 8b5508                   mov        edx, dword ptr [ebp + 8]
00481dac 8b4508                   mov        eax, dword ptr [ebp + 8]
00481daf f30f104a20               movss      xmm1, dword ptr [edx + 0x20]
00481db4 f30f5c481c               subss      xmm1, dword ptr [eax + 0x1c]
00481db9 f30f59c1                 mulss      xmm0, xmm1
00481dbd 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00481dc0 0fbf5126                 movsx      edx, word ptr [ecx + 0x26]
00481dc4 83ea01                   sub        edx, 1
00481dc7 f30f2aca                 cvtsi2ss   xmm1, edx
00481dcb f30f5ec1                 divss      xmm0, xmm1
00481dcf 8b4508                   mov        eax, dword ptr [ebp + 8]
00481dd2 f30f58401c               addss      xmm0, dword ptr [eax + 0x1c]
00481dd7 f30f1145d4               movss      dword ptr [ebp - 0x2c], xmm0
00481ddc eb7e                     jmp        0x481e5c
00481dde 8b4518                   mov        eax, dword ptr [ebp + 0x18]
00481de1 99                       cdq        
00481de2 2bc2                     sub        eax, edx
00481de4 d1f8                     sar        eax, 1
00481de6 f30f2ac0                 cvtsi2ss   xmm0, eax
00481dea 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00481ded f30f594118               mulss      xmm0, dword ptr [ecx + 0x18]
00481df2 8b5508                   mov        edx, dword ptr [ebp + 8]
00481df5 f30f104a18               movss      xmm1, dword ptr [edx + 0x18]
00481dfa f30f590dece05600         mulss      xmm1, dword ptr [0x56e0ec]
00481e02 f30f58c1                 addss      xmm0, xmm1
00481e06 f30f5845dc               addss      xmm0, dword ptr [ebp - 0x24]
00481e0b f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481e10 8b4508                   mov        eax, dword ptr [ebp + 8]
00481e13 0fbf4826                 movsx      ecx, word ptr [eax + 0x26]
00481e17 83f901                   cmp        ecx, 1
00481e1a 7e40                     jle        0x481e5c
00481e1c 8b5518                   mov        edx, dword ptr [ebp + 0x18]
00481e1f 81e2feff0000             and        edx, 0xfffe
00481e25 f30f2ac2                 cvtsi2ss   xmm0, edx
00481e29 8b4508                   mov        eax, dword ptr [ebp + 8]
00481e2c 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00481e2f f30f104820               movss      xmm1, dword ptr [eax + 0x20]
00481e34 f30f5c491c               subss      xmm1, dword ptr [ecx + 0x1c]
00481e39 f30f59c1                 mulss      xmm0, xmm1
00481e3d 8b5508                   mov        edx, dword ptr [ebp + 8]
00481e40 0fbf4226                 movsx      eax, word ptr [edx + 0x26]
00481e44 83e801                   sub        eax, 1
00481e47 f30f2ac8                 cvtsi2ss   xmm1, eax
00481e4b f30f5ec1                 divss      xmm0, xmm1
00481e4f 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00481e52 f30f58411c               addss      xmm0, dword ptr [ecx + 0x1c]
00481e57 f30f1145d4               movss      dword ptr [ebp - 0x2c], xmm0
00481e5c 8b5518                   mov        edx, dword ptr [ebp + 0x18]
00481e5f 83e201                   and        edx, 1
00481e62 7412                     je         0x481e76
00481e64 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00481e69 f30f5905f4e05600         mulss      xmm0, dword ptr [0x56e0f4]
00481e71 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481e76 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
00481e79 0fb74838                 movzx      ecx, word ptr [eax + 0x38]
00481e7d 83f909                   cmp        ecx, 9
00481e80 750f                     jne        0x481e91
00481e82 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00481e87 f30f58451c               addss      xmm0, dword ptr [ebp + 0x1c]
00481e8c f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481e91 8b5508                   mov        edx, dword ptr [ebp + 8]
00481e94 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00481e99 f30f584214               addss      xmm0, dword ptr [edx + 0x14]
00481e9e f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481ea3 e980010000               jmp        0x482028
00481ea8 f30f2a4514               cvtsi2ss   xmm0, dword ptr [ebp + 0x14]
00481ead f30f100df0e05600         movss      xmm1, dword ptr [0x56e0f0]
00481eb5 f30f590dd0c85600         mulss      xmm1, dword ptr [0x56c8d0]
00481ebd f30f59c1                 mulss      xmm0, xmm1
00481ec1 8b4508                   mov        eax, dword ptr [ebp + 8]
00481ec4 0fbf4824                 movsx      ecx, word ptr [eax + 0x24]
00481ec8 f30f2ac9                 cvtsi2ss   xmm1, ecx
00481ecc f30f5ec1                 divss      xmm0, xmm1
00481ed0 f30f114594               movss      dword ptr [ebp - 0x6c], xmm0
00481ed5 f30f104594               movss      xmm0, dword ptr [ebp - 0x6c]
00481eda f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481edf 51                       push       ecx
00481ee0 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00481ee5 f30f110424               movss      dword ptr [esp], xmm0
00481eea e8d178fbff               call       0x4397c0
00481eef 83c404                   add        esp, 4
00481ef2 d95d90                   fstp       dword ptr [ebp - 0x70]
00481ef5 8b5508                   mov        edx, dword ptr [ebp + 8]
00481ef8 f30f104590               movss      xmm0, dword ptr [ebp - 0x70]
00481efd f30f59421c               mulss      xmm0, dword ptr [edx + 0x1c]
00481f02 f30f1145bc               movss      dword ptr [ebp - 0x44], xmm0
00481f07 51                       push       ecx
00481f08 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00481f0d f30f110424               movss      dword ptr [esp], xmm0
00481f12 e80979fbff               call       0x439820
00481f17 83c404                   add        esp, 4
00481f1a d95d8c                   fstp       dword ptr [ebp - 0x74]
00481f1d 8b4508                   mov        eax, dword ptr [ebp + 8]
00481f20 f30f10458c               movss      xmm0, dword ptr [ebp - 0x74]
00481f25 f30f594020               mulss      xmm0, dword ptr [eax + 0x20]
00481f2a f30f1145b8               movss      dword ptr [ebp - 0x48], xmm0
00481f2f 51                       push       ecx
00481f30 f30f1045b8               movss      xmm0, dword ptr [ebp - 0x48]
00481f35 f30f110424               movss      dword ptr [esp], xmm0
00481f3a 51                       push       ecx
00481f3b f30f1045bc               movss      xmm0, dword ptr [ebp - 0x44]
00481f40 f30f110424               movss      dword ptr [esp], xmm0
00481f45 e86640fdff               call       0x455fb0
00481f4a 83c408                   add        esp, 8
00481f4d d95dd4                   fstp       dword ptr [ebp - 0x2c]
00481f50 51                       push       ecx
00481f51 f30f1045bc               movss      xmm0, dword ptr [ebp - 0x44]
00481f56 f30f110424               movss      dword ptr [esp], xmm0
00481f5b 51                       push       ecx
00481f5c f30f1045b8               movss      xmm0, dword ptr [ebp - 0x48]
00481f61 f30f110424               movss      dword ptr [esp], xmm0
00481f66 e81573fdff               call       0x459280
00481f6b 83c408                   add        esp, 8
00481f6e d95d88                   fstp       dword ptr [ebp - 0x78]
00481f71 f30f104588               movss      xmm0, dword ptr [ebp - 0x78]
00481f76 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00481f79 f30f584114               addss      xmm0, dword ptr [ecx + 0x14]
00481f7e f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481f83 e9a0000000               jmp        0x482028
00481f88 f30f2a4514               cvtsi2ss   xmm0, dword ptr [ebp + 0x14]
00481f8d f30f100df0e05600         movss      xmm1, dword ptr [0x56e0f0]
00481f95 f30f590dd0c85600         mulss      xmm1, dword ptr [0x56c8d0]
00481f9d f30f59c1                 mulss      xmm0, xmm1
00481fa1 8b5508                   mov        edx, dword ptr [ebp + 8]
00481fa4 0fbf4224                 movsx      eax, word ptr [edx + 0x24]
00481fa8 f30f2ac8                 cvtsi2ss   xmm1, eax
00481fac f30f5ec1                 divss      xmm0, xmm1
00481fb0 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00481fb3 0fbf5124                 movsx      edx, word ptr [ecx + 0x24]
00481fb7 f30f2aca                 cvtsi2ss   xmm1, edx
00481fbb f30f1015f0e05600         movss      xmm2, dword ptr [0x56e0f0]
00481fc3 f30f5ed1                 divss      xmm2, xmm1
00481fc7 f30f58c2                 addss      xmm0, xmm2
00481fcb f30f1145b4               movss      dword ptr [ebp - 0x4c], xmm0
00481fd0 8b4508                   mov        eax, dword ptr [ebp + 8]
00481fd3 f30f1045b4               movss      xmm0, dword ptr [ebp - 0x4c]
00481fd8 f30f584014               addss      xmm0, dword ptr [eax + 0x14]
00481fdd f30f5845dc               addss      xmm0, dword ptr [ebp - 0x24]
00481fe2 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00481fe7 51                       push       ecx
00481fe8 f30f1045b4               movss      xmm0, dword ptr [ebp - 0x4c]
00481fed f30f110424               movss      dword ptr [esp], xmm0
00481ff2 e82978fbff               call       0x439820
00481ff7 d91c24                   fstp       dword ptr [esp]
00481ffa e83186ffff               call       0x47a630
00481fff 83c404                   add        esp, 4
00482002 d95d84                   fstp       dword ptr [ebp - 0x7c]
00482005 f30f104584               movss      xmm0, dword ptr [ebp - 0x7c]
0048200a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0048200d f30f594120               mulss      xmm0, dword ptr [ecx + 0x20]
00482012 f30f100dccc85600         movss      xmm1, dword ptr [0x56c8cc]
0048201a f30f5cc8                 subss      xmm1, xmm0
0048201e f30f594dd4               mulss      xmm1, dword ptr [ebp - 0x2c]
00482023 f30f114dd4               movss      dword ptr [ebp - 0x2c], xmm1
00482028 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0048202b f30f1045d4               movss      xmm0, dword ptr [ebp - 0x2c]
00482030 f30f114220               movss      dword ptr [edx + 0x20], xmm0
00482035 51                       push       ecx
00482036 0f57c0                   xorps      xmm0, xmm0
00482039 f30f110424               movss      dword ptr [esp], xmm0
0048203e 51                       push       ecx
0048203f f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00482044 f30f110424               movss      dword ptr [esp], xmm0
00482049 e8b265fbff               call       0x438600
0048204e 83c404                   add        esp, 4
00482051 d91c24                   fstp       dword ptr [esp]
00482054 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00482057 81c184000000             add        ecx, 0x84
0048205d e8be0efdff               call       0x452f20
00482062 8b4508                   mov        eax, dword ptr [ebp + 8]
00482065 83c008                   add        eax, 8
00482068 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0048206b 83c164                   add        ecx, 0x64
0048206e 8b10                     mov        edx, dword ptr [eax]
00482070 8911                     mov        dword ptr [ecx], edx
00482072 8b5004                   mov        edx, dword ptr [eax + 4]
00482075 895104                   mov        dword ptr [ecx + 4], edx
00482078 8b4008                   mov        eax, dword ptr [eax + 8]
0048207b 894108                   mov        dword ptr [ecx + 8], eax
0048207e 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00482081 f30f1001                 movss      xmm0, dword ptr [ecx]
00482085 0f2e05e8e05600           ucomiss    xmm0, dword ptr [0x56e0e8]
0048208c 9f                       lahf       
0048208d f6c444                   test       ah, 0x44
00482090 7b77                     jnp        0x482109
00482092 8d4de4                   lea        ecx, [ebp - 0x1c]
00482095 e8760dfaff               call       0x422e10
0048209a 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
0048209d f30f1002                 movss      xmm0, dword ptr [edx]
004820a1 f30f114580               movss      dword ptr [ebp - 0x80], xmm0
004820a6 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004820a9 81c184000000             add        ecx, 0x84
004820af e82c72faff               call       0x4292e0
004820b4 d99d7cffffff             fstp       dword ptr [ebp - 0x84]
004820ba 51                       push       ecx
004820bb f30f104580               movss      xmm0, dword ptr [ebp - 0x80]
004820c0 f30f110424               movss      dword ptr [esp], xmm0
004820c5 51                       push       ecx
004820c6 f30f10857cffffff         movss      xmm0, dword ptr [ebp - 0x84]
004820ce f30f110424               movss      dword ptr [esp], xmm0
004820d3 8d45e4                   lea        eax, [ebp - 0x1c]
004820d6 50                       push       eax
004820d7 e85472fbff               call       0x439330
004820dc 83c40c                   add        esp, 0xc
004820df 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004820e2 f30f104164               movss      xmm0, dword ptr [ecx + 0x64]
004820e7 f30f5845e4               addss      xmm0, dword ptr [ebp - 0x1c]
004820ec 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004820ef f30f114264               movss      dword ptr [edx + 0x64], xmm0
004820f4 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004820f7 f30f104068               movss      xmm0, dword ptr [eax + 0x68]
004820fc f30f5845e8               addss      xmm0, dword ptr [ebp - 0x18]
00482101 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00482104 f30f114168               movss      dword ptr [ecx + 0x68], xmm0
00482109 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0048210c f30f100528ef5600         movss      xmm0, dword ptr [0x56ef28]
00482114 f30f11426c               movss      dword ptr [edx + 0x6c], xmm0
00482119 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0048211c c7405001000000           mov        dword ptr [eax + 0x50], 1
00482123 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00482126 8b5114                   mov        edx, dword ptr [ecx + 0x14]
00482129 83ca01                   or         edx, 1
0048212c 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0048212f 895014                   mov        dword ptr [eax + 0x14], edx
00482132 6a00                     push       0
00482134 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00482137 81c1f8040000             add        ecx, 0x4f8
0048213d e8de13faff               call       0x423520
00482142 6a00                     push       0
00482144 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00482147 81c108050000             add        ecx, 0x508
0048214d e8ce13faff               call       0x423520
00482152 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00482155 c7411800000000           mov        dword ptr [ecx + 0x18], 0
0048215c 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0048215f f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00482167 f30f114248               movss      dword ptr [edx + 0x48], xmm0
0048216c 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0048216f 81c174040000             add        ecx, 0x474
00482175 e8f677faff               call       0x429970
0048217a 51                       push       ecx
0048217b f30f1045d4               movss      xmm0, dword ptr [ebp - 0x2c]
00482180 f30f110424               movss      dword ptr [esp], xmm0
00482185 51                       push       ecx
00482186 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
0048218b f30f110424               movss      dword ptr [esp], xmm0
00482190 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00482193 83c070                   add        eax, 0x70
00482196 50                       push       eax
00482197 e89471fbff               call       0x439330
0048219c 83c40c                   add        esp, 0xc
0048219f 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004821a2 8b5508                   mov        edx, dword ptr [ebp + 8]
004821a5 668b4204                 mov        ax, word ptr [edx + 4]
004821a9 6689414e                 mov        word ptr [ecx + 0x4e], ax
004821ad 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004821b0 8b5508                   mov        edx, dword ptr [ebp + 8]
004821b3 668b02                   mov        ax, word ptr [edx]
004821b6 6689414c                 mov        word ptr [ecx + 0x4c], ax
004821ba 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004821bd c7415400000000           mov        dword ptr [ecx + 0x54], 0
004821c4 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004821c7 8b4214                   mov        eax, dword ptr [edx + 0x14]
004821ca 83e0f7                   and        eax, 0xfffffff7
004821cd 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004821d0 894114                   mov        dword ptr [ecx + 0x14], eax
004821d3 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004821d6 8b4214                   mov        eax, dword ptr [edx + 0x14]
004821d9 83e0fb                   and        eax, 0xfffffffb
004821dc 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004821df 894114                   mov        dword ptr [ecx + 0x14], eax
004821e2 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004821e5 8b4214                   mov        eax, dword ptr [edx + 0x14]
004821e8 83c802                   or         eax, 2
004821eb 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004821ee 894114                   mov        dword ptr [ecx + 0x14], eax
004821f1 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004821f4 c782d00400003c000000     mov        dword ptr [edx + 0x4d0], 0x3c
004821fe 6a00                     push       0
00482200 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00482203 81c1a0040000             add        ecx, 0x4a0
00482209 e81213faff               call       0x423520
0048220e 6a00                     push       0
00482210 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00482213 81c1b0040000             add        ecx, 0x4b0
00482219 e80213faff               call       0x423520
0048221e 6a00                     push       0
00482220 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00482223 81c1c0040000             add        ecx, 0x4c0
00482229 e8f212faff               call       0x423520
0048222e 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00482231 c780d404000003000000     mov        dword ptr [eax + 0x4d4], 3
0048223b 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00482241 e86aa7fcff               call       0x44c9b0
00482246 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00482249 89411c                   mov        dword ptr [ecx + 0x1c], eax
0048224c 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0048224f 8b4a1c                   mov        ecx, dword ptr [edx + 0x1c]
00482252 e87977faff               call       0x4299d0
00482257 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0048225a 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
0048225d 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00482260 8991c8050000             mov        dword ptr [ecx + 0x5c8], edx
00482266 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00482269 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
0048226c 898d78ffffff             mov        dword ptr [ebp - 0x88], ecx
00482272 6880d54700               push       0x47d580
00482277 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
0048227d e8ae67fbff               call       0x438a30
00482282 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
00482285 8b828c6d2800             mov        eax, dword ptr [edx + 0x286d8c]
0048228b 89856cffffff             mov        dword ptr [ebp - 0x94], eax
00482291 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00482294 8b11                     mov        edx, dword ptr [ecx]
00482296 52                       push       edx
00482297 e824340000               call       0x4856c0
0048229c 83c404                   add        esp, 4
0048229f 898574ffffff             mov        dword ptr [ebp - 0x8c], eax
004822a5 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004822a8 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004822ab 898d70ffffff             mov        dword ptr [ebp - 0x90], ecx
004822b1 8b9574ffffff             mov        edx, dword ptr [ebp - 0x8c]
004822b7 52                       push       edx
004822b8 8b8570ffffff             mov        eax, dword ptr [ebp - 0x90]
004822be 50                       push       eax
004822bf 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
004822c5 e8d660fbff               call       0x4383a0
004822ca 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004822cd 8b5114                   mov        edx, dword ptr [ecx + 0x14]
004822d0 83ca10                   or         edx, 0x10
004822d3 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004822d6 895014                   mov        dword ptr [eax + 0x14], edx
004822d9 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004822dc 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
004822df 8a82a3040000             mov        al, byte ptr [edx + 0x4a3]
004822e5 24fc                     and        al, 0xfc
004822e7 0c01                     or         al, 1
004822e9 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004822ec 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
004822ef 8882a3040000             mov        byte ptr [edx + 0x4a3], al
004822f5 8b4508                   mov        eax, dword ptr [ebp + 8]
004822f8 690858010000             imul       ecx, dword ptr [eax], 0x158
004822fe 83b9fc075c0000           cmp        dword ptr [ecx + 0x5c07fc], 0
00482305 7466                     je         0x48236d
00482307 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
0048230a 8b8aa06d2800             mov        ecx, dword ptr [edx + 0x286da0]
00482310 e8eb9ff8ff               call       0x40c300
00482315 8b808c6d2800             mov        eax, dword ptr [eax + 0x286d8c]
0048231b 898564ffffff             mov        dword ptr [ebp - 0x9c], eax
00482321 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00482324 691158010000             imul       edx, dword ptr [ecx], 0x158
0048232a 8b82fc075c00             mov        eax, dword ptr [edx + 0x5c07fc]
00482330 898568ffffff             mov        dword ptr [ebp - 0x98], eax
00482336 6a00                     push       0
00482338 6aff                     push       -1
0048233a 51                       push       ecx
0048233b 0f57c0                   xorps      xmm0, xmm0
0048233e f30f110424               movss      dword ptr [esp], xmm0
00482343 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00482346 83c164                   add        ecx, 0x64
00482349 51                       push       ecx
0048234a 8b9568ffffff             mov        edx, dword ptr [ebp - 0x98]
00482350 52                       push       edx
00482351 6a00                     push       0
00482353 8d8534ffffff             lea        eax, [ebp - 0xcc]
00482359 50                       push       eax
0048235a 8b8d64ffffff             mov        ecx, dword ptr [ebp - 0x9c]
00482360 e87b6dffff               call       0x4790e0
00482365 8b08                     mov        ecx, dword ptr [eax]
00482367 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0048236a 894a5c                   mov        dword ptr [edx + 0x5c], ecx
0048236d 8b4508                   mov        eax, dword ptr [ebp + 8]
00482370 690858010000             imul       ecx, dword ptr [eax], 0x158
00482376 81c1a8065c00             add        ecx, 0x5c06a8
0048237c 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0048237f 894a58                   mov        dword ptr [edx + 0x58], ecx
00482382 688080d0ff               push       0xffd08080
00482387 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0048238a 81c188000000             add        ecx, 0x88
00482390 e8bbb2fdff               call       0x45d650
00482395 90                       nop        
00482396 8b4508                   mov        eax, dword ptr [ebp + 8]
00482399 690858010000             imul       ecx, dword ptr [eax], 0x158
0048239f 8b91f4075c00             mov        edx, dword ptr [ecx + 0x5c07f4]
004823a5 8955b0                   mov        dword ptr [ebp - 0x50], edx
004823a8 837db00a                 cmp        dword ptr [ebp - 0x50], 0xa
004823ac 0f874f010000             ja         0x482501
004823b2 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
004823b5 ff2485ac274800           jmp        dword ptr [eax*4 + 0x4827ac]
004823bc 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004823bf 8b5104                   mov        edx, dword ptr [ecx + 4]
004823c2 8d441206                 lea        eax, [edx + edx + 6]
004823c6 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004823c9 894134                   mov        dword ptr [ecx + 0x34], eax
004823cc e930010000               jmp        0x482501
004823d1 8b5508                   mov        edx, dword ptr [ebp + 8]
004823d4 8b4204                   mov        eax, dword ptr [edx + 4]
004823d7 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004823da 8b1485f8ef5a00           mov        edx, dword ptr [eax*4 + 0x5aeff8]
004823e1 895134                   mov        dword ptr [ecx + 0x34], edx
004823e4 e918010000               jmp        0x482501
004823e9 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004823ec c74034ffffffff           mov        dword ptr [eax + 0x34], 0xffffffff
004823f3 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004823f6 8b5114                   mov        edx, dword ptr [ecx + 0x14]
004823f9 83ca10                   or         edx, 0x10
004823fc 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004823ff 895014                   mov        dword ptr [eax + 0x14], edx
00482402 e9fa000000               jmp        0x482501
00482407 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0048240a c7413412000000           mov        dword ptr [ecx + 0x34], 0x12
00482411 e9eb000000               jmp        0x482501
00482416 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00482419 c7423408000000           mov        dword ptr [edx + 0x34], 8
00482420 e9dc000000               jmp        0x482501
00482425 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00482428 0fbf484c                 movsx      ecx, word ptr [eax + 0x4c]
0048242c 69d158010000             imul       edx, ecx, 0x158
00482432 8b4508                   mov        eax, dword ptr [ebp + 8]
00482435 6b480414                 imul       ecx, dword ptr [eax + 4], 0x14
00482439 8d940aac065c00           lea        edx, [edx + ecx + 0x5c06ac]
00482440 b804000000               mov        eax, 4
00482445 6bc803                   imul       ecx, eax, 3
00482448 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0048244b 8b0c0a                   mov        ecx, dword ptr [edx + ecx]
0048244e 894834                   mov        dword ptr [eax + 0x34], ecx
00482451 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00482454 0fbf424c                 movsx      eax, word ptr [edx + 0x4c]
00482458 69c858010000             imul       ecx, eax, 0x158
0048245e 8b5508                   mov        edx, dword ptr [ebp + 8]
00482461 6b420414                 imul       eax, dword ptr [edx + 4], 0x14
00482465 8d8c01ac065c00           lea        ecx, [ecx + eax + 0x5c06ac]
0048246c ba04000000               mov        edx, 4
00482471 c1e202                   shl        edx, 2
00482474 8b0411                   mov        eax, dword ptr [ecx + edx]
00482477 50                       push       eax
00482478 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0048247b 81c188000000             add        ecx, 0x88
00482481 e8cab1fdff               call       0x45d650
00482486 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00482489 8b5114                   mov        edx, dword ptr [ecx + 0x14]
0048248c 83ca10                   or         edx, 0x10
0048248f 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00482492 895014                   mov        dword ptr [eax + 0x14], edx
00482495 eb6a                     jmp        0x482501
00482497 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0048249a c7413407010000           mov        dword ptr [ecx + 0x34], 0x107
004824a1 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004824a4 8b4214                   mov        eax, dword ptr [edx + 0x14]
004824a7 83c810                   or         eax, 0x10
004824aa 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004824ad 894114                   mov        dword ptr [ecx + 0x14], eax
004824b0 eb4f                     jmp        0x482501
004824b2 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004824b5 c742340a010000           mov        dword ptr [edx + 0x34], 0x10a
004824bc 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004824bf 8b4814                   mov        ecx, dword ptr [eax + 0x14]
004824c2 83c910                   or         ecx, 0x10
004824c5 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004824c8 894a14                   mov        dword ptr [edx + 0x14], ecx
004824cb eb34                     jmp        0x482501
004824cd 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004824d0 c740340d010000           mov        dword ptr [eax + 0x34], 0x10d
004824d7 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004824da 8b5114                   mov        edx, dword ptr [ecx + 0x14]
004824dd 83ca10                   or         edx, 0x10
004824e0 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004824e3 895014                   mov        dword ptr [eax + 0x14], edx
004824e6 eb19                     jmp        0x482501
004824e8 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004824eb c7413416010000           mov        dword ptr [ecx + 0x34], 0x116
004824f2 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004824f5 8b4214                   mov        eax, dword ptr [edx + 0x14]
004824f8 83c810                   or         eax, 0x10
004824fb 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004824fe 894114                   mov        dword ptr [ecx + 0x14], eax
00482501 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00482504 8b4214                   mov        eax, dword ptr [edx + 0x14]
00482507 25ffe7ffff               and        eax, 0xffffe7ff
0048250c 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0048250f 894114                   mov        dword ptr [ecx + 0x14], eax
00482512 8b5508                   mov        edx, dword ptr [ebp + 8]
00482515 690258010000             imul       eax, dword ptr [edx], 0x158
0048251b 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0048251e 8b90f0075c00             mov        edx, dword ptr [eax + 0x5c07f0]
00482524 895144                   mov        dword ptr [ecx + 0x44], edx
00482527 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0048252a 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0048252d 8b5140                   mov        edx, dword ptr [ecx + 0x40]
00482530 895040                   mov        dword ptr [eax + 0x40], edx
00482533 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00482536 c7403005000000           mov        dword ptr [eax + 0x30], 5
0048253d 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00482540 8b11                     mov        edx, dword ptr [ecx]
00482542 52                       push       edx
00482543 e8b8310000               call       0x485700
00482548 83c404                   add        esp, 4
0048254b d95dac                   fstp       dword ptr [ebp - 0x54]
0048254e 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00482551 f30f1045ac               movss      xmm0, dword ptr [ebp - 0x54]
00482556 f30f118080000000         movss      dword ptr [eax + 0x80], xmm0
0048255e 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00482561 f30f1045ac               movss      xmm0, dword ptr [ebp - 0x54]
00482566 f30f11417c               movss      dword ptr [ecx + 0x7c], xmm0
0048256b 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0048256e 33c0                     xor        eax, eax
00482570 33c9                     xor        ecx, ecx
00482572 898290000000             mov        dword ptr [edx + 0x90], eax
00482578 898a94000000             mov        dword ptr [edx + 0x94], ecx
0048257e 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00482581 c7423c00000000           mov        dword ptr [edx + 0x3c], 0
00482588 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0048258b 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0048258e 8b5144                   mov        edx, dword ptr [ecx + 0x44]
00482591 895038                   mov        dword ptr [eax + 0x38], edx
00482594 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00482597 83c104                   add        ecx, 4
0048259a e861400000               call       0x486600
0048259f 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004825a2 394138                   cmp        dword ptr [ecx + 0x38], eax
004825a5 0f83ed000000             jae        0x482698
004825ab 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
004825ae 83c204                   add        edx, 4
004825b1 899560ffffff             mov        dword ptr [ebp - 0xa0], edx
004825b7 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004825ba 8b4838                   mov        ecx, dword ptr [eax + 0x38]
004825bd 51                       push       ecx
004825be 8b8d60ffffff             mov        ecx, dword ptr [ebp - 0xa0]
004825c4 e8f7a0ffff               call       0x47c6c0
004825c9 83782001                 cmp        dword ptr [eax + 0x20], 1
004825cd 0f85c5000000             jne        0x482698
004825d3 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
004825d6 83c204                   add        edx, 4
004825d9 89955cffffff             mov        dword ptr [ebp - 0xa4], edx
004825df 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004825e2 8b4838                   mov        ecx, dword ptr [eax + 0x38]
004825e5 51                       push       ecx
004825e6 8b8d5cffffff             mov        ecx, dword ptr [ebp - 0xa4]
004825ec e8cfa0ffff               call       0x47c6c0
004825f1 0fbf5010                 movsx      edx, word ptr [eax + 0x10]
004825f5 83fa01                   cmp        edx, 1
004825f8 744a                     je         0x482644
004825fa 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004825fd 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
00482600 898d50ffffff             mov        dword ptr [ebp - 0xb0], ecx
00482606 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
00482609 83c204                   add        edx, 4
0048260c 899558ffffff             mov        dword ptr [ebp - 0xa8], edx
00482612 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00482615 8b4838                   mov        ecx, dword ptr [eax + 0x38]
00482618 51                       push       ecx
00482619 8b8d58ffffff             mov        ecx, dword ptr [ebp - 0xa8]
0048261f e89ca0ffff               call       0x47c6c0
00482624 0fbf5010                 movsx      edx, word ptr [eax + 0x10]
00482628 83c207                   add        edx, 7
0048262b 899554ffffff             mov        dword ptr [ebp - 0xac], edx
00482631 8b8554ffffff             mov        eax, dword ptr [ebp - 0xac]
00482637 50                       push       eax
00482638 8b8d50ffffff             mov        ecx, dword ptr [ebp - 0xb0]
0048263e e8ad62fbff               call       0x4388f0
00482643 90                       nop        
00482644 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00482647 c7415002000000           mov        dword ptr [ecx + 0x50], 2
0048264e 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00482651 83c270                   add        edx, 0x70
00482654 89954cffffff             mov        dword ptr [ebp - 0xb4], edx
0048265a 51                       push       ecx
0048265b f30f1005e0c85600         movss      xmm0, dword ptr [0x56c8e0]
00482663 f30f110424               movss      dword ptr [esp], xmm0
00482668 8d8528ffffff             lea        eax, [ebp - 0xd8]
0048266e 50                       push       eax
0048266f 8b8d4cffffff             mov        ecx, dword ptr [ebp - 0xb4]
00482675 e8766cfaff               call       0x4292f0
0048267a 50                       push       eax
0048267b 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0048267e 83c164                   add        ecx, 0x64
00482681 e8ba70faff               call       0x429740
00482686 90                       nop        
00482687 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0048268a 8b5138                   mov        edx, dword ptr [ecx + 0x38]
0048268d 83c201                   add        edx, 1
00482690 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00482693 895038                   mov        dword ptr [eax + 0x38], edx
00482696 eb1a                     jmp        0x4826b2
00482698 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0048269b 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
0048269e 899548ffffff             mov        dword ptr [ebp - 0xb8], edx
004826a4 6a02                     push       2
004826a6 8b8d48ffffff             mov        ecx, dword ptr [ebp - 0xb8]
004826ac e83f62fbff               call       0x4388f0
004826b1 90                       nop        
004826b2 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004826b5 e836b6ffff               call       0x47dcf0
004826ba 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004826bd 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004826c0 e80b8ffaff               call       0x42b5d0
004826c5 90                       nop        
004826c6 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004826c9 f30f104148               movss      xmm0, dword ptr [ecx + 0x48]
004826ce 0f2f05e8e05600           comiss     xmm0, dword ptr [0x56e0e8]
004826d5 7666                     jbe        0x48273d
004826d7 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
004826da 8b8aa06d2800             mov        ecx, dword ptr [edx + 0x286da0]
004826e0 e8abd8f8ff               call       0x40ff90
004826e5 8bc8                     mov        ecx, eax
004826e7 e864e1fdff               call       0x460850
004826ec 50                       push       eax
004826ed 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004826f0 83c064                   add        eax, 0x64
004826f3 50                       push       eax
004826f4 e8d739fdff               call       0x4560d0
004826f9 83c408                   add        esp, 8
004826fc d99d44ffffff             fstp       dword ptr [ebp - 0xbc]
00482702 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00482705 f30f104148               movss      xmm0, dword ptr [ecx + 0x48]
0048270a 0f2f8544ffffff           comiss     xmm0, dword ptr [ebp - 0xbc]
00482711 762a                     jbe        0x48273d
00482713 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00482716 e825b3ffff               call       0x47da40
0048271b 90                       nop        
0048271c c78540ffffffffffffff     mov        dword ptr [ebp - 0xc0], 0xffffffff
00482726 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0048272d 8d4d0c                   lea        ecx, [ebp + 0xc]
00482730 e8cb9bffff               call       0x47c300
00482735 8b8540ffffff             mov        eax, dword ptr [ebp - 0xc0]
0048273b eb1f                     jmp        0x48275c
0048273d c7853cffffff00000000     mov        dword ptr [ebp - 0xc4], 0
00482747 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0048274e 8d4d0c                   lea        ecx, [ebp + 0xc]
00482751 e8aa9bffff               call       0x47c300
00482756 8b853cffffff             mov        eax, dword ptr [ebp - 0xc4]
0048275c 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0048275f 64890d00000000           mov        dword ptr fs:[0], ecx
00482766 59                       pop        ecx
00482767 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0048276a 33cd                     xor        ecx, ebp
0048276c e87d020c00               call       0x5429ee
00482771 8be5                     mov        esp, ebp
00482773 5d                       pop        ebp
00482774 c21800                   ret        0x18
00482777 90                       nop        
00482778 831a48                   sbb        dword ptr [edx], 0x48
0048277b 00831a480034             add        byte ptr [ebx + 0x3400481a], al
00482781 1b4800                   sbb        ecx, dword ptr [eax]
00482784 43                       inc        ebx
00482785 1b4800                   sbb        ecx, dword ptr [eax]
00482788 99                       cdq        
00482789 1b4800                   sbb        ecx, dword ptr [eax]
0048278c a81b                     test       al, 0x1b
0048278e 48                       dec        eax
0048278f 001f                     add        byte ptr [edi], bl
00482791 1c48                     sbb        al, 0x48
00482793 00511c                   add        byte ptr [ecx + 0x1c], dl
00482796 48                       dec        eax
00482797 00d4                     add        ah, dl
00482799 1c48                     sbb        al, 0x48
0048279b 0033                     add        byte ptr [ebx], dh
0048279d 1d4800331d               sbb        eax, 0x1d330048
004827a2 48                       dec        eax
004827a3 00a81e480088             add        byte ptr [eax - 0x77ffb7e2], ch
004827a9 1f                       pop        ds
004827aa 48                       dec        eax
004827ab 00bc234800d123           add        byte ptr [ebx + 0x23d10048], bh
004827b2 48                       dec        eax
004827b3 00e9                     add        cl, ch
004827b5 234800                   and        ecx, dword ptr [eax]
004827b8 07                       pop        es
004827b9 2448                     and        al, 0x48
004827bb 0016                     add        byte ptr [esi], dl
004827bd 2448                     and        al, 0x48
004827bf 0001                     add        byte ptr [ecx], al
004827c1 2548002524               and        eax, 0x24250048
004827c6 48                       dec        eax
004827c7 0097244800b2             add        byte ptr [edi - 0x4dffb7dc], dl
004827cd 2448                     and        al, 0x48
004827cf 00cd                     add        ch, cl
004827d1 2448                     and        al, 0x48
004827d3 00e8                     add        al, ch
004827d5 2448                     and        al, 0x48
004827d7 00cc                     add        ah, cl
004827d9 cc                       int3       
004827da cc                       int3       
004827db cc                       int3       
004827dc cc                       int3       
004827dd cc                       int3       
004827de cc                       int3       
004827df cc                       int3       
