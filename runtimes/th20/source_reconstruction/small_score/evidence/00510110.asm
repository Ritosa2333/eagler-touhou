00510110 55                       push       ebp
00510111 8bec                     mov        ebp, esp
00510113 83ec74                   sub        esp, 0x74
00510116 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0051011b 33c5                     xor        eax, ebp
0051011d 8945fc                   mov        dword ptr [ebp - 4], eax
00510120 894de8                   mov        dword ptr [ebp - 0x18], ecx
00510123 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00510126 8b88900f0000             mov        ecx, dword ptr [eax + 0xf90]
0051012c 51                       push       ecx
0051012d 6a15                     push       0x15
0051012f 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00510135 e896f2f3ff               call       0x44f3d0
0051013a 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
0051013d 81c200060000             add        edx, 0x600
00510143 8955ec                   mov        dword ptr [ebp - 0x14], edx
00510146 8d4df0                   lea        ecx, [ebp - 0x10]
00510149 e8c22cf1ff               call       0x422e10
0051014e 90                       nop        
0051014f b9404d5c00               mov        ecx, 0x5c4d40
00510154 e8e7bffcff               call       0x4dc140
00510159 85c0                     test       eax, eax
0051015b 750b                     jne        0x510168
0051015d b9404d5c00               mov        ecx, 0x5c4d40
00510162 e8f9d8fcff               call       0x4dda60
00510167 90                       nop        
00510168 c745e000000000           mov        dword ptr [ebp - 0x20], 0
0051016f eb12                     jmp        0x510183
00510171 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00510174 83c001                   add        eax, 1
00510177 8945e0                   mov        dword ptr [ebp - 0x20], eax
0051017a 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0051017d 83c144                   add        ecx, 0x44
00510180 894dec                   mov        dword ptr [ebp - 0x14], ecx
00510183 837de00d                 cmp        dword ptr [ebp - 0x20], 0xd
00510187 0f8d37030000             jge        0x5104c4
0051018d 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00510190 0fb64238                 movzx      eax, byte ptr [edx + 0x38]
00510194 85c0                     test       eax, eax
00510196 7502                     jne        0x51019a
00510198 ebd7                     jmp        0x510171
0051019a 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0051019d 83c120                   add        ecx, 0x20
005101a0 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
005101a3 6a08                     push       8
005101a5 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
005101a8 e8b333f1ff               call       0x423560
005101ad 0fb6d0                   movzx      edx, al
005101b0 85d2                     test       edx, edx
005101b2 7422                     je         0x5101d6
005101b4 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
005101b7 83c120                   add        ecx, 0x20
005101ba e8113af1ff               call       0x423bd0
005101bf d95dc0                   fstp       dword ptr [ebp - 0x40]
005101c2 f30f1005bcd75600         movss      xmm0, dword ptr [0x56d7bc]
005101ca f30f5e45c0               divss      xmm0, dword ptr [ebp - 0x40]
005101cf f30f1145d4               movss      dword ptr [ebp - 0x2c], xmm0
005101d4 eb0d                     jmp        0x5101e3
005101d6 f30f1005bcd75600         movss      xmm0, dword ptr [0x56d7bc]
005101de f30f1145d4               movss      dword ptr [ebp - 0x2c], xmm0
005101e3 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
005101e6 83c01c                   add        eax, 0x1c
005101e9 8945bc                   mov        dword ptr [ebp - 0x44], eax
005101ec 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
005101ef 0fb65139                 movzx      edx, byte ptr [ecx + 0x39]
005101f3 f30f2ac2                 cvtsi2ss   xmm0, edx
005101f7 f30f5945d4               mulss      xmm0, dword ptr [ebp - 0x2c]
005101fc f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00510204 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00510207 f30f10480c               movss      xmm1, dword ptr [eax + 0xc]
0051020c f30f5cc8                 subss      xmm1, xmm0
00510210 51                       push       ecx
00510211 f30f110c24               movss      dword ptr [esp], xmm1
00510216 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
00510219 e832060000               call       0x510850
0051021e 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00510221 83c11c                   add        ecx, 0x1c
00510224 894db8                   mov        dword ptr [ebp - 0x48], ecx
00510227 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
0051022a 51                       push       ecx
0051022b f30f104210               movss      xmm0, dword ptr [edx + 0x10]
00510230 f30f110424               movss      dword ptr [esp], xmm0
00510235 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00510238 e833060000               call       0x510870
0051023d 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00510240 83c01c                   add        eax, 0x1c
00510243 8945b4                   mov        dword ptr [ebp - 0x4c], eax
00510246 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00510249 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
0051024c 52                       push       edx
0051024d 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00510250 e8bbf7f3ff               call       0x44fa10
00510255 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00510258 8b88940f0000             mov        ecx, dword ptr [eax + 0xf94]
0051025e e82dfdefff               call       0x40ff90
00510263 8bc8                     mov        ecx, eax
00510265 e8e605f5ff               call       0x460850
0051026a 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0051026d f30f1000                 movss      xmm0, dword ptr [eax]
00510271 f30f5c410c               subss      xmm0, dword ptr [ecx + 0xc]
00510276 f30f1145d0               movss      dword ptr [ebp - 0x30], xmm0
0051027b 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
0051027e 8b8a940f0000             mov        ecx, dword ptr [edx + 0xf94]
00510284 e807fdefff               call       0x40ff90
00510289 8bc8                     mov        ecx, eax
0051028b e8c005f5ff               call       0x460850
00510290 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00510293 f30f104004               movss      xmm0, dword ptr [eax + 4]
00510298 f30f5c4110               subss      xmm0, dword ptr [ecx + 0x10]
0051029d f30f1145cc               movss      dword ptr [ebp - 0x34], xmm0
005102a2 f30f1045d0               movss      xmm0, dword ptr [ebp - 0x30]
005102a7 f30f5945d0               mulss      xmm0, dword ptr [ebp - 0x30]
005102ac f30f104dcc               movss      xmm1, dword ptr [ebp - 0x34]
005102b1 f30f594dcc               mulss      xmm1, dword ptr [ebp - 0x34]
005102b6 f30f58c1                 addss      xmm0, xmm1
005102ba f30f2cd0                 cvttss2si  edx, xmm0
005102be 8955e4                   mov        dword ptr [ebp - 0x1c], edx
005102c1 817de400400000           cmp        dword ptr [ebp - 0x1c], 0x4000
005102c8 7e09                     jle        0x5102d3
005102ca c745e4ff000000           mov        dword ptr [ebp - 0x1c], 0xff
005102d1 eb2d                     jmp        0x510300
005102d3 817de400100000           cmp        dword ptr [ebp - 0x1c], 0x1000
005102da 7e1d                     jle        0x5102f9
005102dc 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
005102df 2d00100000               sub        eax, 0x1000
005102e4 c1e007                   shl        eax, 7
005102e7 99                       cdq        
005102e8 b900300000               mov        ecx, 0x3000
005102ed f7f9                     idiv       ecx
005102ef 0580000000               add        eax, 0x80
005102f4 8945e4                   mov        dword ptr [ebp - 0x1c], eax
005102f7 eb07                     jmp        0x510300
005102f9 c745e480000000           mov        dword ptr [ebp - 0x1c], 0x80
00510300 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00510303 0fb64239                 movzx      eax, byte ptr [edx + 0x39]
00510307 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0051030a 8d5401ff                 lea        edx, [ecx + eax - 1]
0051030e 8955d8                   mov        dword ptr [ebp - 0x28], edx
00510311 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00510314 0fb64839                 movzx      ecx, byte ptr [eax + 0x39]
00510318 894ddc                   mov        dword ptr [ebp - 0x24], ecx
0051031b eb09                     jmp        0x510326
0051031d 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
00510320 83ea01                   sub        edx, 1
00510323 8955dc                   mov        dword ptr [ebp - 0x24], edx
00510326 837ddc00                 cmp        dword ptr [ebp - 0x24], 0
0051032a 0f8e8f010000             jle        0x5104bf
00510330 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00510333 83c020                   add        eax, 0x20
00510336 8945b0                   mov        dword ptr [ebp - 0x50], eax
00510339 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0051033c d1e1                     shl        ecx, 1
0051033e ba34000000               mov        edx, 0x34
00510343 2bd1                     sub        edx, ecx
00510345 52                       push       edx
00510346 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00510349 e81232f1ff               call       0x423560
0051034e 0fb6c0                   movzx      eax, al
00510351 85c0                     test       eax, eax
00510353 750b                     jne        0x510360
00510355 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00510358 0fb611                   movzx      edx, byte ptr [ecx]
0051035b 83fa0a                   cmp        edx, 0xa
0051035e 753a                     jne        0x51039a
00510360 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00510363 83c01c                   add        eax, 0x1c
00510366 8945ac                   mov        dword ptr [ebp - 0x54], eax
00510369 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0051036c 0fb611                   movzx      edx, byte ptr [ecx]
0051036f 81c221010000             add        edx, 0x121
00510375 52                       push       edx
00510376 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
00510379 e8520bf6ff               call       0x470ed0
0051037e 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00510381 83c01c                   add        eax, 0x1c
00510384 8945a8                   mov        dword ptr [ebp - 0x58], eax
00510387 0fb64de4                 movzx      ecx, byte ptr [ebp - 0x1c]
0051038b 51                       push       ecx
0051038c 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0051038f e8cc07f6ff               call       0x470b60
00510394 90                       nop        
00510395 e9bc000000               jmp        0x510456
0051039a 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
0051039d 83c220                   add        edx, 0x20
005103a0 8955a4                   mov        dword ptr [ebp - 0x5c], edx
005103a3 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
005103a6 d1e0                     shl        eax, 1
005103a8 b938000000               mov        ecx, 0x38
005103ad 2bc8                     sub        ecx, eax
005103af 51                       push       ecx
005103b0 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
005103b3 e8a831f1ff               call       0x423560
005103b8 0fb6d0                   movzx      edx, al
005103bb 85d2                     test       edx, edx
005103bd 7437                     je         0x5103f6
005103bf 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
005103c2 83c01c                   add        eax, 0x1c
005103c5 8945a0                   mov        dword ptr [ebp - 0x60], eax
005103c8 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
005103cb 0fb611                   movzx      edx, byte ptr [ecx]
005103ce 81c22c010000             add        edx, 0x12c
005103d4 52                       push       edx
005103d5 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
005103d8 e8f30af6ff               call       0x470ed0
005103dd 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
005103e0 83c01c                   add        eax, 0x1c
005103e3 89459c                   mov        dword ptr [ebp - 0x64], eax
005103e6 0fb64de4                 movzx      ecx, byte ptr [ebp - 0x1c]
005103ea 51                       push       ecx
005103eb 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
005103ee e86d07f6ff               call       0x470b60
005103f3 90                       nop        
005103f4 eb60                     jmp        0x510456
005103f6 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
005103f9 83c220                   add        edx, 0x20
005103fc 895598                   mov        dword ptr [ebp - 0x68], edx
005103ff 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
00510402 d1e0                     shl        eax, 1
00510404 b93c000000               mov        ecx, 0x3c
00510409 2bc8                     sub        ecx, eax
0051040b 51                       push       ecx
0051040c 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
0051040f e84c31f1ff               call       0x423560
00510414 0fb6d0                   movzx      edx, al
00510417 85d2                     test       edx, edx
00510419 7437                     je         0x510452
0051041b 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0051041e 83c01c                   add        eax, 0x1c
00510421 894594                   mov        dword ptr [ebp - 0x6c], eax
00510424 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00510427 0fb611                   movzx      edx, byte ptr [ecx]
0051042a 81c236010000             add        edx, 0x136
00510430 52                       push       edx
00510431 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00510434 e8970af6ff               call       0x470ed0
00510439 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0051043c 83c01c                   add        eax, 0x1c
0051043f 894590                   mov        dword ptr [ebp - 0x70], eax
00510442 0fb64de4                 movzx      ecx, byte ptr [ebp - 0x1c]
00510446 51                       push       ecx
00510447 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
0051044a e81107f6ff               call       0x470b60
0051044f 90                       nop        
00510450 eb04                     jmp        0x510456
00510452 eb3c                     jmp        0x510490
00510454 eb3a                     jmp        0x510490
00510456 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
00510459 83c21c                   add        edx, 0x1c
0051045c 89558c                   mov        dword ptr [ebp - 0x74], edx
0051045f 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00510462 83c11c                   add        ecx, 0x1c
00510465 e86678f2ff               call       0x437cd0
0051046a 51                       push       ecx
0051046b f30f10404c               movss      xmm0, dword ptr [eax + 0x4c]
00510470 f30f110424               movss      dword ptr [esp], xmm0
00510475 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00510478 e86369ffff               call       0x506de0
0051047d 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00510480 83c01c                   add        eax, 0x1c
00510483 50                       push       eax
00510484 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0051048a e8c1f0f2ff               call       0x43f550
0051048f 90                       nop        
00510490 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00510493 83c11c                   add        ecx, 0x1c
00510496 e885c6f3ff               call       0x44cb20
0051049b 8945c8                   mov        dword ptr [ebp - 0x38], eax
0051049e 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
005104a1 f30f1001                 movss      xmm0, dword ptr [ecx]
005104a5 f30f5845d4               addss      xmm0, dword ptr [ebp - 0x2c]
005104aa 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
005104ad f30f1102                 movss      dword ptr [edx], xmm0
005104b1 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
005104b4 83e801                   sub        eax, 1
005104b7 8945d8                   mov        dword ptr [ebp - 0x28], eax
005104ba e95efeffff               jmp        0x51031d
005104bf e9adfcffff               jmp        0x510171
005104c4 c745e000000000           mov        dword ptr [ebp - 0x20], 0
005104cb eb12                     jmp        0x5104df
005104cd 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005104d0 83c101                   add        ecx, 1
005104d3 894de0                   mov        dword ptr [ebp - 0x20], ecx
005104d6 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
005104d9 83c244                   add        edx, 0x44
005104dc 8955ec                   mov        dword ptr [ebp - 0x14], edx
005104df 837de005                 cmp        dword ptr [ebp - 0x20], 5
005104e3 0f8d42010000             jge        0x51062b
005104e9 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
005104ec 0fb64838                 movzx      ecx, byte ptr [eax + 0x38]
005104f0 85c9                     test       ecx, ecx
005104f2 7502                     jne        0x5104f6
005104f4 ebd7                     jmp        0x5104cd
005104f6 6a02                     push       2
005104f8 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005104fe e8fd85f7ff               call       0x488b00
00510503 6a02                     push       2
00510505 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0051050b e8f084f7ff               call       0x488a00
00510510 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00510513 8b421c                   mov        eax, dword ptr [edx + 0x1c]
00510516 50                       push       eax
00510517 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0051051d e8ae05f6ff               call       0x470ad0
00510522 6a00                     push       0
00510524 6a00                     push       0
00510526 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0051052c e8ef80faff               call       0x4b8620
00510531 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00510534 83c10c                   add        ecx, 0xc
00510537 8b11                     mov        edx, dword ptr [ecx]
00510539 8955f0                   mov        dword ptr [ebp - 0x10], edx
0051053c 8b4104                   mov        eax, dword ptr [ecx + 4]
0051053f 8945f4                   mov        dword ptr [ebp - 0xc], eax
00510542 8b4908                   mov        ecx, dword ptr [ecx + 8]
00510545 894df8                   mov        dword ptr [ebp - 8], ecx
00510548 f30f100550e05600         movss      xmm0, dword ptr [0x56e050]
00510550 f30f580594cd5600         addss      xmm0, dword ptr [0x56cd94]
00510558 f30f5845f0               addss      xmm0, dword ptr [ebp - 0x10]
0051055d f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00510562 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00510567 f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
0051056f f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00510574 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00510577 837a3c00                 cmp        dword ptr [edx + 0x3c], 0
0051057b 7c5a                     jl         0x5105d7
0051057d 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00510580 f30f5a4040               cvtss2sd   xmm0, dword ptr [eax + 0x40]
00510585 83ec08                   sub        esp, 8
00510588 f20f110424               movsd      qword ptr [esp], xmm0
0051058d 68e0395700               push       0x5739e0
00510592 8d4df0                   lea        ecx, [ebp - 0x10]
00510595 51                       push       ecx
00510596 8b1598065c00             mov        edx, dword ptr [0x5c0698]
0051059c 52                       push       edx
0051059d e8eec3f5ff               call       0x46c990
005105a2 83c414                   add        esp, 0x14
005105a5 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
005105aa f30f5805f8395700         addss      xmm0, dword ptr [0x5739f8]
005105b2 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
005105b7 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
005105ba 8b483c                   mov        ecx, dword ptr [eax + 0x3c]
005105bd 51                       push       ecx
005105be 687cf65600               push       0x56f67c
005105c3 8d55f0                   lea        edx, [ebp - 0x10]
005105c6 52                       push       edx
005105c7 a198065c00               mov        eax, dword ptr [0x5c0698]
005105cc 50                       push       eax
005105cd e8bec3f5ff               call       0x46c990
005105d2 83c410                   add        esp, 0x10
005105d5 eb18                     jmp        0x5105ef
005105d7 68ec395700               push       0x5739ec
005105dc 8d4df0                   lea        ecx, [ebp - 0x10]
005105df 51                       push       ecx
005105e0 8b1598065c00             mov        edx, dword ptr [0x5c0698]
005105e6 52                       push       edx
005105e7 e8a4c3f5ff               call       0x46c990
005105ec 83c40c                   add        esp, 0xc
005105ef 6aff                     push       -1
005105f1 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005105f7 e8d404f6ff               call       0x470ad0
005105fc 6a00                     push       0
005105fe 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00510604 e8f783f7ff               call       0x488a00
00510609 6a00                     push       0
0051060b 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00510611 e8ea84f7ff               call       0x488b00
00510616 6a01                     push       1
00510618 6a01                     push       1
0051061a 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00510620 e8fb7ffaff               call       0x4b8620
00510625 90                       nop        
00510626 e9a2feffff               jmp        0x5104cd
0051062b b801000000               mov        eax, 1
00510630 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00510633 33cd                     xor        ecx, ebp
00510635 e8b4230300               call       0x5429ee
0051063a 8be5                     mov        esp, ebp
0051063c 5d                       pop        ebp
0051063d c3                       ret        
0051063e cc                       int3       
0051063f cc                       int3       
