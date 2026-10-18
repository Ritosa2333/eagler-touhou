00460240 55                       push       ebp
00460241 8bec                     mov        ebp, esp
00460243 83ec78                   sub        esp, 0x78
00460246 894df8                   mov        dword ptr [ebp - 8], ecx
00460249 8b45f8                   mov        eax, dword ptr [ebp - 8]
0046024c 05d0030000               add        eax, 0x3d0
00460251 8945e8                   mov        dword ptr [ebp - 0x18], eax
00460254 6a50                     push       0x50
00460256 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00460259 e80233fcff               call       0x423560
0046025e 0fb6c8                   movzx      ecx, al
00460261 85c9                     test       ecx, ecx
00460263 0f84de020000             je         0x460547
00460269 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0046026c 81c1d0030000             add        ecx, 0x3d0
00460272 e8b9cdffff               call       0x45d030
00460277 85c0                     test       eax, eax
00460279 0f84c6020000             je         0x460545
0046027f 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00460282 81c1d0030000             add        ecx, 0x3d0
00460288 e803fdfaff               call       0x40ff90
0046028d 8945f4                   mov        dword ptr [ebp - 0xc], eax
00460290 837df432                 cmp        dword ptr [ebp - 0xc], 0x32
00460294 0f8cac000000             jl         0x460346
0046029a c745f000000000           mov        dword ptr [ebp - 0x10], 0
004602a1 eb09                     jmp        0x4602ac
004602a3 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004602a6 83c201                   add        edx, 1
004602a9 8955f0                   mov        dword ptr [ebp - 0x10], edx
004602ac 837df050                 cmp        dword ptr [ebp - 0x10], 0x50
004602b0 0f8d90000000             jge        0x460346
004602b6 8b45f8                   mov        eax, dword ptr [ebp - 8]
004602b9 0588020000               add        eax, 0x288
004602be 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004602c1 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004602c4 51                       push       ecx
004602c5 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004602c8 e8b342fbff               call       0x414580
004602cd 0fb65003                 movzx      edx, byte ptr [eax + 3]
004602d1 f30f2ac2                 cvtsi2ss   xmm0, edx
004602d5 b9c4a45b00               mov        ecx, 0x5ba4c4
004602da f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
004602df e84c95fcff               call       0x429830
004602e4 d95de0                   fstp       dword ptr [ebp - 0x20]
004602e7 f30f1045e0               movss      xmm0, dword ptr [ebp - 0x20]
004602ec f30f5905e0c85600         mulss      xmm0, dword ptr [0x56c8e0]
004602f4 f30f5805bcd75600         addss      xmm0, dword ptr [0x56d7bc]
004602fc f30f104ddc               movss      xmm1, dword ptr [ebp - 0x24]
00460301 f30f5ec8                 divss      xmm1, xmm0
00460305 f30f2cc1                 cvttss2si  eax, xmm1
00460309 0fb6c8                   movzx      ecx, al
0046030c 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
0046030f 8b55f8                   mov        edx, dword ptr [ebp - 8]
00460312 81c288020000             add        edx, 0x288
00460318 8955d8                   mov        dword ptr [ebp - 0x28], edx
0046031b 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0046031e 50                       push       eax
0046031f 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00460322 e85942fbff               call       0x414580
00460327 83c003                   add        eax, 3
0046032a 8945ec                   mov        dword ptr [ebp - 0x14], eax
0046032d 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00460330 8a11                     mov        dl, byte ptr [ecx]
00460332 8855ff                   mov        byte ptr [ebp - 1], dl
00460335 0fb645ff                 movzx      eax, byte ptr [ebp - 1]
00460339 2b45d4                   sub        eax, dword ptr [ebp - 0x2c]
0046033c 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0046033f 8801                     mov        byte ptr [ecx], al
00460341 e95dffffff               jmp        0x4602a3
00460346 837df410                 cmp        dword ptr [ebp - 0xc], 0x10
0046034a 0f8da1000000             jge        0x4603f1
00460350 8b55f8                   mov        edx, dword ptr [ebp - 8]
00460353 f30f1082c8030000         movss      xmm0, dword ptr [edx + 0x3c8]
0046035b f30f1145cc               movss      dword ptr [ebp - 0x34], xmm0
00460360 8b45f8                   mov        eax, dword ptr [ebp - 8]
00460363 83c008                   add        eax, 8
00460366 8945d0                   mov        dword ptr [ebp - 0x30], eax
00460369 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0046036c 51                       push       ecx
0046036d 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00460370 e8fbd2ffff               call       0x45d670
00460375 8945c8                   mov        dword ptr [ebp - 0x38], eax
00460378 51                       push       ecx
00460379 f30f1005bcd75600         movss      xmm0, dword ptr [0x56d7bc]
00460381 f30f110424               movss      dword ptr [esp], xmm0
00460386 51                       push       ecx
00460387 f30f1045cc               movss      xmm0, dword ptr [ebp - 0x34]
0046038c f30f110424               movss      dword ptr [esp], xmm0
00460391 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
00460394 52                       push       edx
00460395 e8968ffdff               call       0x439330
0046039a 83c40c                   add        esp, 0xc
0046039d f30f2a45f4               cvtsi2ss   xmm0, dword ptr [ebp - 0xc]
004603a2 8b45f8                   mov        eax, dword ptr [ebp - 8]
004603a5 f30f5980cc030000         mulss      xmm0, dword ptr [eax + 0x3cc]
004603ad f30f5e054ce05600         divss      xmm0, dword ptr [0x56e04c]
004603b5 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004603b8 f30f1089cc030000         movss      xmm1, dword ptr [ecx + 0x3cc]
004603c0 f30f5cc8                 subss      xmm1, xmm0
004603c4 51                       push       ecx
004603c5 f30f110c24               movss      dword ptr [esp], xmm1
004603ca 8b55f8                   mov        edx, dword ptr [ebp - 8]
004603cd 51                       push       ecx
004603ce f30f1082c8030000         movss      xmm0, dword ptr [edx + 0x3c8]
004603d6 f30f110424               movss      dword ptr [esp], xmm0
004603db e82082fdff               call       0x438600
004603e0 83c408                   add        esp, 8
004603e3 8b45f8                   mov        eax, dword ptr [ebp - 8]
004603e6 d998c8030000             fstp       dword ptr [eax + 0x3c8]
004603ec e9d6000000               jmp        0x4604c7
004603f1 b9c4a45b00               mov        ecx, 0x5ba4c4
004603f6 e83594fcff               call       0x429830
004603fb d95dc4                   fstp       dword ptr [ebp - 0x3c]
004603fe f30f1045c4               movss      xmm0, dword ptr [ebp - 0x3c]
00460403 f30f5905ccc85600         mulss      xmm0, dword ptr [0x56c8cc]
0046040b f30f5805e0c85600         addss      xmm0, dword ptr [0x56c8e0]
00460413 f30f1145bc               movss      dword ptr [ebp - 0x44], xmm0
00460418 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0046041b f30f1081c8030000         movss      xmm0, dword ptr [ecx + 0x3c8]
00460423 f30f1145b8               movss      dword ptr [ebp - 0x48], xmm0
00460428 8b55f8                   mov        edx, dword ptr [ebp - 8]
0046042b 83c208                   add        edx, 8
0046042e 8955c0                   mov        dword ptr [ebp - 0x40], edx
00460431 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00460434 50                       push       eax
00460435 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00460438 e833d2ffff               call       0x45d670
0046043d 8945b4                   mov        dword ptr [ebp - 0x4c], eax
00460440 51                       push       ecx
00460441 f30f1045bc               movss      xmm0, dword ptr [ebp - 0x44]
00460446 f30f110424               movss      dword ptr [esp], xmm0
0046044b 51                       push       ecx
0046044c f30f1045b8               movss      xmm0, dword ptr [ebp - 0x48]
00460451 f30f110424               movss      dword ptr [esp], xmm0
00460456 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00460459 51                       push       ecx
0046045a e8d18efdff               call       0x439330
0046045f 83c40c                   add        esp, 0xc
00460462 b9c4a45b00               mov        ecx, 0x5ba4c4
00460467 e86493fcff               call       0x4297d0
0046046c d95db0                   fstp       dword ptr [ebp - 0x50]
0046046f f30f1045b0               movss      xmm0, dword ptr [ebp - 0x50]
00460474 f30f590528ef5600         mulss      xmm0, dword ptr [0x56ef28]
0046047c 8b55f8                   mov        edx, dword ptr [ebp - 8]
0046047f f30f108acc030000         movss      xmm1, dword ptr [edx + 0x3cc]
00460487 f30f5cc8                 subss      xmm1, xmm0
0046048b f30f114dac               movss      dword ptr [ebp - 0x54], xmm1
00460490 8b45f8                   mov        eax, dword ptr [ebp - 8]
00460493 f30f1080c8030000         movss      xmm0, dword ptr [eax + 0x3c8]
0046049b f30f1145a8               movss      dword ptr [ebp - 0x58], xmm0
004604a0 51                       push       ecx
004604a1 f30f1045ac               movss      xmm0, dword ptr [ebp - 0x54]
004604a6 f30f110424               movss      dword ptr [esp], xmm0
004604ab 51                       push       ecx
004604ac f30f1045a8               movss      xmm0, dword ptr [ebp - 0x58]
004604b1 f30f110424               movss      dword ptr [esp], xmm0
004604b6 e84581fdff               call       0x438600
004604bb 83c408                   add        esp, 8
004604be 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004604c1 d999c8030000             fstp       dword ptr [ecx + 0x3c8]
004604c7 837df400                 cmp        dword ptr [ebp - 0xc], 0
004604cb 7e40                     jle        0x46050d
004604cd 8b55f8                   mov        edx, dword ptr [ebp - 8]
004604d0 83c208                   add        edx, 8
004604d3 8955a4                   mov        dword ptr [ebp - 0x5c], edx
004604d6 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004604d9 83e801                   sub        eax, 1
004604dc 50                       push       eax
004604dd 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
004604e0 e88bd1ffff               call       0x45d670
004604e5 8945a0                   mov        dword ptr [ebp - 0x60], eax
004604e8 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004604eb 83c108                   add        ecx, 8
004604ee 894d9c                   mov        dword ptr [ebp - 0x64], ecx
004604f1 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
004604f4 52                       push       edx
004604f5 8d4588                   lea        eax, [ebp - 0x78]
004604f8 50                       push       eax
004604f9 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004604fc 51                       push       ecx
004604fd 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
00460500 e86bd1ffff               call       0x45d670
00460505 8bc8                     mov        ecx, eax
00460507 e85494fdff               call       0x439960
0046050c 90                       nop        
0046050d 6a00                     push       0
0046050f e81c030000               call       0x460830
00460514 83c404                   add        esp, 4
00460517 85c0                     test       eax, eax
00460519 742a                     je         0x460545
0046051b 8b55f8                   mov        edx, dword ptr [ebp - 8]
0046051e 8b4204                   mov        eax, dword ptr [edx + 4]
00460521 894594                   mov        dword ptr [ebp - 0x6c], eax
00460524 6a00                     push       0
00460526 e805030000               call       0x460830
0046052b 83c404                   add        esp, 4
0046052e 8bc8                     mov        ecx, eax
00460530 e81b030000               call       0x460850
00460535 894598                   mov        dword ptr [ebp - 0x68], eax
00460538 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
0046053b 51                       push       ecx
0046053c 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
0046053f e81cd8ffff               call       0x45dd60
00460544 90                       nop        
00460545 eb07                     jmp        0x46054e
00460547 b801000000               mov        eax, 1
0046054c eb18                     jmp        0x460566
0046054e 8b55f8                   mov        edx, dword ptr [ebp - 8]
00460551 81c2d0030000             add        edx, 0x3d0
00460557 895590                   mov        dword ptr [ebp - 0x70], edx
0046055a 6a00                     push       0
0046055c 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
0046055f e8dc2ffcff               call       0x423540
00460564 33c0                     xor        eax, eax
00460566 8be5                     mov        esp, ebp
00460568 5d                       pop        ebp
00460569 c3                       ret        
0046056a cc                       int3       
0046056b cc                       int3       
0046056c cc                       int3       
0046056d cc                       int3       
0046056e cc                       int3       
0046056f cc                       int3       
