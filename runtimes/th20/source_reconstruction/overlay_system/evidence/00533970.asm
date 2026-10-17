00533970 55                       push       ebp
00533971 8bec                     mov        ebp, esp
00533973 81ec84000000             sub        esp, 0x84
00533979 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0053397e 33c5                     xor        eax, ebp
00533980 8945fc                   mov        dword ptr [ebp - 4], eax
00533983 56                       push       esi
00533984 894dd8                   mov        dword ptr [ebp - 0x28], ecx
00533987 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
0053398a 83784000                 cmp        dword ptr [eax + 0x40], 0
0053398e 0f84c1060000             je         0x534055
00533994 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00533997 83c144                   add        ecx, 0x44
0053399a 894dbc                   mov        dword ptr [ebp - 0x44], ecx
0053399d 6a50                     push       0x50
0053399f 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
005339a2 e819fceeff               call       0x4235c0
005339a7 0fb6d0                   movzx      edx, al
005339aa 85d2                     test       edx, edx
005339ac 7421                     je         0x5339cf
005339ae 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
005339b1 8b4840                   mov        ecx, dword ptr [eax + 0x40]
005339b4 51                       push       ecx
005339b5 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
005339bb e840eef6ff               call       0x4a2800
005339c0 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
005339c3 c7424000000000           mov        dword ptr [edx + 0x40], 0
005339ca e986060000               jmp        0x534055
005339cf 6a00                     push       0
005339d1 e8aa06f3ff               call       0x464080
005339d6 83c404                   add        esp, 4
005339d9 8bc8                     mov        ecx, eax
005339db e8d090eeff               call       0x41cab0
005339e0 83f807                   cmp        eax, 7
005339e3 7416                     je         0x5339fb
005339e5 6a00                     push       0
005339e7 e844cef2ff               call       0x460830
005339ec 83c404                   add        esp, 4
005339ef 8bc8                     mov        ecx, eax
005339f1 e85acef2ff               call       0x460850
005339f6 8945c0                   mov        dword ptr [ebp - 0x40], eax
005339f9 eb16                     jmp        0x533a11
005339fb 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
005339fe 8b4828                   mov        ecx, dword ptr [eax + 0x28]
00533a01 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
00533a04 8b01                     mov        eax, dword ptr [ecx]
00533a06 8b4a28                   mov        ecx, dword ptr [edx + 0x28]
00533a09 8b5060                   mov        edx, dword ptr [eax + 0x60]
00533a0c ffd2                     call       edx
00533a0e 8945c0                   mov        dword ptr [ebp - 0x40], eax
00533a11 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
00533a14 8945b8                   mov        dword ptr [ebp - 0x48], eax
00533a17 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00533a1a 8b11                     mov        edx, dword ptr [ecx]
00533a1c 8955ec                   mov        dword ptr [ebp - 0x14], edx
00533a1f 8b4104                   mov        eax, dword ptr [ecx + 4]
00533a22 8945f0                   mov        dword ptr [ebp - 0x10], eax
00533a25 8b4908                   mov        ecx, dword ptr [ecx + 8]
00533a28 894df4                   mov        dword ptr [ebp - 0xc], ecx
00533a2b f30f1005d00a5700         movss      xmm0, dword ptr [0x570ad0]
00533a33 f30f11857cffffff         movss      dword ptr [ebp - 0x84], xmm0
00533a3b 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
00533a3e 8b4240                   mov        eax, dword ptr [edx + 0x40]
00533a41 8945b4                   mov        dword ptr [ebp - 0x4c], eax
00533a44 f30f1005d00a5700         movss      xmm0, dword ptr [0x570ad0]
00533a4c f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
00533a54 51                       push       ecx
00533a55 f30f110424               movss      dword ptr [esp], xmm0
00533a5a f30f1005d00a5700         movss      xmm0, dword ptr [0x570ad0]
00533a62 f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
00533a6a 51                       push       ecx
00533a6b f30f110424               movss      dword ptr [esp], xmm0
00533a70 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00533a75 f30f5c05d00a5700         subss      xmm0, dword ptr [0x570ad0]
00533a7d 51                       push       ecx
00533a7e f30f110424               movss      dword ptr [esp], xmm0
00533a83 f30f1045ec               movss      xmm0, dword ptr [ebp - 0x14]
00533a88 f30f5c05d00a5700         subss      xmm0, dword ptr [0x570ad0]
00533a90 51                       push       ecx
00533a91 f30f110424               movss      dword ptr [esp], xmm0
00533a96 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00533a99 e8b29df6ff               call       0x49d850
00533a9e 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00533aa1 8b517c                   mov        edx, dword ptr [ecx + 0x7c]
00533aa4 52                       push       edx
00533aa5 b958675b00               mov        ecx, 0x5b6758
00533aaa e851a4f6ff               call       0x49df00
00533aaf f30f2ac0                 cvtsi2ss   xmm0, eax
00533ab3 f30f5845ec               addss      xmm0, dword ptr [ebp - 0x14]
00533ab8 f30f1145ec               movss      dword ptr [ebp - 0x14], xmm0
00533abd 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
00533ac0 8b487c                   mov        ecx, dword ptr [eax + 0x7c]
00533ac3 51                       push       ecx
00533ac4 b958675b00               mov        ecx, 0x5b6758
00533ac9 e852a4f6ff               call       0x49df20
00533ace f30f2ac0                 cvtsi2ss   xmm0, eax
00533ad2 f30f5845f0               addss      xmm0, dword ptr [ebp - 0x10]
00533ad7 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00533adc 8d4ddc                   lea        ecx, [ebp - 0x24]
00533adf e8fc42f1ff               call       0x447de0
00533ae4 8d4df8                   lea        ecx, [ebp - 8]
00533ae7 e8f442f1ff               call       0x447de0
00533aec 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00533aef 83c144                   add        ecx, 0x44
00533af2 e899c4edff               call       0x40ff90
00533af7 f30f2ac0                 cvtsi2ss   xmm0, eax
00533afb f30f5905f0e05600         mulss      xmm0, dword ptr [0x56e0f0]
00533b03 f30f5e0534e75600         divss      xmm0, dword ptr [0x56e734]
00533b0b 51                       push       ecx
00533b0c f30f110424               movss      dword ptr [esp], xmm0
00533b11 8d4ddc                   lea        ecx, [ebp - 0x24]
00533b14 e807f4f1ff               call       0x452f20
00533b19 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
00533b1c 8b4240                   mov        eax, dword ptr [edx + 0x40]
00533b1f 8b4818                   mov        ecx, dword ptr [eax + 0x18]
00533b22 894dcc                   mov        dword ptr [ebp - 0x34], ecx
00533b25 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
00533b28 8b4240                   mov        eax, dword ptr [edx + 0x40]
00533b2b 8b4814                   mov        ecx, dword ptr [eax + 0x14]
00533b2e 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
00533b31 c745c800000000           mov        dword ptr [ebp - 0x38], 0
00533b38 eb09                     jmp        0x533b43
00533b3a 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
00533b3d 83c201                   add        edx, 1
00533b40 8955c8                   mov        dword ptr [ebp - 0x38], edx
00533b43 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
00533b46 8b4840                   mov        ecx, dword ptr [eax + 0x40]
00533b49 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
00533b4c 3b11                     cmp        edx, dword ptr [ecx]
00533b4e 0f8de2040000             jge        0x534036
00533b54 c745c400000000           mov        dword ptr [ebp - 0x3c], 0
00533b5b eb09                     jmp        0x533b66
00533b5d 8b45c4                   mov        eax, dword ptr [ebp - 0x3c]
00533b60 83c001                   add        eax, 1
00533b63 8945c4                   mov        dword ptr [ebp - 0x3c], eax
00533b66 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00533b69 8b5140                   mov        edx, dword ptr [ecx + 0x40]
00533b6c 8b45c4                   mov        eax, dword ptr [ebp - 0x3c]
00533b6f 3b4204                   cmp        eax, dword ptr [edx + 4]
00533b72 0f8db9040000             jge        0x534031
00533b78 8d4dec                   lea        ecx, [ebp - 0x14]
00533b7b 51                       push       ecx
00533b7c 8d55e0                   lea        edx, [ebp - 0x20]
00533b7f 52                       push       edx
00533b80 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00533b83 e8b858efff               call       0x429440
00533b88 8d45e0                   lea        eax, [ebp - 0x20]
00533b8b 50                       push       eax
00533b8c e80f67f4ff               call       0x47a2a0
00533b91 83c404                   add        esp, 4
00533b94 d95dd0                   fstp       dword ptr [ebp - 0x30]
00533b97 f30f1005a4cd5600         movss      xmm0, dword ptr [0x56cda4]
00533b9f f30f580594cd5600         addss      xmm0, dword ptr [0x56cd94]
00533ba7 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00533bae f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00533bb6 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00533bb9 8b517c                   mov        edx, dword ptr [ecx + 0x7c]
00533bbc 52                       push       edx
00533bbd b958675b00               mov        ecx, 0x5b6758
00533bc2 f30f1145b0               movss      dword ptr [ebp - 0x50], xmm0
00533bc7 e834a3f6ff               call       0x49df00
00533bcc f30f2ac0                 cvtsi2ss   xmm0, eax
00533bd0 f30f104db0               movss      xmm1, dword ptr [ebp - 0x50]
00533bd5 f30f58c8                 addss      xmm1, xmm0
00533bd9 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
00533bdc 0f2f08                   comiss     xmm1, dword ptr [eax]
00533bdf 0f87a8000000             ja         0x533c8d
00533be5 f30f1005a4cd5600         movss      xmm0, dword ptr [0x56cda4]
00533bed f30f580594cd5600         addss      xmm0, dword ptr [0x56cd94]
00533bf5 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00533bfd 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00533c00 8b517c                   mov        edx, dword ptr [ecx + 0x7c]
00533c03 52                       push       edx
00533c04 b958675b00               mov        ecx, 0x5b6758
00533c09 f30f1145ac               movss      dword ptr [ebp - 0x54], xmm0
00533c0e e8eda2f6ff               call       0x49df00
00533c13 f30f2ac0                 cvtsi2ss   xmm0, eax
00533c17 f30f104dac               movss      xmm1, dword ptr [ebp - 0x54]
00533c1c f30f58c8                 addss      xmm1, xmm0
00533c20 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
00533c23 f30f1000                 movss      xmm0, dword ptr [eax]
00533c27 0f2fc1                   comiss     xmm0, xmm1
00533c2a 7761                     ja         0x533c8d
00533c2c 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00533c2f 8b517c                   mov        edx, dword ptr [ecx + 0x7c]
00533c32 52                       push       edx
00533c33 b958675b00               mov        ecx, 0x5b6758
00533c38 e8e3a2f6ff               call       0x49df20
00533c3d 83e820                   sub        eax, 0x20
00533c40 f30f2ac0                 cvtsi2ss   xmm0, eax
00533c44 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
00533c47 0f2f4004                 comiss     xmm0, dword ptr [eax + 4]
00533c4b 7740                     ja         0x533c8d
00533c4d f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
00533c55 f30f580594cd5600         addss      xmm0, dword ptr [0x56cd94]
00533c5d 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00533c60 8b517c                   mov        edx, dword ptr [ecx + 0x7c]
00533c63 52                       push       edx
00533c64 b958675b00               mov        ecx, 0x5b6758
00533c69 f30f1145a8               movss      dword ptr [ebp - 0x58], xmm0
00533c6e e8ada2f6ff               call       0x49df20
00533c73 f30f2ac0                 cvtsi2ss   xmm0, eax
00533c77 f30f104da8               movss      xmm1, dword ptr [ebp - 0x58]
00533c7c f30f58c8                 addss      xmm1, xmm0
00533c80 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
00533c83 f30f104004               movss      xmm0, dword ptr [eax + 4]
00533c88 0f2fc1                   comiss     xmm0, xmm1
00533c8b 760a                     jbe        0x533c97
00533c8d e988030000               jmp        0x53401a
00533c92 e983030000               jmp        0x53401a
00533c97 f30f1005d00a5700         movss      xmm0, dword ptr [0x570ad0]
00533c9f 0f2f45d0                 comiss     xmm0, dword ptr [ebp - 0x30]
00533ca3 0f826a030000             jb         0x534013
00533ca9 f30f1045d0               movss      xmm0, dword ptr [ebp - 0x30]
00533cae f30f5e05d00a5700         divss      xmm0, dword ptr [0x570ad0]
00533cb6 f30f1145d0               movss      dword ptr [ebp - 0x30], xmm0
00533cbb 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00533cbe c74110ffffffff           mov        dword ptr [ecx + 0x10], 0xffffffff
00533cc5 f30f1045d0               movss      xmm0, dword ptr [ebp - 0x30]
00533cca 0f2f0560315700           comiss     xmm0, dword ptr [0x573160]
00533cd1 7633                     jbe        0x533d06
00533cd3 f30f1045d0               movss      xmm0, dword ptr [ebp - 0x30]
00533cd8 f30f5c0560315700         subss      xmm0, dword ptr [0x573160]
00533ce0 f30f59059ccd5600         mulss      xmm0, dword ptr [0x56cd9c]
00533ce8 f30f5e05f8fb5600         divss      xmm0, dword ptr [0x56fbf8]
00533cf0 f30f100d9ccd5600         movss      xmm1, dword ptr [0x56cd9c]
00533cf8 f30f5cc8                 subss      xmm1, xmm0
00533cfc f30f2cd1                 cvttss2si  edx, xmm1
00533d00 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00533d03 885013                   mov        byte ptr [eax + 0x13], dl
00533d06 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00533d09 83c144                   add        ecx, 0x44
00533d0c 894da4                   mov        dword ptr [ebp - 0x5c], ecx
00533d0f 6a3c                     push       0x3c
00533d11 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
00533d14 e857d3f2ff               call       0x461070
00533d19 0fb6d0                   movzx      edx, al
00533d1c 85d2                     test       edx, edx
00533d1e 742c                     je         0x533d4c
00533d20 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00533d23 0fb67013                 movzx      esi, byte ptr [eax + 0x13]
00533d27 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00533d2a 83c144                   add        ecx, 0x44
00533d2d e85ec2edff               call       0x40ff90
00533d32 b950000000               mov        ecx, 0x50
00533d37 2bc8                     sub        ecx, eax
00533d39 0faff1                   imul       esi, ecx
00533d3c 8bc6                     mov        eax, esi
00533d3e 99                       cdq        
00533d3f b914000000               mov        ecx, 0x14
00533d44 f7f9                     idiv       ecx
00533d46 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
00533d49 884213                   mov        byte ptr [edx + 0x13], al
00533d4c f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00533d54 f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
00533d5c f30f5945d0               mulss      xmm0, dword ptr [ebp - 0x30]
00533d61 51                       push       ecx
00533d62 f30f110424               movss      dword ptr [esp], xmm0
00533d67 8d4df8                   lea        ecx, [ebp - 8]
00533d6a e8b1f1f1ff               call       0x452f20
00533d6f 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00533d72 83c144                   add        ecx, 0x44
00533d75 e816c2edff               call       0x40ff90
00533d7a f30f2ac0                 cvtsi2ss   xmm0, eax
00533d7e f30f5905f0e05600         mulss      xmm0, dword ptr [0x56e0f0]
00533d86 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00533d8e f30f5e0548e05600         divss      xmm0, dword ptr [0x56e048]
00533d96 51                       push       ecx
00533d97 f30f110424               movss      dword ptr [esp], xmm0
00533d9c 8d4df8                   lea        ecx, [ebp - 8]
00533d9f e8bce2f3ff               call       0x472060
00533da4 8d4df8                   lea        ecx, [ebp - 8]
00533da7 e83455efff               call       0x4292e0
00533dac 51                       push       ecx
00533dad d91c24                   fstp       dword ptr [esp]
00533db0 e86b5af0ff               call       0x439820
00533db5 83c404                   add        esp, 4
00533db8 d95da0                   fstp       dword ptr [ebp - 0x60]
00533dbb f30f1045a0               movss      xmm0, dword ptr [ebp - 0x60]
00533dc0 f30f5945d0               mulss      xmm0, dword ptr [ebp - 0x30]
00533dc5 f30f5905ece05600         mulss      xmm0, dword ptr [0x56e0ec]
00533dcd f30f5945e0               mulss      xmm0, dword ptr [ebp - 0x20]
00533dd2 f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
00533dd7 8d4df8                   lea        ecx, [ebp - 8]
00533dda e80155efff               call       0x4292e0
00533ddf 51                       push       ecx
00533de0 d91c24                   fstp       dword ptr [esp]
00533de3 e8385af0ff               call       0x439820
00533de8 83c404                   add        esp, 4
00533deb d95d9c                   fstp       dword ptr [ebp - 0x64]
00533dee f30f10459c               movss      xmm0, dword ptr [ebp - 0x64]
00533df3 f30f5945d0               mulss      xmm0, dword ptr [ebp - 0x30]
00533df8 f30f5905ece05600         mulss      xmm0, dword ptr [0x56e0ec]
00533e00 f30f5945e4               mulss      xmm0, dword ptr [ebp - 0x1c]
00533e05 f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
00533e0a 8d45e0                   lea        eax, [ebp - 0x20]
00533e0d 50                       push       eax
00533e0e 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00533e11 e8ca58efff               call       0x4296e0
00533e16 90                       nop        
00533e17 f30f1005a4cd5600         movss      xmm0, dword ptr [0x56cda4]
00533e1f f30f580594cd5600         addss      xmm0, dword ptr [0x56cd94]
00533e27 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00533e2e f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00533e36 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00533e39 8b517c                   mov        edx, dword ptr [ecx + 0x7c]
00533e3c 52                       push       edx
00533e3d b958675b00               mov        ecx, 0x5b6758
00533e42 f30f114598               movss      dword ptr [ebp - 0x68], xmm0
00533e47 e8b4a0f6ff               call       0x49df00
00533e4c f30f2ac0                 cvtsi2ss   xmm0, eax
00533e50 f30f104d98               movss      xmm1, dword ptr [ebp - 0x68]
00533e55 f30f58c8                 addss      xmm1, xmm0
00533e59 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00533e5c 0f2f08                   comiss     xmm1, dword ptr [eax]
00533e5f 764e                     jbe        0x533eaf
00533e61 f30f1005a4cd5600         movss      xmm0, dword ptr [0x56cda4]
00533e69 f30f580594cd5600         addss      xmm0, dword ptr [0x56cd94]
00533e71 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00533e78 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00533e80 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00533e83 8b517c                   mov        edx, dword ptr [ecx + 0x7c]
00533e86 52                       push       edx
00533e87 b958675b00               mov        ecx, 0x5b6758
00533e8c f30f114594               movss      dword ptr [ebp - 0x6c], xmm0
00533e91 e86aa0f6ff               call       0x49df00
00533e96 f30f2ac0                 cvtsi2ss   xmm0, eax
00533e9a f30f104d94               movss      xmm1, dword ptr [ebp - 0x6c]
00533e9f f30f58c8                 addss      xmm1, xmm0
00533ea3 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00533ea6 f30f1108                 movss      dword ptr [eax], xmm1
00533eaa e989000000               jmp        0x533f38
00533eaf f30f1005a4cd5600         movss      xmm0, dword ptr [0x56cda4]
00533eb7 f30f580594cd5600         addss      xmm0, dword ptr [0x56cd94]
00533ebf f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00533ec7 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00533eca 8b517c                   mov        edx, dword ptr [ecx + 0x7c]
00533ecd 52                       push       edx
00533ece b958675b00               mov        ecx, 0x5b6758
00533ed3 f30f114590               movss      dword ptr [ebp - 0x70], xmm0
00533ed8 e823a0f6ff               call       0x49df00
00533edd f30f2ac0                 cvtsi2ss   xmm0, eax
00533ee1 f30f104d90               movss      xmm1, dword ptr [ebp - 0x70]
00533ee6 f30f58c8                 addss      xmm1, xmm0
00533eea 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00533eed f30f1000                 movss      xmm0, dword ptr [eax]
00533ef1 0f2fc1                   comiss     xmm0, xmm1
00533ef4 7642                     jbe        0x533f38
00533ef6 f30f1005a4cd5600         movss      xmm0, dword ptr [0x56cda4]
00533efe f30f580594cd5600         addss      xmm0, dword ptr [0x56cd94]
00533f06 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00533f0e 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00533f11 8b517c                   mov        edx, dword ptr [ecx + 0x7c]
00533f14 52                       push       edx
00533f15 b958675b00               mov        ecx, 0x5b6758
00533f1a f30f11458c               movss      dword ptr [ebp - 0x74], xmm0
00533f1f e8dc9ff6ff               call       0x49df00
00533f24 f30f2ac0                 cvtsi2ss   xmm0, eax
00533f28 f30f104d8c               movss      xmm1, dword ptr [ebp - 0x74]
00533f2d f30f58c8                 addss      xmm1, xmm0
00533f31 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00533f34 f30f1108                 movss      dword ptr [eax], xmm1
00533f38 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00533f3b 8b517c                   mov        edx, dword ptr [ecx + 0x7c]
00533f3e 52                       push       edx
00533f3f b958675b00               mov        ecx, 0x5b6758
00533f44 e8d79ff6ff               call       0x49df20
00533f49 83e820                   sub        eax, 0x20
00533f4c f30f2ac0                 cvtsi2ss   xmm0, eax
00533f50 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00533f53 0f2f4004                 comiss     xmm0, dword ptr [eax + 4]
00533f57 7627                     jbe        0x533f80
00533f59 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00533f5c 8b517c                   mov        edx, dword ptr [ecx + 0x7c]
00533f5f 52                       push       edx
00533f60 b958675b00               mov        ecx, 0x5b6758
00533f65 e8b69ff6ff               call       0x49df20
00533f6a f30f2ac0                 cvtsi2ss   xmm0, eax
00533f6e f30f580560635700         addss      xmm0, dword ptr [0x576360]
00533f76 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00533f79 f30f114004               movss      dword ptr [eax + 4], xmm0
00533f7e eb7b                     jmp        0x533ffb
00533f80 f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
00533f88 f30f580594cd5600         addss      xmm0, dword ptr [0x56cd94]
00533f90 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00533f93 8b517c                   mov        edx, dword ptr [ecx + 0x7c]
00533f96 52                       push       edx
00533f97 b958675b00               mov        ecx, 0x5b6758
00533f9c f30f114588               movss      dword ptr [ebp - 0x78], xmm0
00533fa1 e87a9ff6ff               call       0x49df20
00533fa6 f30f2ac0                 cvtsi2ss   xmm0, eax
00533faa f30f104d88               movss      xmm1, dword ptr [ebp - 0x78]
00533faf f30f58c8                 addss      xmm1, xmm0
00533fb3 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00533fb6 f30f104004               movss      xmm0, dword ptr [eax + 4]
00533fbb 0f2fc1                   comiss     xmm0, xmm1
00533fbe 763b                     jbe        0x533ffb
00533fc0 f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
00533fc8 f30f580594cd5600         addss      xmm0, dword ptr [0x56cd94]
00533fd0 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00533fd3 8b517c                   mov        edx, dword ptr [ecx + 0x7c]
00533fd6 52                       push       edx
00533fd7 b958675b00               mov        ecx, 0x5b6758
00533fdc f30f114584               movss      dword ptr [ebp - 0x7c], xmm0
00533fe1 e83a9ff6ff               call       0x49df20
00533fe6 f30f2ac0                 cvtsi2ss   xmm0, eax
00533fea f30f104d84               movss      xmm1, dword ptr [ebp - 0x7c]
00533fef f30f58c8                 addss      xmm1, xmm0
00533ff3 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00533ff6 f30f114804               movss      dword ptr [eax + 4], xmm1
00533ffb 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00533ffe 0f57c0                   xorps      xmm0, xmm0
00534001 f30f114108               movss      dword ptr [ecx + 8], xmm0
00534006 8b55cc                   mov        edx, dword ptr [ebp - 0x34]
00534009 0f57c0                   xorps      xmm0, xmm0
0053400c f30f114208               movss      dword ptr [edx + 8], xmm0
00534011 eb07                     jmp        0x53401a
00534013 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00534016 c6401300                 mov        byte ptr [eax + 0x13], 0
0053401a 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0053401d 83c10c                   add        ecx, 0xc
00534020 894dcc                   mov        dword ptr [ebp - 0x34], ecx
00534023 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
00534026 83c21c                   add        edx, 0x1c
00534029 8955d4                   mov        dword ptr [ebp - 0x2c], edx
0053402c e92cfbffff               jmp        0x533b5d
00534031 e904fbffff               jmp        0x533b3a
00534036 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
00534039 8b4840                   mov        ecx, dword ptr [eax + 0x40]
0053403c e87f9df6ff               call       0x49ddc0
00534041 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00534044 83c144                   add        ecx, 0x44
00534047 894d80                   mov        dword ptr [ebp - 0x80], ecx
0053404a 6a00                     push       0
0053404c 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
0053404f e8ecf4eeff               call       0x423540
00534054 90                       nop        
00534055 5e                       pop        esi
00534056 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00534059 33cd                     xor        ecx, ebp
0053405b e88ee90000               call       0x5429ee
00534060 8be5                     mov        esp, ebp
00534062 5d                       pop        ebp
00534063 c3                       ret        
00534064 cc                       int3       
00534065 cc                       int3       
00534066 cc                       int3       
00534067 cc                       int3       
00534068 cc                       int3       
00534069 cc                       int3       
0053406a cc                       int3       
0053406b cc                       int3       
0053406c cc                       int3       
0053406d cc                       int3       
0053406e cc                       int3       
0053406f cc                       int3       
