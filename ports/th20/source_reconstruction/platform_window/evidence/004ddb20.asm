004ddb20  55                                 push       ebp
004ddb21  8b ec                              mov        ebp, esp
004ddb23  81 ec 98 00 00 00                  sub        esp, 0x98
004ddb29  89 4d f8                           mov        dword ptr [ebp - 8], ecx
004ddb2c  b8 6c 01 00 00                     mov        eax, 0x16c
004ddb31  d1 e0                              shl        eax, 1
004ddb33  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004ddb36  8d 94 01 78 02 00 00               lea        edx, [ecx + eax + 0x278]
004ddb3d  89 55 fc                           mov        dword ptr [ebp - 4], edx
004ddb40  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004ddb43  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004ddb46  89 88 fc 00 00 00                  mov        dword ptr [eax + 0xfc], ecx
004ddb4c  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004ddb4f  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
004ddb52  89 82 00 01 00 00                  mov        dword ptr [edx + 0x100], eax
004ddb58  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004ddb5b  81 c1 e0 00 00 00                  add        ecx, 0xe0
004ddb61  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004ddb64  81 c2 04 01 00 00                  add        edx, 0x104
004ddb6a  8b 01                              mov        eax, dword ptr [ecx]
004ddb6c  89 02                              mov        dword ptr [edx], eax
004ddb6e  8b 41 04                           mov        eax, dword ptr [ecx + 4]
004ddb71  89 42 04                           mov        dword ptr [edx + 4], eax
004ddb74  8b 41 08                           mov        eax, dword ptr [ecx + 8]
004ddb77  89 42 08                           mov        dword ptr [edx + 8], eax
004ddb7a  8b 41 0c                           mov        eax, dword ptr [ecx + 0xc]
004ddb7d  89 42 0c                           mov        dword ptr [edx + 0xc], eax
004ddb80  8b 41 10                           mov        eax, dword ptr [ecx + 0x10]
004ddb83  89 42 10                           mov        dword ptr [edx + 0x10], eax
004ddb86  8b 49 14                           mov        ecx, dword ptr [ecx + 0x14]
004ddb89  89 4a 14                           mov        dword ptr [edx + 0x14], ecx
004ddb8c  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004ddb8f  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004ddb92  8b 88 fc 00 00 00                  mov        ecx, dword ptr [eax + 0xfc]
004ddb98  89 8a 04 01 00 00                  mov        dword ptr [edx + 0x104], ecx
004ddb9e  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004ddba1  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004ddba4  8b 88 00 01 00 00                  mov        ecx, dword ptr [eax + 0x100]
004ddbaa  89 8a 08 01 00 00                  mov        dword ptr [edx + 0x108], ecx
004ddbb0  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004ddbb3  a1 f8 87 5b 00                     mov        eax, dword ptr [0x5b87f8]
004ddbb8  89 82 0c 01 00 00                  mov        dword ptr [edx + 0x10c], eax
004ddbbe  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004ddbc1  8b 15 fc 87 5b 00                  mov        edx, dword ptr [0x5b87fc]
004ddbc7  89 91 10 01 00 00                  mov        dword ptr [ecx + 0x110], edx
004ddbcd  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004ddbd0  8b 88 04 01 00 00                  mov        ecx, dword ptr [eax + 0x104]
004ddbd6  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
004ddbd9  f2 0f 2a 45 f4                     cvtsi2sd   xmm0, dword ptr [ebp - 0xc]
004ddbde  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
004ddbe1  c1 ea 1f                           shr        edx, 0x1f
004ddbe4  f2 0f 58 04 d5 d0 cd 56 00         addsd      xmm0, qword ptr [edx*8 + 0x56cdd0]
004ddbed  f2 0f 11 45 c0                     movsd      qword ptr [ebp - 0x40], xmm0
004ddbf2  f2 0f 5a 45 c0                     cvtsd2ss   xmm0, qword ptr [ebp - 0x40]
004ddbf7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004ddbfa  f3 0f 11 80 1c 01 00 00            movss      dword ptr [eax + 0x11c], xmm0
004ddc02  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004ddc05  8b 91 08 01 00 00                  mov        edx, dword ptr [ecx + 0x108]
004ddc0b  89 55 f0                           mov        dword ptr [ebp - 0x10], edx
004ddc0e  f2 0f 2a 45 f0                     cvtsi2sd   xmm0, dword ptr [ebp - 0x10]
004ddc13  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
004ddc16  c1 e8 1f                           shr        eax, 0x1f
004ddc19  f2 0f 58 04 c5 d0 cd 56 00         addsd      xmm0, qword ptr [eax*8 + 0x56cdd0]
004ddc22  f2 0f 11 45 b8                     movsd      qword ptr [ebp - 0x48], xmm0
004ddc27  f2 0f 5a 45 b8                     cvtsd2ss   xmm0, qword ptr [ebp - 0x48]
004ddc2c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004ddc2f  f3 0f 11 81 20 01 00 00            movss      dword ptr [ecx + 0x120], xmm0
004ddc37  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004ddc3a  8b 82 04 01 00 00                  mov        eax, dword ptr [edx + 0x104]
004ddc40  89 45 ec                           mov        dword ptr [ebp - 0x14], eax
004ddc43  f2 0f 2a 45 ec                     cvtsi2sd   xmm0, dword ptr [ebp - 0x14]
004ddc48  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
004ddc4b  c1 e9 1f                           shr        ecx, 0x1f
004ddc4e  f2 0f 58 04 cd d0 cd 56 00         addsd      xmm0, qword ptr [ecx*8 + 0x56cdd0]
004ddc57  f2 0f 11 45 b0                     movsd      qword ptr [ebp - 0x50], xmm0
004ddc5c  f2 0f 5a 45 b0                     cvtsd2ss   xmm0, qword ptr [ebp - 0x50]
004ddc61  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004ddc64  8b 82 0c 01 00 00                  mov        eax, dword ptr [edx + 0x10c]
004ddc6a  89 45 e8                           mov        dword ptr [ebp - 0x18], eax
004ddc6d  f2 0f 2a 4d e8                     cvtsi2sd   xmm1, dword ptr [ebp - 0x18]
004ddc72  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004ddc75  c1 e9 1f                           shr        ecx, 0x1f
004ddc78  f2 0f 58 0c cd d0 cd 56 00         addsd      xmm1, qword ptr [ecx*8 + 0x56cdd0]
004ddc81  f2 0f 11 4d a8                     movsd      qword ptr [ebp - 0x58], xmm1
004ddc86  f2 0f 5a 4d a8                     cvtsd2ss   xmm1, qword ptr [ebp - 0x58]
004ddc8b  f3 0f 58 c1                        addss      xmm0, xmm1
004ddc8f  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004ddc92  f3 0f 11 82 24 01 00 00            movss      dword ptr [edx + 0x124], xmm0
004ddc9a  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004ddc9d  8b 88 08 01 00 00                  mov        ecx, dword ptr [eax + 0x108]
004ddca3  89 4d e4                           mov        dword ptr [ebp - 0x1c], ecx
004ddca6  f2 0f 2a 45 e4                     cvtsi2sd   xmm0, dword ptr [ebp - 0x1c]
004ddcab  8b 55 e4                           mov        edx, dword ptr [ebp - 0x1c]
004ddcae  c1 ea 1f                           shr        edx, 0x1f
004ddcb1  f2 0f 58 04 d5 d0 cd 56 00         addsd      xmm0, qword ptr [edx*8 + 0x56cdd0]
004ddcba  f2 0f 11 45 a0                     movsd      qword ptr [ebp - 0x60], xmm0
004ddcbf  f2 0f 5a 45 a0                     cvtsd2ss   xmm0, qword ptr [ebp - 0x60]
004ddcc4  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004ddcc7  8b 88 10 01 00 00                  mov        ecx, dword ptr [eax + 0x110]
004ddccd  89 4d e0                           mov        dword ptr [ebp - 0x20], ecx
004ddcd0  f2 0f 2a 4d e0                     cvtsi2sd   xmm1, dword ptr [ebp - 0x20]
004ddcd5  8b 55 e0                           mov        edx, dword ptr [ebp - 0x20]
004ddcd8  c1 ea 1f                           shr        edx, 0x1f
004ddcdb  f2 0f 58 0c d5 d0 cd 56 00         addsd      xmm1, qword ptr [edx*8 + 0x56cdd0]
004ddce4  f2 0f 11 4d 98                     movsd      qword ptr [ebp - 0x68], xmm1
004ddce9  f2 0f 5a 4d 98                     cvtsd2ss   xmm1, qword ptr [ebp - 0x68]
004ddcee  f3 0f 58 c1                        addss      xmm0, xmm1
004ddcf2  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004ddcf5  f3 0f 11 80 28 01 00 00            movss      dword ptr [eax + 0x128], xmm0
004ddcfd  b9 6c 01 00 00                     mov        ecx, 0x16c
004ddd02  6b d1 05                           imul       edx, ecx, 5
004ddd05  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
004ddd08  8d 8c 10 78 02 00 00               lea        ecx, [eax + edx + 0x278]
004ddd0f  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004ddd12  f3 0f 10 05 94 cd 56 00            movss      xmm0, dword ptr [0x56cd94]
004ddd1a  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
004ddd22  f3 0f 2c c8                        cvttss2si  ecx, xmm0
004ddd26  03 4d 08                           add        ecx, dword ptr [ebp + 8]
004ddd29  f3 0f 10 05 a4 cd 56 00            movss      xmm0, dword ptr [0x56cda4]
004ddd31  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
004ddd39  f3 0f 2c c0                        cvttss2si  eax, xmm0
004ddd3d  99                                 cdq        
004ddd3e  2b c2                              sub        eax, edx
004ddd40  d1 f8                              sar        eax, 1
004ddd42  03 c8                              add        ecx, eax
004ddd44  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004ddd47  89 8a fc 00 00 00                  mov        dword ptr [edx + 0xfc], ecx
004ddd4d  f3 0f 10 05 90 cd 56 00            movss      xmm0, dword ptr [0x56cd90]
004ddd55  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
004ddd5d  f3 0f 2c c0                        cvttss2si  eax, xmm0
004ddd61  03 45 0c                           add        eax, dword ptr [ebp + 0xc]
004ddd64  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004ddd67  89 81 00 01 00 00                  mov        dword ptr [ecx + 0x100], eax
004ddd6d  f3 0f 10 05 94 cd 56 00            movss      xmm0, dword ptr [0x56cd94]
004ddd75  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
004ddd7d  f3 0f 2c d0                        cvttss2si  edx, xmm0
004ddd81  03 55 08                           add        edx, dword ptr [ebp + 8]
004ddd84  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004ddd87  89 90 e0 00 00 00                  mov        dword ptr [eax + 0xe0], edx
004ddd8d  f3 0f 10 05 90 cd 56 00            movss      xmm0, dword ptr [0x56cd90]
004ddd95  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
004ddd9d  f3 0f 2c c8                        cvttss2si  ecx, xmm0
004ddda1  03 4d 0c                           add        ecx, dword ptr [ebp + 0xc]
004ddda4  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004ddda7  89 8a e4 00 00 00                  mov        dword ptr [edx + 0xe4], ecx
004dddad  f3 0f 10 05 a4 cd 56 00            movss      xmm0, dword ptr [0x56cda4]
004dddb5  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
004dddbd  f3 0f 2c c0                        cvttss2si  eax, xmm0
004dddc1  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dddc4  89 81 e8 00 00 00                  mov        dword ptr [ecx + 0xe8], eax
004dddca  f3 0f 10 05 c0 d7 56 00            movss      xmm0, dword ptr [0x56d7c0]
004dddd2  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
004dddda  f3 0f 2c d0                        cvttss2si  edx, xmm0
004dddde  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004ddde1  89 90 ec 00 00 00                  mov        dword ptr [eax + 0xec], edx
004ddde7  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dddea  81 c1 e0 00 00 00                  add        ecx, 0xe0
004dddf0  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dddf3  81 c2 04 01 00 00                  add        edx, 0x104
004dddf9  8b 01                              mov        eax, dword ptr [ecx]
004dddfb  89 02                              mov        dword ptr [edx], eax
004dddfd  8b 41 04                           mov        eax, dword ptr [ecx + 4]
004dde00  89 42 04                           mov        dword ptr [edx + 4], eax
004dde03  8b 41 08                           mov        eax, dword ptr [ecx + 8]
004dde06  89 42 08                           mov        dword ptr [edx + 8], eax
004dde09  8b 41 0c                           mov        eax, dword ptr [ecx + 0xc]
004dde0c  89 42 0c                           mov        dword ptr [edx + 0xc], eax
004dde0f  8b 41 10                           mov        eax, dword ptr [ecx + 0x10]
004dde12  89 42 10                           mov        dword ptr [edx + 0x10], eax
004dde15  8b 49 14                           mov        ecx, dword ptr [ecx + 0x14]
004dde18  89 4a 14                           mov        dword ptr [edx + 0x14], ecx
004dde1b  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dde1e  8b 82 04 01 00 00                  mov        eax, dword ptr [edx + 0x104]
004dde24  89 45 dc                           mov        dword ptr [ebp - 0x24], eax
004dde27  f2 0f 2a 45 dc                     cvtsi2sd   xmm0, dword ptr [ebp - 0x24]
004dde2c  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
004dde2f  c1 e9 1f                           shr        ecx, 0x1f
004dde32  f2 0f 58 04 cd d0 cd 56 00         addsd      xmm0, qword ptr [ecx*8 + 0x56cdd0]
004dde3b  f2 0f 11 45 90                     movsd      qword ptr [ebp - 0x70], xmm0
004dde40  f2 0f 5a 45 90                     cvtsd2ss   xmm0, qword ptr [ebp - 0x70]
004dde45  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dde48  f3 0f 11 82 1c 01 00 00            movss      dword ptr [edx + 0x11c], xmm0
004dde50  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dde53  8b 88 08 01 00 00                  mov        ecx, dword ptr [eax + 0x108]
004dde59  89 4d d8                           mov        dword ptr [ebp - 0x28], ecx
004dde5c  f2 0f 2a 45 d8                     cvtsi2sd   xmm0, dword ptr [ebp - 0x28]
004dde61  8b 55 d8                           mov        edx, dword ptr [ebp - 0x28]
004dde64  c1 ea 1f                           shr        edx, 0x1f
004dde67  f2 0f 58 04 d5 d0 cd 56 00         addsd      xmm0, qword ptr [edx*8 + 0x56cdd0]
004dde70  f2 0f 11 45 88                     movsd      qword ptr [ebp - 0x78], xmm0
004dde75  f2 0f 5a 45 88                     cvtsd2ss   xmm0, qword ptr [ebp - 0x78]
004dde7a  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dde7d  f3 0f 11 80 20 01 00 00            movss      dword ptr [eax + 0x120], xmm0
004dde85  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dde88  8b 91 04 01 00 00                  mov        edx, dword ptr [ecx + 0x104]
004dde8e  89 55 d4                           mov        dword ptr [ebp - 0x2c], edx
004dde91  f2 0f 2a 45 d4                     cvtsi2sd   xmm0, dword ptr [ebp - 0x2c]
004dde96  8b 45 d4                           mov        eax, dword ptr [ebp - 0x2c]
004dde99  c1 e8 1f                           shr        eax, 0x1f
004dde9c  f2 0f 58 04 c5 d0 cd 56 00         addsd      xmm0, qword ptr [eax*8 + 0x56cdd0]
004ddea5  f2 0f 11 45 80                     movsd      qword ptr [ebp - 0x80], xmm0
004ddeaa  f2 0f 5a 45 80                     cvtsd2ss   xmm0, qword ptr [ebp - 0x80]
004ddeaf  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004ddeb2  8b 91 0c 01 00 00                  mov        edx, dword ptr [ecx + 0x10c]
004ddeb8  89 55 d0                           mov        dword ptr [ebp - 0x30], edx
004ddebb  f2 0f 2a 4d d0                     cvtsi2sd   xmm1, dword ptr [ebp - 0x30]
004ddec0  8b 45 d0                           mov        eax, dword ptr [ebp - 0x30]
004ddec3  c1 e8 1f                           shr        eax, 0x1f
004ddec6  f2 0f 58 0c c5 d0 cd 56 00         addsd      xmm1, qword ptr [eax*8 + 0x56cdd0]
004ddecf  f2 0f 11 8d 78 ff ff ff            movsd      qword ptr [ebp - 0x88], xmm1
004dded7  f2 0f 5a 8d 78 ff ff ff            cvtsd2ss   xmm1, qword ptr [ebp - 0x88]
004ddedf  f3 0f 58 c1                        addss      xmm0, xmm1
004ddee3  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004ddee6  f3 0f 11 81 24 01 00 00            movss      dword ptr [ecx + 0x124], xmm0
004ddeee  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004ddef1  8b 82 08 01 00 00                  mov        eax, dword ptr [edx + 0x108]
004ddef7  89 45 cc                           mov        dword ptr [ebp - 0x34], eax
004ddefa  f2 0f 2a 45 cc                     cvtsi2sd   xmm0, dword ptr [ebp - 0x34]
004ddeff  8b 4d cc                           mov        ecx, dword ptr [ebp - 0x34]
004ddf02  c1 e9 1f                           shr        ecx, 0x1f
004ddf05  f2 0f 58 04 cd d0 cd 56 00         addsd      xmm0, qword ptr [ecx*8 + 0x56cdd0]
004ddf0e  f2 0f 11 85 70 ff ff ff            movsd      qword ptr [ebp - 0x90], xmm0
004ddf16  f2 0f 5a 85 70 ff ff ff            cvtsd2ss   xmm0, qword ptr [ebp - 0x90]
004ddf1e  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004ddf21  8b 82 10 01 00 00                  mov        eax, dword ptr [edx + 0x110]
004ddf27  89 45 c8                           mov        dword ptr [ebp - 0x38], eax
004ddf2a  f2 0f 2a 4d c8                     cvtsi2sd   xmm1, dword ptr [ebp - 0x38]
004ddf2f  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
004ddf32  c1 e9 1f                           shr        ecx, 0x1f
004ddf35  f2 0f 58 0c cd d0 cd 56 00         addsd      xmm1, qword ptr [ecx*8 + 0x56cdd0]
004ddf3e  f2 0f 11 8d 68 ff ff ff            movsd      qword ptr [ebp - 0x98], xmm1
004ddf46  f2 0f 5a 8d 68 ff ff ff            cvtsd2ss   xmm1, qword ptr [ebp - 0x98]
004ddf4e  f3 0f 58 c1                        addss      xmm0, xmm1
004ddf52  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004ddf55  f3 0f 11 82 28 01 00 00            movss      dword ptr [edx + 0x128], xmm0
004ddf5d  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004ddf60  05 e0 00 00 00                     add        eax, 0xe0
004ddf65  50                                 push       eax
004ddf66  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004ddf69  51                                 push       ecx
004ddf6a  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004ddf6d  e8 7e c2 ff ff                     call       0x4da1f0
004ddf72  90                                 nop        
004ddf73  8b e5                              mov        esp, ebp
004ddf75  5d                                 pop        ebp
004ddf76  c2 08 00                           ret        8
