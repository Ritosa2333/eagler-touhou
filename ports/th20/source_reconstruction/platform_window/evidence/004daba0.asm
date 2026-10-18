004daba0  55                                 push       ebp
004daba1  8b ec                              mov        ebp, esp
004daba3  81 ec f4 01 00 00                  sub        esp, 0x1f4
004daba9  56                                 push       esi
004dabaa  57                                 push       edi
004dabab  89 4d f8                           mov        dword ptr [ebp - 8], ecx
004dabae  b8 6c 01 00 00                     mov        eax, 0x16c
004dabb3  d1 e0                              shl        eax, 1
004dabb5  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004dabb8  8d 94 01 78 02 00 00               lea        edx, [ecx + eax + 0x278]
004dabbf  89 55 fc                           mov        dword ptr [ebp - 4], edx
004dabc2  51                                 push       ecx
004dabc3  0f 57 c0                           xorps      xmm0, xmm0
004dabc6  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dabcb  51                                 push       ecx
004dabcc  0f 57 c0                           xorps      xmm0, xmm0
004dabcf  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dabd4  51                                 push       ecx
004dabd5  0f 57 c0                           xorps      xmm0, xmm0
004dabd8  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dabdd  8d 8d 3c fe ff ff                  lea        ecx, [ebp - 0x1c4]
004dabe3  e8 e8 81 f4 ff                     call       0x422dd0
004dabe8  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dabeb  8b 10                              mov        edx, dword ptr [eax]
004dabed  89 11                              mov        dword ptr [ecx], edx
004dabef  8b 50 04                           mov        edx, dword ptr [eax + 4]
004dabf2  89 51 04                           mov        dword ptr [ecx + 4], edx
004dabf5  8b 40 08                           mov        eax, dword ptr [eax + 8]
004dabf8  89 41 08                           mov        dword ptr [ecx + 8], eax
004dabfb  51                                 push       ecx
004dabfc  0f 57 c0                           xorps      xmm0, xmm0
004dabff  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dac04  51                                 push       ecx
004dac05  0f 57 c0                           xorps      xmm0, xmm0
004dac08  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dac0d  51                                 push       ecx
004dac0e  0f 57 c0                           xorps      xmm0, xmm0
004dac11  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dac16  8d 8d 30 fe ff ff                  lea        ecx, [ebp - 0x1d0]
004dac1c  e8 af 81 f4 ff                     call       0x422dd0
004dac21  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dac24  83 c1 0c                           add        ecx, 0xc
004dac27  8b 10                              mov        edx, dword ptr [eax]
004dac29  89 11                              mov        dword ptr [ecx], edx
004dac2b  8b 50 04                           mov        edx, dword ptr [eax + 4]
004dac2e  89 51 04                           mov        dword ptr [ecx + 4], edx
004dac31  8b 40 08                           mov        eax, dword ptr [eax + 8]
004dac34  89 41 08                           mov        dword ptr [ecx + 8], eax
004dac37  51                                 push       ecx
004dac38  0f 57 c0                           xorps      xmm0, xmm0
004dac3b  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dac40  51                                 push       ecx
004dac41  f3 0f 10 05 cc c8 56 00            movss      xmm0, dword ptr [0x56c8cc]
004dac49  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dac4e  51                                 push       ecx
004dac4f  0f 57 c0                           xorps      xmm0, xmm0
004dac52  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dac57  8d 8d 24 fe ff ff                  lea        ecx, [ebp - 0x1dc]
004dac5d  e8 6e 81 f4 ff                     call       0x422dd0
004dac62  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dac65  83 c1 18                           add        ecx, 0x18
004dac68  8b 10                              mov        edx, dword ptr [eax]
004dac6a  89 11                              mov        dword ptr [ecx], edx
004dac6c  8b 50 04                           mov        edx, dword ptr [eax + 4]
004dac6f  89 51 04                           mov        dword ptr [ecx + 4], edx
004dac72  8b 40 08                           mov        eax, dword ptr [eax + 8]
004dac75  89 41 08                           mov        dword ptr [ecx + 8], eax
004dac78  f3 0f 10 05 f0 e0 56 00            movss      xmm0, dword ptr [0x56e0f0]
004dac80  f3 0f 5e 05 0c f0 56 00            divss      xmm0, dword ptr [0x56f00c]
004dac88  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dac8b  f3 0f 11 41 54                     movss      dword ptr [ecx + 0x54], xmm0
004dac90  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dac93  c7 82 e0 00 00 00 00 00 00 00      mov        dword ptr [edx + 0xe0], 0
004dac9d  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004daca0  c7 80 e4 00 00 00 00 00 00 00      mov        dword ptr [eax + 0xe4], 0
004dacaa  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dacad  8b 15 f8 87 5b 00                  mov        edx, dword ptr [0x5b87f8]
004dacb3  89 91 e8 00 00 00                  mov        dword ptr [ecx + 0xe8], edx
004dacb9  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dacbc  8b 0d fc 87 5b 00                  mov        ecx, dword ptr [0x5b87fc]
004dacc2  89 88 ec 00 00 00                  mov        dword ptr [eax + 0xec], ecx
004dacc8  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004daccb  0f 57 c0                           xorps      xmm0, xmm0
004dacce  f3 0f 11 82 f0 00 00 00            movss      dword ptr [edx + 0xf0], xmm0
004dacd6  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dacd9  f3 0f 10 05 cc c8 56 00            movss      xmm0, dword ptr [0x56c8cc]
004dace1  f3 0f 11 80 f4 00 00 00            movss      dword ptr [eax + 0xf4], xmm0
004dace9  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dacec  c7 81 f8 00 00 00 02 00 00 00      mov        dword ptr [ecx + 0xf8], 2
004dacf6  51                                 push       ecx
004dacf7  0f 57 c0                           xorps      xmm0, xmm0
004dacfa  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dacff  51                                 push       ecx
004dad00  0f 57 c0                           xorps      xmm0, xmm0
004dad03  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dad08  51                                 push       ecx
004dad09  0f 57 c0                           xorps      xmm0, xmm0
004dad0c  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dad11  8d 8d 18 fe ff ff                  lea        ecx, [ebp - 0x1e8]
004dad17  e8 b4 80 f4 ff                     call       0x422dd0
004dad1c  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dad1f  83 c2 3c                           add        edx, 0x3c
004dad22  8b 08                              mov        ecx, dword ptr [eax]
004dad24  89 0a                              mov        dword ptr [edx], ecx
004dad26  8b 48 04                           mov        ecx, dword ptr [eax + 4]
004dad29  89 4a 04                           mov        dword ptr [edx + 4], ecx
004dad2c  8b 40 08                           mov        eax, dword ptr [eax + 8]
004dad2f  89 42 08                           mov        dword ptr [edx + 8], eax
004dad32  51                                 push       ecx
004dad33  0f 57 c0                           xorps      xmm0, xmm0
004dad36  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dad3b  51                                 push       ecx
004dad3c  0f 57 c0                           xorps      xmm0, xmm0
004dad3f  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dad44  51                                 push       ecx
004dad45  0f 57 c0                           xorps      xmm0, xmm0
004dad48  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dad4d  8d 8d 0c fe ff ff                  lea        ecx, [ebp - 0x1f4]
004dad53  e8 78 80 f4 ff                     call       0x422dd0
004dad58  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dad5b  83 c1 48                           add        ecx, 0x48
004dad5e  8b 10                              mov        edx, dword ptr [eax]
004dad60  89 11                              mov        dword ptr [ecx], edx
004dad62  8b 50 04                           mov        edx, dword ptr [eax + 4]
004dad65  89 51 04                           mov        dword ptr [ecx + 4], edx
004dad68  8b 40 08                           mov        eax, dword ptr [eax + 8]
004dad6b  89 41 08                           mov        dword ptr [ecx + 8], eax
004dad6e  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dad71  8b 15 f8 87 5b 00                  mov        edx, dword ptr [0x5b87f8]
004dad77  89 51 58                           mov        dword ptr [ecx + 0x58], edx
004dad7a  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dad7d  8b 0d fc 87 5b 00                  mov        ecx, dword ptr [0x5b87fc]
004dad83  89 48 5c                           mov        dword ptr [eax + 0x5c], ecx
004dad86  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dad89  c7 82 fc 00 00 00 00 00 00 00      mov        dword ptr [edx + 0xfc], 0
004dad93  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dad96  c7 80 00 01 00 00 00 00 00 00      mov        dword ptr [eax + 0x100], 0
004dada0  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dada3  81 c1 e0 00 00 00                  add        ecx, 0xe0
004dada9  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dadac  81 c2 04 01 00 00                  add        edx, 0x104
004dadb2  8b 01                              mov        eax, dword ptr [ecx]
004dadb4  89 02                              mov        dword ptr [edx], eax
004dadb6  8b 41 04                           mov        eax, dword ptr [ecx + 4]
004dadb9  89 42 04                           mov        dword ptr [edx + 4], eax
004dadbc  8b 41 08                           mov        eax, dword ptr [ecx + 8]
004dadbf  89 42 08                           mov        dword ptr [edx + 8], eax
004dadc2  8b 41 0c                           mov        eax, dword ptr [ecx + 0xc]
004dadc5  89 42 0c                           mov        dword ptr [edx + 0xc], eax
004dadc8  8b 41 10                           mov        eax, dword ptr [ecx + 0x10]
004dadcb  89 42 10                           mov        dword ptr [edx + 0x10], eax
004dadce  8b 49 14                           mov        ecx, dword ptr [ecx + 0x14]
004dadd1  89 4a 14                           mov        dword ptr [edx + 0x14], ecx
004dadd4  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dadd7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dadda  8b 88 fc 00 00 00                  mov        ecx, dword ptr [eax + 0xfc]
004dade0  89 8a 04 01 00 00                  mov        dword ptr [edx + 0x104], ecx
004dade6  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dade9  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dadec  8b 88 00 01 00 00                  mov        ecx, dword ptr [eax + 0x100]
004dadf2  89 8a 08 01 00 00                  mov        dword ptr [edx + 0x108], ecx
004dadf8  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dadfb  a1 f8 87 5b 00                     mov        eax, dword ptr [0x5b87f8]
004dae00  89 82 0c 01 00 00                  mov        dword ptr [edx + 0x10c], eax
004dae06  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dae09  8b 15 fc 87 5b 00                  mov        edx, dword ptr [0x5b87fc]
004dae0f  89 91 10 01 00 00                  mov        dword ptr [ecx + 0x110], edx
004dae15  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dae18  8b 88 04 01 00 00                  mov        ecx, dword ptr [eax + 0x104]
004dae1e  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
004dae21  f2 0f 2a 45 f4                     cvtsi2sd   xmm0, dword ptr [ebp - 0xc]
004dae26  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
004dae29  c1 ea 1f                           shr        edx, 0x1f
004dae2c  f2 0f 58 04 d5 d0 cd 56 00         addsd      xmm0, qword ptr [edx*8 + 0x56cdd0]
004dae35  f2 0f 11 85 60 ff ff ff            movsd      qword ptr [ebp - 0xa0], xmm0
004dae3d  f2 0f 5a 85 60 ff ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0xa0]
004dae45  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dae48  f3 0f 11 80 1c 01 00 00            movss      dword ptr [eax + 0x11c], xmm0
004dae50  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dae53  8b 91 08 01 00 00                  mov        edx, dword ptr [ecx + 0x108]
004dae59  89 55 f0                           mov        dword ptr [ebp - 0x10], edx
004dae5c  f2 0f 2a 45 f0                     cvtsi2sd   xmm0, dword ptr [ebp - 0x10]
004dae61  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
004dae64  c1 e8 1f                           shr        eax, 0x1f
004dae67  f2 0f 58 04 c5 d0 cd 56 00         addsd      xmm0, qword ptr [eax*8 + 0x56cdd0]
004dae70  f2 0f 11 85 58 ff ff ff            movsd      qword ptr [ebp - 0xa8], xmm0
004dae78  f2 0f 5a 85 58 ff ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0xa8]
004dae80  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dae83  f3 0f 11 81 20 01 00 00            movss      dword ptr [ecx + 0x120], xmm0
004dae8b  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dae8e  8b 82 04 01 00 00                  mov        eax, dword ptr [edx + 0x104]
004dae94  89 45 ec                           mov        dword ptr [ebp - 0x14], eax
004dae97  f2 0f 2a 45 ec                     cvtsi2sd   xmm0, dword ptr [ebp - 0x14]
004dae9c  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
004dae9f  c1 e9 1f                           shr        ecx, 0x1f
004daea2  f2 0f 58 04 cd d0 cd 56 00         addsd      xmm0, qword ptr [ecx*8 + 0x56cdd0]
004daeab  f2 0f 11 85 50 ff ff ff            movsd      qword ptr [ebp - 0xb0], xmm0
004daeb3  f2 0f 5a 85 50 ff ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0xb0]
004daebb  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004daebe  8b 82 0c 01 00 00                  mov        eax, dword ptr [edx + 0x10c]
004daec4  89 45 e8                           mov        dword ptr [ebp - 0x18], eax
004daec7  f2 0f 2a 4d e8                     cvtsi2sd   xmm1, dword ptr [ebp - 0x18]
004daecc  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004daecf  c1 e9 1f                           shr        ecx, 0x1f
004daed2  f2 0f 58 0c cd d0 cd 56 00         addsd      xmm1, qword ptr [ecx*8 + 0x56cdd0]
004daedb  f2 0f 11 8d 48 ff ff ff            movsd      qword ptr [ebp - 0xb8], xmm1
004daee3  f2 0f 5a 8d 48 ff ff ff            cvtsd2ss   xmm1, qword ptr [ebp - 0xb8]
004daeeb  f3 0f 58 c1                        addss      xmm0, xmm1
004daeef  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004daef2  f3 0f 11 82 24 01 00 00            movss      dword ptr [edx + 0x124], xmm0
004daefa  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004daefd  8b 88 08 01 00 00                  mov        ecx, dword ptr [eax + 0x108]
004daf03  89 4d e4                           mov        dword ptr [ebp - 0x1c], ecx
004daf06  f2 0f 2a 45 e4                     cvtsi2sd   xmm0, dword ptr [ebp - 0x1c]
004daf0b  8b 55 e4                           mov        edx, dword ptr [ebp - 0x1c]
004daf0e  c1 ea 1f                           shr        edx, 0x1f
004daf11  f2 0f 58 04 d5 d0 cd 56 00         addsd      xmm0, qword ptr [edx*8 + 0x56cdd0]
004daf1a  f2 0f 11 85 40 ff ff ff            movsd      qword ptr [ebp - 0xc0], xmm0
004daf22  f2 0f 5a 85 40 ff ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0xc0]
004daf2a  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004daf2d  8b 88 10 01 00 00                  mov        ecx, dword ptr [eax + 0x110]
004daf33  89 4d e0                           mov        dword ptr [ebp - 0x20], ecx
004daf36  f2 0f 2a 4d e0                     cvtsi2sd   xmm1, dword ptr [ebp - 0x20]
004daf3b  8b 55 e0                           mov        edx, dword ptr [ebp - 0x20]
004daf3e  c1 ea 1f                           shr        edx, 0x1f
004daf41  f2 0f 58 0c d5 d0 cd 56 00         addsd      xmm1, qword ptr [edx*8 + 0x56cdd0]
004daf4a  f2 0f 11 8d 38 ff ff ff            movsd      qword ptr [ebp - 0xc8], xmm1
004daf52  f2 0f 5a 8d 38 ff ff ff            cvtsd2ss   xmm1, qword ptr [ebp - 0xc8]
004daf5a  f3 0f 58 c1                        addss      xmm0, xmm1
004daf5e  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004daf61  f3 0f 11 80 28 01 00 00            movss      dword ptr [eax + 0x128], xmm0
004daf69  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004daf6c  81 c1 e0 00 00 00                  add        ecx, 0xe0
004daf72  51                                 push       ecx
004daf73  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004daf76  52                                 push       edx
004daf77  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004daf7a  e8 71 f2 ff ff                     call       0x4da1f0
004daf7f  b8 6c 01 00 00                     mov        eax, 0x16c
004daf84  6b c8 00                           imul       ecx, eax, 0
004daf87  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
004daf8a  8d 84 0a 78 02 00 00               lea        eax, [edx + ecx + 0x278]
004daf91  89 45 fc                           mov        dword ptr [ebp - 4], eax
004daf94  b9 6c 01 00 00                     mov        ecx, 0x16c
004daf99  d1 e1                              shl        ecx, 1
004daf9b  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
004daf9e  8d b4 0a 78 02 00 00               lea        esi, [edx + ecx + 0x278]
004dafa5  b9 5b 00 00 00                     mov        ecx, 0x5b
004dafaa  8b 7d fc                           mov        edi, dword ptr [ebp - 4]
004dafad  f3 a5                              rep movsd  dword ptr es:[edi], dword ptr [esi]
004dafaf  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dafb2  c7 80 f8 00 00 00 00 00 00 00      mov        dword ptr [eax + 0xf8], 0
004dafbc  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dafbf  c7 81 fc 00 00 00 00 00 00 00      mov        dword ptr [ecx + 0xfc], 0
004dafc9  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dafcc  c7 82 00 01 00 00 00 00 00 00      mov        dword ptr [edx + 0x100], 0
004dafd6  a1 f8 87 5b 00                     mov        eax, dword ptr [0x5b87f8]
004dafdb  99                                 cdq        
004dafdc  2b c2                              sub        eax, edx
004dafde  d1 f8                              sar        eax, 1
004dafe0  f3 0f 10 05 a4 cd 56 00            movss      xmm0, dword ptr [0x56cda4]
004dafe8  f3 0f 58 05 94 cd 56 00            addss      xmm0, dword ptr [0x56cd94]
004daff0  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
004daff8  f3 0f 2c c8                        cvttss2si  ecx, xmm0
004daffc  2b c1                              sub        eax, ecx
004daffe  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db001  89 82 e0 00 00 00                  mov        dword ptr [edx + 0xe0], eax
004db007  a1 fc 87 5b 00                     mov        eax, dword ptr [0x5b87fc]
004db00c  99                                 cdq        
004db00d  2b c2                              sub        eax, edx
004db00f  d1 f8                              sar        eax, 1
004db011  f3 0f 10 05 c0 d7 56 00            movss      xmm0, dword ptr [0x56d7c0]
004db019  f3 0f 58 05 94 cd 56 00            addss      xmm0, dword ptr [0x56cd94]
004db021  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
004db029  f3 0f 2c c8                        cvttss2si  ecx, xmm0
004db02d  2b c1                              sub        eax, ecx
004db02f  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db032  89 82 e4 00 00 00                  mov        dword ptr [edx + 0xe4], eax
004db038  f3 0f 10 05 a4 cd 56 00            movss      xmm0, dword ptr [0x56cda4]
004db040  f3 0f 58 05 94 cd 56 00            addss      xmm0, dword ptr [0x56cd94]
004db048  f3 0f 2c c0                        cvttss2si  eax, xmm0
004db04c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db04f  89 81 e8 00 00 00                  mov        dword ptr [ecx + 0xe8], eax
004db055  f3 0f 10 05 c0 d7 56 00            movss      xmm0, dword ptr [0x56d7c0]
004db05d  f3 0f 58 05 94 cd 56 00            addss      xmm0, dword ptr [0x56cd94]
004db065  f3 0f 2c d0                        cvttss2si  edx, xmm0
004db069  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db06c  89 90 ec 00 00 00                  mov        dword ptr [eax + 0xec], edx
004db072  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db075  81 c1 e0 00 00 00                  add        ecx, 0xe0
004db07b  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db07e  81 c2 04 01 00 00                  add        edx, 0x104
004db084  8b 01                              mov        eax, dword ptr [ecx]
004db086  89 02                              mov        dword ptr [edx], eax
004db088  8b 41 04                           mov        eax, dword ptr [ecx + 4]
004db08b  89 42 04                           mov        dword ptr [edx + 4], eax
004db08e  8b 41 08                           mov        eax, dword ptr [ecx + 8]
004db091  89 42 08                           mov        dword ptr [edx + 8], eax
004db094  8b 41 0c                           mov        eax, dword ptr [ecx + 0xc]
004db097  89 42 0c                           mov        dword ptr [edx + 0xc], eax
004db09a  8b 41 10                           mov        eax, dword ptr [ecx + 0x10]
004db09d  89 42 10                           mov        dword ptr [edx + 0x10], eax
004db0a0  8b 49 14                           mov        ecx, dword ptr [ecx + 0x14]
004db0a3  89 4a 14                           mov        dword ptr [edx + 0x14], ecx
004db0a6  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db0a9  8b 82 04 01 00 00                  mov        eax, dword ptr [edx + 0x104]
004db0af  89 45 dc                           mov        dword ptr [ebp - 0x24], eax
004db0b2  f2 0f 2a 45 dc                     cvtsi2sd   xmm0, dword ptr [ebp - 0x24]
004db0b7  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
004db0ba  c1 e9 1f                           shr        ecx, 0x1f
004db0bd  f2 0f 58 04 cd d0 cd 56 00         addsd      xmm0, qword ptr [ecx*8 + 0x56cdd0]
004db0c6  f2 0f 11 85 30 ff ff ff            movsd      qword ptr [ebp - 0xd0], xmm0
004db0ce  f2 0f 5a 85 30 ff ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0xd0]
004db0d6  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db0d9  f3 0f 11 82 1c 01 00 00            movss      dword ptr [edx + 0x11c], xmm0
004db0e1  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db0e4  8b 88 08 01 00 00                  mov        ecx, dword ptr [eax + 0x108]
004db0ea  89 4d d8                           mov        dword ptr [ebp - 0x28], ecx
004db0ed  f2 0f 2a 45 d8                     cvtsi2sd   xmm0, dword ptr [ebp - 0x28]
004db0f2  8b 55 d8                           mov        edx, dword ptr [ebp - 0x28]
004db0f5  c1 ea 1f                           shr        edx, 0x1f
004db0f8  f2 0f 58 04 d5 d0 cd 56 00         addsd      xmm0, qword ptr [edx*8 + 0x56cdd0]
004db101  f2 0f 11 85 28 ff ff ff            movsd      qword ptr [ebp - 0xd8], xmm0
004db109  f2 0f 5a 85 28 ff ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0xd8]
004db111  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db114  f3 0f 11 80 20 01 00 00            movss      dword ptr [eax + 0x120], xmm0
004db11c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db11f  8b 91 04 01 00 00                  mov        edx, dword ptr [ecx + 0x104]
004db125  89 55 d4                           mov        dword ptr [ebp - 0x2c], edx
004db128  f2 0f 2a 45 d4                     cvtsi2sd   xmm0, dword ptr [ebp - 0x2c]
004db12d  8b 45 d4                           mov        eax, dword ptr [ebp - 0x2c]
004db130  c1 e8 1f                           shr        eax, 0x1f
004db133  f2 0f 58 04 c5 d0 cd 56 00         addsd      xmm0, qword ptr [eax*8 + 0x56cdd0]
004db13c  f2 0f 11 85 20 ff ff ff            movsd      qword ptr [ebp - 0xe0], xmm0
004db144  f2 0f 5a 85 20 ff ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0xe0]
004db14c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db14f  8b 91 0c 01 00 00                  mov        edx, dword ptr [ecx + 0x10c]
004db155  89 55 d0                           mov        dword ptr [ebp - 0x30], edx
004db158  f2 0f 2a 4d d0                     cvtsi2sd   xmm1, dword ptr [ebp - 0x30]
004db15d  8b 45 d0                           mov        eax, dword ptr [ebp - 0x30]
004db160  c1 e8 1f                           shr        eax, 0x1f
004db163  f2 0f 58 0c c5 d0 cd 56 00         addsd      xmm1, qword ptr [eax*8 + 0x56cdd0]
004db16c  f2 0f 11 8d 18 ff ff ff            movsd      qword ptr [ebp - 0xe8], xmm1
004db174  f2 0f 5a 8d 18 ff ff ff            cvtsd2ss   xmm1, qword ptr [ebp - 0xe8]
004db17c  f3 0f 58 c1                        addss      xmm0, xmm1
004db180  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db183  f3 0f 11 81 24 01 00 00            movss      dword ptr [ecx + 0x124], xmm0
004db18b  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db18e  8b 82 08 01 00 00                  mov        eax, dword ptr [edx + 0x108]
004db194  89 45 cc                           mov        dword ptr [ebp - 0x34], eax
004db197  f2 0f 2a 45 cc                     cvtsi2sd   xmm0, dword ptr [ebp - 0x34]
004db19c  8b 4d cc                           mov        ecx, dword ptr [ebp - 0x34]
004db19f  c1 e9 1f                           shr        ecx, 0x1f
004db1a2  f2 0f 58 04 cd d0 cd 56 00         addsd      xmm0, qword ptr [ecx*8 + 0x56cdd0]
004db1ab  f2 0f 11 85 10 ff ff ff            movsd      qword ptr [ebp - 0xf0], xmm0
004db1b3  f2 0f 5a 85 10 ff ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0xf0]
004db1bb  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db1be  8b 82 10 01 00 00                  mov        eax, dword ptr [edx + 0x110]
004db1c4  89 45 c8                           mov        dword ptr [ebp - 0x38], eax
004db1c7  f2 0f 2a 4d c8                     cvtsi2sd   xmm1, dword ptr [ebp - 0x38]
004db1cc  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
004db1cf  c1 e9 1f                           shr        ecx, 0x1f
004db1d2  f2 0f 58 0c cd d0 cd 56 00         addsd      xmm1, qword ptr [ecx*8 + 0x56cdd0]
004db1db  f2 0f 11 8d 08 ff ff ff            movsd      qword ptr [ebp - 0xf8], xmm1
004db1e3  f2 0f 5a 8d 08 ff ff ff            cvtsd2ss   xmm1, qword ptr [ebp - 0xf8]
004db1eb  f3 0f 58 c1                        addss      xmm0, xmm1
004db1ef  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db1f2  f3 0f 11 82 28 01 00 00            movss      dword ptr [edx + 0x128], xmm0
004db1fa  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db1fd  05 e0 00 00 00                     add        eax, 0xe0
004db202  50                                 push       eax
004db203  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db206  51                                 push       ecx
004db207  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004db20a  e8 e1 ef ff ff                     call       0x4da1f0
004db20f  ba 6c 01 00 00                     mov        edx, 0x16c
004db214  c1 e2 00                           shl        edx, 0
004db217  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
004db21a  8d 8c 10 78 02 00 00               lea        ecx, [eax + edx + 0x278]
004db221  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004db224  ba 6c 01 00 00                     mov        edx, 0x16c
004db229  6b c2 00                           imul       eax, edx, 0
004db22c  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004db22f  8d b4 01 78 02 00 00               lea        esi, [ecx + eax + 0x278]
004db236  b9 5b 00 00 00                     mov        ecx, 0x5b
004db23b  8b 7d fc                           mov        edi, dword ptr [ebp - 4]
004db23e  f3 a5                              rep movsd  dword ptr es:[edi], dword ptr [esi]
004db240  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db243  c7 82 f8 00 00 00 01 00 00 00      mov        dword ptr [edx + 0xf8], 1
004db24d  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db250  c7 80 fc 00 00 00 00 00 00 00      mov        dword ptr [eax + 0xfc], 0
004db25a  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db25d  c7 81 00 01 00 00 00 00 00 00      mov        dword ptr [ecx + 0x100], 0
004db267  f3 0f 10 05 ac cd 56 00            movss      xmm0, dword ptr [0x56cdac]
004db26f  f3 0f 5c 05 a4 cd 56 00            subss      xmm0, dword ptr [0x56cda4]
004db277  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
004db27f  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
004db287  f3 0f 2c d0                        cvttss2si  edx, xmm0
004db28b  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db28e  89 90 e0 00 00 00                  mov        dword ptr [eax + 0xe0], edx
004db294  f3 0f 10 05 90 cd 56 00            movss      xmm0, dword ptr [0x56cd90]
004db29c  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
004db2a4  f3 0f 2c c8                        cvttss2si  ecx, xmm0
004db2a8  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db2ab  89 8a e4 00 00 00                  mov        dword ptr [edx + 0xe4], ecx
004db2b1  f3 0f 10 05 a4 cd 56 00            movss      xmm0, dword ptr [0x56cda4]
004db2b9  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
004db2c1  f3 0f 2c c0                        cvttss2si  eax, xmm0
004db2c5  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db2c8  89 81 e8 00 00 00                  mov        dword ptr [ecx + 0xe8], eax
004db2ce  f3 0f 10 05 c0 d7 56 00            movss      xmm0, dword ptr [0x56d7c0]
004db2d6  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
004db2de  f3 0f 2c d0                        cvttss2si  edx, xmm0
004db2e2  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db2e5  89 90 ec 00 00 00                  mov        dword ptr [eax + 0xec], edx
004db2eb  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db2ee  81 c1 e0 00 00 00                  add        ecx, 0xe0
004db2f4  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db2f7  81 c2 04 01 00 00                  add        edx, 0x104
004db2fd  8b 01                              mov        eax, dword ptr [ecx]
004db2ff  89 02                              mov        dword ptr [edx], eax
004db301  8b 41 04                           mov        eax, dword ptr [ecx + 4]
004db304  89 42 04                           mov        dword ptr [edx + 4], eax
004db307  8b 41 08                           mov        eax, dword ptr [ecx + 8]
004db30a  89 42 08                           mov        dword ptr [edx + 8], eax
004db30d  8b 41 0c                           mov        eax, dword ptr [ecx + 0xc]
004db310  89 42 0c                           mov        dword ptr [edx + 0xc], eax
004db313  8b 41 10                           mov        eax, dword ptr [ecx + 0x10]
004db316  89 42 10                           mov        dword ptr [edx + 0x10], eax
004db319  8b 49 14                           mov        ecx, dword ptr [ecx + 0x14]
004db31c  89 4a 14                           mov        dword ptr [edx + 0x14], ecx
004db31f  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db322  8b 82 04 01 00 00                  mov        eax, dword ptr [edx + 0x104]
004db328  89 45 c4                           mov        dword ptr [ebp - 0x3c], eax
004db32b  f2 0f 2a 45 c4                     cvtsi2sd   xmm0, dword ptr [ebp - 0x3c]
004db330  8b 4d c4                           mov        ecx, dword ptr [ebp - 0x3c]
004db333  c1 e9 1f                           shr        ecx, 0x1f
004db336  f2 0f 58 04 cd d0 cd 56 00         addsd      xmm0, qword ptr [ecx*8 + 0x56cdd0]
004db33f  f2 0f 11 85 00 ff ff ff            movsd      qword ptr [ebp - 0x100], xmm0
004db347  f2 0f 5a 85 00 ff ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0x100]
004db34f  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db352  f3 0f 11 82 1c 01 00 00            movss      dword ptr [edx + 0x11c], xmm0
004db35a  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db35d  8b 88 08 01 00 00                  mov        ecx, dword ptr [eax + 0x108]
004db363  89 4d c0                           mov        dword ptr [ebp - 0x40], ecx
004db366  f2 0f 2a 45 c0                     cvtsi2sd   xmm0, dword ptr [ebp - 0x40]
004db36b  8b 55 c0                           mov        edx, dword ptr [ebp - 0x40]
004db36e  c1 ea 1f                           shr        edx, 0x1f
004db371  f2 0f 58 04 d5 d0 cd 56 00         addsd      xmm0, qword ptr [edx*8 + 0x56cdd0]
004db37a  f2 0f 11 85 f8 fe ff ff            movsd      qword ptr [ebp - 0x108], xmm0
004db382  f2 0f 5a 85 f8 fe ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0x108]
004db38a  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db38d  f3 0f 11 80 20 01 00 00            movss      dword ptr [eax + 0x120], xmm0
004db395  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db398  8b 91 04 01 00 00                  mov        edx, dword ptr [ecx + 0x104]
004db39e  89 55 bc                           mov        dword ptr [ebp - 0x44], edx
004db3a1  f2 0f 2a 45 bc                     cvtsi2sd   xmm0, dword ptr [ebp - 0x44]
004db3a6  8b 45 bc                           mov        eax, dword ptr [ebp - 0x44]
004db3a9  c1 e8 1f                           shr        eax, 0x1f
004db3ac  f2 0f 58 04 c5 d0 cd 56 00         addsd      xmm0, qword ptr [eax*8 + 0x56cdd0]
004db3b5  f2 0f 11 85 f0 fe ff ff            movsd      qword ptr [ebp - 0x110], xmm0
004db3bd  f2 0f 5a 85 f0 fe ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0x110]
004db3c5  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db3c8  8b 91 0c 01 00 00                  mov        edx, dword ptr [ecx + 0x10c]
004db3ce  89 55 b8                           mov        dword ptr [ebp - 0x48], edx
004db3d1  f2 0f 2a 4d b8                     cvtsi2sd   xmm1, dword ptr [ebp - 0x48]
004db3d6  8b 45 b8                           mov        eax, dword ptr [ebp - 0x48]
004db3d9  c1 e8 1f                           shr        eax, 0x1f
004db3dc  f2 0f 58 0c c5 d0 cd 56 00         addsd      xmm1, qword ptr [eax*8 + 0x56cdd0]
004db3e5  f2 0f 11 8d e8 fe ff ff            movsd      qword ptr [ebp - 0x118], xmm1
004db3ed  f2 0f 5a 8d e8 fe ff ff            cvtsd2ss   xmm1, qword ptr [ebp - 0x118]
004db3f5  f3 0f 58 c1                        addss      xmm0, xmm1
004db3f9  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db3fc  f3 0f 11 81 24 01 00 00            movss      dword ptr [ecx + 0x124], xmm0
004db404  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db407  8b 82 08 01 00 00                  mov        eax, dword ptr [edx + 0x108]
004db40d  89 45 b4                           mov        dword ptr [ebp - 0x4c], eax
004db410  f2 0f 2a 45 b4                     cvtsi2sd   xmm0, dword ptr [ebp - 0x4c]
004db415  8b 4d b4                           mov        ecx, dword ptr [ebp - 0x4c]
004db418  c1 e9 1f                           shr        ecx, 0x1f
004db41b  f2 0f 58 04 cd d0 cd 56 00         addsd      xmm0, qword ptr [ecx*8 + 0x56cdd0]
004db424  f2 0f 11 85 e0 fe ff ff            movsd      qword ptr [ebp - 0x120], xmm0
004db42c  f2 0f 5a 85 e0 fe ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0x120]
004db434  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db437  8b 82 10 01 00 00                  mov        eax, dword ptr [edx + 0x110]
004db43d  89 45 b0                           mov        dword ptr [ebp - 0x50], eax
004db440  f2 0f 2a 4d b0                     cvtsi2sd   xmm1, dword ptr [ebp - 0x50]
004db445  8b 4d b0                           mov        ecx, dword ptr [ebp - 0x50]
004db448  c1 e9 1f                           shr        ecx, 0x1f
004db44b  f2 0f 58 0c cd d0 cd 56 00         addsd      xmm1, qword ptr [ecx*8 + 0x56cdd0]
004db454  f2 0f 11 8d d8 fe ff ff            movsd      qword ptr [ebp - 0x128], xmm1
004db45c  f2 0f 5a 8d d8 fe ff ff            cvtsd2ss   xmm1, qword ptr [ebp - 0x128]
004db464  f3 0f 58 c1                        addss      xmm0, xmm1
004db468  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db46b  f3 0f 11 82 28 01 00 00            movss      dword ptr [edx + 0x128], xmm0
004db473  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db476  05 e0 00 00 00                     add        eax, 0xe0
004db47b  50                                 push       eax
004db47c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db47f  51                                 push       ecx
004db480  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004db483  e8 68 ed ff ff                     call       0x4da1f0
004db488  ba 6c 01 00 00                     mov        edx, 0x16c
004db48d  c1 e2 02                           shl        edx, 2
004db490  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
004db493  8d 8c 10 78 02 00 00               lea        ecx, [eax + edx + 0x278]
004db49a  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004db49d  ba 6c 01 00 00                     mov        edx, 0x16c
004db4a2  6b c2 00                           imul       eax, edx, 0
004db4a5  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004db4a8  8d b4 01 78 02 00 00               lea        esi, [ecx + eax + 0x278]
004db4af  b9 5b 00 00 00                     mov        ecx, 0x5b
004db4b4  8b 7d fc                           mov        edi, dword ptr [ebp - 4]
004db4b7  f3 a5                              rep movsd  dword ptr es:[edi], dword ptr [esi]
004db4b9  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db4bc  c7 82 f8 00 00 00 01 00 00 00      mov        dword ptr [edx + 0xf8], 1
004db4c6  b8 6c 01 00 00                     mov        eax, 0x16c
004db4cb  d1 e0                              shl        eax, 1
004db4cd  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db4d0  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
004db4d3  8b 84 02 74 03 00 00               mov        eax, dword ptr [edx + eax + 0x374]
004db4da  89 81 fc 00 00 00                  mov        dword ptr [ecx + 0xfc], eax
004db4e0  b9 6c 01 00 00                     mov        ecx, 0x16c
004db4e5  d1 e1                              shl        ecx, 1
004db4e7  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db4ea  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
004db4ed  8b 8c 08 78 03 00 00               mov        ecx, dword ptr [eax + ecx + 0x378]
004db4f4  89 8a 00 01 00 00                  mov        dword ptr [edx + 0x100], ecx
004db4fa  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db4fd  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db500  8b 88 fc 00 00 00                  mov        ecx, dword ptr [eax + 0xfc]
004db506  89 8a e0 00 00 00                  mov        dword ptr [edx + 0xe0], ecx
004db50c  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db50f  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db512  8b 88 00 01 00 00                  mov        ecx, dword ptr [eax + 0x100]
004db518  89 8a e4 00 00 00                  mov        dword ptr [edx + 0xe4], ecx
004db51e  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db521  a1 f8 87 5b 00                     mov        eax, dword ptr [0x5b87f8]
004db526  89 82 e8 00 00 00                  mov        dword ptr [edx + 0xe8], eax
004db52c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db52f  8b 15 fc 87 5b 00                  mov        edx, dword ptr [0x5b87fc]
004db535  89 91 ec 00 00 00                  mov        dword ptr [ecx + 0xec], edx
004db53b  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db53e  05 e0 00 00 00                     add        eax, 0xe0
004db543  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db546  81 c1 04 01 00 00                  add        ecx, 0x104
004db54c  8b 10                              mov        edx, dword ptr [eax]
004db54e  89 11                              mov        dword ptr [ecx], edx
004db550  8b 50 04                           mov        edx, dword ptr [eax + 4]
004db553  89 51 04                           mov        dword ptr [ecx + 4], edx
004db556  8b 50 08                           mov        edx, dword ptr [eax + 8]
004db559  89 51 08                           mov        dword ptr [ecx + 8], edx
004db55c  8b 50 0c                           mov        edx, dword ptr [eax + 0xc]
004db55f  89 51 0c                           mov        dword ptr [ecx + 0xc], edx
004db562  8b 50 10                           mov        edx, dword ptr [eax + 0x10]
004db565  89 51 10                           mov        dword ptr [ecx + 0x10], edx
004db568  8b 40 14                           mov        eax, dword ptr [eax + 0x14]
004db56b  89 41 14                           mov        dword ptr [ecx + 0x14], eax
004db56e  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db571  8b 91 04 01 00 00                  mov        edx, dword ptr [ecx + 0x104]
004db577  89 55 ac                           mov        dword ptr [ebp - 0x54], edx
004db57a  f2 0f 2a 45 ac                     cvtsi2sd   xmm0, dword ptr [ebp - 0x54]
004db57f  8b 45 ac                           mov        eax, dword ptr [ebp - 0x54]
004db582  c1 e8 1f                           shr        eax, 0x1f
004db585  f2 0f 58 04 c5 d0 cd 56 00         addsd      xmm0, qword ptr [eax*8 + 0x56cdd0]
004db58e  f2 0f 11 85 d0 fe ff ff            movsd      qword ptr [ebp - 0x130], xmm0
004db596  f2 0f 5a 85 d0 fe ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0x130]
004db59e  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db5a1  f3 0f 11 81 1c 01 00 00            movss      dword ptr [ecx + 0x11c], xmm0
004db5a9  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db5ac  8b 82 08 01 00 00                  mov        eax, dword ptr [edx + 0x108]
004db5b2  89 45 a8                           mov        dword ptr [ebp - 0x58], eax
004db5b5  f2 0f 2a 45 a8                     cvtsi2sd   xmm0, dword ptr [ebp - 0x58]
004db5ba  8b 4d a8                           mov        ecx, dword ptr [ebp - 0x58]
004db5bd  c1 e9 1f                           shr        ecx, 0x1f
004db5c0  f2 0f 58 04 cd d0 cd 56 00         addsd      xmm0, qword ptr [ecx*8 + 0x56cdd0]
004db5c9  f2 0f 11 85 c8 fe ff ff            movsd      qword ptr [ebp - 0x138], xmm0
004db5d1  f2 0f 5a 85 c8 fe ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0x138]
004db5d9  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db5dc  f3 0f 11 82 20 01 00 00            movss      dword ptr [edx + 0x120], xmm0
004db5e4  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db5e7  8b 88 04 01 00 00                  mov        ecx, dword ptr [eax + 0x104]
004db5ed  89 4d a4                           mov        dword ptr [ebp - 0x5c], ecx
004db5f0  f2 0f 2a 45 a4                     cvtsi2sd   xmm0, dword ptr [ebp - 0x5c]
004db5f5  8b 55 a4                           mov        edx, dword ptr [ebp - 0x5c]
004db5f8  c1 ea 1f                           shr        edx, 0x1f
004db5fb  f2 0f 58 04 d5 d0 cd 56 00         addsd      xmm0, qword ptr [edx*8 + 0x56cdd0]
004db604  f2 0f 11 85 c0 fe ff ff            movsd      qword ptr [ebp - 0x140], xmm0
004db60c  f2 0f 5a 85 c0 fe ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0x140]
004db614  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db617  8b 88 0c 01 00 00                  mov        ecx, dword ptr [eax + 0x10c]
004db61d  89 4d a0                           mov        dword ptr [ebp - 0x60], ecx
004db620  f2 0f 2a 4d a0                     cvtsi2sd   xmm1, dword ptr [ebp - 0x60]
004db625  8b 55 a0                           mov        edx, dword ptr [ebp - 0x60]
004db628  c1 ea 1f                           shr        edx, 0x1f
004db62b  f2 0f 58 0c d5 d0 cd 56 00         addsd      xmm1, qword ptr [edx*8 + 0x56cdd0]
004db634  f2 0f 11 8d b8 fe ff ff            movsd      qword ptr [ebp - 0x148], xmm1
004db63c  f2 0f 5a 8d b8 fe ff ff            cvtsd2ss   xmm1, qword ptr [ebp - 0x148]
004db644  f3 0f 58 c1                        addss      xmm0, xmm1
004db648  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db64b  f3 0f 11 80 24 01 00 00            movss      dword ptr [eax + 0x124], xmm0
004db653  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db656  8b 91 08 01 00 00                  mov        edx, dword ptr [ecx + 0x108]
004db65c  89 55 9c                           mov        dword ptr [ebp - 0x64], edx
004db65f  f2 0f 2a 45 9c                     cvtsi2sd   xmm0, dword ptr [ebp - 0x64]
004db664  8b 45 9c                           mov        eax, dword ptr [ebp - 0x64]
004db667  c1 e8 1f                           shr        eax, 0x1f
004db66a  f2 0f 58 04 c5 d0 cd 56 00         addsd      xmm0, qword ptr [eax*8 + 0x56cdd0]
004db673  f2 0f 11 85 b0 fe ff ff            movsd      qword ptr [ebp - 0x150], xmm0
004db67b  f2 0f 5a 85 b0 fe ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0x150]
004db683  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db686  8b 91 10 01 00 00                  mov        edx, dword ptr [ecx + 0x110]
004db68c  89 55 98                           mov        dword ptr [ebp - 0x68], edx
004db68f  f2 0f 2a 4d 98                     cvtsi2sd   xmm1, dword ptr [ebp - 0x68]
004db694  8b 45 98                           mov        eax, dword ptr [ebp - 0x68]
004db697  c1 e8 1f                           shr        eax, 0x1f
004db69a  f2 0f 58 0c c5 d0 cd 56 00         addsd      xmm1, qword ptr [eax*8 + 0x56cdd0]
004db6a3  f2 0f 11 8d a8 fe ff ff            movsd      qword ptr [ebp - 0x158], xmm1
004db6ab  f2 0f 5a 8d a8 fe ff ff            cvtsd2ss   xmm1, qword ptr [ebp - 0x158]
004db6b3  f3 0f 58 c1                        addss      xmm0, xmm1
004db6b7  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db6ba  f3 0f 11 81 28 01 00 00            movss      dword ptr [ecx + 0x128], xmm0
004db6c2  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db6c5  81 c2 e0 00 00 00                  add        edx, 0xe0
004db6cb  52                                 push       edx
004db6cc  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db6cf  50                                 push       eax
004db6d0  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004db6d3  e8 18 eb ff ff                     call       0x4da1f0
004db6d8  b9 6c 01 00 00                     mov        ecx, 0x16c
004db6dd  6b d1 05                           imul       edx, ecx, 5
004db6e0  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
004db6e3  8d 8c 10 78 02 00 00               lea        ecx, [eax + edx + 0x278]
004db6ea  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004db6ed  ba 6c 01 00 00                     mov        edx, 0x16c
004db6f2  d1 e2                              shl        edx, 1
004db6f4  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
004db6f7  8d b4 10 78 02 00 00               lea        esi, [eax + edx + 0x278]
004db6fe  b9 5b 00 00 00                     mov        ecx, 0x5b
004db703  8b 7d fc                           mov        edi, dword ptr [ebp - 4]
004db706  f3 a5                              rep movsd  dword ptr es:[edi], dword ptr [esi]
004db708  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db70b  c7 81 f8 00 00 00 01 00 00 00      mov        dword ptr [ecx + 0xf8], 1
004db715  ba 6c 01 00 00                     mov        edx, 0x16c
004db71a  d1 e2                              shl        edx, 1
004db71c  f3 0f 10 05 ac cd 56 00            movss      xmm0, dword ptr [0x56cdac]
004db724  f3 0f 5c 05 a4 cd 56 00            subss      xmm0, dword ptr [0x56cda4]
004db72c  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
004db734  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
004db73c  f3 0f 2c c0                        cvttss2si  eax, xmm0
004db740  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004db743  8b 8c 11 74 03 00 00               mov        ecx, dword ptr [ecx + edx + 0x374]
004db74a  03 c8                              add        ecx, eax
004db74c  f3 0f 10 05 a4 cd 56 00            movss      xmm0, dword ptr [0x56cda4]
004db754  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
004db75c  f3 0f 2c c0                        cvttss2si  eax, xmm0
004db760  99                                 cdq        
004db761  2b c2                              sub        eax, edx
004db763  d1 f8                              sar        eax, 1
004db765  03 c8                              add        ecx, eax
004db767  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db76a  89 8a fc 00 00 00                  mov        dword ptr [edx + 0xfc], ecx
004db770  b8 6c 01 00 00                     mov        eax, 0x16c
004db775  d1 e0                              shl        eax, 1
004db777  f3 0f 10 05 90 cd 56 00            movss      xmm0, dword ptr [0x56cd90]
004db77f  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
004db787  f3 0f 2c c8                        cvttss2si  ecx, xmm0
004db78b  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
004db78e  03 8c 02 78 03 00 00               add        ecx, dword ptr [edx + eax + 0x378]
004db795  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db798  89 88 00 01 00 00                  mov        dword ptr [eax + 0x100], ecx
004db79e  b9 6c 01 00 00                     mov        ecx, 0x16c
004db7a3  d1 e1                              shl        ecx, 1
004db7a5  f3 0f 10 05 ac cd 56 00            movss      xmm0, dword ptr [0x56cdac]
004db7ad  f3 0f 5c 05 a4 cd 56 00            subss      xmm0, dword ptr [0x56cda4]
004db7b5  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
004db7bd  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
004db7c5  f3 0f 2c d0                        cvttss2si  edx, xmm0
004db7c9  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
004db7cc  03 94 08 74 03 00 00               add        edx, dword ptr [eax + ecx + 0x374]
004db7d3  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db7d6  89 91 e0 00 00 00                  mov        dword ptr [ecx + 0xe0], edx
004db7dc  ba 6c 01 00 00                     mov        edx, 0x16c
004db7e1  d1 e2                              shl        edx, 1
004db7e3  f3 0f 10 05 90 cd 56 00            movss      xmm0, dword ptr [0x56cd90]
004db7eb  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
004db7f3  f3 0f 2c c0                        cvttss2si  eax, xmm0
004db7f7  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004db7fa  03 84 11 78 03 00 00               add        eax, dword ptr [ecx + edx + 0x378]
004db801  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db804  89 82 e4 00 00 00                  mov        dword ptr [edx + 0xe4], eax
004db80a  b8 6c 01 00 00                     mov        eax, 0x16c
004db80f  d1 e0                              shl        eax, 1
004db811  f3 0f 10 05 a4 cd 56 00            movss      xmm0, dword ptr [0x56cda4]
004db819  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
004db821  f3 0f 2c c8                        cvttss2si  ecx, xmm0
004db825  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
004db828  03 8c 02 74 03 00 00               add        ecx, dword ptr [edx + eax + 0x374]
004db82f  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db832  89 88 e8 00 00 00                  mov        dword ptr [eax + 0xe8], ecx
004db838  b9 6c 01 00 00                     mov        ecx, 0x16c
004db83d  d1 e1                              shl        ecx, 1
004db83f  f3 0f 10 05 c0 d7 56 00            movss      xmm0, dword ptr [0x56d7c0]
004db847  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
004db84f  f3 0f 2c d0                        cvttss2si  edx, xmm0
004db853  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
004db856  03 94 08 78 03 00 00               add        edx, dword ptr [eax + ecx + 0x378]
004db85d  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db860  89 91 ec 00 00 00                  mov        dword ptr [ecx + 0xec], edx
004db866  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db869  81 c2 e0 00 00 00                  add        edx, 0xe0
004db86f  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db872  05 04 01 00 00                     add        eax, 0x104
004db877  8b 0a                              mov        ecx, dword ptr [edx]
004db879  89 08                              mov        dword ptr [eax], ecx
004db87b  8b 4a 04                           mov        ecx, dword ptr [edx + 4]
004db87e  89 48 04                           mov        dword ptr [eax + 4], ecx
004db881  8b 4a 08                           mov        ecx, dword ptr [edx + 8]
004db884  89 48 08                           mov        dword ptr [eax + 8], ecx
004db887  8b 4a 0c                           mov        ecx, dword ptr [edx + 0xc]
004db88a  89 48 0c                           mov        dword ptr [eax + 0xc], ecx
004db88d  8b 4a 10                           mov        ecx, dword ptr [edx + 0x10]
004db890  89 48 10                           mov        dword ptr [eax + 0x10], ecx
004db893  8b 52 14                           mov        edx, dword ptr [edx + 0x14]
004db896  89 50 14                           mov        dword ptr [eax + 0x14], edx
004db899  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db89c  8b 88 04 01 00 00                  mov        ecx, dword ptr [eax + 0x104]
004db8a2  89 4d 94                           mov        dword ptr [ebp - 0x6c], ecx
004db8a5  f2 0f 2a 45 94                     cvtsi2sd   xmm0, dword ptr [ebp - 0x6c]
004db8aa  8b 55 94                           mov        edx, dword ptr [ebp - 0x6c]
004db8ad  c1 ea 1f                           shr        edx, 0x1f
004db8b0  f2 0f 58 04 d5 d0 cd 56 00         addsd      xmm0, qword ptr [edx*8 + 0x56cdd0]
004db8b9  f2 0f 11 85 a0 fe ff ff            movsd      qword ptr [ebp - 0x160], xmm0
004db8c1  f2 0f 5a 85 a0 fe ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0x160]
004db8c9  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db8cc  f3 0f 11 80 1c 01 00 00            movss      dword ptr [eax + 0x11c], xmm0
004db8d4  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db8d7  8b 91 08 01 00 00                  mov        edx, dword ptr [ecx + 0x108]
004db8dd  89 55 90                           mov        dword ptr [ebp - 0x70], edx
004db8e0  f2 0f 2a 45 90                     cvtsi2sd   xmm0, dword ptr [ebp - 0x70]
004db8e5  8b 45 90                           mov        eax, dword ptr [ebp - 0x70]
004db8e8  c1 e8 1f                           shr        eax, 0x1f
004db8eb  f2 0f 58 04 c5 d0 cd 56 00         addsd      xmm0, qword ptr [eax*8 + 0x56cdd0]
004db8f4  f2 0f 11 85 98 fe ff ff            movsd      qword ptr [ebp - 0x168], xmm0
004db8fc  f2 0f 5a 85 98 fe ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0x168]
004db904  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db907  f3 0f 11 81 20 01 00 00            movss      dword ptr [ecx + 0x120], xmm0
004db90f  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db912  8b 82 04 01 00 00                  mov        eax, dword ptr [edx + 0x104]
004db918  89 45 8c                           mov        dword ptr [ebp - 0x74], eax
004db91b  f2 0f 2a 45 8c                     cvtsi2sd   xmm0, dword ptr [ebp - 0x74]
004db920  8b 4d 8c                           mov        ecx, dword ptr [ebp - 0x74]
004db923  c1 e9 1f                           shr        ecx, 0x1f
004db926  f2 0f 58 04 cd d0 cd 56 00         addsd      xmm0, qword ptr [ecx*8 + 0x56cdd0]
004db92f  f2 0f 11 85 90 fe ff ff            movsd      qword ptr [ebp - 0x170], xmm0
004db937  f2 0f 5a 85 90 fe ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0x170]
004db93f  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db942  8b 82 0c 01 00 00                  mov        eax, dword ptr [edx + 0x10c]
004db948  89 45 88                           mov        dword ptr [ebp - 0x78], eax
004db94b  f2 0f 2a 4d 88                     cvtsi2sd   xmm1, dword ptr [ebp - 0x78]
004db950  8b 4d 88                           mov        ecx, dword ptr [ebp - 0x78]
004db953  c1 e9 1f                           shr        ecx, 0x1f
004db956  f2 0f 58 0c cd d0 cd 56 00         addsd      xmm1, qword ptr [ecx*8 + 0x56cdd0]
004db95f  f2 0f 11 8d 88 fe ff ff            movsd      qword ptr [ebp - 0x178], xmm1
004db967  f2 0f 5a 8d 88 fe ff ff            cvtsd2ss   xmm1, qword ptr [ebp - 0x178]
004db96f  f3 0f 58 c1                        addss      xmm0, xmm1
004db973  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db976  f3 0f 11 82 24 01 00 00            movss      dword ptr [edx + 0x124], xmm0
004db97e  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db981  8b 88 08 01 00 00                  mov        ecx, dword ptr [eax + 0x108]
004db987  89 4d 84                           mov        dword ptr [ebp - 0x7c], ecx
004db98a  f2 0f 2a 45 84                     cvtsi2sd   xmm0, dword ptr [ebp - 0x7c]
004db98f  8b 55 84                           mov        edx, dword ptr [ebp - 0x7c]
004db992  c1 ea 1f                           shr        edx, 0x1f
004db995  f2 0f 58 04 d5 d0 cd 56 00         addsd      xmm0, qword ptr [edx*8 + 0x56cdd0]
004db99e  f2 0f 11 85 80 fe ff ff            movsd      qword ptr [ebp - 0x180], xmm0
004db9a6  f2 0f 5a 85 80 fe ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0x180]
004db9ae  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db9b1  8b 88 10 01 00 00                  mov        ecx, dword ptr [eax + 0x110]
004db9b7  89 4d 80                           mov        dword ptr [ebp - 0x80], ecx
004db9ba  f2 0f 2a 4d 80                     cvtsi2sd   xmm1, dword ptr [ebp - 0x80]
004db9bf  8b 55 80                           mov        edx, dword ptr [ebp - 0x80]
004db9c2  c1 ea 1f                           shr        edx, 0x1f
004db9c5  f2 0f 58 0c d5 d0 cd 56 00         addsd      xmm1, qword ptr [edx*8 + 0x56cdd0]
004db9ce  f2 0f 11 8d 78 fe ff ff            movsd      qword ptr [ebp - 0x188], xmm1
004db9d6  f2 0f 5a 8d 78 fe ff ff            cvtsd2ss   xmm1, qword ptr [ebp - 0x188]
004db9de  f3 0f 58 c1                        addss      xmm0, xmm1
004db9e2  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004db9e5  f3 0f 11 80 28 01 00 00            movss      dword ptr [eax + 0x128], xmm0
004db9ed  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004db9f0  81 c1 e0 00 00 00                  add        ecx, 0xe0
004db9f6  51                                 push       ecx
004db9f7  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004db9fa  52                                 push       edx
004db9fb  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004db9fe  e8 ed e7 ff ff                     call       0x4da1f0
004dba03  b8 6c 01 00 00                     mov        eax, 0x16c
004dba08  6b c8 03                           imul       ecx, eax, 3
004dba0b  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
004dba0e  8d 84 0a 78 02 00 00               lea        eax, [edx + ecx + 0x278]
004dba15  89 45 fc                           mov        dword ptr [ebp - 4], eax
004dba18  b9 6c 01 00 00                     mov        ecx, 0x16c
004dba1d  6b d1 00                           imul       edx, ecx, 0
004dba20  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
004dba23  8d b4 10 78 02 00 00               lea        esi, [eax + edx + 0x278]
004dba2a  b9 5b 00 00 00                     mov        ecx, 0x5b
004dba2f  8b 7d fc                           mov        edi, dword ptr [ebp - 4]
004dba32  f3 a5                              rep movsd  dword ptr es:[edi], dword ptr [esi]
004dba34  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dba37  c7 81 f8 00 00 00 03 00 00 00      mov        dword ptr [ecx + 0xf8], 3
004dba41  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dba44  c7 82 fc 00 00 00 00 00 00 00      mov        dword ptr [edx + 0xfc], 0
004dba4e  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dba51  c7 80 00 01 00 00 00 00 00 00      mov        dword ptr [eax + 0x100], 0
004dba5b  f3 0f 2a 05 f8 87 5b 00            cvtsi2ss   xmm0, dword ptr [0x5b87f8]
004dba63  f3 0f 10 0d a4 cd 56 00            movss      xmm1, dword ptr [0x56cda4]
004dba6b  f3 0f 58 0d 94 cd 56 00            addss      xmm1, dword ptr [0x56cd94]
004dba73  f3 0f 5c c1                        subss      xmm0, xmm1
004dba77  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
004dba7f  f3 0f 2c c8                        cvttss2si  ecx, xmm0
004dba83  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dba86  89 8a e0 00 00 00                  mov        dword ptr [edx + 0xe0], ecx
004dba8c  f3 0f 2a 05 fc 87 5b 00            cvtsi2ss   xmm0, dword ptr [0x5b87fc]
004dba94  f3 0f 10 0d c0 d7 56 00            movss      xmm1, dword ptr [0x56d7c0]
004dba9c  f3 0f 58 0d 94 cd 56 00            addss      xmm1, dword ptr [0x56cd94]
004dbaa4  f3 0f 5c c1                        subss      xmm0, xmm1
004dbaa8  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
004dbab0  f3 0f 2c c0                        cvttss2si  eax, xmm0
004dbab4  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dbab7  89 81 e4 00 00 00                  mov        dword ptr [ecx + 0xe4], eax
004dbabd  f3 0f 10 05 a4 cd 56 00            movss      xmm0, dword ptr [0x56cda4]
004dbac5  f3 0f 58 05 94 cd 56 00            addss      xmm0, dword ptr [0x56cd94]
004dbacd  f3 0f 2c d0                        cvttss2si  edx, xmm0
004dbad1  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dbad4  89 90 e8 00 00 00                  mov        dword ptr [eax + 0xe8], edx
004dbada  f3 0f 10 05 c0 d7 56 00            movss      xmm0, dword ptr [0x56d7c0]
004dbae2  f3 0f 58 05 94 cd 56 00            addss      xmm0, dword ptr [0x56cd94]
004dbaea  f3 0f 2c c8                        cvttss2si  ecx, xmm0
004dbaee  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dbaf1  89 8a ec 00 00 00                  mov        dword ptr [edx + 0xec], ecx
004dbaf7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dbafa  05 e0 00 00 00                     add        eax, 0xe0
004dbaff  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dbb02  81 c1 04 01 00 00                  add        ecx, 0x104
004dbb08  8b 10                              mov        edx, dword ptr [eax]
004dbb0a  89 11                              mov        dword ptr [ecx], edx
004dbb0c  8b 50 04                           mov        edx, dword ptr [eax + 4]
004dbb0f  89 51 04                           mov        dword ptr [ecx + 4], edx
004dbb12  8b 50 08                           mov        edx, dword ptr [eax + 8]
004dbb15  89 51 08                           mov        dword ptr [ecx + 8], edx
004dbb18  8b 50 0c                           mov        edx, dword ptr [eax + 0xc]
004dbb1b  89 51 0c                           mov        dword ptr [ecx + 0xc], edx
004dbb1e  8b 50 10                           mov        edx, dword ptr [eax + 0x10]
004dbb21  89 51 10                           mov        dword ptr [ecx + 0x10], edx
004dbb24  8b 40 14                           mov        eax, dword ptr [eax + 0x14]
004dbb27  89 41 14                           mov        dword ptr [ecx + 0x14], eax
004dbb2a  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dbb2d  8b 91 04 01 00 00                  mov        edx, dword ptr [ecx + 0x104]
004dbb33  89 95 7c ff ff ff                  mov        dword ptr [ebp - 0x84], edx
004dbb39  f2 0f 2a 85 7c ff ff ff            cvtsi2sd   xmm0, dword ptr [ebp - 0x84]
004dbb41  8b 85 7c ff ff ff                  mov        eax, dword ptr [ebp - 0x84]
004dbb47  c1 e8 1f                           shr        eax, 0x1f
004dbb4a  f2 0f 58 04 c5 d0 cd 56 00         addsd      xmm0, qword ptr [eax*8 + 0x56cdd0]
004dbb53  f2 0f 11 85 70 fe ff ff            movsd      qword ptr [ebp - 0x190], xmm0
004dbb5b  f2 0f 5a 85 70 fe ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0x190]
004dbb63  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dbb66  f3 0f 11 81 1c 01 00 00            movss      dword ptr [ecx + 0x11c], xmm0
004dbb6e  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dbb71  8b 82 08 01 00 00                  mov        eax, dword ptr [edx + 0x108]
004dbb77  89 85 78 ff ff ff                  mov        dword ptr [ebp - 0x88], eax
004dbb7d  f2 0f 2a 85 78 ff ff ff            cvtsi2sd   xmm0, dword ptr [ebp - 0x88]
004dbb85  8b 8d 78 ff ff ff                  mov        ecx, dword ptr [ebp - 0x88]
004dbb8b  c1 e9 1f                           shr        ecx, 0x1f
004dbb8e  f2 0f 58 04 cd d0 cd 56 00         addsd      xmm0, qword ptr [ecx*8 + 0x56cdd0]
004dbb97  f2 0f 11 85 68 fe ff ff            movsd      qword ptr [ebp - 0x198], xmm0
004dbb9f  f2 0f 5a 85 68 fe ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0x198]
004dbba7  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dbbaa  f3 0f 11 82 20 01 00 00            movss      dword ptr [edx + 0x120], xmm0
004dbbb2  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dbbb5  8b 88 04 01 00 00                  mov        ecx, dword ptr [eax + 0x104]
004dbbbb  89 8d 74 ff ff ff                  mov        dword ptr [ebp - 0x8c], ecx
004dbbc1  f2 0f 2a 85 74 ff ff ff            cvtsi2sd   xmm0, dword ptr [ebp - 0x8c]
004dbbc9  8b 95 74 ff ff ff                  mov        edx, dword ptr [ebp - 0x8c]
004dbbcf  c1 ea 1f                           shr        edx, 0x1f
004dbbd2  f2 0f 58 04 d5 d0 cd 56 00         addsd      xmm0, qword ptr [edx*8 + 0x56cdd0]
004dbbdb  f2 0f 11 85 60 fe ff ff            movsd      qword ptr [ebp - 0x1a0], xmm0
004dbbe3  f2 0f 5a 85 60 fe ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0x1a0]
004dbbeb  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dbbee  8b 88 0c 01 00 00                  mov        ecx, dword ptr [eax + 0x10c]
004dbbf4  89 8d 70 ff ff ff                  mov        dword ptr [ebp - 0x90], ecx
004dbbfa  f2 0f 2a 8d 70 ff ff ff            cvtsi2sd   xmm1, dword ptr [ebp - 0x90]
004dbc02  8b 95 70 ff ff ff                  mov        edx, dword ptr [ebp - 0x90]
004dbc08  c1 ea 1f                           shr        edx, 0x1f
004dbc0b  f2 0f 58 0c d5 d0 cd 56 00         addsd      xmm1, qword ptr [edx*8 + 0x56cdd0]
004dbc14  f2 0f 11 8d 58 fe ff ff            movsd      qword ptr [ebp - 0x1a8], xmm1
004dbc1c  f2 0f 5a 8d 58 fe ff ff            cvtsd2ss   xmm1, qword ptr [ebp - 0x1a8]
004dbc24  f3 0f 58 c1                        addss      xmm0, xmm1
004dbc28  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dbc2b  f3 0f 11 80 24 01 00 00            movss      dword ptr [eax + 0x124], xmm0
004dbc33  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dbc36  8b 91 08 01 00 00                  mov        edx, dword ptr [ecx + 0x108]
004dbc3c  89 95 6c ff ff ff                  mov        dword ptr [ebp - 0x94], edx
004dbc42  f2 0f 2a 85 6c ff ff ff            cvtsi2sd   xmm0, dword ptr [ebp - 0x94]
004dbc4a  8b 85 6c ff ff ff                  mov        eax, dword ptr [ebp - 0x94]
004dbc50  c1 e8 1f                           shr        eax, 0x1f
004dbc53  f2 0f 58 04 c5 d0 cd 56 00         addsd      xmm0, qword ptr [eax*8 + 0x56cdd0]
004dbc5c  f2 0f 11 85 50 fe ff ff            movsd      qword ptr [ebp - 0x1b0], xmm0
004dbc64  f2 0f 5a 85 50 fe ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0x1b0]
004dbc6c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dbc6f  8b 91 10 01 00 00                  mov        edx, dword ptr [ecx + 0x110]
004dbc75  89 95 68 ff ff ff                  mov        dword ptr [ebp - 0x98], edx
004dbc7b  f2 0f 2a 8d 68 ff ff ff            cvtsi2sd   xmm1, dword ptr [ebp - 0x98]
004dbc83  8b 85 68 ff ff ff                  mov        eax, dword ptr [ebp - 0x98]
004dbc89  c1 e8 1f                           shr        eax, 0x1f
004dbc8c  f2 0f 58 0c c5 d0 cd 56 00         addsd      xmm1, qword ptr [eax*8 + 0x56cdd0]
004dbc95  f2 0f 11 8d 48 fe ff ff            movsd      qword ptr [ebp - 0x1b8], xmm1
004dbc9d  f2 0f 5a 8d 48 fe ff ff            cvtsd2ss   xmm1, qword ptr [ebp - 0x1b8]
004dbca5  f3 0f 58 c1                        addss      xmm0, xmm1
004dbca9  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dbcac  f3 0f 11 81 28 01 00 00            movss      dword ptr [ecx + 0x128], xmm0
004dbcb4  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dbcb7  81 c2 e0 00 00 00                  add        edx, 0xe0
004dbcbd  52                                 push       edx
004dbcbe  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dbcc1  50                                 push       eax
004dbcc2  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004dbcc5  e8 26 e5 ff ff                     call       0x4da1f0
004dbcca  90                                 nop        
004dbccb  5f                                 pop        edi
004dbccc  5e                                 pop        esi
004dbccd  8b e5                              mov        esp, ebp
004dbccf  5d                                 pop        ebp
004dbcd0  c3                                 ret        
