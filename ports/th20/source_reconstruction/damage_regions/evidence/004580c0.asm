004580c0 55                       push       ebp
004580c1 8bec                     mov        ebp, esp
004580c3 83ec64                   sub        esp, 0x64
004580c6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004580cb 33c5                     xor        eax, ebp
004580cd 8945fc                   mov        dword ptr [ebp - 4], eax
004580d0 68a0984300               push       0x4398a0
004580d5 6a04                     push       4
004580d7 6a08                     push       8
004580d9 8d45bc                   lea        eax, [ebp - 0x44]
004580dc 50                       push       eax
004580dd e83e3bfbff               call       0x40bc20
004580e2 68a0984300               push       0x4398a0
004580e7 6a04                     push       4
004580e9 6a08                     push       8
004580eb 8d4ddc                   lea        ecx, [ebp - 0x24]
004580ee 51                       push       ecx
004580ef e82c3bfbff               call       0x40bc20
004580f4 90                       nop        
004580f5 51                       push       ecx
004580f6 f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
004580fb f30f110424               movss      dword ptr [esp], xmm0
00458100 51                       push       ecx
00458101 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00458106 f30f110424               movss      dword ptr [esp], xmm0
0045810b 51                       push       ecx
0045810c f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00458111 f30f110424               movss      dword ptr [esp], xmm0
00458116 51                       push       ecx
00458117 f30f104508               movss      xmm0, dword ptr [ebp + 8]
0045811c f30f110424               movss      dword ptr [esp], xmm0
00458121 e8badeffff               call       0x455fe0
00458126 83c410                   add        esp, 0x10
00458129 d95dac                   fstp       dword ptr [ebp - 0x54]
0045812c f30f1045ac               movss      xmm0, dword ptr [ebp - 0x54]
00458131 f30f104d14               movss      xmm1, dword ptr [ebp + 0x14]
00458136 f30f5e0dd0c85600         divss      xmm1, dword ptr [0x56c8d0]
0045813e 51                       push       ecx
0045813f f30f110c24               movss      dword ptr [esp], xmm1
00458144 f30f104d10               movss      xmm1, dword ptr [ebp + 0x10]
00458149 f30f5e0dd0c85600         divss      xmm1, dword ptr [0x56c8d0]
00458151 51                       push       ecx
00458152 f30f110c24               movss      dword ptr [esp], xmm1
00458157 f30f11459c               movss      dword ptr [ebp - 0x64], xmm0
0045815c e84fdeffff               call       0x455fb0
00458161 83c408                   add        esp, 8
00458164 d95da8                   fstp       dword ptr [ebp - 0x58]
00458167 f30f1045a8               movss      xmm0, dword ptr [ebp - 0x58]
0045816c f30f104d28               movss      xmm1, dword ptr [ebp + 0x28]
00458171 f30f5e0dd0c85600         divss      xmm1, dword ptr [0x56c8d0]
00458179 51                       push       ecx
0045817a f30f110c24               movss      dword ptr [esp], xmm1
0045817f f30f104d24               movss      xmm1, dword ptr [ebp + 0x24]
00458184 f30f5e0dd0c85600         divss      xmm1, dword ptr [0x56c8d0]
0045818c 51                       push       ecx
0045818d f30f110c24               movss      dword ptr [esp], xmm1
00458192 f30f1145a4               movss      dword ptr [ebp - 0x5c], xmm0
00458197 e814deffff               call       0x455fb0
0045819c 83c408                   add        esp, 8
0045819f d95da0                   fstp       dword ptr [ebp - 0x60]
004581a2 f30f1045a4               movss      xmm0, dword ptr [ebp - 0x5c]
004581a7 f30f5845a0               addss      xmm0, dword ptr [ebp - 0x60]
004581ac f30f104d9c               movss      xmm1, dword ptr [ebp - 0x64]
004581b1 0f2fc8                   comiss     xmm1, xmm0
004581b4 7207                     jb         0x4581bd
004581b6 32c0                     xor        al, al
004581b8 e9a1040000               jmp        0x45865e
004581bd f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004581c2 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
004581c9 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004581d1 ba08000000               mov        edx, 8
004581d6 6bc200                   imul       eax, edx, 0
004581d9 f30f114405bc             movss      dword ptr [ebp + eax - 0x44], xmm0
004581df f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004581e4 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
004581eb f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004581f3 b908000000               mov        ecx, 8
004581f8 6bd100                   imul       edx, ecx, 0
004581fb f30f114415c0             movss      dword ptr [ebp + edx - 0x40], xmm0
00458201 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00458206 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
0045820d f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00458215 b808000000               mov        eax, 8
0045821a c1e000                   shl        eax, 0
0045821d f30f114405bc             movss      dword ptr [ebp + eax - 0x44], xmm0
00458223 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00458228 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00458230 b908000000               mov        ecx, 8
00458235 c1e100                   shl        ecx, 0
00458238 f30f11440dc0             movss      dword ptr [ebp + ecx - 0x40], xmm0
0045823e f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00458243 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0045824b ba08000000               mov        edx, 8
00458250 d1e2                     shl        edx, 1
00458252 f30f114415bc             movss      dword ptr [ebp + edx - 0x44], xmm0
00458258 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
0045825d f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00458265 b808000000               mov        eax, 8
0045826a d1e0                     shl        eax, 1
0045826c f30f114405c0             movss      dword ptr [ebp + eax - 0x40], xmm0
00458272 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00458277 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0045827f b908000000               mov        ecx, 8
00458284 6bd103                   imul       edx, ecx, 3
00458287 f30f114415bc             movss      dword ptr [ebp + edx - 0x44], xmm0
0045828d f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00458292 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00458299 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004582a1 b808000000               mov        eax, 8
004582a6 6bc803                   imul       ecx, eax, 3
004582a9 f30f11440dc0             movss      dword ptr [ebp + ecx - 0x40], xmm0
004582af f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
004582b4 0f2e05e8e05600           ucomiss    xmm0, dword ptr [0x56e0e8]
004582bb 9f                       lahf       
004582bc f6c444                   test       ah, 0x44
004582bf 7b1d                     jnp        0x4582de
004582c1 6a04                     push       4
004582c3 51                       push       ecx
004582c4 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
004582c9 f30f110424               movss      dword ptr [esp], xmm0
004582ce 8d55bc                   lea        edx, [ebp - 0x44]
004582d1 52                       push       edx
004582d2 8d45bc                   lea        eax, [ebp - 0x44]
004582d5 50                       push       eax
004582d6 e8550d0000               call       0x459030
004582db 83c410                   add        esp, 0x10
004582de f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
004582e3 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
004582ea f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004582f2 b908000000               mov        ecx, 8
004582f7 6bd100                   imul       edx, ecx, 0
004582fa f30f114415dc             movss      dword ptr [ebp + edx - 0x24], xmm0
00458300 f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
00458305 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
0045830c f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00458314 b808000000               mov        eax, 8
00458319 6bc800                   imul       ecx, eax, 0
0045831c f30f11440de0             movss      dword ptr [ebp + ecx - 0x20], xmm0
00458322 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
00458327 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
0045832e f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00458336 ba08000000               mov        edx, 8
0045833b c1e200                   shl        edx, 0
0045833e f30f114415dc             movss      dword ptr [ebp + edx - 0x24], xmm0
00458344 f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
00458349 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00458351 b808000000               mov        eax, 8
00458356 c1e000                   shl        eax, 0
00458359 f30f114405e0             movss      dword ptr [ebp + eax - 0x20], xmm0
0045835f f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
00458364 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0045836c b908000000               mov        ecx, 8
00458371 d1e1                     shl        ecx, 1
00458373 f30f11440ddc             movss      dword ptr [ebp + ecx - 0x24], xmm0
00458379 f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
0045837e f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00458386 ba08000000               mov        edx, 8
0045838b d1e2                     shl        edx, 1
0045838d f30f114415e0             movss      dword ptr [ebp + edx - 0x20], xmm0
00458393 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
00458398 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004583a0 b808000000               mov        eax, 8
004583a5 6bc803                   imul       ecx, eax, 3
004583a8 f30f11440ddc             movss      dword ptr [ebp + ecx - 0x24], xmm0
004583ae f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
004583b3 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
004583ba f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004583c2 ba08000000               mov        edx, 8
004583c7 6bc203                   imul       eax, edx, 3
004583ca f30f114405e0             movss      dword ptr [ebp + eax - 0x20], xmm0
004583d0 f30f10452c               movss      xmm0, dword ptr [ebp + 0x2c]
004583d5 0f2e05e8e05600           ucomiss    xmm0, dword ptr [0x56e0e8]
004583dc 9f                       lahf       
004583dd f6c444                   test       ah, 0x44
004583e0 7b1d                     jnp        0x4583ff
004583e2 6a04                     push       4
004583e4 51                       push       ecx
004583e5 f30f10452c               movss      xmm0, dword ptr [ebp + 0x2c]
004583ea f30f110424               movss      dword ptr [esp], xmm0
004583ef 8d4ddc                   lea        ecx, [ebp - 0x24]
004583f2 51                       push       ecx
004583f3 8d55dc                   lea        edx, [ebp - 0x24]
004583f6 52                       push       edx
004583f7 e8340c0000               call       0x459030
004583fc 83c410                   add        esp, 0x10
004583ff c745b800000000           mov        dword ptr [ebp - 0x48], 0
00458406 eb09                     jmp        0x458411
00458408 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
0045840b 83c001                   add        eax, 1
0045840e 8945b8                   mov        dword ptr [ebp - 0x48], eax
00458411 837db804                 cmp        dword ptr [ebp - 0x48], 4
00458415 7d5e                     jge        0x458475
00458417 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
0045841a f30f1044cdbc             movss      xmm0, dword ptr [ebp + ecx*8 - 0x44]
00458420 f30f584508               addss      xmm0, dword ptr [ebp + 8]
00458425 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
00458428 f30f1144d5bc             movss      dword ptr [ebp + edx*8 - 0x44], xmm0
0045842e 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
00458431 f30f1044c5c0             movss      xmm0, dword ptr [ebp + eax*8 - 0x40]
00458437 f30f58450c               addss      xmm0, dword ptr [ebp + 0xc]
0045843c 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
0045843f f30f1144cdc0             movss      dword ptr [ebp + ecx*8 - 0x40], xmm0
00458445 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
00458448 f30f1044d5dc             movss      xmm0, dword ptr [ebp + edx*8 - 0x24]
0045844e f30f58451c               addss      xmm0, dword ptr [ebp + 0x1c]
00458453 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
00458456 f30f1144c5dc             movss      dword ptr [ebp + eax*8 - 0x24], xmm0
0045845c 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
0045845f f30f1044cde0             movss      xmm0, dword ptr [ebp + ecx*8 - 0x20]
00458465 f30f584520               addss      xmm0, dword ptr [ebp + 0x20]
0045846a 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
0045846d f30f1144d5e0             movss      dword ptr [ebp + edx*8 - 0x20], xmm0
00458473 eb93                     jmp        0x458408
00458475 8d45dc                   lea        eax, [ebp - 0x24]
00458478 50                       push       eax
00458479 51                       push       ecx
0045847a f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
0045847f f30f110424               movss      dword ptr [esp], xmm0
00458484 51                       push       ecx
00458485 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
0045848a f30f110424               movss      dword ptr [esp], xmm0
0045848f 51                       push       ecx
00458490 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00458495 f30f110424               movss      dword ptr [esp], xmm0
0045849a 51                       push       ecx
0045849b f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
004584a0 f30f110424               movss      dword ptr [esp], xmm0
004584a5 51                       push       ecx
004584a6 f30f104508               movss      xmm0, dword ptr [ebp + 8]
004584ab f30f110424               movss      dword ptr [esp], xmm0
004584b0 e8ab060000               call       0x458b60
004584b5 83c418                   add        esp, 0x18
004584b8 0fb6c8                   movzx      ecx, al
004584bb 85c9                     test       ecx, ecx
004584bd 7407                     je         0x4584c6
004584bf b001                     mov        al, 1
004584c1 e998010000               jmp        0x45865e
004584c6 8d55bc                   lea        edx, [ebp - 0x44]
004584c9 52                       push       edx
004584ca 51                       push       ecx
004584cb f30f10452c               movss      xmm0, dword ptr [ebp + 0x2c]
004584d0 f30f110424               movss      dword ptr [esp], xmm0
004584d5 51                       push       ecx
004584d6 f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
004584db f30f110424               movss      dword ptr [esp], xmm0
004584e0 51                       push       ecx
004584e1 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
004584e6 f30f110424               movss      dword ptr [esp], xmm0
004584eb 51                       push       ecx
004584ec f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
004584f1 f30f110424               movss      dword ptr [esp], xmm0
004584f6 51                       push       ecx
004584f7 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
004584fc f30f110424               movss      dword ptr [esp], xmm0
00458501 e85a060000               call       0x458b60
00458506 83c418                   add        esp, 0x18
00458509 0fb6c0                   movzx      eax, al
0045850c 85c0                     test       eax, eax
0045850e 7407                     je         0x458517
00458510 b001                     mov        al, 1
00458512 e947010000               jmp        0x45865e
00458517 c745b000000000           mov        dword ptr [ebp - 0x50], 0
0045851e eb09                     jmp        0x458529
00458520 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00458523 83c101                   add        ecx, 1
00458526 894db0                   mov        dword ptr [ebp - 0x50], ecx
00458529 837db004                 cmp        dword ptr [ebp - 0x50], 4
0045852d 0f8d29010000             jge        0x45865c
00458533 c745b400000000           mov        dword ptr [ebp - 0x4c], 0
0045853a eb09                     jmp        0x458545
0045853c 8b55b4                   mov        edx, dword ptr [ebp - 0x4c]
0045853f 83c201                   add        edx, 1
00458542 8955b4                   mov        dword ptr [ebp - 0x4c], edx
00458545 837db404                 cmp        dword ptr [ebp - 0x4c], 4
00458549 0f8d08010000             jge        0x458657
0045854f b804000000               mov        eax, 4
00458554 c1e000                   shl        eax, 0
00458557 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
0045855a 8b94c888e65600           mov        edx, dword ptr [eax + ecx*8 + 0x56e688]
00458561 51                       push       ecx
00458562 f30f1044d5e0             movss      xmm0, dword ptr [ebp + edx*8 - 0x20]
00458568 f30f110424               movss      dword ptr [esp], xmm0
0045856d b804000000               mov        eax, 4
00458572 c1e000                   shl        eax, 0
00458575 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00458578 8b94c888e65600           mov        edx, dword ptr [eax + ecx*8 + 0x56e688]
0045857f 51                       push       ecx
00458580 f30f1044d5dc             movss      xmm0, dword ptr [ebp + edx*8 - 0x24]
00458586 f30f110424               movss      dword ptr [esp], xmm0
0045858b b804000000               mov        eax, 4
00458590 6bc800                   imul       ecx, eax, 0
00458593 8b55b4                   mov        edx, dword ptr [ebp - 0x4c]
00458596 8b84d188e65600           mov        eax, dword ptr [ecx + edx*8 + 0x56e688]
0045859d 51                       push       ecx
0045859e f30f1044c5e0             movss      xmm0, dword ptr [ebp + eax*8 - 0x20]
004585a4 f30f110424               movss      dword ptr [esp], xmm0
004585a9 b904000000               mov        ecx, 4
004585ae 6bd100                   imul       edx, ecx, 0
004585b1 8b45b4                   mov        eax, dword ptr [ebp - 0x4c]
004585b4 8b8cc288e65600           mov        ecx, dword ptr [edx + eax*8 + 0x56e688]
004585bb 51                       push       ecx
004585bc f30f1044cddc             movss      xmm0, dword ptr [ebp + ecx*8 - 0x24]
004585c2 f30f110424               movss      dword ptr [esp], xmm0
004585c7 ba04000000               mov        edx, 4
004585cc c1e200                   shl        edx, 0
004585cf 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
004585d2 8b8cc288e65600           mov        ecx, dword ptr [edx + eax*8 + 0x56e688]
004585d9 51                       push       ecx
004585da f30f1044cdc0             movss      xmm0, dword ptr [ebp + ecx*8 - 0x40]
004585e0 f30f110424               movss      dword ptr [esp], xmm0
004585e5 ba04000000               mov        edx, 4
004585ea c1e200                   shl        edx, 0
004585ed 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
004585f0 8b8cc288e65600           mov        ecx, dword ptr [edx + eax*8 + 0x56e688]
004585f7 51                       push       ecx
004585f8 f30f1044cdbc             movss      xmm0, dword ptr [ebp + ecx*8 - 0x44]
004585fe f30f110424               movss      dword ptr [esp], xmm0
00458603 ba04000000               mov        edx, 4
00458608 6bc200                   imul       eax, edx, 0
0045860b 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
0045860e 8b94c888e65600           mov        edx, dword ptr [eax + ecx*8 + 0x56e688]
00458615 51                       push       ecx
00458616 f30f1044d5c0             movss      xmm0, dword ptr [ebp + edx*8 - 0x40]
0045861c f30f110424               movss      dword ptr [esp], xmm0
00458621 b804000000               mov        eax, 4
00458626 6bc800                   imul       ecx, eax, 0
00458629 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
0045862c 8b84d188e65600           mov        eax, dword ptr [ecx + edx*8 + 0x56e688]
00458633 51                       push       ecx
00458634 f30f1044c5bc             movss      xmm0, dword ptr [ebp + eax*8 - 0x44]
0045863a f30f110424               movss      dword ptr [esp], xmm0
0045863f e8dce2ffff               call       0x456920
00458644 83c420                   add        esp, 0x20
00458647 0fb6c8                   movzx      ecx, al
0045864a 85c9                     test       ecx, ecx
0045864c 7404                     je         0x458652
0045864e b001                     mov        al, 1
00458650 eb0c                     jmp        0x45865e
00458652 e9e5feffff               jmp        0x45853c
00458657 e9c4feffff               jmp        0x458520
0045865c 32c0                     xor        al, al
0045865e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00458661 33cd                     xor        ecx, ebp
00458663 e886a30e00               call       0x5429ee
00458668 8be5                     mov        esp, ebp
0045866a 5d                       pop        ebp
0045866b c3                       ret        
0045866c cc                       int3       
0045866d cc                       int3       
0045866e cc                       int3       
0045866f cc                       int3       
