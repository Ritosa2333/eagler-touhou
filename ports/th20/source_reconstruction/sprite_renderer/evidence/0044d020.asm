0044d020 55                       push       ebp
0044d021 8bec                     mov        ebp, esp
0044d023 83ec54                   sub        esp, 0x54
0044d026 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0044d02b 33c5                     xor        eax, ebp
0044d02d 8945fc                   mov        dword ptr [ebp - 4], eax
0044d030 894df4                   mov        dword ptr [ebp - 0xc], ecx
0044d033 b814000000               mov        eax, 0x14
0044d038 d1e0                     shl        eax, 1
0044d03a 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d03d f30f1005bccd5600         movss      xmm0, dword ptr [0x56cdbc]
0044d045 f30f118401200e0006       movss      dword ptr [ecx + eax + 0x6000e20], xmm0
0044d04e ba14000000               mov        edx, 0x14
0044d053 6bc200                   imul       eax, edx, 0
0044d056 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d059 f30f1005bccd5600         movss      xmm0, dword ptr [0x56cdbc]
0044d061 f30f118401200e0006       movss      dword ptr [ecx + eax + 0x6000e20], xmm0
0044d06a ba14000000               mov        edx, 0x14
0044d06f 6bc203                   imul       eax, edx, 3
0044d072 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d075 f30f100598cd5600         movss      xmm0, dword ptr [0x56cd98]
0044d07d f30f118401200e0006       movss      dword ptr [ecx + eax + 0x6000e20], xmm0
0044d086 ba14000000               mov        edx, 0x14
0044d08b c1e200                   shl        edx, 0
0044d08e 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0044d091 f30f100598cd5600         movss      xmm0, dword ptr [0x56cd98]
0044d099 f30f118410200e0006       movss      dword ptr [eax + edx + 0x6000e20], xmm0
0044d0a2 b914000000               mov        ecx, 0x14
0044d0a7 c1e100                   shl        ecx, 0
0044d0aa 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d0ad f30f1005bccd5600         movss      xmm0, dword ptr [0x56cdbc]
0044d0b5 f30f11840a240e0006       movss      dword ptr [edx + ecx + 0x6000e24], xmm0
0044d0be b814000000               mov        eax, 0x14
0044d0c3 6bc800                   imul       ecx, eax, 0
0044d0c6 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d0c9 f30f1005bccd5600         movss      xmm0, dword ptr [0x56cdbc]
0044d0d1 f30f11840a240e0006       movss      dword ptr [edx + ecx + 0x6000e24], xmm0
0044d0da b814000000               mov        eax, 0x14
0044d0df 6bc803                   imul       ecx, eax, 3
0044d0e2 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d0e5 f30f100598cd5600         movss      xmm0, dword ptr [0x56cd98]
0044d0ed f30f11840a240e0006       movss      dword ptr [edx + ecx + 0x6000e24], xmm0
0044d0f6 b814000000               mov        eax, 0x14
0044d0fb d1e0                     shl        eax, 1
0044d0fd 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d100 f30f100598cd5600         movss      xmm0, dword ptr [0x56cd98]
0044d108 f30f118401240e0006       movss      dword ptr [ecx + eax + 0x6000e24], xmm0
0044d111 ba14000000               mov        edx, 0x14
0044d116 6bc203                   imul       eax, edx, 3
0044d119 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d11c 0f57c0                   xorps      xmm0, xmm0
0044d11f f30f118401280e0006       movss      dword ptr [ecx + eax + 0x6000e28], xmm0
0044d128 ba14000000               mov        edx, 0x14
0044d12d d1e2                     shl        edx, 1
0044d12f 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0044d132 0f57c0                   xorps      xmm0, xmm0
0044d135 f30f118410280e0006       movss      dword ptr [eax + edx + 0x6000e28], xmm0
0044d13e b914000000               mov        ecx, 0x14
0044d143 c1e100                   shl        ecx, 0
0044d146 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d149 0f57c0                   xorps      xmm0, xmm0
0044d14c f30f11840a280e0006       movss      dword ptr [edx + ecx + 0x6000e28], xmm0
0044d155 b814000000               mov        eax, 0x14
0044d15a 6bc800                   imul       ecx, eax, 0
0044d15d 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d160 0f57c0                   xorps      xmm0, xmm0
0044d163 f30f11840a280e0006       movss      dword ptr [edx + ecx + 0x6000e28], xmm0
0044d16c b814000000               mov        eax, 0x14
0044d171 d1e0                     shl        eax, 1
0044d173 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d176 0f57c0                   xorps      xmm0, xmm0
0044d179 f30f1184012c0e0006       movss      dword ptr [ecx + eax + 0x6000e2c], xmm0
0044d182 ba14000000               mov        edx, 0x14
0044d187 6bc200                   imul       eax, edx, 0
0044d18a 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d18d 0f57c0                   xorps      xmm0, xmm0
0044d190 f30f1184012c0e0006       movss      dword ptr [ecx + eax + 0x6000e2c], xmm0
0044d199 ba14000000               mov        edx, 0x14
0044d19e 6bc203                   imul       eax, edx, 3
0044d1a1 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d1a4 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
0044d1ac f30f1184012c0e0006       movss      dword ptr [ecx + eax + 0x6000e2c], xmm0
0044d1b5 ba14000000               mov        edx, 0x14
0044d1ba c1e200                   shl        edx, 0
0044d1bd 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0044d1c0 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
0044d1c8 f30f1184102c0e0006       movss      dword ptr [eax + edx + 0x6000e2c], xmm0
0044d1d1 b914000000               mov        ecx, 0x14
0044d1d6 c1e100                   shl        ecx, 0
0044d1d9 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d1dc 0f57c0                   xorps      xmm0, xmm0
0044d1df f30f11840a300e0006       movss      dword ptr [edx + ecx + 0x6000e30], xmm0
0044d1e8 b814000000               mov        eax, 0x14
0044d1ed 6bc800                   imul       ecx, eax, 0
0044d1f0 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d1f3 0f57c0                   xorps      xmm0, xmm0
0044d1f6 f30f11840a300e0006       movss      dword ptr [edx + ecx + 0x6000e30], xmm0
0044d1ff b814000000               mov        eax, 0x14
0044d204 6bc803                   imul       ecx, eax, 3
0044d207 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d20a f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
0044d212 f30f11840a300e0006       movss      dword ptr [edx + ecx + 0x6000e30], xmm0
0044d21b b814000000               mov        eax, 0x14
0044d220 d1e0                     shl        eax, 1
0044d222 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d225 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
0044d22d f30f118401300e0006       movss      dword ptr [ecx + eax + 0x6000e30], xmm0
0044d236 ba14000000               mov        edx, 0x14
0044d23b 6bc200                   imul       eax, edx, 0
0044d23e 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d241 8d9401200e0006           lea        edx, [ecx + eax + 0x6000e20]
0044d248 b818000000               mov        eax, 0x18
0044d24d 6bc800                   imul       ecx, eax, 0
0044d250 81c180ef5a00             add        ecx, 0x5aef80
0044d256 8b02                     mov        eax, dword ptr [edx]
0044d258 8901                     mov        dword ptr [ecx], eax
0044d25a 8b4204                   mov        eax, dword ptr [edx + 4]
0044d25d 894104                   mov        dword ptr [ecx + 4], eax
0044d260 8b5208                   mov        edx, dword ptr [edx + 8]
0044d263 895108                   mov        dword ptr [ecx + 8], edx
0044d266 b814000000               mov        eax, 0x14
0044d26b c1e000                   shl        eax, 0
0044d26e 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d271 8d9401200e0006           lea        edx, [ecx + eax + 0x6000e20]
0044d278 b818000000               mov        eax, 0x18
0044d27d c1e000                   shl        eax, 0
0044d280 0580ef5a00               add        eax, 0x5aef80
0044d285 8b0a                     mov        ecx, dword ptr [edx]
0044d287 8908                     mov        dword ptr [eax], ecx
0044d289 8b4a04                   mov        ecx, dword ptr [edx + 4]
0044d28c 894804                   mov        dword ptr [eax + 4], ecx
0044d28f 8b5208                   mov        edx, dword ptr [edx + 8]
0044d292 895008                   mov        dword ptr [eax + 8], edx
0044d295 b814000000               mov        eax, 0x14
0044d29a d1e0                     shl        eax, 1
0044d29c 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d29f 8d9401200e0006           lea        edx, [ecx + eax + 0x6000e20]
0044d2a6 b818000000               mov        eax, 0x18
0044d2ab d1e0                     shl        eax, 1
0044d2ad 0580ef5a00               add        eax, 0x5aef80
0044d2b2 8b0a                     mov        ecx, dword ptr [edx]
0044d2b4 8908                     mov        dword ptr [eax], ecx
0044d2b6 8b4a04                   mov        ecx, dword ptr [edx + 4]
0044d2b9 894804                   mov        dword ptr [eax + 4], ecx
0044d2bc 8b5208                   mov        edx, dword ptr [edx + 8]
0044d2bf 895008                   mov        dword ptr [eax + 8], edx
0044d2c2 b814000000               mov        eax, 0x14
0044d2c7 6bc803                   imul       ecx, eax, 3
0044d2ca 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d2cd 8d840a200e0006           lea        eax, [edx + ecx + 0x6000e20]
0044d2d4 b918000000               mov        ecx, 0x18
0044d2d9 6bd103                   imul       edx, ecx, 3
0044d2dc 81c280ef5a00             add        edx, 0x5aef80
0044d2e2 8b08                     mov        ecx, dword ptr [eax]
0044d2e4 890a                     mov        dword ptr [edx], ecx
0044d2e6 8b4804                   mov        ecx, dword ptr [eax + 4]
0044d2e9 894a04                   mov        dword ptr [edx + 4], ecx
0044d2ec 8b4008                   mov        eax, dword ptr [eax + 8]
0044d2ef 894208                   mov        dword ptr [edx + 8], eax
0044d2f2 b914000000               mov        ecx, 0x14
0044d2f7 6bd100                   imul       edx, ecx, 0
0044d2fa b818000000               mov        eax, 0x18
0044d2ff 6bc800                   imul       ecx, eax, 0
0044d302 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0044d305 8b94102c0e0006           mov        edx, dword ptr [eax + edx + 0x6000e2c]
0044d30c 899190ef5a00             mov        dword ptr [ecx + 0x5aef90], edx
0044d312 b814000000               mov        eax, 0x14
0044d317 6bc800                   imul       ecx, eax, 0
0044d31a ba18000000               mov        edx, 0x18
0044d31f 6bc200                   imul       eax, edx, 0
0044d322 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d325 8b8c0a300e0006           mov        ecx, dword ptr [edx + ecx + 0x6000e30]
0044d32c 898894ef5a00             mov        dword ptr [eax + 0x5aef94], ecx
0044d332 ba14000000               mov        edx, 0x14
0044d337 c1e200                   shl        edx, 0
0044d33a b818000000               mov        eax, 0x18
0044d33f c1e000                   shl        eax, 0
0044d342 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d345 8b94112c0e0006           mov        edx, dword ptr [ecx + edx + 0x6000e2c]
0044d34c 899090ef5a00             mov        dword ptr [eax + 0x5aef90], edx
0044d352 b814000000               mov        eax, 0x14
0044d357 c1e000                   shl        eax, 0
0044d35a b918000000               mov        ecx, 0x18
0044d35f c1e100                   shl        ecx, 0
0044d362 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d365 8b8402300e0006           mov        eax, dword ptr [edx + eax + 0x6000e30]
0044d36c 898194ef5a00             mov        dword ptr [ecx + 0x5aef94], eax
0044d372 b914000000               mov        ecx, 0x14
0044d377 d1e1                     shl        ecx, 1
0044d379 ba18000000               mov        edx, 0x18
0044d37e d1e2                     shl        edx, 1
0044d380 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0044d383 8b8c082c0e0006           mov        ecx, dword ptr [eax + ecx + 0x6000e2c]
0044d38a 898a90ef5a00             mov        dword ptr [edx + 0x5aef90], ecx
0044d390 ba14000000               mov        edx, 0x14
0044d395 d1e2                     shl        edx, 1
0044d397 b818000000               mov        eax, 0x18
0044d39c d1e0                     shl        eax, 1
0044d39e 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d3a1 8b9411300e0006           mov        edx, dword ptr [ecx + edx + 0x6000e30]
0044d3a8 899094ef5a00             mov        dword ptr [eax + 0x5aef94], edx
0044d3ae b814000000               mov        eax, 0x14
0044d3b3 6bc803                   imul       ecx, eax, 3
0044d3b6 ba18000000               mov        edx, 0x18
0044d3bb 6bc203                   imul       eax, edx, 3
0044d3be 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d3c1 8b8c0a2c0e0006           mov        ecx, dword ptr [edx + ecx + 0x6000e2c]
0044d3c8 898890ef5a00             mov        dword ptr [eax + 0x5aef90], ecx
0044d3ce ba14000000               mov        edx, 0x14
0044d3d3 6bc203                   imul       eax, edx, 3
0044d3d6 b918000000               mov        ecx, 0x18
0044d3db 6bd103                   imul       edx, ecx, 3
0044d3de 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d3e1 8b8401300e0006           mov        eax, dword ptr [ecx + eax + 0x6000e30]
0044d3e8 898294ef5a00             mov        dword ptr [edx + 0x5aef94], eax
0044d3ee c745f000000000           mov        dword ptr [ebp - 0x10], 0
0044d3f5 b9404d5c00               mov        ecx, 0x5c4d40
0044d3fa e83153fcff               call       0x412730
0044d3ff 8945c4                   mov        dword ptr [ebp - 0x3c], eax
0044d402 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
0044d405 8b11                     mov        edx, dword ptr [ecx]
0044d407 8b4268                   mov        eax, dword ptr [edx + 0x68]
0044d40a 8945bc                   mov        dword ptr [ebp - 0x44], eax
0044d40d 6a00                     push       0
0044d40f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d412 81c11c0e0006             add        ecx, 0x6000e1c
0044d418 51                       push       ecx
0044d419 6a01                     push       1
0044d41b 6802010000               push       0x102
0044d420 6a00                     push       0
0044d422 68d0020000               push       0x2d0
0044d427 8b55c4                   mov        edx, dword ptr [ebp - 0x3c]
0044d42a 52                       push       edx
0044d42b ff55bc                   call       dword ptr [ebp - 0x44]
0044d42e 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0044d431 8b881c0e0006             mov        ecx, dword ptr [eax + 0x6000e1c]
0044d437 894db8                   mov        dword ptr [ebp - 0x48], ecx
0044d43a 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d43d 8b821c0e0006             mov        eax, dword ptr [edx + 0x6000e1c]
0044d443 8b08                     mov        ecx, dword ptr [eax]
0044d445 8b512c                   mov        edx, dword ptr [ecx + 0x2c]
0044d448 8955b4                   mov        dword ptr [ebp - 0x4c], edx
0044d44b 6a00                     push       0
0044d44d 8d45f8                   lea        eax, [ebp - 8]
0044d450 50                       push       eax
0044d451 6a00                     push       0
0044d453 6a00                     push       0
0044d455 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
0044d458 51                       push       ecx
0044d459 ff55b4                   call       dword ptr [ebp - 0x4c]
0044d45c 8b55f8                   mov        edx, dword ptr [ebp - 8]
0044d45f 8955f0                   mov        dword ptr [ebp - 0x10], edx
0044d462 837df000                 cmp        dword ptr [ebp - 0x10], 0
0044d466 7505                     jne        0x44d46d
0044d468 e904040000               jmp        0x44d871
0044d46d 6a50                     push       0x50
0044d46f 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0044d472 05200e0006               add        eax, 0x6000e20
0044d477 50                       push       eax
0044d478 b914000000               mov        ecx, 0x14
0044d47d 6bd100                   imul       edx, ecx, 0
0044d480 0355f0                   add        edx, dword ptr [ebp - 0x10]
0044d483 52                       push       edx
0044d484 e897690f00               call       0x543e20
0044d489 83c40c                   add        esp, 0xc
0044d48c c745ec00000000           mov        dword ptr [ebp - 0x14], 0
0044d493 eb09                     jmp        0x44d49e
0044d495 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
0044d498 83c001                   add        eax, 1
0044d49b 8945ec                   mov        dword ptr [ebp - 0x14], eax
0044d49e 837dec04                 cmp        dword ptr [ebp - 0x14], 4
0044d4a2 7d2a                     jge        0x44d4ce
0044d4a4 6b4dec14                 imul       ecx, dword ptr [ebp - 0x14], 0x14
0044d4a8 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d4ab f30f10840a240e0006       movss      xmm0, dword ptr [edx + ecx + 0x6000e24]
0044d4b4 f30f580598cd5600         addss      xmm0, dword ptr [0x56cd98]
0044d4bc 6b45ec14                 imul       eax, dword ptr [ebp - 0x14], 0x14
0044d4c0 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d4c3 f30f118401240e0006       movss      dword ptr [ecx + eax + 0x6000e24], xmm0
0044d4cc ebc7                     jmp        0x44d495
0044d4ce 6a50                     push       0x50
0044d4d0 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d4d3 81c2200e0006             add        edx, 0x6000e20
0044d4d9 52                       push       edx
0044d4da b814000000               mov        eax, 0x14
0044d4df 6bc80c                   imul       ecx, eax, 0xc
0044d4e2 034df0                   add        ecx, dword ptr [ebp - 0x10]
0044d4e5 51                       push       ecx
0044d4e6 e835690f00               call       0x543e20
0044d4eb 83c40c                   add        esp, 0xc
0044d4ee c745e800000000           mov        dword ptr [ebp - 0x18], 0
0044d4f5 eb09                     jmp        0x44d500
0044d4f7 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
0044d4fa 83c201                   add        edx, 1
0044d4fd 8955e8                   mov        dword ptr [ebp - 0x18], edx
0044d500 837de804                 cmp        dword ptr [ebp - 0x18], 4
0044d504 7d2a                     jge        0x44d530
0044d506 6b45e814                 imul       eax, dword ptr [ebp - 0x18], 0x14
0044d50a 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d50d f30f108401240e0006       movss      xmm0, dword ptr [ecx + eax + 0x6000e24]
0044d516 f30f5c05a0cd5600         subss      xmm0, dword ptr [0x56cda0]
0044d51e 6b55e814                 imul       edx, dword ptr [ebp - 0x18], 0x14
0044d522 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0044d525 f30f118410240e0006       movss      dword ptr [eax + edx + 0x6000e24], xmm0
0044d52e ebc7                     jmp        0x44d4f7
0044d530 6a50                     push       0x50
0044d532 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d535 81c1200e0006             add        ecx, 0x6000e20
0044d53b 51                       push       ecx
0044d53c ba14000000               mov        edx, 0x14
0044d541 6bc218                   imul       eax, edx, 0x18
0044d544 0345f0                   add        eax, dword ptr [ebp - 0x10]
0044d547 50                       push       eax
0044d548 e8d3680f00               call       0x543e20
0044d54d 83c40c                   add        esp, 0xc
0044d550 c745e400000000           mov        dword ptr [ebp - 0x1c], 0
0044d557 eb09                     jmp        0x44d562
0044d559 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0044d55c 83c101                   add        ecx, 1
0044d55f 894de4                   mov        dword ptr [ebp - 0x1c], ecx
0044d562 837de404                 cmp        dword ptr [ebp - 0x1c], 4
0044d566 7d2a                     jge        0x44d592
0044d568 6b55e414                 imul       edx, dword ptr [ebp - 0x1c], 0x14
0044d56c 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0044d56f f30f108410200e0006       movss      xmm0, dword ptr [eax + edx + 0x6000e20]
0044d578 f30f580598cd5600         addss      xmm0, dword ptr [0x56cd98]
0044d580 6b4de414                 imul       ecx, dword ptr [ebp - 0x1c], 0x14
0044d584 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d587 f30f11840a200e0006       movss      dword ptr [edx + ecx + 0x6000e20], xmm0
0044d590 ebc7                     jmp        0x44d559
0044d592 c745e000000000           mov        dword ptr [ebp - 0x20], 0
0044d599 eb09                     jmp        0x44d5a4
0044d59b 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0044d59e 83c001                   add        eax, 1
0044d5a1 8945e0                   mov        dword ptr [ebp - 0x20], eax
0044d5a4 837de004                 cmp        dword ptr [ebp - 0x20], 4
0044d5a8 7d2a                     jge        0x44d5d4
0044d5aa 6b4de014                 imul       ecx, dword ptr [ebp - 0x20], 0x14
0044d5ae 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d5b1 f30f10840a240e0006       movss      xmm0, dword ptr [edx + ecx + 0x6000e24]
0044d5ba f30f580598cd5600         addss      xmm0, dword ptr [0x56cd98]
0044d5c2 6b45e014                 imul       eax, dword ptr [ebp - 0x20], 0x14
0044d5c6 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d5c9 f30f118401240e0006       movss      dword ptr [ecx + eax + 0x6000e24], xmm0
0044d5d2 ebc7                     jmp        0x44d59b
0044d5d4 6a50                     push       0x50
0044d5d6 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d5d9 81c2200e0006             add        edx, 0x6000e20
0044d5df 52                       push       edx
0044d5e0 b814000000               mov        eax, 0x14
0044d5e5 c1e002                   shl        eax, 2
0044d5e8 0345f0                   add        eax, dword ptr [ebp - 0x10]
0044d5eb 50                       push       eax
0044d5ec e82f680f00               call       0x543e20
0044d5f1 83c40c                   add        esp, 0xc
0044d5f4 c745dc00000000           mov        dword ptr [ebp - 0x24], 0
0044d5fb eb09                     jmp        0x44d606
0044d5fd 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0044d600 83c101                   add        ecx, 1
0044d603 894ddc                   mov        dword ptr [ebp - 0x24], ecx
0044d606 837ddc04                 cmp        dword ptr [ebp - 0x24], 4
0044d60a 7d2a                     jge        0x44d636
0044d60c 6b55dc14                 imul       edx, dword ptr [ebp - 0x24], 0x14
0044d610 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0044d613 f30f108410240e0006       movss      xmm0, dword ptr [eax + edx + 0x6000e24]
0044d61c f30f580598cd5600         addss      xmm0, dword ptr [0x56cd98]
0044d624 6b4ddc14                 imul       ecx, dword ptr [ebp - 0x24], 0x14
0044d628 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d62b f30f11840a240e0006       movss      dword ptr [edx + ecx + 0x6000e24], xmm0
0044d634 ebc7                     jmp        0x44d5fd
0044d636 6a50                     push       0x50
0044d638 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0044d63b 05200e0006               add        eax, 0x6000e20
0044d640 50                       push       eax
0044d641 b914000000               mov        ecx, 0x14
0044d646 c1e104                   shl        ecx, 4
0044d649 034df0                   add        ecx, dword ptr [ebp - 0x10]
0044d64c 51                       push       ecx
0044d64d e8ce670f00               call       0x543e20
0044d652 83c40c                   add        esp, 0xc
0044d655 c745d800000000           mov        dword ptr [ebp - 0x28], 0
0044d65c eb09                     jmp        0x44d667
0044d65e 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
0044d661 83c201                   add        edx, 1
0044d664 8955d8                   mov        dword ptr [ebp - 0x28], edx
0044d667 837dd804                 cmp        dword ptr [ebp - 0x28], 4
0044d66b 7d2a                     jge        0x44d697
0044d66d 6b45d814                 imul       eax, dword ptr [ebp - 0x28], 0x14
0044d671 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d674 f30f108401240e0006       movss      xmm0, dword ptr [ecx + eax + 0x6000e24]
0044d67d f30f5c05a0cd5600         subss      xmm0, dword ptr [0x56cda0]
0044d685 6b55d814                 imul       edx, dword ptr [ebp - 0x28], 0x14
0044d689 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0044d68c f30f118410240e0006       movss      dword ptr [eax + edx + 0x6000e24], xmm0
0044d695 ebc7                     jmp        0x44d65e
0044d697 6a50                     push       0x50
0044d699 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d69c 81c1200e0006             add        ecx, 0x6000e20
0044d6a2 51                       push       ecx
0044d6a3 ba14000000               mov        edx, 0x14
0044d6a8 6bc21c                   imul       eax, edx, 0x1c
0044d6ab 0345f0                   add        eax, dword ptr [ebp - 0x10]
0044d6ae 50                       push       eax
0044d6af e86c670f00               call       0x543e20
0044d6b4 83c40c                   add        esp, 0xc
0044d6b7 c745d400000000           mov        dword ptr [ebp - 0x2c], 0
0044d6be eb09                     jmp        0x44d6c9
0044d6c0 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0044d6c3 83c101                   add        ecx, 1
0044d6c6 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
0044d6c9 837dd404                 cmp        dword ptr [ebp - 0x2c], 4
0044d6cd 7d2a                     jge        0x44d6f9
0044d6cf 6b55d414                 imul       edx, dword ptr [ebp - 0x2c], 0x14
0044d6d3 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0044d6d6 f30f108410200e0006       movss      xmm0, dword ptr [eax + edx + 0x6000e20]
0044d6df f30f5c05a0cd5600         subss      xmm0, dword ptr [0x56cda0]
0044d6e7 6b4dd414                 imul       ecx, dword ptr [ebp - 0x2c], 0x14
0044d6eb 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d6ee f30f11840a200e0006       movss      dword ptr [edx + ecx + 0x6000e20], xmm0
0044d6f7 ebc7                     jmp        0x44d6c0
0044d6f9 c745d000000000           mov        dword ptr [ebp - 0x30], 0
0044d700 eb09                     jmp        0x44d70b
0044d702 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
0044d705 83c001                   add        eax, 1
0044d708 8945d0                   mov        dword ptr [ebp - 0x30], eax
0044d70b 837dd004                 cmp        dword ptr [ebp - 0x30], 4
0044d70f 7d2a                     jge        0x44d73b
0044d711 6b4dd014                 imul       ecx, dword ptr [ebp - 0x30], 0x14
0044d715 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d718 f30f10840a240e0006       movss      xmm0, dword ptr [edx + ecx + 0x6000e24]
0044d721 f30f580598cd5600         addss      xmm0, dword ptr [0x56cd98]
0044d729 6b45d014                 imul       eax, dword ptr [ebp - 0x30], 0x14
0044d72d 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d730 f30f118401240e0006       movss      dword ptr [ecx + eax + 0x6000e24], xmm0
0044d739 ebc7                     jmp        0x44d702
0044d73b 6a50                     push       0x50
0044d73d 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d740 81c2200e0006             add        edx, 0x6000e20
0044d746 52                       push       edx
0044d747 b814000000               mov        eax, 0x14
0044d74c c1e003                   shl        eax, 3
0044d74f 0345f0                   add        eax, dword ptr [ebp - 0x10]
0044d752 50                       push       eax
0044d753 e8c8660f00               call       0x543e20
0044d758 83c40c                   add        esp, 0xc
0044d75b c745cc00000000           mov        dword ptr [ebp - 0x34], 0
0044d762 eb09                     jmp        0x44d76d
0044d764 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0044d767 83c101                   add        ecx, 1
0044d76a 894dcc                   mov        dword ptr [ebp - 0x34], ecx
0044d76d 837dcc04                 cmp        dword ptr [ebp - 0x34], 4
0044d771 7d2a                     jge        0x44d79d
0044d773 6b55cc14                 imul       edx, dword ptr [ebp - 0x34], 0x14
0044d777 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0044d77a f30f108410240e0006       movss      xmm0, dword ptr [eax + edx + 0x6000e24]
0044d783 f30f580598cd5600         addss      xmm0, dword ptr [0x56cd98]
0044d78b 6b4dcc14                 imul       ecx, dword ptr [ebp - 0x34], 0x14
0044d78f 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d792 f30f11840a240e0006       movss      dword ptr [edx + ecx + 0x6000e24], xmm0
0044d79b ebc7                     jmp        0x44d764
0044d79d 6a50                     push       0x50
0044d79f 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0044d7a2 05200e0006               add        eax, 0x6000e20
0044d7a7 50                       push       eax
0044d7a8 b914000000               mov        ecx, 0x14
0044d7ad 6bd114                   imul       edx, ecx, 0x14
0044d7b0 0355f0                   add        edx, dword ptr [ebp - 0x10]
0044d7b3 52                       push       edx
0044d7b4 e867660f00               call       0x543e20
0044d7b9 83c40c                   add        esp, 0xc
0044d7bc c745c800000000           mov        dword ptr [ebp - 0x38], 0
0044d7c3 eb09                     jmp        0x44d7ce
0044d7c5 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
0044d7c8 83c001                   add        eax, 1
0044d7cb 8945c8                   mov        dword ptr [ebp - 0x38], eax
0044d7ce 837dc804                 cmp        dword ptr [ebp - 0x38], 4
0044d7d2 7d2a                     jge        0x44d7fe
0044d7d4 6b4dc814                 imul       ecx, dword ptr [ebp - 0x38], 0x14
0044d7d8 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d7db f30f10840a240e0006       movss      xmm0, dword ptr [edx + ecx + 0x6000e24]
0044d7e4 f30f5c05a0cd5600         subss      xmm0, dword ptr [0x56cda0]
0044d7ec 6b45c814                 imul       eax, dword ptr [ebp - 0x38], 0x14
0044d7f0 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d7f3 f30f118401240e0006       movss      dword ptr [ecx + eax + 0x6000e24], xmm0
0044d7fc ebc7                     jmp        0x44d7c5
0044d7fe 6a50                     push       0x50
0044d800 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044d803 81c2200e0006             add        edx, 0x6000e20
0044d809 52                       push       edx
0044d80a b814000000               mov        eax, 0x14
0044d80f c1e005                   shl        eax, 5
0044d812 0345f0                   add        eax, dword ptr [ebp - 0x10]
0044d815 50                       push       eax
0044d816 e805660f00               call       0x543e20
0044d81b 83c40c                   add        esp, 0xc
0044d81e 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d821 8b911c0e0006             mov        edx, dword ptr [ecx + 0x6000e1c]
0044d827 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0044d82a 8b881c0e0006             mov        ecx, dword ptr [eax + 0x6000e1c]
0044d830 8b12                     mov        edx, dword ptr [edx]
0044d832 51                       push       ecx
0044d833 8b4230                   mov        eax, dword ptr [edx + 0x30]
0044d836 ffd0                     call       eax
0044d838 b9404d5c00               mov        ecx, 0x5c4d40
0044d83d e8ee4efcff               call       0x412730
0044d842 8945c0                   mov        dword ptr [ebp - 0x40], eax
0044d845 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0044d848 8b11                     mov        edx, dword ptr [ecx]
0044d84a 8b8290010000             mov        eax, dword ptr [edx + 0x190]
0044d850 8945ac                   mov        dword ptr [ebp - 0x54], eax
0044d853 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044d856 8b911c0e0006             mov        edx, dword ptr [ecx + 0x6000e1c]
0044d85c 8955b0                   mov        dword ptr [ebp - 0x50], edx
0044d85f 6a14                     push       0x14
0044d861 6a00                     push       0
0044d863 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
0044d866 50                       push       eax
0044d867 6a00                     push       0
0044d869 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0044d86c 51                       push       ecx
0044d86d ff55ac                   call       dword ptr [ebp - 0x54]
0044d870 90                       nop        
0044d871 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044d874 33cd                     xor        ecx, ebp
0044d876 e873510f00               call       0x5429ee
0044d87b 8be5                     mov        esp, ebp
0044d87d 5d                       pop        ebp
0044d87e c3                       ret        
0044d87f cc                       int3       
