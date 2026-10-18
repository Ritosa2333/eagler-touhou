004da1f0  55                                 push       ebp
004da1f1  8b ec                              mov        ebp, esp
004da1f3  81 ec c4 00 00 00                  sub        esp, 0xc4
004da1f9  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
004da1fe  33 c5                              xor        eax, ebp
004da200  89 45 fc                           mov        dword ptr [ebp - 4], eax
004da203  89 4d 94                           mov        dword ptr [ebp - 0x6c], ecx
004da206  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
004da209  8b 48 0c                           mov        ecx, dword ptr [eax + 0xc]
004da20c  d1 e9                              shr        ecx, 1
004da20e  89 4d d4                           mov        dword ptr [ebp - 0x2c], ecx
004da211  f2 0f 2a 45 d4                     cvtsi2sd   xmm0, dword ptr [ebp - 0x2c]
004da216  8b 55 d4                           mov        edx, dword ptr [ebp - 0x2c]
004da219  c1 ea 1f                           shr        edx, 0x1f
004da21c  f2 0f 58 04 d5 d0 cd 56 00         addsd      xmm0, qword ptr [edx*8 + 0x56cdd0]
004da225  f2 0f 11 45 8c                     movsd      qword ptr [ebp - 0x74], xmm0
004da22a  f2 0f 5a 45 8c                     cvtsd2ss   xmm0, qword ptr [ebp - 0x74]
004da22f  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004da232  f3 0f 10 48 54                     movss      xmm1, dword ptr [eax + 0x54]
004da237  f3 0f 5e 0d d0 c8 56 00            divss      xmm1, dword ptr [0x56c8d0]
004da23f  51                                 push       ecx
004da240  f3 0f 11 0c 24                     movss      dword ptr [esp], xmm1
004da245  f3 0f 11 45 a8                     movss      dword ptr [ebp - 0x58], xmm0
004da24a  e8 01 f6 f5 ff                     call       0x439850
004da24f  83 c4 04                           add        esp, 4
004da252  d9 5d a4                           fstp       dword ptr [ebp - 0x5c]
004da255  f3 0f 10 45 a8                     movss      xmm0, dword ptr [ebp - 0x58]
004da25a  f3 0f 5e 45 a4                     divss      xmm0, dword ptr [ebp - 0x5c]
004da25f  f3 0f 11 45 a0                     movss      dword ptr [ebp - 0x60], xmm0
004da264  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
004da267  8b 51 04                           mov        edx, dword ptr [ecx + 4]
004da26a  89 55 d0                           mov        dword ptr [ebp - 0x30], edx
004da26d  f2 0f 2a 45 d0                     cvtsi2sd   xmm0, dword ptr [ebp - 0x30]
004da272  8b 45 d0                           mov        eax, dword ptr [ebp - 0x30]
004da275  c1 e8 1f                           shr        eax, 0x1f
004da278  f2 0f 58 04 c5 d0 cd 56 00         addsd      xmm0, qword ptr [eax*8 + 0x56cdd0]
004da281  f2 0f 11 45 84                     movsd      qword ptr [ebp - 0x7c], xmm0
004da286  f2 0f 5a 45 84                     cvtsd2ss   xmm0, qword ptr [ebp - 0x7c]
004da28b  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
004da28e  8b 51 0c                           mov        edx, dword ptr [ecx + 0xc]
004da291  89 55 cc                           mov        dword ptr [ebp - 0x34], edx
004da294  f2 0f 2a 4d cc                     cvtsi2sd   xmm1, dword ptr [ebp - 0x34]
004da299  8b 45 cc                           mov        eax, dword ptr [ebp - 0x34]
004da29c  c1 e8 1f                           shr        eax, 0x1f
004da29f  f2 0f 58 0c c5 d0 cd 56 00         addsd      xmm1, qword ptr [eax*8 + 0x56cdd0]
004da2a8  f2 0f 11 8d 7c ff ff ff            movsd      qword ptr [ebp - 0x84], xmm1
004da2b0  f2 0f 5a 8d 7c ff ff ff            cvtsd2ss   xmm1, qword ptr [ebp - 0x84]
004da2b8  f3 0f 5e 0d d0 c8 56 00            divss      xmm1, dword ptr [0x56c8d0]
004da2c0  f3 0f 58 c1                        addss      xmm0, xmm1
004da2c4  f3 0f 11 45 9c                     movss      dword ptr [ebp - 0x64], xmm0
004da2c9  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
004da2cc  8b 51 08                           mov        edx, dword ptr [ecx + 8]
004da2cf  89 55 c8                           mov        dword ptr [ebp - 0x38], edx
004da2d2  f2 0f 2a 45 c8                     cvtsi2sd   xmm0, dword ptr [ebp - 0x38]
004da2d7  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
004da2da  c1 e8 1f                           shr        eax, 0x1f
004da2dd  f2 0f 58 04 c5 d0 cd 56 00         addsd      xmm0, qword ptr [eax*8 + 0x56cdd0]
004da2e6  f2 0f 11 85 74 ff ff ff            movsd      qword ptr [ebp - 0x8c], xmm0
004da2ee  f2 0f 5a 85 74 ff ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0x8c]
004da2f6  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
004da2fe  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
004da301  8b 11                              mov        edx, dword ptr [ecx]
004da303  89 55 c4                           mov        dword ptr [ebp - 0x3c], edx
004da306  f2 0f 2a 4d c4                     cvtsi2sd   xmm1, dword ptr [ebp - 0x3c]
004da30b  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
004da30e  c1 e8 1f                           shr        eax, 0x1f
004da311  f2 0f 58 0c c5 d0 cd 56 00         addsd      xmm1, qword ptr [eax*8 + 0x56cdd0]
004da31a  f2 0f 11 8d 6c ff ff ff            movsd      qword ptr [ebp - 0x94], xmm1
004da322  f2 0f 5a 8d 6c ff ff ff            cvtsd2ss   xmm1, qword ptr [ebp - 0x94]
004da32a  f3 0f 58 c8                        addss      xmm1, xmm0
004da32e  f3 0f 11 4d 98                     movss      dword ptr [ebp - 0x68], xmm1
004da333  51                                 push       ecx
004da334  f3 0f 10 45 a0                     movss      xmm0, dword ptr [ebp - 0x60]
004da339  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004da33e  51                                 push       ecx
004da33f  f3 0f 10 45 9c                     movss      xmm0, dword ptr [ebp - 0x64]
004da344  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004da349  51                                 push       ecx
004da34a  f3 0f 10 45 98                     movss      xmm0, dword ptr [ebp - 0x68]
004da34f  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004da354  8d 4d d8                           lea        ecx, [ebp - 0x28]
004da357  e8 74 8a f4 ff                     call       0x422dd0
004da35c  51                                 push       ecx
004da35d  0f 57 c0                           xorps      xmm0, xmm0
004da360  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004da365  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004da368  8b 91 e4 00 00 00                  mov        edx, dword ptr [ecx + 0xe4]
004da36e  89 55 c0                           mov        dword ptr [ebp - 0x40], edx
004da371  f2 0f 2a 45 c0                     cvtsi2sd   xmm0, dword ptr [ebp - 0x40]
004da376  8b 45 c0                           mov        eax, dword ptr [ebp - 0x40]
004da379  c1 e8 1f                           shr        eax, 0x1f
004da37c  f2 0f 58 04 c5 d0 cd 56 00         addsd      xmm0, qword ptr [eax*8 + 0x56cdd0]
004da385  f2 0f 11 85 64 ff ff ff            movsd      qword ptr [ebp - 0x9c], xmm0
004da38d  f2 0f 5a 85 64 ff ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0x9c]
004da395  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004da398  8b 91 ec 00 00 00                  mov        edx, dword ptr [ecx + 0xec]
004da39e  89 55 bc                           mov        dword ptr [ebp - 0x44], edx
004da3a1  f2 0f 2a 4d bc                     cvtsi2sd   xmm1, dword ptr [ebp - 0x44]
004da3a6  8b 45 bc                           mov        eax, dword ptr [ebp - 0x44]
004da3a9  c1 e8 1f                           shr        eax, 0x1f
004da3ac  f2 0f 58 0c c5 d0 cd 56 00         addsd      xmm1, qword ptr [eax*8 + 0x56cdd0]
004da3b5  f2 0f 11 8d 5c ff ff ff            movsd      qword ptr [ebp - 0xa4], xmm1
004da3bd  f2 0f 5a 8d 5c ff ff ff            cvtsd2ss   xmm1, qword ptr [ebp - 0xa4]
004da3c5  f3 0f 5e 0d d0 c8 56 00            divss      xmm1, dword ptr [0x56c8d0]
004da3cd  f3 0f 58 c1                        addss      xmm0, xmm1
004da3d1  51                                 push       ecx
004da3d2  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004da3d7  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004da3da  8b 91 e0 00 00 00                  mov        edx, dword ptr [ecx + 0xe0]
004da3e0  89 55 b8                           mov        dword ptr [ebp - 0x48], edx
004da3e3  f2 0f 2a 45 b8                     cvtsi2sd   xmm0, dword ptr [ebp - 0x48]
004da3e8  8b 45 b8                           mov        eax, dword ptr [ebp - 0x48]
004da3eb  c1 e8 1f                           shr        eax, 0x1f
004da3ee  f2 0f 58 04 c5 d0 cd 56 00         addsd      xmm0, qword ptr [eax*8 + 0x56cdd0]
004da3f7  f2 0f 11 85 54 ff ff ff            movsd      qword ptr [ebp - 0xac], xmm0
004da3ff  f2 0f 5a 85 54 ff ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0xac]
004da407  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004da40a  8b 91 e8 00 00 00                  mov        edx, dword ptr [ecx + 0xe8]
004da410  89 55 b4                           mov        dword ptr [ebp - 0x4c], edx
004da413  f2 0f 2a 4d b4                     cvtsi2sd   xmm1, dword ptr [ebp - 0x4c]
004da418  8b 45 b4                           mov        eax, dword ptr [ebp - 0x4c]
004da41b  c1 e8 1f                           shr        eax, 0x1f
004da41e  f2 0f 58 0c c5 d0 cd 56 00         addsd      xmm1, qword ptr [eax*8 + 0x56cdd0]
004da427  f2 0f 11 8d 4c ff ff ff            movsd      qword ptr [ebp - 0xb4], xmm1
004da42f  f2 0f 5a 8d 4c ff ff ff            cvtsd2ss   xmm1, qword ptr [ebp - 0xb4]
004da437  f3 0f 5e 0d d0 c8 56 00            divss      xmm1, dword ptr [0x56c8d0]
004da43f  f3 0f 58 c1                        addss      xmm0, xmm1
004da443  51                                 push       ecx
004da444  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004da449  8d 4d e4                           lea        ecx, [ebp - 0x1c]
004da44c  e8 7f 89 f4 ff                     call       0x422dd0
004da451  51                                 push       ecx
004da452  0f 57 c0                           xorps      xmm0, xmm0
004da455  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004da45a  51                                 push       ecx
004da45b  f3 0f 10 05 f4 e0 56 00            movss      xmm0, dword ptr [0x56e0f4]
004da463  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004da468  51                                 push       ecx
004da469  0f 57 c0                           xorps      xmm0, xmm0
004da46c  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004da471  8d 4d f0                           lea        ecx, [ebp - 0x10]
004da474  e8 57 89 f4 ff                     call       0x422dd0
004da479  8d 4d f0                           lea        ecx, [ebp - 0x10]
004da47c  51                                 push       ecx
004da47d  8d 55 e4                           lea        edx, [ebp - 0x1c]
004da480  52                                 push       edx
004da481  8d 45 d8                           lea        eax, [ebp - 0x28]
004da484  50                                 push       eax
004da485  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004da488  83 c1 60                           add        ecx, 0x60
004da48b  51                                 push       ecx
004da48c  e8 29 62 06 00                     call       0x5406ba
004da491  51                                 push       ecx
004da492  f3 0f 10 05 a0 1e 57 00            movss      xmm0, dword ptr [0x571ea0]
004da49a  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004da49f  51                                 push       ecx
004da4a0  f3 0f 10 05 cc c8 56 00            movss      xmm0, dword ptr [0x56c8cc]
004da4a8  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004da4ad  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
004da4b0  8b 42 08                           mov        eax, dword ptr [edx + 8]
004da4b3  89 45 b0                           mov        dword ptr [ebp - 0x50], eax
004da4b6  f2 0f 2a 45 b0                     cvtsi2sd   xmm0, dword ptr [ebp - 0x50]
004da4bb  8b 4d b0                           mov        ecx, dword ptr [ebp - 0x50]
004da4be  c1 e9 1f                           shr        ecx, 0x1f
004da4c1  f2 0f 58 04 cd d0 cd 56 00         addsd      xmm0, qword ptr [ecx*8 + 0x56cdd0]
004da4ca  f2 0f 11 85 44 ff ff ff            movsd      qword ptr [ebp - 0xbc], xmm0
004da4d2  f2 0f 5a 85 44 ff ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0xbc]
004da4da  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
004da4dd  8b 42 0c                           mov        eax, dword ptr [edx + 0xc]
004da4e0  89 45 ac                           mov        dword ptr [ebp - 0x54], eax
004da4e3  f2 0f 2a 4d ac                     cvtsi2sd   xmm1, dword ptr [ebp - 0x54]
004da4e8  8b 4d ac                           mov        ecx, dword ptr [ebp - 0x54]
004da4eb  c1 e9 1f                           shr        ecx, 0x1f
004da4ee  f2 0f 58 0c cd d0 cd 56 00         addsd      xmm1, qword ptr [ecx*8 + 0x56cdd0]
004da4f7  f2 0f 11 8d 3c ff ff ff            movsd      qword ptr [ebp - 0xc4], xmm1
004da4ff  f2 0f 5a 8d 3c ff ff ff            cvtsd2ss   xmm1, qword ptr [ebp - 0xc4]
004da507  f3 0f 5e c1                        divss      xmm0, xmm1
004da50b  51                                 push       ecx
004da50c  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004da511  8b 55 08                           mov        edx, dword ptr [ebp + 8]
004da514  51                                 push       ecx
004da515  f3 0f 10 42 54                     movss      xmm0, dword ptr [edx + 0x54]
004da51a  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004da51f  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004da522  05 a0 00 00 00                     add        eax, 0xa0
004da527  50                                 push       eax
004da528  e8 93 61 06 00                     call       0x5406c0
004da52d  90                                 nop        
004da52e  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004da531  33 cd                              xor        ecx, ebp
004da533  e8 b6 84 06 00                     call       0x5429ee
004da538  8b e5                              mov        esp, ebp
004da53a  5d                                 pop        ebp
004da53b  c2 08 00                           ret        8
