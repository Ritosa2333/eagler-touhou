0046d300  55                                 push       ebp
0046d301  8b ec                              mov        ebp, esp
0046d303  81 ec 70 01 00 00                  sub        esp, 0x170
0046d309  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0046d30e  33 c5                              xor        eax, ebp
0046d310  89 45 fc                           mov        dword ptr [ebp - 4], eax
0046d313  89 4d c8                           mov        dword ptr [ebp - 0x38], ecx
0046d316  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046d319  50                                 push       eax
0046d31a  e8 01 2b 0e 00                     call       0x54fe20
0046d31f  83 c4 04                           add        esp, 4
0046d322  89 85 7c ff ff ff                  mov        dword ptr [ebp - 0x84], eax
0046d328  51                                 push       ecx
0046d329  0f 57 c0                           xorps      xmm0, xmm0
0046d32c  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
0046d331  51                                 push       ecx
0046d332  0f 57 c0                           xorps      xmm0, xmm0
0046d335  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
0046d33a  51                                 push       ecx
0046d33b  0f 57 c0                           xorps      xmm0, xmm0
0046d33e  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
0046d343  8d 4d f0                           lea        ecx, [ebp - 0x10]
0046d346  e8 85 5a fb ff                     call       0x422dd0
0046d34b  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046d34e  8a 91 aa 04 00 00                  mov        dl, byte ptr [ecx + 0x4aa]
0046d354  80 ca 01                           or         dl, 1
0046d357  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046d35a  88 90 aa 04 00 00                  mov        byte ptr [eax + 0x4aa], dl
0046d360  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046d363  8a 91 b4 04 00 00                  mov        dl, byte ptr [ecx + 0x4b4]
0046d369  80 e2 00                           and        dl, 0
0046d36c  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046d36f  88 90 b4 04 00 00                  mov        byte ptr [eax + 0x4b4], dl
0046d375  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046d378  c7 81 b8 04 00 00 01 00 00 00      mov        dword ptr [ecx + 0x4b8], 1
0046d382  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046d385  c7 82 bc 04 00 00 01 00 00 00      mov        dword ptr [edx + 0x4bc], 1
0046d38f  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046d392  83 c0 10                           add        eax, 0x10
0046d395  89 85 e0 fe ff ff                  mov        dword ptr [ebp - 0x120], eax
0046d39b  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046d39e  81 c1 00 01 00 00                  add        ecx, 0x100
0046d3a4  51                                 push       ecx
0046d3a5  8b 8d e0 fe ff ff                  mov        ecx, dword ptr [ebp - 0x120]
0046d3ab  e8 40 37 00 00                     call       0x470af0
0046d3b0  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046d3b3  83 c2 10                           add        edx, 0x10
0046d3b6  89 95 a4 fe ff ff                  mov        dword ptr [ebp - 0x15c], edx
0046d3bc  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046d3bf  8b 88 10 01 00 00                  mov        ecx, dword ptr [eax + 0x110]
0046d3c5  51                                 push       ecx
0046d3c6  8b 8d a4 fe ff ff                  mov        ecx, dword ptr [ebp - 0x15c]
0046d3cc  e8 6f 09 ff ff                     call       0x45dd40
0046d3d1  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046d3d4  83 c2 10                           add        edx, 0x10
0046d3d7  89 95 a0 fe ff ff                  mov        dword ptr [ebp - 0x160], edx
0046d3dd  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046d3e0  51                                 push       ecx
0046d3e1  f3 0f 10 80 18 01 00 00            movss      xmm0, dword ptr [eax + 0x118]
0046d3e9  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
0046d3ee  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046d3f1  51                                 push       ecx
0046d3f2  f3 0f 10 81 14 01 00 00            movss      xmm0, dword ptr [ecx + 0x114]
0046d3fa  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
0046d3ff  8b 8d a0 fe ff ff                  mov        ecx, dword ptr [ebp - 0x160]
0046d405  e8 46 3a 00 00                     call       0x470e50
0046d40a  90                                 nop        
0046d40b  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046d40e  8b 82 28 01 00 00                  mov        eax, dword ptr [edx + 0x128]
0046d414  89 45 8c                           mov        dword ptr [ebp - 0x74], eax
0046d417  83 7d 8c 0d                        cmp        dword ptr [ebp - 0x74], 0xd
0046d41b  77 0a                              ja         0x46d427
0046d41d  8b 4d 8c                           mov        ecx, dword ptr [ebp - 0x74]
0046d420  ff 24 8d 08 e9 46 00               jmp        dword ptr [ecx*4 + 0x46e908]
0046d427  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046d42a  f3 0f 10 82 14 01 00 00            movss      xmm0, dword ptr [edx + 0x114]
0046d432  0f 2e 05 cc c8 56 00               ucomiss    xmm0, dword ptr [0x56c8cc]
0046d439  9f                                 lahf       
0046d43a  f6 c4 44                           test       ah, 0x44
0046d43d  7b 1a                              jnp        0x46d459
0046d43f  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046d442  8a 88 b0 04 00 00                  mov        cl, byte ptr [eax + 0x4b0]
0046d448  80 e1 f3                           and        cl, 0xf3
0046d44b  80 c9 04                           or         cl, 4
0046d44e  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046d451  88 8a b0 04 00 00                  mov        byte ptr [edx + 0x4b0], cl
0046d457  eb 15                              jmp        0x46d46e
0046d459  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046d45c  8a 88 b0 04 00 00                  mov        cl, byte ptr [eax + 0x4b0]
0046d462  80 e1 f3                           and        cl, 0xf3
0046d465  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046d468  88 8a b0 04 00 00                  mov        byte ptr [edx + 0x4b0], cl
0046d46e  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046d471  f3 0f 2a 80 3c a2 01 00            cvtsi2ss   xmm0, dword ptr [eax + 0x1a23c]
0046d479  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046d47c  f3 0f 59 81 14 01 00 00            mulss      xmm0, dword ptr [ecx + 0x114]
0046d484  f3 0f 11 45 c0                     movss      dword ptr [ebp - 0x40], xmm0
0046d489  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046d48c  f3 0f 10 05 ac f7 56 00            movss      xmm0, dword ptr [0x56f7ac]
0046d494  f3 0f 59 82 18 01 00 00            mulss      xmm0, dword ptr [edx + 0x118]
0046d49c  f3 0f 11 45 bc                     movss      dword ptr [ebp - 0x44], xmm0
0046d4a1  e9 11 03 00 00                     jmp        0x46d7b7
0046d4a6  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046d4a9  8a 88 b0 04 00 00                  mov        cl, byte ptr [eax + 0x4b0]
0046d4af  80 e1 f3                           and        cl, 0xf3
0046d4b2  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046d4b5  88 8a b0 04 00 00                  mov        byte ptr [edx + 0x4b0], cl
0046d4bb  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046d4be  f3 0f 10 05 0c f0 56 00            movss      xmm0, dword ptr [0x56f00c]
0046d4c6  f3 0f 59 80 14 01 00 00            mulss      xmm0, dword ptr [eax + 0x114]
0046d4ce  f3 0f 11 45 c0                     movss      dword ptr [ebp - 0x40], xmm0
0046d4d3  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046d4d6  f3 0f 10 05 a4 f7 56 00            movss      xmm0, dword ptr [0x56f7a4]
0046d4de  f3 0f 59 81 18 01 00 00            mulss      xmm0, dword ptr [ecx + 0x118]
0046d4e6  f3 0f 11 45 bc                     movss      dword ptr [ebp - 0x44], xmm0
0046d4eb  e9 c7 02 00 00                     jmp        0x46d7b7
0046d4f0  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046d4f3  f3 0f 10 82 14 01 00 00            movss      xmm0, dword ptr [edx + 0x114]
0046d4fb  0f 2e 05 cc c8 56 00               ucomiss    xmm0, dword ptr [0x56c8cc]
0046d502  9f                                 lahf       
0046d503  f6 c4 44                           test       ah, 0x44
0046d506  7b 1a                              jnp        0x46d522
0046d508  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046d50b  8a 88 b0 04 00 00                  mov        cl, byte ptr [eax + 0x4b0]
0046d511  80 e1 f3                           and        cl, 0xf3
0046d514  80 c9 04                           or         cl, 4
0046d517  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046d51a  88 8a b0 04 00 00                  mov        byte ptr [edx + 0x4b0], cl
0046d520  eb 15                              jmp        0x46d537
0046d522  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046d525  8a 88 b0 04 00 00                  mov        cl, byte ptr [eax + 0x4b0]
0046d52b  80 e1 f3                           and        cl, 0xf3
0046d52e  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046d531  88 8a b0 04 00 00                  mov        byte ptr [edx + 0x4b0], cl
0046d537  f3 0f 10 05 b4 f7 56 00            movss      xmm0, dword ptr [0x56f7b4]
0046d53f  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
0046d547  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046d54a  f3 0f 59 80 14 01 00 00            mulss      xmm0, dword ptr [eax + 0x114]
0046d552  f3 0f 11 45 c0                     movss      dword ptr [ebp - 0x40], xmm0
0046d557  f3 0f 10 05 94 cd 56 00            movss      xmm0, dword ptr [0x56cd94]
0046d55f  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
0046d567  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046d56a  f3 0f 59 81 18 01 00 00            mulss      xmm0, dword ptr [ecx + 0x118]
0046d572  f3 0f 11 45 bc                     movss      dword ptr [ebp - 0x44], xmm0
0046d577  e9 3b 02 00 00                     jmp        0x46d7b7
0046d57c  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046d57f  f3 0f 10 82 14 01 00 00            movss      xmm0, dword ptr [edx + 0x114]
0046d587  0f 2e 05 cc c8 56 00               ucomiss    xmm0, dword ptr [0x56c8cc]
0046d58e  9f                                 lahf       
0046d58f  f6 c4 44                           test       ah, 0x44
0046d592  7b 1a                              jnp        0x46d5ae
0046d594  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046d597  8a 88 b0 04 00 00                  mov        cl, byte ptr [eax + 0x4b0]
0046d59d  80 e1 f3                           and        cl, 0xf3
0046d5a0  80 c9 04                           or         cl, 4
0046d5a3  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046d5a6  88 8a b0 04 00 00                  mov        byte ptr [edx + 0x4b0], cl
0046d5ac  eb 15                              jmp        0x46d5c3
0046d5ae  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046d5b1  8a 88 b0 04 00 00                  mov        cl, byte ptr [eax + 0x4b0]
0046d5b7  80 e1 f3                           and        cl, 0xf3
0046d5ba  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046d5bd  88 8a b0 04 00 00                  mov        byte ptr [edx + 0x4b0], cl
0046d5c3  f3 0f 10 05 b8 f7 56 00            movss      xmm0, dword ptr [0x56f7b8]
0046d5cb  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
0046d5d3  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046d5d6  f3 0f 59 80 14 01 00 00            mulss      xmm0, dword ptr [eax + 0x114]
0046d5de  f3 0f 11 45 c0                     movss      dword ptr [ebp - 0x40], xmm0
0046d5e3  f3 0f 10 05 bc f7 56 00            movss      xmm0, dword ptr [0x56f7bc]
0046d5eb  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
0046d5f3  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046d5f6  f3 0f 59 81 18 01 00 00            mulss      xmm0, dword ptr [ecx + 0x118]
0046d5fe  f3 0f 11 45 bc                     movss      dword ptr [ebp - 0x44], xmm0
0046d603  e9 af 01 00 00                     jmp        0x46d7b7
0046d608  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046d60b  8a 82 b0 04 00 00                  mov        al, byte ptr [edx + 0x4b0]
0046d611  24 f3                              and        al, 0xf3
0046d613  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046d616  88 81 b0 04 00 00                  mov        byte ptr [ecx + 0x4b0], al
0046d61c  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046d61f  f3 0f 10 05 a0 f7 56 00            movss      xmm0, dword ptr [0x56f7a0]
0046d627  f3 0f 59 82 14 01 00 00            mulss      xmm0, dword ptr [edx + 0x114]
0046d62f  f3 0f 11 45 c0                     movss      dword ptr [ebp - 0x40], xmm0
0046d634  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046d637  f3 0f 10 05 34 e7 56 00            movss      xmm0, dword ptr [0x56e734]
0046d63f  f3 0f 59 80 18 01 00 00            mulss      xmm0, dword ptr [eax + 0x118]
0046d647  f3 0f 11 45 bc                     movss      dword ptr [ebp - 0x44], xmm0
0046d64c  e9 66 01 00 00                     jmp        0x46d7b7
0046d651  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046d654  8a 91 b0 04 00 00                  mov        dl, byte ptr [ecx + 0x4b0]
0046d65a  80 e2 f3                           and        dl, 0xf3
0046d65d  80 ca 04                           or         dl, 4
0046d660  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046d663  88 90 b0 04 00 00                  mov        byte ptr [eax + 0x4b0], dl
0046d669  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046d66c  f3 0f 10 05 a0 f7 56 00            movss      xmm0, dword ptr [0x56f7a0]
0046d674  f3 0f 59 81 14 01 00 00            mulss      xmm0, dword ptr [ecx + 0x114]
0046d67c  f3 0f 11 45 c0                     movss      dword ptr [ebp - 0x40], xmm0
0046d681  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046d684  f3 0f 10 05 34 e7 56 00            movss      xmm0, dword ptr [0x56e734]
0046d68c  f3 0f 59 82 18 01 00 00            mulss      xmm0, dword ptr [edx + 0x118]
0046d694  f3 0f 11 45 bc                     movss      dword ptr [ebp - 0x44], xmm0
0046d699  e9 19 01 00 00                     jmp        0x46d7b7
0046d69e  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046d6a1  8a 88 b0 04 00 00                  mov        cl, byte ptr [eax + 0x4b0]
0046d6a7  80 e1 f3                           and        cl, 0xf3
0046d6aa  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046d6ad  88 8a b0 04 00 00                  mov        byte ptr [edx + 0x4b0], cl
0046d6b3  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046d6b6  f3 0f 10 05 a8 f7 56 00            movss      xmm0, dword ptr [0x56f7a8]
0046d6be  f3 0f 59 80 14 01 00 00            mulss      xmm0, dword ptr [eax + 0x114]
0046d6c6  f3 0f 11 45 c0                     movss      dword ptr [ebp - 0x40], xmm0
0046d6cb  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046d6ce  f3 0f 10 05 90 cd 56 00            movss      xmm0, dword ptr [0x56cd90]
0046d6d6  f3 0f 59 81 18 01 00 00            mulss      xmm0, dword ptr [ecx + 0x118]
0046d6de  f3 0f 11 45 bc                     movss      dword ptr [ebp - 0x44], xmm0
0046d6e3  e9 cf 00 00 00                     jmp        0x46d7b7
0046d6e8  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046d6eb  8a 82 b0 04 00 00                  mov        al, byte ptr [edx + 0x4b0]
0046d6f1  24 f3                              and        al, 0xf3
0046d6f3  0c 04                              or         al, 4
0046d6f5  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046d6f8  88 81 b0 04 00 00                  mov        byte ptr [ecx + 0x4b0], al
0046d6fe  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046d701  f3 0f 10 05 a8 f7 56 00            movss      xmm0, dword ptr [0x56f7a8]
0046d709  f3 0f 59 82 14 01 00 00            mulss      xmm0, dword ptr [edx + 0x114]
0046d711  f3 0f 11 45 c0                     movss      dword ptr [ebp - 0x40], xmm0
0046d716  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046d719  f3 0f 10 05 90 cd 56 00            movss      xmm0, dword ptr [0x56cd90]
0046d721  f3 0f 59 80 18 01 00 00            mulss      xmm0, dword ptr [eax + 0x118]
0046d729  f3 0f 11 45 bc                     movss      dword ptr [ebp - 0x44], xmm0
0046d72e  e9 84 00 00 00                     jmp        0x46d7b7
0046d733  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046d736  f3 0f 10 81 14 01 00 00            movss      xmm0, dword ptr [ecx + 0x114]
0046d73e  0f 2e 05 cc c8 56 00               ucomiss    xmm0, dword ptr [0x56c8cc]
0046d745  9f                                 lahf       
0046d746  f6 c4 44                           test       ah, 0x44
0046d749  7b 18                              jnp        0x46d763
0046d74b  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046d74e  8a 82 b0 04 00 00                  mov        al, byte ptr [edx + 0x4b0]
0046d754  24 f3                              and        al, 0xf3
0046d756  0c 04                              or         al, 4
0046d758  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046d75b  88 81 b0 04 00 00                  mov        byte ptr [ecx + 0x4b0], al
0046d761  eb 14                              jmp        0x46d777
0046d763  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046d766  8a 82 b0 04 00 00                  mov        al, byte ptr [edx + 0x4b0]
0046d76c  24 f3                              and        al, 0xf3
0046d76e  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046d771  88 81 b0 04 00 00                  mov        byte ptr [ecx + 0x4b0], al
0046d777  f3 0f 10 05 ac f7 56 00            movss      xmm0, dword ptr [0x56f7ac]
0046d77f  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
0046d787  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046d78a  f3 0f 59 82 14 01 00 00            mulss      xmm0, dword ptr [edx + 0x114]
0046d792  f3 0f 11 45 c0                     movss      dword ptr [ebp - 0x40], xmm0
0046d797  f3 0f 10 05 b0 f7 56 00            movss      xmm0, dword ptr [0x56f7b0]
0046d79f  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
0046d7a7  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046d7aa  f3 0f 59 80 18 01 00 00            mulss      xmm0, dword ptr [eax + 0x118]
0046d7b2  f3 0f 11 45 bc                     movss      dword ptr [ebp - 0x44], xmm0
0046d7b7  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046d7ba  8b 91 38 01 00 00                  mov        edx, dword ptr [ecx + 0x138]
0046d7c0  89 55 88                           mov        dword ptr [ebp - 0x78], edx
0046d7c3  83 7d 88 00                        cmp        dword ptr [ebp - 0x78], 0
0046d7c7  74 0f                              je         0x46d7d8
0046d7c9  83 7d 88 02                        cmp        dword ptr [ebp - 0x78], 2
0046d7cd  0f 84 38 03 00 00                  je         0x46db0b
0046d7d3  e9 29 06 00 00                     jmp        0x46de01
0046d7d8  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046d7db  8b 88 28 01 00 00                  mov        ecx, dword ptr [eax + 0x128]
0046d7e1  89 4d a4                           mov        dword ptr [ebp - 0x5c], ecx
0046d7e4  8b 55 a4                           mov        edx, dword ptr [ebp - 0x5c]
0046d7e7  83 ea 02                           sub        edx, 2
0046d7ea  89 55 a4                           mov        dword ptr [ebp - 0x5c], edx
0046d7ed  83 7d a4 0b                        cmp        dword ptr [ebp - 0x5c], 0xb
0046d7f1  0f 87 e6 02 00 00                  ja         0x46dadd
0046d7f7  8b 45 a4                           mov        eax, dword ptr [ebp - 0x5c]
0046d7fa  ff 24 85 40 e9 46 00               jmp        dword ptr [eax*4 + 0x46e940]
0046d801  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046d804  83 b9 28 01 00 00 06               cmp        dword ptr [ecx + 0x128], 6
0046d80b  74 15                              je         0x46d822
0046d80d  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046d810  83 ba 28 01 00 00 08               cmp        dword ptr [edx + 0x128], 8
0046d817  74 09                              je         0x46d822
0046d819  c7 45 84 04 02 00 00               mov        dword ptr [ebp - 0x7c], 0x204
0046d820  eb 07                              jmp        0x46d829
0046d822  c7 45 84 40 01 00 00               mov        dword ptr [ebp - 0x7c], 0x140
0046d829  8b 45 84                           mov        eax, dword ptr [ebp - 0x7c]
0046d82c  89 85 9c fe ff ff                  mov        dword ptr [ebp - 0x164], eax
0046d832  0f 57 c0                           xorps      xmm0, xmm0
0046d835  f3 0f 11 45 9c                     movss      dword ptr [ebp - 0x64], xmm0
0046d83a  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046d83d  89 4d c4                           mov        dword ptr [ebp - 0x3c], ecx
0046d840  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046d843  0f b6 02                           movzx      eax, byte ptr [edx]
0046d846  85 c0                              test       eax, eax
0046d848  74 61                              je         0x46d8ab
0046d84a  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046d84d  83 c1 10                           add        ecx, 0x10
0046d850  89 8d 68 ff ff ff                  mov        dword ptr [ebp - 0x98], ecx
0046d856  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046d859  0f b6 02                           movzx      eax, byte ptr [edx]
0046d85c  8b 8d 9c fe ff ff                  mov        ecx, dword ptr [ebp - 0x164]
0046d862  8d 54 08 e0                        lea        edx, [eax + ecx - 0x20]
0046d866  52                                 push       edx
0046d867  8b 8d 68 ff ff ff                  mov        ecx, dword ptr [ebp - 0x98]
0046d86d  e8 5e 36 00 00                     call       0x470ed0
0046d872  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046d875  83 c1 10                           add        ecx, 0x10
0046d878  e8 53 a4 fc ff                     call       0x437cd0
0046d87d  f3 0f 10 05 d8 c8 56 00            movss      xmm0, dword ptr [0x56c8d8]
0046d885  f3 0f 5c 05 18 88 5b 00            subss      xmm0, dword ptr [0x5b8818]
0046d88d  f3 0f 10 48 4c                     movss      xmm1, dword ptr [eax + 0x4c]
0046d892  f3 0f 5e c8                        divss      xmm1, xmm0
0046d896  f3 0f 58 4d 9c                     addss      xmm1, dword ptr [ebp - 0x64]
0046d89b  f3 0f 11 4d 9c                     movss      dword ptr [ebp - 0x64], xmm1
0046d8a0  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046d8a3  83 c0 01                           add        eax, 1
0046d8a6  89 45 c4                           mov        dword ptr [ebp - 0x3c], eax
0046d8a9  eb 95                              jmp        0x46d840
0046d8ab  f3 0f 10 45 9c                     movss      xmm0, dword ptr [ebp - 0x64]
0046d8b0  0f 57 05 d0 d7 56 00               xorps      xmm0, xmmword ptr [0x56d7d0]
0046d8b7  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046d8ba  f3 0f 59 81 14 01 00 00            mulss      xmm0, dword ptr [ecx + 0x114]
0046d8c2  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
0046d8ca  f3 0f 11 45 f0                     movss      dword ptr [ebp - 0x10], xmm0
0046d8cf  e9 32 02 00 00                     jmp        0x46db06
0046d8d4  c7 85 94 fe ff ff c8 02 00 00      mov        dword ptr [ebp - 0x16c], 0x2c8
0046d8de  0f 57 c0                           xorps      xmm0, xmm0
0046d8e1  f3 0f 11 45 98                     movss      dword ptr [ebp - 0x68], xmm0
0046d8e6  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046d8e9  89 55 c4                           mov        dword ptr [ebp - 0x3c], edx
0046d8ec  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046d8ef  0f b6 08                           movzx      ecx, byte ptr [eax]
0046d8f2  85 c9                              test       ecx, ecx
0046d8f4  0f 84 99 00 00 00                  je         0x46d993
0046d8fa  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046d8fd  0f b6 02                           movzx      eax, byte ptr [edx]
0046d900  83 f8 20                           cmp        eax, 0x20
0046d903  7c 26                              jl         0x46d92b
0046d905  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046d908  83 c1 10                           add        ecx, 0x10
0046d90b  89 8d 64 ff ff ff                  mov        dword ptr [ebp - 0x9c], ecx
0046d911  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046d914  0f b6 02                           movzx      eax, byte ptr [edx]
0046d917  05 a8 02 00 00                     add        eax, 0x2a8
0046d91c  50                                 push       eax
0046d91d  8b 8d 64 ff ff ff                  mov        ecx, dword ptr [ebp - 0x9c]
0046d923  e8 a8 35 00 00                     call       0x470ed0
0046d928  90                                 nop        
0046d929  eb 2c                              jmp        0x46d957
0046d92b  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046d92e  83 c1 10                           add        ecx, 0x10
0046d931  89 8d 60 ff ff ff                  mov        dword ptr [ebp - 0xa0], ecx
0046d937  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046d93a  0f b6 02                           movzx      eax, byte ptr [edx]
0046d93d  b9 20 00 00 00                     mov        ecx, 0x20
0046d942  2b c8                              sub        ecx, eax
0046d944  81 c1 29 03 00 00                  add        ecx, 0x329
0046d94a  51                                 push       ecx
0046d94b  8b 8d 60 ff ff ff                  mov        ecx, dword ptr [ebp - 0xa0]
0046d951  e8 7a 35 00 00                     call       0x470ed0
0046d956  90                                 nop        
0046d957  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046d95a  83 c1 10                           add        ecx, 0x10
0046d95d  e8 6e a3 fc ff                     call       0x437cd0
0046d962  f3 0f 10 05 d8 c8 56 00            movss      xmm0, dword ptr [0x56c8d8]
0046d96a  f3 0f 5c 05 18 88 5b 00            subss      xmm0, dword ptr [0x5b8818]
0046d972  f3 0f 10 48 4c                     movss      xmm1, dword ptr [eax + 0x4c]
0046d977  f3 0f 5e c8                        divss      xmm1, xmm0
0046d97b  f3 0f 58 4d 98                     addss      xmm1, dword ptr [ebp - 0x68]
0046d980  f3 0f 11 4d 98                     movss      dword ptr [ebp - 0x68], xmm1
0046d985  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046d988  83 c2 01                           add        edx, 1
0046d98b  89 55 c4                           mov        dword ptr [ebp - 0x3c], edx
0046d98e  e9 59 ff ff ff                     jmp        0x46d8ec
0046d993  f3 0f 10 45 98                     movss      xmm0, dword ptr [ebp - 0x68]
0046d998  0f 57 05 d0 d7 56 00               xorps      xmm0, xmmword ptr [0x56d7d0]
0046d99f  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046d9a2  f3 0f 59 80 14 01 00 00            mulss      xmm0, dword ptr [eax + 0x114]
0046d9aa  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
0046d9b2  f3 0f 11 45 f0                     movss      dword ptr [ebp - 0x10], xmm0
0046d9b7  e9 4a 01 00 00                     jmp        0x46db06
0046d9bc  0f 57 c0                           xorps      xmm0, xmm0
0046d9bf  f3 0f 11 45 b4                     movss      dword ptr [ebp - 0x4c], xmm0
0046d9c4  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046d9c7  89 4d c4                           mov        dword ptr [ebp - 0x3c], ecx
0046d9ca  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046d9cd  0f b6 02                           movzx      eax, byte ptr [edx]
0046d9d0  85 c0                              test       eax, eax
0046d9d2  74 6b                              je         0x46da3f
0046d9d4  8b 4d c4                           mov        ecx, dword ptr [ebp - 0x3c]
0046d9d7  0f b6 11                           movzx      edx, byte ptr [ecx]
0046d9da  83 fa 2e                           cmp        edx, 0x2e
0046d9dd  75 2f                              jne        0x46da0e
0046d9df  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046d9e2  f3 0f 10 05 c0 f7 56 00            movss      xmm0, dword ptr [0x56f7c0]
0046d9ea  f3 0f 59 80 14 01 00 00            mulss      xmm0, dword ptr [eax + 0x114]
0046d9f2  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
0046d9fa  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046da02  f3 0f 58 45 b4                     addss      xmm0, dword ptr [ebp - 0x4c]
0046da07  f3 0f 11 45 b4                     movss      dword ptr [ebp - 0x4c], xmm0
0046da0c  eb 26                              jmp        0x46da34
0046da0e  f3 0f 10 45 c0                     movss      xmm0, dword ptr [ebp - 0x40]
0046da13  0f 57 05 d0 d7 56 00               xorps      xmm0, xmmword ptr [0x56d7d0]
0046da1a  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
0046da22  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046da2a  f3 0f 58 45 b4                     addss      xmm0, dword ptr [ebp - 0x4c]
0046da2f  f3 0f 11 45 b4                     movss      dword ptr [ebp - 0x4c], xmm0
0046da34  8b 4d c4                           mov        ecx, dword ptr [ebp - 0x3c]
0046da37  83 c1 01                           add        ecx, 1
0046da3a  89 4d c4                           mov        dword ptr [ebp - 0x3c], ecx
0046da3d  eb 8b                              jmp        0x46d9ca
0046da3f  f3 0f 10 45 b4                     movss      xmm0, dword ptr [ebp - 0x4c]
0046da44  f3 0f 11 45 f0                     movss      dword ptr [ebp - 0x10], xmm0
0046da49  e9 b8 00 00 00                     jmp        0x46db06
0046da4e  0f 57 c0                           xorps      xmm0, xmm0
0046da51  f3 0f 11 45 b0                     movss      dword ptr [ebp - 0x50], xmm0
0046da56  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046da59  89 55 c4                           mov        dword ptr [ebp - 0x3c], edx
0046da5c  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046da5f  0f b6 08                           movzx      ecx, byte ptr [eax]
0046da62  85 c9                              test       ecx, ecx
0046da64  74 6b                              je         0x46dad1
0046da66  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046da69  0f b6 02                           movzx      eax, byte ptr [edx]
0046da6c  83 f8 2c                           cmp        eax, 0x2c
0046da6f  75 2f                              jne        0x46daa0
0046da71  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046da74  f3 0f 10 05 c0 f7 56 00            movss      xmm0, dword ptr [0x56f7c0]
0046da7c  f3 0f 59 81 14 01 00 00            mulss      xmm0, dword ptr [ecx + 0x114]
0046da84  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
0046da8c  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046da94  f3 0f 58 45 b0                     addss      xmm0, dword ptr [ebp - 0x50]
0046da99  f3 0f 11 45 b0                     movss      dword ptr [ebp - 0x50], xmm0
0046da9e  eb 26                              jmp        0x46dac6
0046daa0  f3 0f 10 45 c0                     movss      xmm0, dword ptr [ebp - 0x40]
0046daa5  0f 57 05 d0 d7 56 00               xorps      xmm0, xmmword ptr [0x56d7d0]
0046daac  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
0046dab4  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046dabc  f3 0f 58 45 b0                     addss      xmm0, dword ptr [ebp - 0x50]
0046dac1  f3 0f 11 45 b0                     movss      dword ptr [ebp - 0x50], xmm0
0046dac6  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046dac9  83 c2 01                           add        edx, 1
0046dacc  89 55 c4                           mov        dword ptr [ebp - 0x3c], edx
0046dacf  eb 8b                              jmp        0x46da5c
0046dad1  f3 0f 10 45 b0                     movss      xmm0, dword ptr [ebp - 0x50]
0046dad6  f3 0f 11 45 f0                     movss      dword ptr [ebp - 0x10], xmm0
0046dadb  eb 29                              jmp        0x46db06
0046dadd  f3 0f 2a 85 7c ff ff ff            cvtsi2ss   xmm0, dword ptr [ebp - 0x84]
0046dae5  0f 57 05 d0 d7 56 00               xorps      xmm0, xmmword ptr [0x56d7d0]
0046daec  f3 0f 59 45 c0                     mulss      xmm0, dword ptr [ebp - 0x40]
0046daf1  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
0046daf9  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046db01  f3 0f 11 45 f0                     movss      dword ptr [ebp - 0x10], xmm0
0046db06  e9 f6 02 00 00                     jmp        0x46de01
0046db0b  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046db0e  8b 88 28 01 00 00                  mov        ecx, dword ptr [eax + 0x128]
0046db14  89 4d a0                           mov        dword ptr [ebp - 0x60], ecx
0046db17  8b 55 a0                           mov        edx, dword ptr [ebp - 0x60]
0046db1a  83 ea 02                           sub        edx, 2
0046db1d  89 55 a0                           mov        dword ptr [ebp - 0x60], edx
0046db20  83 7d a0 0b                        cmp        dword ptr [ebp - 0x60], 0xb
0046db24  0f 87 b6 02 00 00                  ja         0x46dde0
0046db2a  8b 45 a0                           mov        eax, dword ptr [ebp - 0x60]
0046db2d  ff 24 85 70 e9 46 00               jmp        dword ptr [eax*4 + 0x46e970]
0046db34  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046db37  83 b9 28 01 00 00 06               cmp        dword ptr [ecx + 0x128], 6
0046db3e  74 15                              je         0x46db55
0046db40  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046db43  83 ba 28 01 00 00 08               cmp        dword ptr [edx + 0x128], 8
0046db4a  74 09                              je         0x46db55
0046db4c  c7 45 80 04 02 00 00               mov        dword ptr [ebp - 0x80], 0x204
0046db53  eb 07                              jmp        0x46db5c
0046db55  c7 45 80 40 01 00 00               mov        dword ptr [ebp - 0x80], 0x140
0046db5c  8b 45 80                           mov        eax, dword ptr [ebp - 0x80]
0046db5f  89 85 5c ff ff ff                  mov        dword ptr [ebp - 0xa4], eax
0046db65  0f 57 c0                           xorps      xmm0, xmm0
0046db68  f3 0f 11 45 94                     movss      dword ptr [ebp - 0x6c], xmm0
0046db6d  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046db70  89 4d c4                           mov        dword ptr [ebp - 0x3c], ecx
0046db73  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046db76  0f b6 02                           movzx      eax, byte ptr [edx]
0046db79  85 c0                              test       eax, eax
0046db7b  74 61                              je         0x46dbde
0046db7d  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046db80  83 c1 10                           add        ecx, 0x10
0046db83  89 8d 58 ff ff ff                  mov        dword ptr [ebp - 0xa8], ecx
0046db89  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046db8c  0f b6 02                           movzx      eax, byte ptr [edx]
0046db8f  8b 8d 5c ff ff ff                  mov        ecx, dword ptr [ebp - 0xa4]
0046db95  8d 54 08 e0                        lea        edx, [eax + ecx - 0x20]
0046db99  52                                 push       edx
0046db9a  8b 8d 58 ff ff ff                  mov        ecx, dword ptr [ebp - 0xa8]
0046dba0  e8 2b 33 00 00                     call       0x470ed0
0046dba5  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046dba8  83 c1 10                           add        ecx, 0x10
0046dbab  e8 20 a1 fc ff                     call       0x437cd0
0046dbb0  f3 0f 10 05 d8 c8 56 00            movss      xmm0, dword ptr [0x56c8d8]
0046dbb8  f3 0f 5c 05 18 88 5b 00            subss      xmm0, dword ptr [0x5b8818]
0046dbc0  f3 0f 10 48 4c                     movss      xmm1, dword ptr [eax + 0x4c]
0046dbc5  f3 0f 5e c8                        divss      xmm1, xmm0
0046dbc9  f3 0f 58 4d 94                     addss      xmm1, dword ptr [ebp - 0x6c]
0046dbce  f3 0f 11 4d 94                     movss      dword ptr [ebp - 0x6c], xmm1
0046dbd3  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046dbd6  83 c0 01                           add        eax, 1
0046dbd9  89 45 c4                           mov        dword ptr [ebp - 0x3c], eax
0046dbdc  eb 95                              jmp        0x46db73
0046dbde  f3 0f 10 45 94                     movss      xmm0, dword ptr [ebp - 0x6c]
0046dbe3  0f 57 05 d0 d7 56 00               xorps      xmm0, xmmword ptr [0x56d7d0]
0046dbea  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046dbed  f3 0f 59 81 14 01 00 00            mulss      xmm0, dword ptr [ecx + 0x114]
0046dbf5  f3 0f 11 45 f0                     movss      dword ptr [ebp - 0x10], xmm0
0046dbfa  e9 02 02 00 00                     jmp        0x46de01
0046dbff  c7 85 90 fe ff ff c8 02 00 00      mov        dword ptr [ebp - 0x170], 0x2c8
0046dc09  0f 57 c0                           xorps      xmm0, xmm0
0046dc0c  f3 0f 11 45 90                     movss      dword ptr [ebp - 0x70], xmm0
0046dc11  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046dc14  89 55 c4                           mov        dword ptr [ebp - 0x3c], edx
0046dc17  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046dc1a  0f b6 08                           movzx      ecx, byte ptr [eax]
0046dc1d  85 c9                              test       ecx, ecx
0046dc1f  0f 84 99 00 00 00                  je         0x46dcbe
0046dc25  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046dc28  0f b6 02                           movzx      eax, byte ptr [edx]
0046dc2b  83 f8 20                           cmp        eax, 0x20
0046dc2e  7c 26                              jl         0x46dc56
0046dc30  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046dc33  83 c1 10                           add        ecx, 0x10
0046dc36  89 8d 54 ff ff ff                  mov        dword ptr [ebp - 0xac], ecx
0046dc3c  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046dc3f  0f b6 02                           movzx      eax, byte ptr [edx]
0046dc42  05 a8 02 00 00                     add        eax, 0x2a8
0046dc47  50                                 push       eax
0046dc48  8b 8d 54 ff ff ff                  mov        ecx, dword ptr [ebp - 0xac]
0046dc4e  e8 7d 32 00 00                     call       0x470ed0
0046dc53  90                                 nop        
0046dc54  eb 2c                              jmp        0x46dc82
0046dc56  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046dc59  83 c1 10                           add        ecx, 0x10
0046dc5c  89 8d 50 ff ff ff                  mov        dword ptr [ebp - 0xb0], ecx
0046dc62  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046dc65  0f b6 02                           movzx      eax, byte ptr [edx]
0046dc68  b9 20 00 00 00                     mov        ecx, 0x20
0046dc6d  2b c8                              sub        ecx, eax
0046dc6f  81 c1 29 03 00 00                  add        ecx, 0x329
0046dc75  51                                 push       ecx
0046dc76  8b 8d 50 ff ff ff                  mov        ecx, dword ptr [ebp - 0xb0]
0046dc7c  e8 4f 32 00 00                     call       0x470ed0
0046dc81  90                                 nop        
0046dc82  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046dc85  83 c1 10                           add        ecx, 0x10
0046dc88  e8 43 a0 fc ff                     call       0x437cd0
0046dc8d  f3 0f 10 05 d8 c8 56 00            movss      xmm0, dword ptr [0x56c8d8]
0046dc95  f3 0f 5c 05 18 88 5b 00            subss      xmm0, dword ptr [0x5b8818]
0046dc9d  f3 0f 10 48 4c                     movss      xmm1, dword ptr [eax + 0x4c]
0046dca2  f3 0f 5e c8                        divss      xmm1, xmm0
0046dca6  f3 0f 58 4d 90                     addss      xmm1, dword ptr [ebp - 0x70]
0046dcab  f3 0f 11 4d 90                     movss      dword ptr [ebp - 0x70], xmm1
0046dcb0  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046dcb3  83 c2 01                           add        edx, 1
0046dcb6  89 55 c4                           mov        dword ptr [ebp - 0x3c], edx
0046dcb9  e9 59 ff ff ff                     jmp        0x46dc17
0046dcbe  f3 0f 10 45 90                     movss      xmm0, dword ptr [ebp - 0x70]
0046dcc3  0f 57 05 d0 d7 56 00               xorps      xmm0, xmmword ptr [0x56d7d0]
0046dcca  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046dccd  f3 0f 59 80 14 01 00 00            mulss      xmm0, dword ptr [eax + 0x114]
0046dcd5  f3 0f 11 45 f0                     movss      dword ptr [ebp - 0x10], xmm0
0046dcda  e9 22 01 00 00                     jmp        0x46de01
0046dcdf  0f 57 c0                           xorps      xmm0, xmm0
0046dce2  f3 0f 11 45 ac                     movss      dword ptr [ebp - 0x54], xmm0
0046dce7  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046dcea  89 4d c4                           mov        dword ptr [ebp - 0x3c], ecx
0046dced  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046dcf0  0f b6 02                           movzx      eax, byte ptr [edx]
0046dcf3  85 c0                              test       eax, eax
0046dcf5  74 5b                              je         0x46dd52
0046dcf7  8b 4d c4                           mov        ecx, dword ptr [ebp - 0x3c]
0046dcfa  0f b6 11                           movzx      edx, byte ptr [ecx]
0046dcfd  83 fa 2e                           cmp        edx, 0x2e
0046dd00  75 27                              jne        0x46dd29
0046dd02  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046dd05  f3 0f 10 05 c0 f7 56 00            movss      xmm0, dword ptr [0x56f7c0]
0046dd0d  f3 0f 59 80 14 01 00 00            mulss      xmm0, dword ptr [eax + 0x114]
0046dd15  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046dd1d  f3 0f 58 45 ac                     addss      xmm0, dword ptr [ebp - 0x54]
0046dd22  f3 0f 11 45 ac                     movss      dword ptr [ebp - 0x54], xmm0
0046dd27  eb 1e                              jmp        0x46dd47
0046dd29  f3 0f 10 45 c0                     movss      xmm0, dword ptr [ebp - 0x40]
0046dd2e  0f 57 05 d0 d7 56 00               xorps      xmm0, xmmword ptr [0x56d7d0]
0046dd35  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046dd3d  f3 0f 58 45 ac                     addss      xmm0, dword ptr [ebp - 0x54]
0046dd42  f3 0f 11 45 ac                     movss      dword ptr [ebp - 0x54], xmm0
0046dd47  8b 4d c4                           mov        ecx, dword ptr [ebp - 0x3c]
0046dd4a  83 c1 01                           add        ecx, 1
0046dd4d  89 4d c4                           mov        dword ptr [ebp - 0x3c], ecx
0046dd50  eb 9b                              jmp        0x46dced
0046dd52  f3 0f 10 45 ac                     movss      xmm0, dword ptr [ebp - 0x54]
0046dd57  f3 0f 11 45 f0                     movss      dword ptr [ebp - 0x10], xmm0
0046dd5c  e9 a0 00 00 00                     jmp        0x46de01
0046dd61  0f 57 c0                           xorps      xmm0, xmm0
0046dd64  f3 0f 11 45 a8                     movss      dword ptr [ebp - 0x58], xmm0
0046dd69  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046dd6c  89 55 c4                           mov        dword ptr [ebp - 0x3c], edx
0046dd6f  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046dd72  0f b6 08                           movzx      ecx, byte ptr [eax]
0046dd75  85 c9                              test       ecx, ecx
0046dd77  74 5b                              je         0x46ddd4
0046dd79  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046dd7c  0f b6 02                           movzx      eax, byte ptr [edx]
0046dd7f  83 f8 2c                           cmp        eax, 0x2c
0046dd82  75 27                              jne        0x46ddab
0046dd84  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046dd87  f3 0f 10 05 c0 f7 56 00            movss      xmm0, dword ptr [0x56f7c0]
0046dd8f  f3 0f 59 81 14 01 00 00            mulss      xmm0, dword ptr [ecx + 0x114]
0046dd97  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046dd9f  f3 0f 58 45 a8                     addss      xmm0, dword ptr [ebp - 0x58]
0046dda4  f3 0f 11 45 a8                     movss      dword ptr [ebp - 0x58], xmm0
0046dda9  eb 1e                              jmp        0x46ddc9
0046ddab  f3 0f 10 45 c0                     movss      xmm0, dword ptr [ebp - 0x40]
0046ddb0  0f 57 05 d0 d7 56 00               xorps      xmm0, xmmword ptr [0x56d7d0]
0046ddb7  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046ddbf  f3 0f 58 45 a8                     addss      xmm0, dword ptr [ebp - 0x58]
0046ddc4  f3 0f 11 45 a8                     movss      dword ptr [ebp - 0x58], xmm0
0046ddc9  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046ddcc  83 c2 01                           add        edx, 1
0046ddcf  89 55 c4                           mov        dword ptr [ebp - 0x3c], edx
0046ddd2  eb 9b                              jmp        0x46dd6f
0046ddd4  f3 0f 10 45 a8                     movss      xmm0, dword ptr [ebp - 0x58]
0046ddd9  f3 0f 11 45 f0                     movss      dword ptr [ebp - 0x10], xmm0
0046ddde  eb 21                              jmp        0x46de01
0046dde0  f3 0f 2a 85 7c ff ff ff            cvtsi2ss   xmm0, dword ptr [ebp - 0x84]
0046dde8  0f 57 05 d0 d7 56 00               xorps      xmm0, xmmword ptr [0x56d7d0]
0046ddef  f3 0f 59 45 c0                     mulss      xmm0, dword ptr [ebp - 0x40]
0046ddf4  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046ddfc  f3 0f 11 45 f0                     movss      dword ptr [ebp - 0x10], xmm0
0046de01  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046de04  8b 88 3c 01 00 00                  mov        ecx, dword ptr [eax + 0x13c]
0046de0a  89 8d 78 ff ff ff                  mov        dword ptr [ebp - 0x88], ecx
0046de10  83 bd 78 ff ff ff 00               cmp        dword ptr [ebp - 0x88], 0
0046de17  74 0b                              je         0x46de24
0046de19  83 bd 78 ff ff ff 02               cmp        dword ptr [ebp - 0x88], 2
0046de20  74 25                              je         0x46de47
0046de22  eb 3c                              jmp        0x46de60
0046de24  f3 0f 10 45 bc                     movss      xmm0, dword ptr [ebp - 0x44]
0046de29  0f 57 05 d0 d7 56 00               xorps      xmm0, xmmword ptr [0x56d7d0]
0046de30  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
0046de38  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046de40  f3 0f 11 45 f4                     movss      dword ptr [ebp - 0xc], xmm0
0046de45  eb 19                              jmp        0x46de60
0046de47  f3 0f 10 45 bc                     movss      xmm0, dword ptr [ebp - 0x44]
0046de4c  0f 57 05 d0 d7 56 00               xorps      xmm0, xmmword ptr [0x56d7d0]
0046de53  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046de5b  f3 0f 11 45 f4                     movss      dword ptr [ebp - 0xc], xmm0
0046de60  8d 4d cc                           lea        ecx, [ebp - 0x34]
0046de63  e8 a8 4f fb ff                     call       0x422e10
0046de68  8d 4d e4                           lea        ecx, [ebp - 0x1c]
0046de6b  e8 a0 4f fb ff                     call       0x422e10
0046de70  8d 4d d8                           lea        ecx, [ebp - 0x28]
0046de73  e8 98 4f fb ff                     call       0x422e10
0046de78  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0046de7b  89 55 cc                           mov        dword ptr [ebp - 0x34], edx
0046de7e  8b 45 f4                           mov        eax, dword ptr [ebp - 0xc]
0046de81  89 45 d0                           mov        dword ptr [ebp - 0x30], eax
0046de84  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0046de87  89 4d d4                           mov        dword ptr [ebp - 0x2c], ecx
0046de8a  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046de8d  89 55 c4                           mov        dword ptr [ebp - 0x3c], edx
0046de90  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046de93  0f b6 08                           movzx      ecx, byte ptr [eax]
0046de96  85 c9                              test       ecx, ecx
0046de98  0f 84 59 0a 00 00                  je         0x46e8f7
0046de9e  0f 57 c0                           xorps      xmm0, xmm0
0046dea1  f3 0f 11 45 d8                     movss      dword ptr [ebp - 0x28], xmm0
0046dea6  0f 57 c0                           xorps      xmm0, xmm0
0046dea9  f3 0f 11 45 dc                     movss      dword ptr [ebp - 0x24], xmm0
0046deae  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046deb1  0f b6 02                           movzx      eax, byte ptr [edx]
0046deb4  83 f8 0a                           cmp        eax, 0xa
0046deb7  75 31                              jne        0x46deea
0046deb9  f3 0f 10 45 cc                     movss      xmm0, dword ptr [ebp - 0x34]
0046debe  f3 0f 11 45 f0                     movss      dword ptr [ebp - 0x10], xmm0
0046dec3  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046dec6  f3 0f 10 45 bc                     movss      xmm0, dword ptr [ebp - 0x44]
0046decb  f3 0f 59 81 18 01 00 00            mulss      xmm0, dword ptr [ecx + 0x118]
0046ded3  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046dedb  f3 0f 58 45 f4                     addss      xmm0, dword ptr [ebp - 0xc]
0046dee0  f3 0f 11 45 f4                     movss      dword ptr [ebp - 0xc], xmm0
0046dee5  e9 ff 09 00 00                     jmp        0x46e8e9
0046deea  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046deed  0f b6 02                           movzx      eax, byte ptr [edx]
0046def0  83 f8 20                           cmp        eax, 0x20
0046def3  75 50                              jne        0x46df45
0046def5  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046def8  83 b9 28 01 00 00 06               cmp        dword ptr [ecx + 0x128], 6
0046deff  74 24                              je         0x46df25
0046df01  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046df04  83 ba 28 01 00 00 07               cmp        dword ptr [edx + 0x128], 7
0046df0b  74 18                              je         0x46df25
0046df0d  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046df10  83 b8 28 01 00 00 08               cmp        dword ptr [eax + 0x128], 8
0046df17  74 0c                              je         0x46df25
0046df19  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046df1c  83 b9 28 01 00 00 09               cmp        dword ptr [ecx + 0x128], 9
0046df23  75 04                              jne        0x46df29
0046df25  eb 1e                              jmp        0x46df45
0046df29  f3 0f 10 45 c0                     movss      xmm0, dword ptr [ebp - 0x40]
0046df2e  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046df36  f3 0f 58 45 f0                     addss      xmm0, dword ptr [ebp - 0x10]
0046df3b  f3 0f 11 45 f0                     movss      dword ptr [ebp - 0x10], xmm0
0046df40  e9 a4 09 00 00                     jmp        0x46e8e9
0046df45  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046df48  8b 82 28 01 00 00                  mov        eax, dword ptr [edx + 0x128]
0046df4e  89 85 74 ff ff ff                  mov        dword ptr [ebp - 0x8c], eax
0046df54  83 bd 74 ff ff ff 0d               cmp        dword ptr [ebp - 0x8c], 0xd
0046df5b  0f 87 9d 05 00 00                  ja         0x46e4fe
0046df61  8b 8d 74 ff ff ff                  mov        ecx, dword ptr [ebp - 0x8c]
0046df67  ff 24 8d a0 e9 46 00               jmp        dword ptr [ecx*4 + 0x46e9a0]
0046df6e  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046df71  83 c2 10                           add        edx, 0x10
0046df74  89 95 4c ff ff ff                  mov        dword ptr [ebp - 0xb4], edx
0046df7a  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046df7d  0f b6 08                           movzx      ecx, byte ptr [eax]
0046df80  83 e9 20                           sub        ecx, 0x20
0046df83  51                                 push       ecx
0046df84  8b 8d 4c ff ff ff                  mov        ecx, dword ptr [ebp - 0xb4]
0046df8a  e8 41 2f 00 00                     call       0x470ed0
0046df8f  90                                 nop        
0046df90  e9 69 05 00 00                     jmp        0x46e4fe
0046df95  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046df98  83 c2 10                           add        edx, 0x10
0046df9b  89 95 48 ff ff ff                  mov        dword ptr [ebp - 0xb8], edx
0046dfa1  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046dfa4  0f b6 08                           movzx      ecx, byte ptr [eax]
0046dfa7  83 c1 42                           add        ecx, 0x42
0046dfaa  51                                 push       ecx
0046dfab  8b 8d 48 ff ff ff                  mov        ecx, dword ptr [ebp - 0xb8]
0046dfb1  e8 1a 2f 00 00                     call       0x470ed0
0046dfb6  90                                 nop        
0046dfb7  e9 42 05 00 00                     jmp        0x46e4fe
0046dfbc  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046dfbf  83 c2 10                           add        edx, 0x10
0046dfc2  89 95 44 ff ff ff                  mov        dword ptr [ebp - 0xbc], edx
0046dfc8  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046dfcb  0f b6 08                           movzx      ecx, byte ptr [eax]
0046dfce  81 c1 20 01 00 00                  add        ecx, 0x120
0046dfd4  51                                 push       ecx
0046dfd5  8b 8d 44 ff ff ff                  mov        ecx, dword ptr [ebp - 0xbc]
0046dfdb  e8 f0 2e 00 00                     call       0x470ed0
0046dfe0  90                                 nop        
0046dfe1  e9 18 05 00 00                     jmp        0x46e4fe
0046dfe6  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046dfe9  83 c2 10                           add        edx, 0x10
0046dfec  89 95 40 ff ff ff                  mov        dword ptr [ebp - 0xc0], edx
0046dff2  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046dff5  0f b6 08                           movzx      ecx, byte ptr [eax]
0046dff8  81 c1 e4 01 00 00                  add        ecx, 0x1e4
0046dffe  51                                 push       ecx
0046dfff  8b 8d 40 ff ff ff                  mov        ecx, dword ptr [ebp - 0xc0]
0046e005  e8 c6 2e 00 00                     call       0x470ed0
0046e00a  90                                 nop        
0046e00b  e9 ee 04 00 00                     jmp        0x46e4fe
0046e010  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046e013  83 c2 10                           add        edx, 0x10
0046e016  89 95 3c ff ff ff                  mov        dword ptr [ebp - 0xc4], edx
0046e01c  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046e01f  0f b6 08                           movzx      ecx, byte ptr [eax]
0046e022  81 c1 82 01 00 00                  add        ecx, 0x182
0046e028  51                                 push       ecx
0046e029  8b 8d 3c ff ff ff                  mov        ecx, dword ptr [ebp - 0xc4]
0046e02f  e8 9c 2e 00 00                     call       0x470ed0
0046e034  90                                 nop        
0046e035  e9 c4 04 00 00                     jmp        0x46e4fe
0046e03a  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046e03d  83 c2 10                           add        edx, 0x10
0046e040  89 95 38 ff ff ff                  mov        dword ptr [ebp - 0xc8], edx
0046e046  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046e049  0f b6 08                           movzx      ecx, byte ptr [eax]
0046e04c  81 c1 46 02 00 00                  add        ecx, 0x246
0046e052  51                                 push       ecx
0046e053  8b 8d 38 ff ff ff                  mov        ecx, dword ptr [ebp - 0xc8]
0046e059  e8 72 2e 00 00                     call       0x470ed0
0046e05e  90                                 nop        
0046e05f  e9 9a 04 00 00                     jmp        0x46e4fe
0046e064  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046e067  0f b6 02                           movzx      eax, byte ptr [edx]
0046e06a  83 f8 20                           cmp        eax, 0x20
0046e06d  7c 26                              jl         0x46e095
0046e06f  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e072  83 c1 10                           add        ecx, 0x10
0046e075  89 8d 34 ff ff ff                  mov        dword ptr [ebp - 0xcc], ecx
0046e07b  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046e07e  0f b6 02                           movzx      eax, byte ptr [edx]
0046e081  05 a8 02 00 00                     add        eax, 0x2a8
0046e086  50                                 push       eax
0046e087  8b 8d 34 ff ff ff                  mov        ecx, dword ptr [ebp - 0xcc]
0046e08d  e8 3e 2e 00 00                     call       0x470ed0
0046e092  90                                 nop        
0046e093  eb 2c                              jmp        0x46e0c1
0046e095  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e098  83 c1 10                           add        ecx, 0x10
0046e09b  89 8d 30 ff ff ff                  mov        dword ptr [ebp - 0xd0], ecx
0046e0a1  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046e0a4  0f b6 02                           movzx      eax, byte ptr [edx]
0046e0a7  b9 20 00 00 00                     mov        ecx, 0x20
0046e0ac  2b c8                              sub        ecx, eax
0046e0ae  81 c1 29 03 00 00                  add        ecx, 0x329
0046e0b4  51                                 push       ecx
0046e0b5  8b 8d 30 ff ff ff                  mov        ecx, dword ptr [ebp - 0xd0]
0046e0bb  e8 10 2e 00 00                     call       0x470ed0
0046e0c0  90                                 nop        
0046e0c1  e9 38 04 00 00                     jmp        0x46e4fe
0046e0c6  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046e0c9  0f b6 02                           movzx      eax, byte ptr [edx]
0046e0cc  83 f8 20                           cmp        eax, 0x20
0046e0cf  7c 26                              jl         0x46e0f7
0046e0d1  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e0d4  83 c1 10                           add        ecx, 0x10
0046e0d7  89 8d 2c ff ff ff                  mov        dword ptr [ebp - 0xd4], ecx
0046e0dd  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046e0e0  0f b6 02                           movzx      eax, byte ptr [edx]
0046e0e3  05 0c 03 00 00                     add        eax, 0x30c
0046e0e8  50                                 push       eax
0046e0e9  8b 8d 2c ff ff ff                  mov        ecx, dword ptr [ebp - 0xd4]
0046e0ef  e8 dc 2d 00 00                     call       0x470ed0
0046e0f4  90                                 nop        
0046e0f5  eb 2c                              jmp        0x46e123
0046e0f7  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e0fa  83 c1 10                           add        ecx, 0x10
0046e0fd  89 8d 28 ff ff ff                  mov        dword ptr [ebp - 0xd8], ecx
0046e103  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046e106  0f b6 02                           movzx      eax, byte ptr [edx]
0046e109  b9 20 00 00 00                     mov        ecx, 0x20
0046e10e  2b c8                              sub        ecx, eax
0046e110  81 c1 8d 03 00 00                  add        ecx, 0x38d
0046e116  51                                 push       ecx
0046e117  8b 8d 28 ff ff ff                  mov        ecx, dword ptr [ebp - 0xd8]
0046e11d  e8 ae 2d 00 00                     call       0x470ed0
0046e122  90                                 nop        
0046e123  e9 d6 03 00 00                     jmp        0x46e4fe
0046e128  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046e12b  f3 0f 10 05 a0 f7 56 00            movss      xmm0, dword ptr [0x56f7a0]
0046e133  f3 0f 59 82 14 01 00 00            mulss      xmm0, dword ptr [edx + 0x114]
0046e13b  f3 0f 11 45 c0                     movss      dword ptr [ebp - 0x40], xmm0
0046e140  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046e143  0f b6 08                           movzx      ecx, byte ptr [eax]
0046e146  83 f9 61                           cmp        ecx, 0x61
0046e149  7c 32                              jl         0x46e17d
0046e14b  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046e14e  0f b6 02                           movzx      eax, byte ptr [edx]
0046e151  83 f8 7a                           cmp        eax, 0x7a
0046e154  7f 27                              jg         0x46e17d
0046e156  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e159  83 c1 10                           add        ecx, 0x10
0046e15c  89 8d 24 ff ff ff                  mov        dword ptr [ebp - 0xdc], ecx
0046e162  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046e165  0f b6 02                           movzx      eax, byte ptr [edx]
0046e168  83 c0 74                           add        eax, 0x74
0046e16b  50                                 push       eax
0046e16c  8b 8d 24 ff ff ff                  mov        ecx, dword ptr [ebp - 0xdc]
0046e172  e8 59 2d 00 00                     call       0x470ed0
0046e177  90                                 nop        
0046e178  e9 de 01 00 00                     jmp        0x46e35b
0046e17d  8b 4d c4                           mov        ecx, dword ptr [ebp - 0x3c]
0046e180  0f b6 11                           movzx      edx, byte ptr [ecx]
0046e183  83 fa 41                           cmp        edx, 0x41
0046e186  7c 35                              jl         0x46e1bd
0046e188  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046e18b  0f b6 08                           movzx      ecx, byte ptr [eax]
0046e18e  83 f9 5a                           cmp        ecx, 0x5a
0046e191  7f 2a                              jg         0x46e1bd
0046e193  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046e196  83 c2 10                           add        edx, 0x10
0046e199  89 95 20 ff ff ff                  mov        dword ptr [ebp - 0xe0], edx
0046e19f  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046e1a2  0f b6 08                           movzx      ecx, byte ptr [eax]
0046e1a5  81 c1 94 00 00 00                  add        ecx, 0x94
0046e1ab  51                                 push       ecx
0046e1ac  8b 8d 20 ff ff ff                  mov        ecx, dword ptr [ebp - 0xe0]
0046e1b2  e8 19 2d 00 00                     call       0x470ed0
0046e1b7  90                                 nop        
0046e1b8  e9 9e 01 00 00                     jmp        0x46e35b
0046e1bd  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046e1c0  0f b6 02                           movzx      eax, byte ptr [edx]
0046e1c3  83 f8 2f                           cmp        eax, 0x2f
0046e1c6  75 22                              jne        0x46e1ea
0046e1c8  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e1cb  83 c1 10                           add        ecx, 0x10
0046e1ce  89 8d 1c ff ff ff                  mov        dword ptr [ebp - 0xe4], ecx
0046e1d4  68 ce 00 00 00                     push       0xce
0046e1d9  8b 8d 1c ff ff ff                  mov        ecx, dword ptr [ebp - 0xe4]
0046e1df  e8 ec 2c 00 00                     call       0x470ed0
0046e1e4  90                                 nop        
0046e1e5  e9 71 01 00 00                     jmp        0x46e35b
0046e1ea  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046e1ed  0f b6 02                           movzx      eax, byte ptr [edx]
0046e1f0  83 f8 3a                           cmp        eax, 0x3a
0046e1f3  75 22                              jne        0x46e217
0046e1f5  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e1f8  83 c1 10                           add        ecx, 0x10
0046e1fb  89 8d 18 ff ff ff                  mov        dword ptr [ebp - 0xe8], ecx
0046e201  68 cf 00 00 00                     push       0xcf
0046e206  8b 8d 18 ff ff ff                  mov        ecx, dword ptr [ebp - 0xe8]
0046e20c  e8 bf 2c 00 00                     call       0x470ed0
0046e211  90                                 nop        
0046e212  e9 44 01 00 00                     jmp        0x46e35b
0046e217  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046e21a  0f b6 02                           movzx      eax, byte ptr [edx]
0046e21d  83 f8 2d                           cmp        eax, 0x2d
0046e220  75 22                              jne        0x46e244
0046e222  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e225  83 c1 10                           add        ecx, 0x10
0046e228  89 8d 14 ff ff ff                  mov        dword ptr [ebp - 0xec], ecx
0046e22e  68 d0 00 00 00                     push       0xd0
0046e233  8b 8d 14 ff ff ff                  mov        ecx, dword ptr [ebp - 0xec]
0046e239  e8 92 2c 00 00                     call       0x470ed0
0046e23e  90                                 nop        
0046e23f  e9 17 01 00 00                     jmp        0x46e35b
0046e244  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046e247  0f b6 02                           movzx      eax, byte ptr [edx]
0046e24a  83 f8 2a                           cmp        eax, 0x2a
0046e24d  75 22                              jne        0x46e271
0046e24f  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e252  83 c1 10                           add        ecx, 0x10
0046e255  89 8d 10 ff ff ff                  mov        dword ptr [ebp - 0xf0], ecx
0046e25b  68 d1 00 00 00                     push       0xd1
0046e260  8b 8d 10 ff ff ff                  mov        ecx, dword ptr [ebp - 0xf0]
0046e266  e8 65 2c 00 00                     call       0x470ed0
0046e26b  90                                 nop        
0046e26c  e9 ea 00 00 00                     jmp        0x46e35b
0046e271  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046e274  0f b6 02                           movzx      eax, byte ptr [edx]
0046e277  83 f8 25                           cmp        eax, 0x25
0046e27a  75 22                              jne        0x46e29e
0046e27c  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e27f  83 c1 10                           add        ecx, 0x10
0046e282  89 8d 0c ff ff ff                  mov        dword ptr [ebp - 0xf4], ecx
0046e288  68 d2 00 00 00                     push       0xd2
0046e28d  8b 8d 0c ff ff ff                  mov        ecx, dword ptr [ebp - 0xf4]
0046e293  e8 38 2c 00 00                     call       0x470ed0
0046e298  90                                 nop        
0046e299  e9 bd 00 00 00                     jmp        0x46e35b
0046e29e  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046e2a1  0f b6 02                           movzx      eax, byte ptr [edx]
0046e2a4  83 f8 24                           cmp        eax, 0x24
0046e2a7  75 22                              jne        0x46e2cb
0046e2a9  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e2ac  83 c1 10                           add        ecx, 0x10
0046e2af  89 8d 08 ff ff ff                  mov        dword ptr [ebp - 0xf8], ecx
0046e2b5  68 1f 01 00 00                     push       0x11f
0046e2ba  8b 8d 08 ff ff ff                  mov        ecx, dword ptr [ebp - 0xf8]
0046e2c0  e8 0b 2c 00 00                     call       0x470ed0
0046e2c5  90                                 nop        
0046e2c6  e9 90 00 00 00                     jmp        0x46e35b
0046e2cb  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046e2ce  0f b6 02                           movzx      eax, byte ptr [edx]
0046e2d1  83 f8 2e                           cmp        eax, 0x2e
0046e2d4  75 36                              jne        0x46e30c
0046e2d6  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e2d9  83 c1 10                           add        ecx, 0x10
0046e2dc  89 8d 04 ff ff ff                  mov        dword ptr [ebp - 0xfc], ecx
0046e2e2  68 d3 00 00 00                     push       0xd3
0046e2e7  8b 8d 04 ff ff ff                  mov        ecx, dword ptr [ebp - 0xfc]
0046e2ed  e8 de 2b 00 00                     call       0x470ed0
0046e2f2  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046e2f5  f3 0f 10 05 e0 c8 56 00            movss      xmm0, dword ptr [0x56c8e0]
0046e2fd  f3 0f 59 82 14 01 00 00            mulss      xmm0, dword ptr [edx + 0x114]
0046e305  f3 0f 11 45 c0                     movss      dword ptr [ebp - 0x40], xmm0
0046e30a  eb 4f                              jmp        0x46e35b
0046e30c  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046e30f  0f b6 08                           movzx      ecx, byte ptr [eax]
0046e312  83 f9 2b                           cmp        ecx, 0x2b
0046e315  75 1f                              jne        0x46e336
0046e317  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046e31a  83 c2 10                           add        edx, 0x10
0046e31d  89 95 00 ff ff ff                  mov        dword ptr [ebp - 0x100], edx
0046e323  68 d4 00 00 00                     push       0xd4
0046e328  8b 8d 00 ff ff ff                  mov        ecx, dword ptr [ebp - 0x100]
0046e32e  e8 9d 2b 00 00                     call       0x470ed0
0046e333  90                                 nop        
0046e334  eb 25                              jmp        0x46e35b
0046e336  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046e339  83 c0 10                           add        eax, 0x10
0046e33c  89 85 fc fe ff ff                  mov        dword ptr [ebp - 0x104], eax
0046e342  8b 4d c4                           mov        ecx, dword ptr [ebp - 0x3c]
0046e345  0f b6 11                           movzx      edx, byte ptr [ecx]
0046e348  81 c2 94 00 00 00                  add        edx, 0x94
0046e34e  52                                 push       edx
0046e34f  8b 8d fc fe ff ff                  mov        ecx, dword ptr [ebp - 0x104]
0046e355  e8 76 2b 00 00                     call       0x470ed0
0046e35a  90                                 nop        
0046e35b  e9 9e 01 00 00                     jmp        0x46e4fe
0046e360  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046e363  83 b8 28 01 00 00 0a               cmp        dword ptr [eax + 0x128], 0xa
0046e36a  75 0c                              jne        0x46e378
0046e36c  c7 85 6c ff ff ff 0d 01 00 00      mov        dword ptr [ebp - 0x94], 0x10d
0046e376  eb 2e                              jmp        0x46e3a6
0046e378  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046e37b  83 b9 28 01 00 00 0b               cmp        dword ptr [ecx + 0x128], 0xb
0046e382  75 0c                              jne        0x46e390
0046e384  c7 85 70 ff ff ff fe 00 00 00      mov        dword ptr [ebp - 0x90], 0xfe
0046e38e  eb 0a                              jmp        0x46e39a
0046e390  c7 85 70 ff ff ff ef 00 00 00      mov        dword ptr [ebp - 0x90], 0xef
0046e39a  8b 95 70 ff ff ff                  mov        edx, dword ptr [ebp - 0x90]
0046e3a0  89 95 6c ff ff ff                  mov        dword ptr [ebp - 0x94], edx
0046e3a6  8b 85 6c ff ff ff                  mov        eax, dword ptr [ebp - 0x94]
0046e3ac  89 45 b8                           mov        dword ptr [ebp - 0x48], eax
0046e3af  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046e3b2  f3 0f 10 05 a8 f7 56 00            movss      xmm0, dword ptr [0x56f7a8]
0046e3ba  f3 0f 59 81 14 01 00 00            mulss      xmm0, dword ptr [ecx + 0x114]
0046e3c2  f3 0f 11 45 c0                     movss      dword ptr [ebp - 0x40], xmm0
0046e3c7  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046e3ca  0f b6 02                           movzx      eax, byte ptr [edx]
0046e3cd  83 f8 2f                           cmp        eax, 0x2f
0046e3d0  75 24                              jne        0x46e3f6
0046e3d2  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e3d5  83 c1 10                           add        ecx, 0x10
0046e3d8  89 8d f8 fe ff ff                  mov        dword ptr [ebp - 0x108], ecx
0046e3de  8b 55 b8                           mov        edx, dword ptr [ebp - 0x48]
0046e3e1  83 c2 0a                           add        edx, 0xa
0046e3e4  52                                 push       edx
0046e3e5  8b 8d f8 fe ff ff                  mov        ecx, dword ptr [ebp - 0x108]
0046e3eb  e8 e0 2a 00 00                     call       0x470ed0
0046e3f0  90                                 nop        
0046e3f1  e9 08 01 00 00                     jmp        0x46e4fe
0046e3f6  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046e3f9  0f b6 08                           movzx      ecx, byte ptr [eax]
0046e3fc  83 f9 2e                           cmp        ecx, 0x2e
0046e3ff  75 24                              jne        0x46e425
0046e401  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046e404  83 c2 10                           add        edx, 0x10
0046e407  89 95 f4 fe ff ff                  mov        dword ptr [ebp - 0x10c], edx
0046e40d  8b 45 b8                           mov        eax, dword ptr [ebp - 0x48]
0046e410  83 c0 0b                           add        eax, 0xb
0046e413  50                                 push       eax
0046e414  8b 8d f4 fe ff ff                  mov        ecx, dword ptr [ebp - 0x10c]
0046e41a  e8 b1 2a 00 00                     call       0x470ed0
0046e41f  90                                 nop        
0046e420  e9 d9 00 00 00                     jmp        0x46e4fe
0046e425  8b 4d c4                           mov        ecx, dword ptr [ebp - 0x3c]
0046e428  0f b6 11                           movzx      edx, byte ptr [ecx]
0046e42b  83 fa 73                           cmp        edx, 0x73
0046e42e  75 24                              jne        0x46e454
0046e430  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046e433  83 c0 10                           add        eax, 0x10
0046e436  89 85 f0 fe ff ff                  mov        dword ptr [ebp - 0x110], eax
0046e43c  8b 4d b8                           mov        ecx, dword ptr [ebp - 0x48]
0046e43f  83 c1 0c                           add        ecx, 0xc
0046e442  51                                 push       ecx
0046e443  8b 8d f0 fe ff ff                  mov        ecx, dword ptr [ebp - 0x110]
0046e449  e8 82 2a 00 00                     call       0x470ed0
0046e44e  90                                 nop        
0046e44f  e9 aa 00 00 00                     jmp        0x46e4fe
0046e454  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046e457  0f b6 02                           movzx      eax, byte ptr [edx]
0046e45a  83 f8 2a                           cmp        eax, 0x2a
0046e45d  75 21                              jne        0x46e480
0046e45f  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e462  83 c1 10                           add        ecx, 0x10
0046e465  89 8d ec fe ff ff                  mov        dword ptr [ebp - 0x114], ecx
0046e46b  8b 55 b8                           mov        edx, dword ptr [ebp - 0x48]
0046e46e  83 c2 0d                           add        edx, 0xd
0046e471  52                                 push       edx
0046e472  8b 8d ec fe ff ff                  mov        ecx, dword ptr [ebp - 0x114]
0046e478  e8 53 2a 00 00                     call       0x470ed0
0046e47d  90                                 nop        
0046e47e  eb 7e                              jmp        0x46e4fe
0046e480  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046e483  0f b6 08                           movzx      ecx, byte ptr [eax]
0046e486  83 f9 2c                           cmp        ecx, 0x2c
0046e489  75 4d                              jne        0x46e4d8
0046e48b  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046e48e  83 c2 10                           add        edx, 0x10
0046e491  89 95 e8 fe ff ff                  mov        dword ptr [ebp - 0x118], edx
0046e497  8b 45 b8                           mov        eax, dword ptr [ebp - 0x48]
0046e49a  83 c0 0e                           add        eax, 0xe
0046e49d  50                                 push       eax
0046e49e  8b 8d e8 fe ff ff                  mov        ecx, dword ptr [ebp - 0x118]
0046e4a4  e8 27 2a 00 00                     call       0x470ed0
0046e4a9  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046e4ac  f3 0f 10 05 e0 c8 56 00            movss      xmm0, dword ptr [0x56c8e0]
0046e4b4  f3 0f 59 81 14 01 00 00            mulss      xmm0, dword ptr [ecx + 0x114]
0046e4bc  f3 0f 11 45 c0                     movss      dword ptr [ebp - 0x40], xmm0
0046e4c1  f3 0f 10 05 d8 c8 56 00            movss      xmm0, dword ptr [0x56c8d8]
0046e4c9  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046e4d1  f3 0f 11 45 dc                     movss      dword ptr [ebp - 0x24], xmm0
0046e4d6  eb 26                              jmp        0x46e4fe
0046e4d8  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046e4db  83 c2 10                           add        edx, 0x10
0046e4de  89 95 e4 fe ff ff                  mov        dword ptr [ebp - 0x11c], edx
0046e4e4  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046e4e7  0f b6 08                           movzx      ecx, byte ptr [eax]
0046e4ea  8b 55 b8                           mov        edx, dword ptr [ebp - 0x48]
0046e4ed  8d 44 11 d0                        lea        eax, [ecx + edx - 0x30]
0046e4f1  50                                 push       eax
0046e4f2  8b 8d e4 fe ff ff                  mov        ecx, dword ptr [ebp - 0x11c]
0046e4f8  e8 d3 29 00 00                     call       0x470ed0
0046e4fd  90                                 nop        
0046e4fe  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046e501  83 b9 28 01 00 00 06               cmp        dword ptr [ecx + 0x128], 6
0046e508  74 40                              je         0x46e54a
0046e50a  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046e50d  83 ba 28 01 00 00 07               cmp        dword ptr [edx + 0x128], 7
0046e514  74 34                              je         0x46e54a
0046e516  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046e519  83 b8 28 01 00 00 08               cmp        dword ptr [eax + 0x128], 8
0046e520  74 28                              je         0x46e54a
0046e522  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046e525  83 b9 28 01 00 00 09               cmp        dword ptr [ecx + 0x128], 9
0046e52c  74 1c                              je         0x46e54a
0046e52e  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046e531  83 ba 28 01 00 00 0c               cmp        dword ptr [edx + 0x128], 0xc
0046e538  74 10                              je         0x46e54a
0046e53a  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046e53d  83 b8 28 01 00 00 0d               cmp        dword ptr [eax + 0x128], 0xd
0046e544  0f 85 c2 00 00 00                  jne        0x46e60c
0046e54a  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e54d  83 c1 10                           add        ecx, 0x10
0046e550  e8 7b 97 fc ff                     call       0x437cd0
0046e555  f3 0f 10 05 d8 c8 56 00            movss      xmm0, dword ptr [0x56c8d8]
0046e55d  f3 0f 5c 05 18 88 5b 00            subss      xmm0, dword ptr [0x5b8818]
0046e565  f3 0f 10 48 4c                     movss      xmm1, dword ptr [eax + 0x4c]
0046e56a  f3 0f 5e c8                        divss      xmm1, xmm0
0046e56e  f3 0f 5e 0d 18 88 5b 00            divss      xmm1, dword ptr [0x5b8818]
0046e576  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046e579  f3 0f 59 89 14 01 00 00            mulss      xmm1, dword ptr [ecx + 0x114]
0046e581  f3 0f 11 4d c0                     movss      dword ptr [ebp - 0x40], xmm1
0046e586  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046e589  83 c2 10                           add        edx, 0x10
0046e58c  89 95 d8 fe ff ff                  mov        dword ptr [ebp - 0x128], edx
0046e592  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e595  83 c1 10                           add        ecx, 0x10
0046e598  e8 33 97 fc ff                     call       0x437cd0
0046e59d  f3 0f 10 40 48                     movss      xmm0, dword ptr [eax + 0x48]
0046e5a2  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
0046e5aa  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046e5b2  f3 0f 11 85 b8 fe ff ff            movss      dword ptr [ebp - 0x148], xmm0
0046e5ba  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e5bd  83 c1 10                           add        ecx, 0x10
0046e5c0  e8 0b 97 fc ff                     call       0x437cd0
0046e5c5  f3 0f 10 40 4c                     movss      xmm0, dword ptr [eax + 0x4c]
0046e5ca  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
0046e5d2  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046e5da  f3 0f 11 85 dc fe ff ff            movss      dword ptr [ebp - 0x124], xmm0
0046e5e2  51                                 push       ecx
0046e5e3  f3 0f 10 85 b8 fe ff ff            movss      xmm0, dword ptr [ebp - 0x148]
0046e5eb  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
0046e5f0  51                                 push       ecx
0046e5f1  f3 0f 10 85 dc fe ff ff            movss      xmm0, dword ptr [ebp - 0x124]
0046e5f9  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
0046e5fe  8b 8d d8 fe ff ff                  mov        ecx, dword ptr [ebp - 0x128]
0046e604  e8 87 28 00 00                     call       0x470e90
0046e609  90                                 nop        
0046e60a  eb 64                              jmp        0x46e670
0046e60c  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046e60f  83 c0 10                           add        eax, 0x10
0046e612  89 85 cc fe ff ff                  mov        dword ptr [ebp - 0x134], eax
0046e618  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e61b  83 c1 10                           add        ecx, 0x10
0046e61e  e8 ad 96 fc ff                     call       0x437cd0
0046e623  f3 0f 10 40 48                     movss      xmm0, dword ptr [eax + 0x48]
0046e628  f3 0f 11 85 d4 fe ff ff            movss      dword ptr [ebp - 0x12c], xmm0
0046e630  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e633  83 c1 10                           add        ecx, 0x10
0046e636  e8 95 96 fc ff                     call       0x437cd0
0046e63b  f3 0f 10 40 4c                     movss      xmm0, dword ptr [eax + 0x4c]
0046e640  f3 0f 11 85 d0 fe ff ff            movss      dword ptr [ebp - 0x130], xmm0
0046e648  51                                 push       ecx
0046e649  f3 0f 10 85 d4 fe ff ff            movss      xmm0, dword ptr [ebp - 0x12c]
0046e651  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
0046e656  51                                 push       ecx
0046e657  f3 0f 10 85 d0 fe ff ff            movss      xmm0, dword ptr [ebp - 0x130]
0046e65f  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
0046e664  8b 8d cc fe ff ff                  mov        ecx, dword ptr [ebp - 0x134]
0046e66a  e8 21 28 00 00                     call       0x470e90
0046e66f  90                                 nop        
0046e670  8d 4d f0                           lea        ecx, [ebp - 0x10]
0046e673  51                                 push       ecx
0046e674  8d 4d d8                           lea        ecx, [ebp - 0x28]
0046e677  e8 64 b0 fb ff                     call       0x4296e0
0046e67c  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046e67f  51                                 push       ecx
0046e680  f3 0f 10 82 1c 01 00 00            movss      xmm0, dword ptr [edx + 0x11c]
0046e688  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
0046e68d  8d 45 d8                           lea        eax, [ebp - 0x28]
0046e690  50                                 push       eax
0046e691  8d 4d e4                           lea        ecx, [ebp - 0x1c]
0046e694  51                                 push       ecx
0046e695  e8 06 a9 fe ff                     call       0x458fa0
0046e69a  83 c4 0c                           add        esp, 0xc
0046e69d  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046e6a0  81 c2 00 01 00 00                  add        edx, 0x100
0046e6a6  52                                 push       edx
0046e6a7  8d 4d e4                           lea        ecx, [ebp - 0x1c]
0046e6aa  e8 31 b0 fb ff                     call       0x4296e0
0046e6af  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046e6b2  83 c0 10                           add        eax, 0x10
0046e6b5  89 85 c8 fe ff ff                  mov        dword ptr [ebp - 0x138], eax
0046e6bb  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046e6be  51                                 push       ecx
0046e6bf  f3 0f 10 81 1c 01 00 00            movss      xmm0, dword ptr [ecx + 0x11c]
0046e6c7  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
0046e6cc  8b 8d c8 fe ff ff                  mov        ecx, dword ptr [ebp - 0x138]
0046e6d2  e8 b9 1e fe ff                     call       0x450590
0046e6d7  90                                 nop        
0046e6d8  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046e6db  83 ba 2c 01 00 00 00               cmp        dword ptr [edx + 0x12c], 0
0046e6e2  0f 84 44 01 00 00                  je         0x46e82c
0046e6e8  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046e6eb  83 c0 10                           add        eax, 0x10
0046e6ee  89 85 c4 fe ff ff                  mov        dword ptr [ebp - 0x13c], eax
0046e6f4  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046e6f7  8b 91 0c 01 00 00                  mov        edx, dword ptr [ecx + 0x10c]
0046e6fd  81 e2 00 00 00 ff                  and        edx, 0xff000000
0046e703  52                                 push       edx
0046e704  8b 8d c4 fe ff ff                  mov        ecx, dword ptr [ebp - 0x13c]
0046e70a  e8 01 13 fe ff                     call       0x44fa10
0046e70f  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046e712  83 c0 10                           add        eax, 0x10
0046e715  89 85 c0 fe ff ff                  mov        dword ptr [ebp - 0x140], eax
0046e71b  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046e71e  8b 91 0c 01 00 00                  mov        edx, dword ptr [ecx + 0x10c]
0046e724  c1 ea 18                           shr        edx, 0x18
0046e727  d1 ea                              shr        edx, 1
0046e729  52                                 push       edx
0046e72a  8b 8d c0 fe ff ff                  mov        ecx, dword ptr [ebp - 0x140]
0046e730  e8 2b 24 00 00                     call       0x470b60
0046e735  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046e738  83 c0 10                           add        eax, 0x10
0046e73b  89 85 bc fe ff ff                  mov        dword ptr [ebp - 0x144], eax
0046e741  f3 0f 10 05 d0 c8 56 00            movss      xmm0, dword ptr [0x56c8d0]
0046e749  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046e751  f3 0f 58 45 e4                     addss      xmm0, dword ptr [ebp - 0x1c]
0046e756  51                                 push       ecx
0046e757  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
0046e75c  8b 8d bc fe ff ff                  mov        ecx, dword ptr [ebp - 0x144]
0046e762  e8 b9 23 00 00                     call       0x470b20
0046e767  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e76a  83 c1 10                           add        ecx, 0x10
0046e76d  89 8d 98 fe ff ff                  mov        dword ptr [ebp - 0x168], ecx
0046e773  f3 0f 10 05 d0 c8 56 00            movss      xmm0, dword ptr [0x56c8d0]
0046e77b  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046e783  f3 0f 58 45 e8                     addss      xmm0, dword ptr [ebp - 0x18]
0046e788  51                                 push       ecx
0046e789  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
0046e78e  8b 8d 98 fe ff ff                  mov        ecx, dword ptr [ebp - 0x168]
0046e794  e8 a7 23 00 00                     call       0x470b40
0046e799  90                                 nop        
0046e79a  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e79d  83 c1 10                           add        ecx, 0x10
0046e7a0  e8 1b 92 fc ff                     call       0x4379c0
0046e7a5  d9 9d b4 fe ff ff                  fstp       dword ptr [ebp - 0x14c]
0046e7ab  f3 0f 10 85 b4 fe ff ff            movss      xmm0, dword ptr [ebp - 0x14c]
0046e7b3  0f 2e 05 cc c8 56 00               ucomiss    xmm0, dword ptr [0x56c8cc]
0046e7ba  9f                                 lahf       
0046e7bb  f6 c4 44                           test       ah, 0x44
0046e7be  7a 18                              jp         0x46e7d8
0046e7c0  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046e7c3  8a 82 b0 04 00 00                  mov        al, byte ptr [edx + 0x4b0]
0046e7c9  24 f3                              and        al, 0xf3
0046e7cb  0c 04                              or         al, 4
0046e7cd  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e7d0  88 81 b0 04 00 00                  mov        byte ptr [ecx + 0x4b0], al
0046e7d6  eb 14                              jmp        0x46e7ec
0046e7d8  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046e7db  8a 82 b0 04 00 00                  mov        al, byte ptr [edx + 0x4b0]
0046e7e1  24 f3                              and        al, 0xf3
0046e7e3  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e7e6  88 81 b0 04 00 00                  mov        byte ptr [ecx + 0x4b0], al
0046e7ec  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046e7ef  f3 0f 10 82 1c 01 00 00            movss      xmm0, dword ptr [edx + 0x11c]
0046e7f7  0f 2e 05 e8 e0 56 00               ucomiss    xmm0, dword ptr [0x56e0e8]
0046e7fe  9f                                 lahf       
0046e7ff  f6 c4 44                           test       ah, 0x44
0046e802  7a 15                              jp         0x46e819
0046e804  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046e807  83 c0 10                           add        eax, 0x10
0046e80a  50                                 push       eax
0046e80b  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
0046e811  e8 3a 0d fd ff                     call       0x43f550
0046e816  90                                 nop        
0046e817  eb 13                              jmp        0x46e82c
0046e819  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e81c  83 c1 10                           add        ecx, 0x10
0046e81f  51                                 push       ecx
0046e820  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
0046e826  e8 05 0e fd ff                     call       0x43f630
0046e82b  90                                 nop        
0046e82c  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046e82f  83 c2 10                           add        edx, 0x10
0046e832  89 95 b0 fe ff ff                  mov        dword ptr [ebp - 0x150], edx
0046e838  51                                 push       ecx
0046e839  f3 0f 10 45 e4                     movss      xmm0, dword ptr [ebp - 0x1c]
0046e83e  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
0046e843  8b 8d b0 fe ff ff                  mov        ecx, dword ptr [ebp - 0x150]
0046e849  e8 d2 22 00 00                     call       0x470b20
0046e84e  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046e851  83 c0 10                           add        eax, 0x10
0046e854  89 85 ac fe ff ff                  mov        dword ptr [ebp - 0x154], eax
0046e85a  51                                 push       ecx
0046e85b  f3 0f 10 45 e8                     movss      xmm0, dword ptr [ebp - 0x18]
0046e860  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
0046e865  8b 8d ac fe ff ff                  mov        ecx, dword ptr [ebp - 0x154]
0046e86b  e8 d0 22 00 00                     call       0x470b40
0046e870  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046e873  83 c1 10                           add        ecx, 0x10
0046e876  89 8d a8 fe ff ff                  mov        dword ptr [ebp - 0x158], ecx
0046e87c  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046e87f  8b 82 0c 01 00 00                  mov        eax, dword ptr [edx + 0x10c]
0046e885  50                                 push       eax
0046e886  8b 8d a8 fe ff ff                  mov        ecx, dword ptr [ebp - 0x158]
0046e88c  e8 7f 11 fe ff                     call       0x44fa10
0046e891  90                                 nop        
0046e892  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046e895  f3 0f 10 81 1c 01 00 00            movss      xmm0, dword ptr [ecx + 0x11c]
0046e89d  0f 2e 05 e8 e0 56 00               ucomiss    xmm0, dword ptr [0x56e0e8]
0046e8a4  9f                                 lahf       
0046e8a5  f6 c4 44                           test       ah, 0x44
0046e8a8  7a 15                              jp         0x46e8bf
0046e8aa  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046e8ad  83 c2 10                           add        edx, 0x10
0046e8b0  52                                 push       edx
0046e8b1  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
0046e8b7  e8 94 0c fd ff                     call       0x43f550
0046e8bc  90                                 nop        
0046e8bd  eb 13                              jmp        0x46e8d2
0046e8bf  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046e8c2  83 c0 10                           add        eax, 0x10
0046e8c5  50                                 push       eax
0046e8c6  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
0046e8cc  e8 5f 0d fd ff                     call       0x43f630
0046e8d1  90                                 nop        
0046e8d2  f3 0f 10 45 c0                     movss      xmm0, dword ptr [ebp - 0x40]
0046e8d7  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046e8df  f3 0f 58 45 f0                     addss      xmm0, dword ptr [ebp - 0x10]
0046e8e4  f3 0f 11 45 f0                     movss      dword ptr [ebp - 0x10], xmm0
0046e8e9  8b 4d c4                           mov        ecx, dword ptr [ebp - 0x3c]
0046e8ec  83 c1 01                           add        ecx, 1
0046e8ef  89 4d c4                           mov        dword ptr [ebp - 0x3c], ecx
0046e8f2  e9 99 f5 ff ff                     jmp        0x46de90
0046e8f7  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046e8fa  33 cd                              xor        ecx, ebp
0046e8fc  e8 ed 40 0d 00                     call       0x5429ee
0046e901  8b e5                              mov        esp, ebp
0046e903  5d                                 pop        ebp
0046e904  c2 04 00                           ret        4
