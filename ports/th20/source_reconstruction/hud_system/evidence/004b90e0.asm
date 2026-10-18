004b90e0 55                       push       ebp
004b90e1 8bec                     mov        ebp, esp
004b90e3 81ecfc000000             sub        esp, 0xfc
004b90e9 894dfc                   mov        dword ptr [ebp - 4], ecx
004b90ec 8b4508                   mov        eax, dword ptr [ebp + 8]
004b90ef 8945ec                   mov        dword ptr [ebp - 0x14], eax
004b90f2 837dec06                 cmp        dword ptr [ebp - 0x14], 6
004b90f6 0f87d0050000             ja         0x4b96cc
004b90fc 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004b90ff ff248dd4964b00           jmp        dword ptr [ecx*4 + 0x4b96d4]
004b9106 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b9109 81c2bc000000             add        edx, 0xbc
004b910f 8955e4                   mov        dword ptr [ebp - 0x1c], edx
004b9112 6a00                     push       0
004b9114 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004b9117 e864b4f5ff               call       0x414580
004b911c 8bc8                     mov        ecx, eax
004b911e e8ad6bf9ff               call       0x44fcd0
004b9123 90                       nop        
004b9124 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b9127 8b88cc020000             mov        ecx, dword ptr [eax + 0x2cc]
004b912d 894de0                   mov        dword ptr [ebp - 0x20], ecx
004b9130 6a00                     push       0
004b9132 6aff                     push       -1
004b9134 6a31                     push       0x31
004b9136 6838065700               push       0x570638
004b913b 8d952cffffff             lea        edx, [ebp - 0xd4]
004b9141 52                       push       edx
004b9142 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004b9145 e8267bf9ff               call       0x450c70
004b914a 8b00                     mov        eax, dword ptr [eax]
004b914c 8945d8                   mov        dword ptr [ebp - 0x28], eax
004b914f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b9152 81c1bc000000             add        ecx, 0xbc
004b9158 894ddc                   mov        dword ptr [ebp - 0x24], ecx
004b915b 6a00                     push       0
004b915d 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004b9160 e81bb4f5ff               call       0x414580
004b9165 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
004b9168 8910                     mov        dword ptr [eax], edx
004b916a 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004b916d 8945f0                   mov        dword ptr [ebp - 0x10], eax
004b9170 c745f480969800           mov        dword ptr [ebp - 0xc], 0x989680
004b9177 c745e800000000           mov        dword ptr [ebp - 0x18], 0
004b917e c745f800000000           mov        dword ptr [ebp - 8], 0
004b9185 eb09                     jmp        0x4b9190
004b9187 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004b918a 83c101                   add        ecx, 1
004b918d 894df8                   mov        dword ptr [ebp - 8], ecx
004b9190 837df808                 cmp        dword ptr [ebp - 8], 8
004b9194 0f8d13010000             jge        0x4b92ad
004b919a 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b919d 81c294000000             add        edx, 0x94
004b91a3 8955d4                   mov        dword ptr [ebp - 0x2c], edx
004b91a6 8b45f8                   mov        eax, dword ptr [ebp - 8]
004b91a9 50                       push       eax
004b91aa 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004b91ad e8ceb3f5ff               call       0x414580
004b91b2 8bc8                     mov        ecx, eax
004b91b4 e8176bf9ff               call       0x44fcd0
004b91b9 90                       nop        
004b91ba 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004b91c0 e88be7f7ff               call       0x437950
004b91c5 8945d0                   mov        dword ptr [ebp - 0x30], eax
004b91c8 6a00                     push       0
004b91ca 6aff                     push       -1
004b91cc 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004b91cf 83c104                   add        ecx, 4
004b91d2 51                       push       ecx
004b91d3 6a00                     push       0
004b91d5 8d9528ffffff             lea        edx, [ebp - 0xd8]
004b91db 52                       push       edx
004b91dc 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004b91df e88c7af9ff               call       0x450c70
004b91e4 8b00                     mov        eax, dword ptr [eax]
004b91e6 8945c8                   mov        dword ptr [ebp - 0x38], eax
004b91e9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b91ec 81c194000000             add        ecx, 0x94
004b91f2 894dcc                   mov        dword ptr [ebp - 0x34], ecx
004b91f5 8b55f8                   mov        edx, dword ptr [ebp - 8]
004b91f8 52                       push       edx
004b91f9 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004b91fc e87fb3f5ff               call       0x414580
004b9201 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004b9204 8908                     mov        dword ptr [eax], ecx
004b9206 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004b9209 99                       cdq        
004b920a f77df4                   idiv       dword ptr [ebp - 0xc]
004b920d 85c0                     test       eax, eax
004b920f 7407                     je         0x4b9218
004b9211 c745e801000000           mov        dword ptr [ebp - 0x18], 1
004b9218 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b921b 81c294000000             add        edx, 0x94
004b9221 8955c4                   mov        dword ptr [ebp - 0x3c], edx
004b9224 8b45f8                   mov        eax, dword ptr [ebp - 8]
004b9227 50                       push       eax
004b9228 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004b922b e850b3f5ff               call       0x414580
004b9230 8945c0                   mov        dword ptr [ebp - 0x40], eax
004b9233 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004b9236 99                       cdq        
004b9237 f77df4                   idiv       dword ptr [ebp - 0xc]
004b923a 05ef000000               add        eax, 0xef
004b923f 50                       push       eax
004b9240 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004b9243 e8282bf9ff               call       0x44bd70
004b9248 90                       nop        
004b9249 837de800                 cmp        dword ptr [ebp - 0x18], 0
004b924d 7522                     jne        0x4b9271
004b924f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b9252 81c194000000             add        ecx, 0x94
004b9258 894dbc                   mov        dword ptr [ebp - 0x44], ecx
004b925b 8b55f8                   mov        edx, dword ptr [ebp - 8]
004b925e 52                       push       edx
004b925f 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004b9262 e819b3f5ff               call       0x414580
004b9267 8bc8                     mov        ecx, eax
004b9269 e8b26ef9ff               call       0x450120
004b926e 90                       nop        
004b926f eb1f                     jmp        0x4b9290
004b9271 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b9274 0594000000               add        eax, 0x94
004b9279 8945b8                   mov        dword ptr [ebp - 0x48], eax
004b927c 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004b927f 51                       push       ecx
004b9280 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004b9283 e8f8b2f5ff               call       0x414580
004b9288 8bc8                     mov        ecx, eax
004b928a e8a167f9ff               call       0x44fa30
004b928f 90                       nop        
004b9290 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004b9293 99                       cdq        
004b9294 f77df4                   idiv       dword ptr [ebp - 0xc]
004b9297 8955f0                   mov        dword ptr [ebp - 0x10], edx
004b929a 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004b929d 99                       cdq        
004b929e b90a000000               mov        ecx, 0xa
004b92a3 f7f9                     idiv       ecx
004b92a5 8945f4                   mov        dword ptr [ebp - 0xc], eax
004b92a8 e9dafeffff               jmp        0x4b9187
004b92ad 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b92b0 81c294000000             add        edx, 0x94
004b92b6 8955b4                   mov        dword ptr [ebp - 0x4c], edx
004b92b9 6a08                     push       8
004b92bb 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
004b92be e8bdb2f5ff               call       0x414580
004b92c3 8bc8                     mov        ecx, eax
004b92c5 e8066af9ff               call       0x44fcd0
004b92ca 90                       nop        
004b92cb 817d0c40420f00           cmp        dword ptr [ebp + 0xc], 0xf4240
004b92d2 7c6c                     jl         0x4b9340
004b92d4 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004b92da e871e6f7ff               call       0x437950
004b92df 8945b0                   mov        dword ptr [ebp - 0x50], eax
004b92e2 6a00                     push       0
004b92e4 6aff                     push       -1
004b92e6 6a0c                     push       0xc
004b92e8 6a00                     push       0
004b92ea 8d8524ffffff             lea        eax, [ebp - 0xdc]
004b92f0 50                       push       eax
004b92f1 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
004b92f4 e87779f9ff               call       0x450c70
004b92f9 8b08                     mov        ecx, dword ptr [eax]
004b92fb 894da8                   mov        dword ptr [ebp - 0x58], ecx
004b92fe 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b9301 81c294000000             add        edx, 0x94
004b9307 8955ac                   mov        dword ptr [ebp - 0x54], edx
004b930a 6a08                     push       8
004b930c 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
004b930f e86cb2f5ff               call       0x414580
004b9314 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004b9317 8908                     mov        dword ptr [eax], ecx
004b9319 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b931c 81c294000000             add        edx, 0x94
004b9322 8955a4                   mov        dword ptr [ebp - 0x5c], edx
004b9325 6a08                     push       8
004b9327 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
004b932a e851b2f5ff               call       0x414580
004b932f 8945a0                   mov        dword ptr [ebp - 0x60], eax
004b9332 68fd000000               push       0xfd
004b9337 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
004b933a e8312af9ff               call       0x44bd70
004b933f 90                       nop        
004b9340 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b9343 0594000000               add        eax, 0x94
004b9348 89459c                   mov        dword ptr [ebp - 0x64], eax
004b934b 6a09                     push       9
004b934d 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
004b9350 e82bb2f5ff               call       0x414580
004b9355 8bc8                     mov        ecx, eax
004b9357 e87469f9ff               call       0x44fcd0
004b935c 90                       nop        
004b935d 817d0ce8030000           cmp        dword ptr [ebp + 0xc], 0x3e8
004b9364 7c6b                     jl         0x4b93d1
004b9366 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004b936c e8dfe5f7ff               call       0x437950
004b9371 894598                   mov        dword ptr [ebp - 0x68], eax
004b9374 6a00                     push       0
004b9376 6aff                     push       -1
004b9378 6a0d                     push       0xd
004b937a 6a00                     push       0
004b937c 8d8d20ffffff             lea        ecx, [ebp - 0xe0]
004b9382 51                       push       ecx
004b9383 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
004b9386 e8e578f9ff               call       0x450c70
004b938b 8b10                     mov        edx, dword ptr [eax]
004b938d 895590                   mov        dword ptr [ebp - 0x70], edx
004b9390 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b9393 0594000000               add        eax, 0x94
004b9398 894594                   mov        dword ptr [ebp - 0x6c], eax
004b939b 6a09                     push       9
004b939d 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
004b93a0 e8dbb1f5ff               call       0x414580
004b93a5 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
004b93a8 8908                     mov        dword ptr [eax], ecx
004b93aa 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b93ad 81c294000000             add        edx, 0x94
004b93b3 89558c                   mov        dword ptr [ebp - 0x74], edx
004b93b6 6a09                     push       9
004b93b8 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004b93bb e8c0b1f5ff               call       0x414580
004b93c0 894588                   mov        dword ptr [ebp - 0x78], eax
004b93c3 68fd000000               push       0xfd
004b93c8 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
004b93cb e8a029f9ff               call       0x44bd70
004b93d0 90                       nop        
004b93d1 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b93d4 c7803801000001000000     mov        dword ptr [eax + 0x138], 1
004b93de 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b93e1 8b91cc020000             mov        edx, dword ptr [ecx + 0x2cc]
004b93e7 895584                   mov        dword ptr [ebp - 0x7c], edx
004b93ea 6a00                     push       0
004b93ec 6aff                     push       -1
004b93ee 6a54                     push       0x54
004b93f0 6838065700               push       0x570638
004b93f5 8d851cffffff             lea        eax, [ebp - 0xe4]
004b93fb 50                       push       eax
004b93fc 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
004b93ff e86c78f9ff               call       0x450c70
004b9404 8b08                     mov        ecx, dword ptr [eax]
004b9406 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b9409 898a14010000             mov        dword ptr [edx + 0x114], ecx
004b940f e9b8020000               jmp        0x4b96cc
004b9414 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b9417 05bc000000               add        eax, 0xbc
004b941c 894580                   mov        dword ptr [ebp - 0x80], eax
004b941f 6a00                     push       0
004b9421 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
004b9424 e857b1f5ff               call       0x414580
004b9429 8bc8                     mov        ecx, eax
004b942b e8a068f9ff               call       0x44fcd0
004b9430 90                       nop        
004b9431 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b9434 8b91cc020000             mov        edx, dword ptr [ecx + 0x2cc]
004b943a 89957cffffff             mov        dword ptr [ebp - 0x84], edx
004b9440 6a00                     push       0
004b9442 6aff                     push       -1
004b9444 6a32                     push       0x32
004b9446 6838065700               push       0x570638
004b944b 8d8518ffffff             lea        eax, [ebp - 0xe8]
004b9451 50                       push       eax
004b9452 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
004b9458 e81378f9ff               call       0x450c70
004b945d 8b08                     mov        ecx, dword ptr [eax]
004b945f 898d74ffffff             mov        dword ptr [ebp - 0x8c], ecx
004b9465 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b9468 81c2bc000000             add        edx, 0xbc
004b946e 899578ffffff             mov        dword ptr [ebp - 0x88], edx
004b9474 6a00                     push       0
004b9476 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
004b947c e8ffb0f5ff               call       0x414580
004b9481 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
004b9487 8908                     mov        dword ptr [eax], ecx
004b9489 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b948c c7823801000001000000     mov        dword ptr [edx + 0x138], 1
004b9496 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b9499 8b88cc020000             mov        ecx, dword ptr [eax + 0x2cc]
004b949f 898d70ffffff             mov        dword ptr [ebp - 0x90], ecx
004b94a5 6a00                     push       0
004b94a7 6aff                     push       -1
004b94a9 6a54                     push       0x54
004b94ab 6838065700               push       0x570638
004b94b0 8d9514ffffff             lea        edx, [ebp - 0xec]
004b94b6 52                       push       edx
004b94b7 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
004b94bd e8ae77f9ff               call       0x450c70
004b94c2 8b00                     mov        eax, dword ptr [eax]
004b94c4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b94c7 898114010000             mov        dword ptr [ecx + 0x114], eax
004b94cd e9fa010000               jmp        0x4b96cc
004b94d2 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b94d5 81c2bc000000             add        edx, 0xbc
004b94db 89956cffffff             mov        dword ptr [ebp - 0x94], edx
004b94e1 6a01                     push       1
004b94e3 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
004b94e9 e892b0f5ff               call       0x414580
004b94ee 8bc8                     mov        ecx, eax
004b94f0 e8db67f9ff               call       0x44fcd0
004b94f5 90                       nop        
004b94f6 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b94f9 8b88cc020000             mov        ecx, dword ptr [eax + 0x2cc]
004b94ff 898d68ffffff             mov        dword ptr [ebp - 0x98], ecx
004b9505 6a00                     push       0
004b9507 6aff                     push       -1
004b9509 6a33                     push       0x33
004b950b 6838065700               push       0x570638
004b9510 8d9510ffffff             lea        edx, [ebp - 0xf0]
004b9516 52                       push       edx
004b9517 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
004b951d e84e77f9ff               call       0x450c70
004b9522 8b00                     mov        eax, dword ptr [eax]
004b9524 898560ffffff             mov        dword ptr [ebp - 0xa0], eax
004b952a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b952d 81c1bc000000             add        ecx, 0xbc
004b9533 898d64ffffff             mov        dword ptr [ebp - 0x9c], ecx
004b9539 6a01                     push       1
004b953b 8b8d64ffffff             mov        ecx, dword ptr [ebp - 0x9c]
004b9541 e83ab0f5ff               call       0x414580
004b9546 8b9560ffffff             mov        edx, dword ptr [ebp - 0xa0]
004b954c 8910                     mov        dword ptr [eax], edx
004b954e e979010000               jmp        0x4b96cc
004b9553 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b9556 05bc000000               add        eax, 0xbc
004b955b 89855cffffff             mov        dword ptr [ebp - 0xa4], eax
004b9561 6a01                     push       1
004b9563 8b8d5cffffff             mov        ecx, dword ptr [ebp - 0xa4]
004b9569 e812b0f5ff               call       0x414580
004b956e 8bc8                     mov        ecx, eax
004b9570 e85b67f9ff               call       0x44fcd0
004b9575 90                       nop        
004b9576 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b9579 8b91cc020000             mov        edx, dword ptr [ecx + 0x2cc]
004b957f 899558ffffff             mov        dword ptr [ebp - 0xa8], edx
004b9585 6a00                     push       0
004b9587 6aff                     push       -1
004b9589 6a34                     push       0x34
004b958b 6838065700               push       0x570638
004b9590 8d850cffffff             lea        eax, [ebp - 0xf4]
004b9596 50                       push       eax
004b9597 8b8d58ffffff             mov        ecx, dword ptr [ebp - 0xa8]
004b959d e8ce76f9ff               call       0x450c70
004b95a2 8b08                     mov        ecx, dword ptr [eax]
004b95a4 898d50ffffff             mov        dword ptr [ebp - 0xb0], ecx
004b95aa 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b95ad 81c2bc000000             add        edx, 0xbc
004b95b3 899554ffffff             mov        dword ptr [ebp - 0xac], edx
004b95b9 6a01                     push       1
004b95bb 8b8d54ffffff             mov        ecx, dword ptr [ebp - 0xac]
004b95c1 e8baaff5ff               call       0x414580
004b95c6 8b8d50ffffff             mov        ecx, dword ptr [ebp - 0xb0]
004b95cc 8908                     mov        dword ptr [eax], ecx
004b95ce e9f9000000               jmp        0x4b96cc
004b95d3 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b95d6 81c2bc000000             add        edx, 0xbc
004b95dc 89954cffffff             mov        dword ptr [ebp - 0xb4], edx
004b95e2 6a01                     push       1
004b95e4 8b8d4cffffff             mov        ecx, dword ptr [ebp - 0xb4]
004b95ea e891aff5ff               call       0x414580
004b95ef 8bc8                     mov        ecx, eax
004b95f1 e8da66f9ff               call       0x44fcd0
004b95f6 90                       nop        
004b95f7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b95fa 8b88cc020000             mov        ecx, dword ptr [eax + 0x2cc]
004b9600 898d48ffffff             mov        dword ptr [ebp - 0xb8], ecx
004b9606 6a00                     push       0
004b9608 6aff                     push       -1
004b960a 6a35                     push       0x35
004b960c 6838065700               push       0x570638
004b9611 8d9508ffffff             lea        edx, [ebp - 0xf8]
004b9617 52                       push       edx
004b9618 8b8d48ffffff             mov        ecx, dword ptr [ebp - 0xb8]
004b961e e84d76f9ff               call       0x450c70
004b9623 8b00                     mov        eax, dword ptr [eax]
004b9625 898540ffffff             mov        dword ptr [ebp - 0xc0], eax
004b962b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b962e 81c1bc000000             add        ecx, 0xbc
004b9634 898d44ffffff             mov        dword ptr [ebp - 0xbc], ecx
004b963a 6a01                     push       1
004b963c 8b8d44ffffff             mov        ecx, dword ptr [ebp - 0xbc]
004b9642 e839aff5ff               call       0x414580
004b9647 8b9540ffffff             mov        edx, dword ptr [ebp - 0xc0]
004b964d 8910                     mov        dword ptr [eax], edx
004b964f eb7b                     jmp        0x4b96cc
004b9651 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b9654 05bc000000               add        eax, 0xbc
004b9659 89853cffffff             mov        dword ptr [ebp - 0xc4], eax
004b965f 6a00                     push       0
004b9661 8b8d3cffffff             mov        ecx, dword ptr [ebp - 0xc4]
004b9667 e814aff5ff               call       0x414580
004b966c 8bc8                     mov        ecx, eax
004b966e e85d66f9ff               call       0x44fcd0
004b9673 90                       nop        
004b9674 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b9677 8b91cc020000             mov        edx, dword ptr [ecx + 0x2cc]
004b967d 899538ffffff             mov        dword ptr [ebp - 0xc8], edx
004b9683 6a00                     push       0
004b9685 6aff                     push       -1
004b9687 6a36                     push       0x36
004b9689 6838065700               push       0x570638
004b968e 8d8504ffffff             lea        eax, [ebp - 0xfc]
004b9694 50                       push       eax
004b9695 8b8d38ffffff             mov        ecx, dword ptr [ebp - 0xc8]
004b969b e8d075f9ff               call       0x450c70
004b96a0 8b08                     mov        ecx, dword ptr [eax]
004b96a2 898d30ffffff             mov        dword ptr [ebp - 0xd0], ecx
004b96a8 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b96ab 81c2bc000000             add        edx, 0xbc
004b96b1 899534ffffff             mov        dword ptr [ebp - 0xcc], edx
004b96b7 6a00                     push       0
004b96b9 8b8d34ffffff             mov        ecx, dword ptr [ebp - 0xcc]
004b96bf e8bcaef5ff               call       0x414580
004b96c4 8b8d30ffffff             mov        ecx, dword ptr [ebp - 0xd0]
004b96ca 8908                     mov        dword ptr [eax], ecx
004b96cc 8be5                     mov        esp, ebp
004b96ce 5d                       pop        ebp
004b96cf c20800                   ret        8
004b96d2 6690                     nop        
004b96d4 06                       push       es
004b96d5 91                       xchg       ecx, eax
004b96d6 4b                       dec        ebx
004b96d7 001494                   add        byte ptr [esp + edx*4], dl
004b96da 4b                       dec        ebx
004b96db 00d2                     add        dl, dl
004b96dd 94                       xchg       esp, eax
004b96de 4b                       dec        ebx
004b96df 005395                   add        byte ptr [ebx - 0x6b], dl
004b96e2 4b                       dec        ebx
004b96e3 00d3                     add        bl, dl
004b96e5 95                       xchg       ebp, eax
004b96e6 4b                       dec        ebx
004b96e7 00cc                     add        ah, cl
004b96e9 96                       xchg       esi, eax
004b96ea 4b                       dec        ebx
004b96eb 005196                   add        byte ptr [ecx - 0x6a], dl
004b96ee 4b                       dec        ebx
