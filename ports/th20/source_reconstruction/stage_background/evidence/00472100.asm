00472100 55                       push       ebp
00472101 8bec                     mov        ebp, esp
00472103 83ec2c                   sub        esp, 0x2c
00472106 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0047210b 33c5                     xor        eax, ebp
0047210d 8945fc                   mov        dword ptr [ebp - 4], eax
00472110 56                       push       esi
00472111 57                       push       edi
00472112 894dec                   mov        dword ptr [ebp - 0x14], ecx
00472115 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00472118 8b8844330000             mov        ecx, dword ptr [eax + 0x3344]
0047211e c1e903                   shr        ecx, 3
00472121 83e101                   and        ecx, 1
00472124 740a                     je         0x472130
00472126 b801000000               mov        eax, 1
0047212b e999010000               jmp        0x4722c9
00472130 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00472133 8b8244330000             mov        eax, dword ptr [edx + 0x3344]
00472139 c1e802                   shr        eax, 2
0047213c 83e001                   and        eax, 1
0047213f 741d                     je         0x47215e
00472141 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00472144 81c148330000             add        ecx, 0x3348
0047214a e841def9ff               call       0x40ff90
0047214f 83f83c                   cmp        eax, 0x3c
00472152 7c0a                     jl         0x47215e
00472154 b801000000               mov        eax, 1
00472159 e96b010000               jmp        0x4722c9
0047215e c745e800000000           mov        dword ptr [ebp - 0x18], 0
00472165 eb09                     jmp        0x472170
00472167 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0047216a 83c101                   add        ecx, 1
0047216d 894de8                   mov        dword ptr [ebp - 0x18], ecx
00472170 837de803                 cmp        dword ptr [ebp - 0x18], 3
00472174 7d26                     jge        0x47219c
00472176 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
00472179 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
0047217c 0f57c0                   xorps      xmm0, xmm0
0047217f f30f1184d040030000       movss      dword ptr [eax + edx*8 + 0x340], xmm0
00472188 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0047218b 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
0047218e 0f57c0                   xorps      xmm0, xmm0
00472191 f30f1184ca44030000       movss      dword ptr [edx + ecx*8 + 0x344], xmm0
0047219a ebcb                     jmp        0x472167
0047219c 51                       push       ecx
0047219d 0f57c0                   xorps      xmm0, xmm0
004721a0 f30f110424               movss      dword ptr [esp], xmm0
004721a5 51                       push       ecx
004721a6 0f57c0                   xorps      xmm0, xmm0
004721a9 f30f110424               movss      dword ptr [esp], xmm0
004721ae 51                       push       ecx
004721af 0f57c0                   xorps      xmm0, xmm0
004721b2 f30f110424               movss      dword ptr [esp], xmm0
004721b7 8d4dd4                   lea        ecx, [ebp - 0x2c]
004721ba e8110cfbff               call       0x422dd0
004721bf 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004721c2 81c158030000             add        ecx, 0x358
004721c8 8b10                     mov        edx, dword ptr [eax]
004721ca 8911                     mov        dword ptr [ecx], edx
004721cc 8b5004                   mov        edx, dword ptr [eax + 4]
004721cf 895104                   mov        dword ptr [ecx + 4], edx
004721d2 8b4008                   mov        eax, dword ptr [eax + 8]
004721d5 894108                   mov        dword ptr [ecx + 8], eax
004721d8 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004721db 81c120020000             add        ecx, 0x220
004721e1 894de0                   mov        dword ptr [ebp - 0x20], ecx
004721e4 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004721e7 81c25c020000             add        edx, 0x25c
004721ed 52                       push       edx
004721ee 8d45f0                   lea        eax, [ebp - 0x10]
004721f1 50                       push       eax
004721f2 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004721f5 e87673fbff               call       0x429570
004721fa 8d4df0                   lea        ecx, [ebp - 0x10]
004721fd 51                       push       ecx
004721fe 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00472201 81c238020000             add        edx, 0x238
00472207 52                       push       edx
00472208 e803adfeff               call       0x45cf10
0047220d 83c408                   add        esp, 8
00472210 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00472213 c7801c33000080808000     mov        dword ptr [eax + 0x331c], 0x808080
0047221d 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00472220 8b9144330000             mov        edx, dword ptr [ecx + 0x3344]
00472226 c1ea02                   shr        edx, 2
00472229 83e201                   and        edx, 1
0047222c 7413                     je         0x472241
0047222e 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00472231 81c148330000             add        ecx, 0x3348
00472237 e854ddf9ff               call       0x40ff90
0047223c 83f81e                   cmp        eax, 0x1e
0047223f 7d14                     jge        0x472255
00472241 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00472244 e897170000               call       0x4739e0
00472249 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0047224c 83c110                   add        ecx, 0x10
0047224f e85c2f0000               call       0x4751b0
00472254 90                       nop        
00472255 8b75ec                   mov        esi, dword ptr [ebp - 0x14]
00472258 81c614020000             add        esi, 0x214
0047225e b86c010000               mov        eax, 0x16c
00472263 6bf803                   imul       edi, eax, 3
00472266 81c7b84f5c00             add        edi, 0x5c4fb8
0047226c b95b000000               mov        ecx, 0x5b
00472271 f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
00472273 c745e400000000           mov        dword ptr [ebp - 0x1c], 0
0047227a eb09                     jmp        0x472285
0047227c 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0047227f 83c101                   add        ecx, 1
00472282 894de4                   mov        dword ptr [ebp - 0x1c], ecx
00472285 837de408                 cmp        dword ptr [ebp - 0x1c], 8
00472289 7d19                     jge        0x4722a4
0047228b 6955e4e4050000           imul       edx, dword ptr [ebp - 0x1c], 0x5e4
00472292 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00472295 8d8c1084030000           lea        ecx, [eax + edx + 0x384]
0047229c e82f93fbff               call       0x42b5d0
004722a1 90                       nop        
004722a2 ebd8                     jmp        0x47227c
004722a4 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004722a7 83c110                   add        ecx, 0x10
004722aa e831000000               call       0x4722e0
004722af 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004722b2 8b915c330000             mov        edx, dword ptr [ecx + 0x335c]
004722b8 83c201                   add        edx, 1
004722bb 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004722be 89905c330000             mov        dword ptr [eax + 0x335c], edx
004722c4 b801000000               mov        eax, 1
004722c9 5f                       pop        edi
004722ca 5e                       pop        esi
004722cb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004722ce 33cd                     xor        ecx, ebp
004722d0 e819070d00               call       0x5429ee
004722d5 8be5                     mov        esp, ebp
004722d7 5d                       pop        ebp
004722d8 c3                       ret        
004722d9 cc                       int3       
004722da cc                       int3       
004722db cc                       int3       
004722dc cc                       int3       
004722dd cc                       int3       
004722de cc                       int3       
004722df cc                       int3       
