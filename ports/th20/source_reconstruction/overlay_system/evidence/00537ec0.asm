00537ec0 55                       push       ebp
00537ec1 8bec                     mov        ebp, esp
00537ec3 83ec60                   sub        esp, 0x60
00537ec6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00537ecb 33c5                     xor        eax, ebp
00537ecd 8945fc                   mov        dword ptr [ebp - 4], eax
00537ed0 894de8                   mov        dword ptr [ebp - 0x18], ecx
00537ed3 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00537ed6 05e4000000               add        eax, 0xe4
00537edb 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00537ede 6a00                     push       0
00537ee0 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00537ee3 e8f806f4ff               call       0x4785e0
00537ee8 0fb6c8                   movzx      ecx, al
00537eeb 85c9                     test       ecx, ecx
00537eed 741d                     je         0x537f0c
00537eef b9a8a45b00               mov        ecx, 0x5ba4a8
00537ef4 e8d718efff               call       0x4297d0
00537ef9 51                       push       ecx
00537efa d91c24                   fstp       dword ptr [esp]
00537efd 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00537f00 81c1d4000000             add        ecx, 0xd4
00537f06 e815b0f1ff               call       0x452f20
00537f0b 90                       nop        
00537f0c 8d4df8                   lea        ecx, [ebp - 8]
00537f0f e8ccfef0ff               call       0x447de0
00537f14 90                       nop        
00537f15 6a00                     push       0
00537f17 e81489f2ff               call       0x460830
00537f1c 83c404                   add        esp, 4
00537f1f 8bc8                     mov        ecx, eax
00537f21 e8ba76fcff               call       0x4ff5e0
00537f26 0fb6d0                   movzx      edx, al
00537f29 85d2                     test       edx, edx
00537f2b 752d                     jne        0x537f5a
00537f2d b808000000               mov        eax, 8
00537f32 6bc800                   imul       ecx, eax, 0
00537f35 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
00537f38 8d840a80000000           lea        eax, [edx + ecx + 0x80]
00537f3f 8945e0                   mov        dword ptr [ebp - 0x20], eax
00537f42 8d4db8                   lea        ecx, [ebp - 0x48]
00537f45 51                       push       ecx
00537f46 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00537f49 e812020000               call       0x538160
00537f4e 50                       push       eax
00537f4f 8d4df8                   lea        ecx, [ebp - 8]
00537f52 e819f2ffff               call       0x537170
00537f57 90                       nop        
00537f58 eb2b                     jmp        0x537f85
00537f5a ba08000000               mov        edx, 8
00537f5f c1e200                   shl        edx, 0
00537f62 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00537f65 8d8c1080000000           lea        ecx, [eax + edx + 0x80]
00537f6c 894ddc                   mov        dword ptr [ebp - 0x24], ecx
00537f6f 8d55b0                   lea        edx, [ebp - 0x50]
00537f72 52                       push       edx
00537f73 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00537f76 e8e5010000               call       0x538160
00537f7b 50                       push       eax
00537f7c 8d4df8                   lea        ecx, [ebp - 8]
00537f7f e8ecf1ffff               call       0x537170
00537f84 90                       nop        
00537f85 8d4dec                   lea        ecx, [ebp - 0x14]
00537f88 e883aeeeff               call       0x422e10
00537f8d 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00537f90 81c1d4000000             add        ecx, 0xd4
00537f96 e84513efff               call       0x4292e0
00537f9b 51                       push       ecx
00537f9c d91c24                   fstp       dword ptr [esp]
00537f9f e88c15f0ff               call       0x439530
00537fa4 83c404                   add        esp, 4
00537fa7 d95dd8                   fstp       dword ptr [ebp - 0x28]
00537faa f30f1045d8               movss      xmm0, dword ptr [ebp - 0x28]
00537faf f30f590578665700         mulss      xmm0, dword ptr [0x576678]
00537fb7 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00537fbc 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00537fbf 05d4000000               add        eax, 0xd4
00537fc4 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00537fc7 51                       push       ecx
00537fc8 f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
00537fd0 f30f110424               movss      dword ptr [esp], xmm0
00537fd5 8d4dc0                   lea        ecx, [ebp - 0x40]
00537fd8 51                       push       ecx
00537fd9 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00537fdc e8af13efff               call       0x429390
00537fe1 8bc8                     mov        ecx, eax
00537fe3 e8f812efff               call       0x4292e0
00537fe8 51                       push       ecx
00537fe9 d91c24                   fstp       dword ptr [esp]
00537fec e83f15f0ff               call       0x439530
00537ff1 83c404                   add        esp, 4
00537ff4 d95dd0                   fstp       dword ptr [ebp - 0x30]
00537ff7 f30f1045d0               movss      xmm0, dword ptr [ebp - 0x30]
00537ffc f30f5905bcd75600         mulss      xmm0, dword ptr [0x56d7bc]
00538004 f30f1145ec               movss      dword ptr [ebp - 0x14], xmm0
00538009 8b55f8                   mov        edx, dword ptr [ebp - 8]
0053800c 52                       push       edx
0053800d 8d4dec                   lea        ecx, [ebp - 0x14]
00538010 e82bb0f1ff               call       0x453040
00538015 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00538018 8b8828010000             mov        ecx, dword ptr [eax + 0x128]
0053801e e86d7fedff               call       0x40ff90
00538023 8bc8                     mov        ecx, eax
00538025 e86676fcff               call       0x4ff690
0053802a f30f1045ec               movss      xmm0, dword ptr [ebp - 0x14]
0053802f f30f590598cd5600         mulss      xmm0, dword ptr [0x56cd98]
00538037 f30f2cc8                 cvttss2si  ecx, xmm0
0053803b 0308                     add        ecx, dword ptr [eax]
0053803d 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
00538040 894a70                   mov        dword ptr [edx + 0x70], ecx
00538043 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00538046 8b8828010000             mov        ecx, dword ptr [eax + 0x128]
0053804c e83f7fedff               call       0x40ff90
00538051 8bc8                     mov        ecx, eax
00538053 e83876fcff               call       0x4ff690
00538058 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
0053805d f30f590598cd5600         mulss      xmm0, dword ptr [0x56cd98]
00538065 f30f2cc8                 cvttss2si  ecx, xmm0
00538069 034804                   add        ecx, dword ptr [eax + 4]
0053806c 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
0053806f 894a74                   mov        dword ptr [edx + 0x74], ecx
00538072 6a00                     push       0
00538074 e8b787f2ff               call       0x460830
00538079 83c404                   add        esp, 4
0053807c 8bc8                     mov        ecx, eax
0053807e e85d75fcff               call       0x4ff5e0
00538083 0fb6c0                   movzx      eax, al
00538086 85c0                     test       eax, eax
00538088 7525                     jne        0x5380af
0053808a b908000000               mov        ecx, 8
0053808f 6bd100                   imul       edx, ecx, 0
00538092 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00538095 8d8c1080000000           lea        ecx, [eax + edx + 0x80]
0053809c 51                       push       ecx
0053809d 8d55a8                   lea        edx, [ebp - 0x58]
005380a0 52                       push       edx
005380a1 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
005380a4 83c170                   add        ecx, 0x70
005380a7 e844f0fbff               call       0x4f70f0
005380ac 90                       nop        
005380ad eb23                     jmp        0x5380d2
005380af b808000000               mov        eax, 8
005380b4 c1e000                   shl        eax, 0
005380b7 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
005380ba 8d940180000000           lea        edx, [ecx + eax + 0x80]
005380c1 52                       push       edx
005380c2 8d45a0                   lea        eax, [ebp - 0x60]
005380c5 50                       push       eax
005380c6 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
005380c9 83c170                   add        ecx, 0x70
005380cc e81ff0fbff               call       0x4f70f0
005380d1 90                       nop        
005380d2 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
005380da f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
005380e2 f30f5e05b0cd5600         divss      xmm0, dword ptr [0x56cdb0]
005380ea b9a8a45b00               mov        ecx, 0x5ba4a8
005380ef f30f1145c8               movss      dword ptr [ebp - 0x38], xmm0
005380f4 e83717efff               call       0x429830
005380f9 d95dcc                   fstp       dword ptr [ebp - 0x34]
005380fc f30f1045cc               movss      xmm0, dword ptr [ebp - 0x34]
00538101 f30f100df0e05600         movss      xmm1, dword ptr [0x56e0f0]
00538109 f30f590dd0c85600         mulss      xmm1, dword ptr [0x56c8d0]
00538111 f30f59c1                 mulss      xmm0, xmm1
00538115 f30f5e05b0cd5600         divss      xmm0, dword ptr [0x56cdb0]
0053811d f30f104dc8               movss      xmm1, dword ptr [ebp - 0x38]
00538122 f30f58c8                 addss      xmm1, xmm0
00538126 51                       push       ecx
00538127 f30f110c24               movss      dword ptr [esp], xmm1
0053812c 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0053812f 81c1d4000000             add        ecx, 0xd4
00538135 e886aff1ff               call       0x4530c0
0053813a 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0053813d 81c1e4000000             add        ecx, 0xe4
00538143 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
00538146 6a00                     push       0
00538148 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
0053814b e8f0b3eeff               call       0x423540
00538150 90                       nop        
00538151 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00538154 33cd                     xor        ecx, ebp
00538156 e893a80000               call       0x5429ee
0053815b 8be5                     mov        esp, ebp
0053815d 5d                       pop        ebp
0053815e c3                       ret        
0053815f cc                       int3       
