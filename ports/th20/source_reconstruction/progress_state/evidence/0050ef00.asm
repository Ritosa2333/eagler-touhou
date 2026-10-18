0050ef00  55                                 push       ebp
0050ef01  8b ec                              mov        ebp, esp
0050ef03  83 ec 1c                           sub        esp, 0x1c
0050ef06  56                                 push       esi
0050ef07  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0050ef0a  b8 43 52 00 00                     mov        eax, 0x5243
0050ef0f  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050ef12  66 89 01                           mov        word ptr [ecx], ax
0050ef15  ba 01 00 00 00                     mov        edx, 1
0050ef1a  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050ef1d  66 89 50 02                        mov        word ptr [eax + 2], dx
0050ef21  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050ef24  c7 41 08 e8 7a 00 00               mov        dword ptr [ecx + 8], 0x7ae8
0050ef2b  c7 45 f4 00 00 00 00               mov        dword ptr [ebp - 0xc], 0
0050ef32  eb 09                              jmp        0x50ef3d
0050ef34  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
0050ef37  83 c2 01                           add        edx, 1
0050ef3a  89 55 f4                           mov        dword ptr [ebp - 0xc], edx
0050ef3d  83 7d f4 07                        cmp        dword ptr [ebp - 0xc], 7
0050ef41  0f 8d db 00 00 00                  jge        0x50f022
0050ef47  c7 45 f8 00 00 00 00               mov        dword ptr [ebp - 8], 0
0050ef4e  eb 09                              jmp        0x50ef59
0050ef50  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
0050ef53  83 c0 01                           add        eax, 1
0050ef56  89 45 f8                           mov        dword ptr [ebp - 8], eax
0050ef59  83 7d f8 0a                        cmp        dword ptr [ebp - 8], 0xa
0050ef5d  0f 8d ba 00 00 00                  jge        0x50f01d
0050ef63  69 4d f8 10 27 00 00               imul       ecx, dword ptr [ebp - 8], 0x2710
0050ef6a  b8 a0 86 01 00                     mov        eax, 0x186a0
0050ef6f  2b c1                              sub        eax, ecx
0050ef71  99                                 cdq        
0050ef72  69 4d f4 90 01 00 00               imul       ecx, dword ptr [ebp - 0xc], 0x190
0050ef79  8b 75 fc                           mov        esi, dword ptr [ebp - 4]
0050ef7c  8d 4c 0e 18                        lea        ecx, [esi + ecx + 0x18]
0050ef80  6b 75 f8 28                        imul       esi, dword ptr [ebp - 8], 0x28
0050ef84  89 04 31                           mov        dword ptr [ecx + esi], eax
0050ef87  89 54 31 04                        mov        dword ptr [ecx + esi + 4], edx
0050ef8b  69 55 f4 90 01 00 00               imul       edx, dword ptr [ebp - 0xc], 0x190
0050ef92  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050ef95  8d 4c 10 18                        lea        ecx, [eax + edx + 0x18]
0050ef99  6b 55 f8 28                        imul       edx, dword ptr [ebp - 8], 0x28
0050ef9d  c6 44 11 08 01                     mov        byte ptr [ecx + edx + 8], 1
0050efa2  68 10 37 57 00                     push       0x573710
0050efa7  6a 0a                              push       0xa
0050efa9  69 45 f4 90 01 00 00               imul       eax, dword ptr [ebp - 0xc], 0x190
0050efb0  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050efb3  8d 54 01 18                        lea        edx, [ecx + eax + 0x18]
0050efb7  6b 45 f8 28                        imul       eax, dword ptr [ebp - 8], 0x28
0050efbb  8d 4c 02 0a                        lea        ecx, [edx + eax + 0xa]
0050efbf  51                                 push       ecx
0050efc0  e8 4b 96 03 00                     call       0x548610
0050efc5  83 c4 0c                           add        esp, 0xc
0050efc8  69 55 f4 90 01 00 00               imul       edx, dword ptr [ebp - 0xc], 0x190
0050efcf  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050efd2  8d 4c 10 18                        lea        ecx, [eax + edx + 0x18]
0050efd6  6b 55 f8 28                        imul       edx, dword ptr [ebp - 8], 0x28
0050efda  33 c0                              xor        eax, eax
0050efdc  33 f6                              xor        esi, esi
0050efde  89 44 11 18                        mov        dword ptr [ecx + edx + 0x18], eax
0050efe2  89 74 11 1c                        mov        dword ptr [ecx + edx + 0x1c], esi
0050efe6  69 4d f4 90 01 00 00               imul       ecx, dword ptr [ebp - 0xc], 0x190
0050efed  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050eff0  8d 44 0a 18                        lea        eax, [edx + ecx + 0x18]
0050eff4  6b 4d f8 28                        imul       ecx, dword ptr [ebp - 8], 0x28
0050eff8  c6 44 08 09 00                     mov        byte ptr [eax + ecx + 9], 0
0050effd  69 55 f4 90 01 00 00               imul       edx, dword ptr [ebp - 0xc], 0x190
0050f004  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050f007  8d 4c 10 18                        lea        ecx, [eax + edx + 0x18]
0050f00b  6b 55 f8 28                        imul       edx, dword ptr [ebp - 8], 0x28
0050f00f  0f 57 c0                           xorps      xmm0, xmm0
0050f012  f3 0f 11 44 11 20                  movss      dword ptr [ecx + edx + 0x20], xmm0
0050f018  e9 33 ff ff ff                     jmp        0x50ef50
0050f01d  e9 12 ff ff ff                     jmp        0x50ef34
0050f022  c7 45 f0 00 00 00 00               mov        dword ptr [ebp - 0x10], 0
0050f029  eb 09                              jmp        0x50f034
0050f02b  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0050f02e  83 c0 01                           add        eax, 1
0050f031  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0050f034  83 7d f0 71                        cmp        dword ptr [ebp - 0x10], 0x71
0050f038  7d 51                              jge        0x50f08b
0050f03a  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050f03d  81 c1 08 0b 00 00                  add        ecx, 0xb08
0050f043  89 4d ec                           mov        dword ptr [ebp - 0x14], ecx
0050f046  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0050f049  52                                 push       edx
0050f04a  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
0050f04d  e8 be 7d f7 ff                     call       0x486e10
0050f052  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050f055  89 88 d0 00 00 00                  mov        dword ptr [eax + 0xd0], ecx
0050f05b  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0050f05e  8b 04 95 58 f0 5a 00               mov        eax, dword ptr [edx*4 + 0x5af058]
0050f065  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
0050f068  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050f06b  81 c1 08 0b 00 00                  add        ecx, 0xb08
0050f071  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
0050f074  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0050f077  52                                 push       edx
0050f078  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0050f07b  e8 90 7d f7 ff                     call       0x486e10
0050f080  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
0050f083  89 88 d4 00 00 00                  mov        dword ptr [eax + 0xd4], ecx
0050f089  eb a0                              jmp        0x50f02b
0050f08b  5e                                 pop        esi
0050f08c  8b e5                              mov        esp, ebp
0050f08e  5d                                 pop        ebp
0050f08f  c3                                 ret        
