004a5350 55                       push       ebp
004a5351 8bec                     mov        ebp, esp
004a5353 83ec50                   sub        esp, 0x50
004a5356 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004a535b 33c5                     xor        eax, ebp
004a535d 8945fc                   mov        dword ptr [ebp - 4], eax
004a5360 56                       push       esi
004a5361 894dec                   mov        dword ptr [ebp - 0x14], ecx
004a5364 8d4df0                   lea        ecx, [ebp - 0x10]
004a5367 e8a4daf7ff               call       0x422e10
004a536c b9a8a45b00               mov        ecx, 0x5ba4a8
004a5371 e85a44f8ff               call       0x4297d0
004a5376 d95ddc                   fstp       dword ptr [ebp - 0x24]
004a5379 c745e800000000           mov        dword ptr [ebp - 0x18], 0
004a5380 eb09                     jmp        0x4a538b
004a5382 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004a5385 83c001                   add        eax, 1
004a5388 8945e8                   mov        dword ptr [ebp - 0x18], eax
004a538b 837de80f                 cmp        dword ptr [ebp - 0x18], 0xf
004a538f 0f8d03020000             jge        0x4a5598
004a5395 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004a5398 83c10c                   add        ecx, 0xc
004a539b 894dd8                   mov        dword ptr [ebp - 0x28], ecx
004a539e 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004a53a1 52                       push       edx
004a53a2 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004a53a5 e8d6f1f6ff               call       0x414580
004a53aa 8b00                     mov        eax, dword ptr [eax]
004a53ac 8945e0                   mov        dword ptr [ebp - 0x20], eax
004a53af 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004a53b2 83b98c00000000           cmp        dword ptr [ecx + 0x8c], 0
004a53b9 0f8496000000             je         0x4a5455
004a53bf 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004a53c2 81c290000000             add        edx, 0x90
004a53c8 8955d4                   mov        dword ptr [ebp - 0x2c], edx
004a53cb 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004a53ce 8b888c000000             mov        ecx, dword ptr [eax + 0x8c]
004a53d4 51                       push       ecx
004a53d5 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004a53d8 e883e1f7ff               call       0x423560
004a53dd 0fb6d0                   movzx      edx, al
004a53e0 85d2                     test       edx, edx
004a53e2 7471                     je         0x4a5455
004a53e4 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004a53e7 83c04c                   add        eax, 0x4c
004a53ea 8945d0                   mov        dword ptr [ebp - 0x30], eax
004a53ed 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004a53f0 51                       push       ecx
004a53f1 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004a53f4 e887f1f6ff               call       0x414580
004a53f9 833800                   cmp        dword ptr [eax], 0
004a53fc 7e57                     jle        0x4a5455
004a53fe 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004a5401 83c24c                   add        edx, 0x4c
004a5404 8955c8                   mov        dword ptr [ebp - 0x38], edx
004a5407 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004a540a 83c04c                   add        eax, 0x4c
004a540d 8945cc                   mov        dword ptr [ebp - 0x34], eax
004a5410 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004a5413 81c190000000             add        ecx, 0x90
004a5419 e872abf6ff               call       0x40ff90
004a541e 8bf0                     mov        esi, eax
004a5420 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004a5423 51                       push       ecx
004a5424 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004a5427 e854f1f6ff               call       0x414580
004a542c 8bd6                     mov        edx, esi
004a542e 0faf10                   imul       edx, dword ptr [eax]
004a5431 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004a5434 8bc2                     mov        eax, edx
004a5436 99                       cdq        
004a5437 f7b98c000000             idiv       dword ptr [ecx + 0x8c]
004a543d 8bf0                     mov        esi, eax
004a543f 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004a5442 52                       push       edx
004a5443 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004a5446 e835f1f6ff               call       0x414580
004a544b 8b00                     mov        eax, dword ptr [eax]
004a544d 2bc6                     sub        eax, esi
004a544f 0345e0                   add        eax, dword ptr [ebp - 0x20]
004a5452 8945e0                   mov        dword ptr [ebp - 0x20], eax
004a5455 c745e400000000           mov        dword ptr [ebp - 0x1c], 0
004a545c eb09                     jmp        0x4a5467
004a545e 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a5461 83c101                   add        ecx, 1
004a5464 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004a5467 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004a546a 3b55e0                   cmp        edx, dword ptr [ebp - 0x20]
004a546d 0f8d20010000             jge        0x4a5593
004a5473 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004a5476 51                       push       ecx
004a5477 f30f1080a4000000         movss      xmm0, dword ptr [eax + 0xa4]
004a547f f30f110424               movss      dword ptr [esp], xmm0
004a5484 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004a5487 51                       push       ecx
004a5488 f30f1081a0000000         movss      xmm0, dword ptr [ecx + 0xa0]
004a5490 f30f110424               movss      dword ptr [esp], xmm0
004a5495 51                       push       ecx
004a5496 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
004a549b f30f110424               movss      dword ptr [esp], xmm0
004a54a0 8d55f0                   lea        edx, [ebp - 0x10]
004a54a3 52                       push       edx
004a54a4 e8e73cfbff               call       0x459190
004a54a9 83c410                   add        esp, 0x10
004a54ac b9a8a45b00               mov        ecx, 0x5ba4a8
004a54b1 e87a43f8ff               call       0x429830
004a54b6 d95dc4                   fstp       dword ptr [ebp - 0x3c]
004a54b9 f30f1045c4               movss      xmm0, dword ptr [ebp - 0x3c]
004a54be f30f5905ece05600         mulss      xmm0, dword ptr [0x56e0ec]
004a54c6 f30f5805ece05600         addss      xmm0, dword ptr [0x56e0ec]
004a54ce 51                       push       ecx
004a54cf f30f110424               movss      dword ptr [esp], xmm0
004a54d4 8d4df0                   lea        ecx, [ebp - 0x10]
004a54d7 e8b441f8ff               call       0x429690
004a54dc 0f57c0                   xorps      xmm0, xmm0
004a54df f30f1145f8               movss      dword ptr [ebp - 8], xmm0
004a54e4 8b4508                   mov        eax, dword ptr [ebp + 8]
004a54e7 50                       push       eax
004a54e8 8d4df0                   lea        ecx, [ebp - 0x10]
004a54eb e8f041f8ff               call       0x4296e0
004a54f0 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004a54f3 8b11                     mov        edx, dword ptr [ecx]
004a54f5 52                       push       edx
004a54f6 e8d53affff               call       0x498fd0
004a54fb 83c404                   add        esp, 4
004a54fe 8945c0                   mov        dword ptr [ebp - 0x40], eax
004a5501 6aff                     push       -1
004a5503 6a00                     push       0
004a5505 6a00                     push       0
004a5507 51                       push       ecx
004a5508 f30f100534fd5600         movss      xmm0, dword ptr [0x56fd34]
004a5510 f30f110424               movss      dword ptr [esp], xmm0
004a5515 f30f1005f8e05600         movss      xmm0, dword ptr [0x56e0f8]
004a551d f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004a5525 51                       push       ecx
004a5526 f30f110424               movss      dword ptr [esp], xmm0
004a552b 6aff                     push       -1
004a552d 8d45f0                   lea        eax, [ebp - 0x10]
004a5530 50                       push       eax
004a5531 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004a5534 83c101                   add        ecx, 1
004a5537 51                       push       ecx
004a5538 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004a553b e850e70100               call       0x4c3c90
004a5540 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
004a5548 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004a5550 f30f5845dc               addss      xmm0, dword ptr [ebp - 0x24]
004a5555 b9a8a45b00               mov        ecx, 0x5ba4a8
004a555a f30f1145b8               movss      dword ptr [ebp - 0x48], xmm0
004a555f e86c42f8ff               call       0x4297d0
004a5564 d95dbc                   fstp       dword ptr [ebp - 0x44]
004a5567 f30f1045bc               movss      xmm0, dword ptr [ebp - 0x44]
004a556c f30f5e05e0c85600         divss      xmm0, dword ptr [0x56c8e0]
004a5574 f30f104db8               movss      xmm1, dword ptr [ebp - 0x48]
004a5579 f30f58c8                 addss      xmm1, xmm0
004a557d 51                       push       ecx
004a557e f30f110c24               movss      dword ptr [esp], xmm1
004a5583 e8b82ff9ff               call       0x438540
004a5588 83c404                   add        esp, 4
004a558b d95ddc                   fstp       dword ptr [ebp - 0x24]
004a558e e9cbfeffff               jmp        0x4a545e
004a5593 e9eafdffff               jmp        0x4a5382
004a5598 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004a559b 83c20c                   add        edx, 0xc
004a559e 8955b0                   mov        dword ptr [ebp - 0x50], edx
004a55a1 c745b400000000           mov        dword ptr [ebp - 0x4c], 0
004a55a8 8d45b4                   lea        eax, [ebp - 0x4c]
004a55ab 50                       push       eax
004a55ac 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
004a55af e83c2cffff               call       0x4981f0
004a55b4 90                       nop        
004a55b5 5e                       pop        esi
004a55b6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a55b9 33cd                     xor        ecx, ebp
004a55bb e82ed40900               call       0x5429ee
004a55c0 8be5                     mov        esp, ebp
004a55c2 5d                       pop        ebp
004a55c3 c20400                   ret        4
004a55c6 cc                       int3       
004a55c7 cc                       int3       
004a55c8 cc                       int3       
004a55c9 cc                       int3       
004a55ca cc                       int3       
004a55cb cc                       int3       
004a55cc cc                       int3       
004a55cd cc                       int3       
004a55ce cc                       int3       
004a55cf cc                       int3       
