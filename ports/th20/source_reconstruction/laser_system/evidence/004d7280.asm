004d7280 55                       push       ebp
004d7281 8bec                     mov        ebp, esp
004d7283 83ec74                   sub        esp, 0x74
004d7286 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004d728b 33c5                     xor        eax, ebp
004d728d 8945fc                   mov        dword ptr [ebp - 4], eax
004d7290 894dec                   mov        dword ptr [ebp - 0x14], ecx
004d7293 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d7296 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004d7299 c1e905                   shr        ecx, 5
004d729c 83e101                   and        ecx, 1
004d729f 7407                     je         0x4d72a8
004d72a1 33c0                     xor        eax, eax
004d72a3 e981040000               jmp        0x4d7729
004d72a8 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d72ab 83bad806000000           cmp        dword ptr [edx + 0x6d8], 0
004d72b2 7507                     jne        0x4d72bb
004d72b4 33c0                     xor        eax, eax
004d72b6 e96e040000               jmp        0x4d7729
004d72bb 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d72be 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004d72c1 83c920                   or         ecx, 0x20
004d72c4 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d72c7 894a1c                   mov        dword ptr [edx + 0x1c], ecx
004d72ca 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d72cd 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004d72d0 83e1bf                   and        ecx, 0xffffffbf
004d72d3 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d72d6 894a1c                   mov        dword ptr [edx + 0x1c], ecx
004d72d9 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d72dc 8b10                     mov        edx, dword ptr [eax]
004d72de 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d72e1 8b4208                   mov        eax, dword ptr [edx + 8]
004d72e4 ffd0                     call       eax
004d72e6 90                       nop        
004d72e7 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d72ea 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004d72ed 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004d72f0 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004d72f3 8b8a98060000             mov        ecx, dword ptr [edx + 0x698]
004d72f9 0b889c060000             or         ecx, dword ptr [eax + 0x69c]
004d72ff 0f8499000000             je         0x4d739e
004d7305 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d7308 8b8298060000             mov        eax, dword ptr [edx + 0x698]
004d730e c1e81e                   shr        eax, 0x1e
004d7311 83e001                   and        eax, 1
004d7314 7467                     je         0x4d737d
004d7316 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d7319 81c190000000             add        ecx, 0x90
004d731f 894de0                   mov        dword ptr [ebp - 0x20], ecx
004d7322 6a05                     push       5
004d7324 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004d7327 e85453faff               call       0x47c680
004d732c 8945dc                   mov        dword ptr [ebp - 0x24], eax
004d732f 6a00                     push       0
004d7331 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004d7334 e857c2f4ff               call       0x423590
004d7339 0fb6d0                   movzx      edx, al
004d733c 85d2                     test       edx, edx
004d733e 741a                     je         0x4d735a
004d7340 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d7343 8b8898060000             mov        ecx, dword ptr [eax + 0x698]
004d7349 81e1ffffffbf             and        ecx, 0xbfffffff
004d734f 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d7352 898a98060000             mov        dword ptr [edx + 0x698], ecx
004d7358 eb23                     jmp        0x4d737d
004d735a 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d735d 0590000000               add        eax, 0x90
004d7362 8945d8                   mov        dword ptr [ebp - 0x28], eax
004d7365 6a05                     push       5
004d7367 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004d736a e81153faff               call       0x47c680
004d736f 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004d7372 6a00                     push       0
004d7374 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004d7377 e8a420f5ff               call       0x429420
004d737c 90                       nop        
004d737d 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d7380 83b9cc06000000           cmp        dword ptr [ecx + 0x6cc], 0
004d7387 7415                     je         0x4d739e
004d7389 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d738c 8b82cc060000             mov        eax, dword ptr [edx + 0x6cc]
004d7392 83e801                   sub        eax, 1
004d7395 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d7398 8981cc060000             mov        dword ptr [ecx + 0x6cc], eax
004d739e 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d73a1 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d73a4 f30f108218070000         movss      xmm0, dword ptr [edx + 0x718]
004d73ac 0f2f4074                 comiss     xmm0, dword ptr [eax + 0x74]
004d73b0 7661                     jbe        0x4d7413
004d73b2 b9e4ef5a00               mov        ecx, 0x5aefe4
004d73b7 e8241ff5ff               call       0x4292e0
004d73bc d95dd0                   fstp       dword ptr [ebp - 0x30]
004d73bf 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d73c2 f30f1045d0               movss      xmm0, dword ptr [ebp - 0x30]
004d73c7 f30f59417c               mulss      xmm0, dword ptr [ecx + 0x7c]
004d73cc f30f1145c8               movss      dword ptr [ebp - 0x38], xmm0
004d73d1 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d73d4 f30f104274               movss      xmm0, dword ptr [edx + 0x74]
004d73d9 f30f1145cc               movss      dword ptr [ebp - 0x34], xmm0
004d73de f30f1045cc               movss      xmm0, dword ptr [ebp - 0x34]
004d73e3 f30f5845c8               addss      xmm0, dword ptr [ebp - 0x38]
004d73e8 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d73eb f30f114074               movss      dword ptr [eax + 0x74], xmm0
004d73f0 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d73f3 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d73f6 f30f104174               movss      xmm0, dword ptr [ecx + 0x74]
004d73fb 0f2f8218070000           comiss     xmm0, dword ptr [edx + 0x718]
004d7402 760f                     jbe        0x4d7413
004d7404 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d7407 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d740a 8b9118070000             mov        edx, dword ptr [ecx + 0x718]
004d7410 895074                   mov        dword ptr [eax + 0x74], edx
004d7413 b9e4ef5a00               mov        ecx, 0x5aefe4
004d7418 e8c31ef5ff               call       0x4292e0
004d741d d95dc4                   fstp       dword ptr [ebp - 0x3c]
004d7420 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d7423 f30f1045c4               movss      xmm0, dword ptr [ebp - 0x3c]
004d7428 f30f598014070000         mulss      xmm0, dword ptr [eax + 0x714]
004d7430 f30f1145c0               movss      dword ptr [ebp - 0x40], xmm0
004d7435 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d7438 f30f104170               movss      xmm0, dword ptr [ecx + 0x70]
004d743d f30f1145bc               movss      dword ptr [ebp - 0x44], xmm0
004d7442 51                       push       ecx
004d7443 f30f1045c0               movss      xmm0, dword ptr [ebp - 0x40]
004d7448 f30f110424               movss      dword ptr [esp], xmm0
004d744d 51                       push       ecx
004d744e f30f1045bc               movss      xmm0, dword ptr [ebp - 0x44]
004d7453 f30f110424               movss      dword ptr [esp], xmm0
004d7458 e8a311f6ff               call       0x438600
004d745d 83c408                   add        esp, 8
004d7460 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d7463 d95a70                   fstp       dword ptr [edx + 0x70]
004d7466 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d7469 05f8060000               add        eax, 0x6f8
004d746e 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d7471 83c158                   add        ecx, 0x58
004d7474 8b10                     mov        edx, dword ptr [eax]
004d7476 8911                     mov        dword ptr [ecx], edx
004d7478 8b5004                   mov        edx, dword ptr [eax + 4]
004d747b 895104                   mov        dword ptr [ecx + 4], edx
004d747e 8b4008                   mov        eax, dword ptr [eax + 8]
004d7481 894108                   mov        dword ptr [ecx + 8], eax
004d7484 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d7487 8b9154070000             mov        edx, dword ptr [ecx + 0x754]
004d748d 83e201                   and        edx, 1
004d7490 7457                     je         0x4d74e9
004d7492 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d7495 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004d749b e890b2f3ff               call       0x412730
004d74a0 8945b8                   mov        dword ptr [ebp - 0x48], eax
004d74a3 6a00                     push       0
004d74a5 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004d74a8 e8b3e1faff               call       0x485660
004d74ad 85c0                     test       eax, eax
004d74af 7438                     je         0x4d74e9
004d74b1 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d74b4 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004d74ba e871b2f3ff               call       0x412730
004d74bf 8945b4                   mov        dword ptr [ebp - 0x4c], eax
004d74c2 6a00                     push       0
004d74c4 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
004d74c7 e894e1faff               call       0x485660
004d74cc 8bc8                     mov        ecx, eax
004d74ce e8ed2dfaff               call       0x47a2c0
004d74d3 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d74d6 83c258                   add        edx, 0x58
004d74d9 8b08                     mov        ecx, dword ptr [eax]
004d74db 890a                     mov        dword ptr [edx], ecx
004d74dd 8b4804                   mov        ecx, dword ptr [eax + 4]
004d74e0 894a04                   mov        dword ptr [edx + 4], ecx
004d74e3 8b4008                   mov        eax, dword ptr [eax + 8]
004d74e6 894208                   mov        dword ptr [edx + 8], eax
004d74e9 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d74ec 81c104070000             add        ecx, 0x704
004d74f2 894db0                   mov        dword ptr [ebp - 0x50], ecx
004d74f5 b9e4ef5a00               mov        ecx, 0x5aefe4
004d74fa e8e11df5ff               call       0x4292e0
004d74ff 51                       push       ecx
004d7500 d91c24                   fstp       dword ptr [esp]
004d7503 8d558c                   lea        edx, [ebp - 0x74]
004d7506 52                       push       edx
004d7507 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
004d750a e8e11df5ff               call       0x4292f0
004d750f 50                       push       eax
004d7510 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d7513 83c158                   add        ecx, 0x58
004d7516 e8c521f5ff               call       0x4296e0
004d751b 90                       nop        
004d751c 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d751f f30f108044070000         movss      xmm0, dword ptr [eax + 0x744]
004d7527 0f2e05e8e05600           ucomiss    xmm0, dword ptr [0x56e0e8]
004d752e 9f                       lahf       
004d752f f6c444                   test       ah, 0x44
004d7532 7b5d                     jnp        0x4d7591
004d7534 8d4df0                   lea        ecx, [ebp - 0x10]
004d7537 e8d4b8f4ff               call       0x422e10
004d753c 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d753f 51                       push       ecx
004d7540 f30f108144070000         movss      xmm0, dword ptr [ecx + 0x744]
004d7548 f30f110424               movss      dword ptr [esp], xmm0
004d754d 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d7550 51                       push       ecx
004d7551 f30f104270               movss      xmm0, dword ptr [edx + 0x70]
004d7556 f30f110424               movss      dword ptr [esp], xmm0
004d755b 8d45f0                   lea        eax, [ebp - 0x10]
004d755e 50                       push       eax
004d755f e8cc1df6ff               call       0x439330
004d7564 83c40c                   add        esp, 0xc
004d7567 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d756a f30f104158               movss      xmm0, dword ptr [ecx + 0x58]
004d756f f30f5845f0               addss      xmm0, dword ptr [ebp - 0x10]
004d7574 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d7577 f30f114258               movss      dword ptr [edx + 0x58], xmm0
004d757c 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d757f f30f10405c               movss      xmm0, dword ptr [eax + 0x5c]
004d7584 f30f5845f4               addss      xmm0, dword ptr [ebp - 0xc]
004d7589 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d758c f30f11415c               movss      dword ptr [ecx + 0x5c], xmm0
004d7591 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d7594 8b4220                   mov        eax, dword ptr [edx + 0x20]
004d7597 8945e8                   mov        dword ptr [ebp - 0x18], eax
004d759a 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004d759d 83e902                   sub        ecx, 2
004d75a0 894de8                   mov        dword ptr [ebp - 0x18], ecx
004d75a3 837de803                 cmp        dword ptr [ebp - 0x18], 3
004d75a7 0f877a010000             ja         0x4d7727
004d75ad 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004d75b0 ff249538774d00           jmp        dword ptr [edx*4 + 0x4d7738]
004d75b7 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d75ba 83c028                   add        eax, 0x28
004d75bd 8945ac                   mov        dword ptr [ebp - 0x54], eax
004d75c0 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d75c3 8b9128070000             mov        edx, dword ptr [ecx + 0x728]
004d75c9 52                       push       edx
004d75ca 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
004d75cd e8eebff4ff               call       0x4235c0
004d75d2 0fb6c0                   movzx      eax, al
004d75d5 85c0                     test       eax, eax
004d75d7 7417                     je         0x4d75f0
004d75d9 6a00                     push       0
004d75db 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d75de 83c128                   add        ecx, 0x28
004d75e1 e83abff4ff               call       0x423520
004d75e6 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d75e9 c7412004000000           mov        dword ptr [ecx + 0x20], 4
004d75f0 e932010000               jmp        0x4d7727
004d75f5 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d75f8 83c228                   add        edx, 0x28
004d75fb 8955a8                   mov        dword ptr [ebp - 0x58], edx
004d75fe 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d7601 8b882c070000             mov        ecx, dword ptr [eax + 0x72c]
004d7607 51                       push       ecx
004d7608 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004d760b e8b0bff4ff               call       0x4235c0
004d7610 0fb6d0                   movzx      edx, al
004d7613 85d2                     test       edx, edx
004d7615 742a                     je         0x4d7641
004d7617 6a00                     push       0
004d7619 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d761c 83c128                   add        ecx, 0x28
004d761f e8fcbef4ff               call       0x423520
004d7624 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d7627 c7402002000000           mov        dword ptr [eax + 0x20], 2
004d762e 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d7631 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d7634 8b8220070000             mov        eax, dword ptr [edx + 0x720]
004d763a 894178                   mov        dword ptr [ecx + 0x78], eax
004d763d eb3c                     jmp        0x4d767b
004d763f eb3a                     jmp        0x4d767b
004d7641 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d7644 83c128                   add        ecx, 0x28
004d7647 e884c5f4ff               call       0x423bd0
004d764c d95da4                   fstp       dword ptr [ebp - 0x5c]
004d764f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d7652 f30f1045a4               movss      xmm0, dword ptr [ebp - 0x5c]
004d7657 f30f598120070000         mulss      xmm0, dword ptr [ecx + 0x720]
004d765f 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d7662 f30f2a8a2c070000         cvtsi2ss   xmm1, dword ptr [edx + 0x72c]
004d766a f30f5ec1                 divss      xmm0, xmm1
004d766e 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d7671 f30f114078               movss      dword ptr [eax + 0x78], xmm0
004d7676 e9ac000000               jmp        0x4d7727
004d767b 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d767e 83c128                   add        ecx, 0x28
004d7681 894da0                   mov        dword ptr [ebp - 0x60], ecx
004d7684 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d7687 8b8230070000             mov        eax, dword ptr [edx + 0x730]
004d768d 50                       push       eax
004d768e 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
004d7691 e82abff4ff               call       0x4235c0
004d7696 0fb6c8                   movzx      ecx, al
004d7699 85c9                     test       ecx, ecx
004d769b 741b                     je         0x4d76b8
004d769d 6a00                     push       0
004d769f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d76a2 83c128                   add        ecx, 0x28
004d76a5 e876bef4ff               call       0x423520
004d76aa 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d76ad c7422005000000           mov        dword ptr [edx + 0x20], 5
004d76b4 eb04                     jmp        0x4d76ba
004d76b6 eb02                     jmp        0x4d76ba
004d76b8 eb6d                     jmp        0x4d7727
004d76ba 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d76bd 83c028                   add        eax, 0x28
004d76c0 89459c                   mov        dword ptr [ebp - 0x64], eax
004d76c3 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d76c6 8b9134070000             mov        edx, dword ptr [ecx + 0x734]
004d76cc 52                       push       edx
004d76cd 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
004d76d0 e8ebbef4ff               call       0x4235c0
004d76d5 0fb6c0                   movzx      eax, al
004d76d8 85c0                     test       eax, eax
004d76da 7407                     je         0x4d76e3
004d76dc b801000000               mov        eax, 1
004d76e1 eb46                     jmp        0x4d7729
004d76e3 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d76e6 83c128                   add        ecx, 0x28
004d76e9 e8e2c4f4ff               call       0x423bd0
004d76ee d95d98                   fstp       dword ptr [ebp - 0x68]
004d76f1 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d76f4 f30f104598               movss      xmm0, dword ptr [ebp - 0x68]
004d76f9 f30f598120070000         mulss      xmm0, dword ptr [ecx + 0x720]
004d7701 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d7704 f30f2a8a34070000         cvtsi2ss   xmm1, dword ptr [edx + 0x734]
004d770c f30f5ec1                 divss      xmm0, xmm1
004d7710 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d7713 f30f108820070000         movss      xmm1, dword ptr [eax + 0x720]
004d771b f30f5cc8                 subss      xmm1, xmm0
004d771f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d7722 f30f114978               movss      dword ptr [ecx + 0x78], xmm1
004d7727 33c0                     xor        eax, eax
004d7729 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d772c 33cd                     xor        ecx, ebp
004d772e e8bbb20600               call       0x5429ee
004d7733 8be5                     mov        esp, ebp
004d7735 5d                       pop        ebp
004d7736 c3                       ret        
004d7737 90                       nop        
004d7738 7b76                     jnp        0x4d77b0
004d773a 4d                       dec        ebp
004d773b 00b7754d00f5             add        byte ptr [edi - 0xaffb28b], dh
004d7741 754d                     jne        0x4d7790
004d7743 00ba764d00cc             add        byte ptr [edx - 0x33ffb28a], bh
004d7749 cc                       int3       
004d774a cc                       int3       
004d774b cc                       int3       
004d774c cc                       int3       
004d774d cc                       int3       
004d774e cc                       int3       
004d774f cc                       int3       
