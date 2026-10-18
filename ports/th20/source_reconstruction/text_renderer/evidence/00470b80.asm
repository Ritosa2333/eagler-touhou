00470b80  55                                 push       ebp
00470b81  8b ec                              mov        ebp, esp
00470b83  83 ec 18                           sub        esp, 0x18
00470b86  56                                 push       esi
00470b87  57                                 push       edi
00470b88  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00470b8b  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00470b8e  e8 3d 71 fc ff                     call       0x437cd0
00470b93  89 45 f8                           mov        dword ptr [ebp - 8], eax
00470b96  f3 0f 2a 45 08                     cvtsi2ss   xmm0, dword ptr [ebp + 8]
00470b9b  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
00470b9e  f3 0f 5e 40 20                     divss      xmm0, dword ptr [eax + 0x20]
00470ba3  f3 0f 11 45 f4                     movss      dword ptr [ebp - 0xc], xmm0
00470ba8  b9 08 00 00 00                     mov        ecx, 8
00470bad  d1 e1                              shl        ecx, 1
00470baf  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00470bb2  f3 0f 10 45 f4                     movss      xmm0, dword ptr [ebp - 0xc]
00470bb7  f3 0f 11 84 0a 78 03 00 00         movss      dword ptr [edx + ecx + 0x378], xmm0
00470bc0  b8 08 00 00 00                     mov        eax, 8
00470bc5  6b c8 00                           imul       ecx, eax, 0
00470bc8  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00470bcb  f3 0f 10 45 f4                     movss      xmm0, dword ptr [ebp - 0xc]
00470bd0  f3 0f 11 84 0a 78 03 00 00         movss      dword ptr [edx + ecx + 0x378], xmm0
00470bd9  8b 45 08                           mov        eax, dword ptr [ebp + 8]
00470bdc  03 45 10                           add        eax, dword ptr [ebp + 0x10]
00470bdf  f3 0f 2a c0                        cvtsi2ss   xmm0, eax
00470be3  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
00470be6  f3 0f 5e 41 20                     divss      xmm0, dword ptr [ecx + 0x20]
00470beb  f3 0f 11 45 f0                     movss      dword ptr [ebp - 0x10], xmm0
00470bf0  ba 08 00 00 00                     mov        edx, 8
00470bf5  6b c2 03                           imul       eax, edx, 3
00470bf8  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00470bfb  f3 0f 10 45 f0                     movss      xmm0, dword ptr [ebp - 0x10]
00470c00  f3 0f 11 84 01 78 03 00 00         movss      dword ptr [ecx + eax + 0x378], xmm0
00470c09  ba 08 00 00 00                     mov        edx, 8
00470c0e  c1 e2 00                           shl        edx, 0
00470c11  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00470c14  f3 0f 10 45 f0                     movss      xmm0, dword ptr [ebp - 0x10]
00470c19  f3 0f 11 84 10 78 03 00 00         movss      dword ptr [eax + edx + 0x378], xmm0
00470c22  f3 0f 2a 45 0c                     cvtsi2ss   xmm0, dword ptr [ebp + 0xc]
00470c27  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
00470c2a  f3 0f 5e 41 1c                     divss      xmm0, dword ptr [ecx + 0x1c]
00470c2f  f3 0f 11 45 ec                     movss      dword ptr [ebp - 0x14], xmm0
00470c34  ba 08 00 00 00                     mov        edx, 8
00470c39  c1 e2 00                           shl        edx, 0
00470c3c  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00470c3f  f3 0f 10 45 ec                     movss      xmm0, dword ptr [ebp - 0x14]
00470c44  f3 0f 11 84 10 7c 03 00 00         movss      dword ptr [eax + edx + 0x37c], xmm0
00470c4d  b9 08 00 00 00                     mov        ecx, 8
00470c52  6b d1 00                           imul       edx, ecx, 0
00470c55  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00470c58  f3 0f 10 45 ec                     movss      xmm0, dword ptr [ebp - 0x14]
00470c5d  f3 0f 11 84 10 7c 03 00 00         movss      dword ptr [eax + edx + 0x37c], xmm0
00470c66  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
00470c69  03 4d 14                           add        ecx, dword ptr [ebp + 0x14]
00470c6c  f3 0f 2a c1                        cvtsi2ss   xmm0, ecx
00470c70  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
00470c73  f3 0f 5e 42 20                     divss      xmm0, dword ptr [edx + 0x20]
00470c78  f3 0f 11 45 e8                     movss      dword ptr [ebp - 0x18], xmm0
00470c7d  b8 08 00 00 00                     mov        eax, 8
00470c82  6b c8 03                           imul       ecx, eax, 3
00470c85  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00470c88  f3 0f 10 45 e8                     movss      xmm0, dword ptr [ebp - 0x18]
00470c8d  f3 0f 11 84 0a 7c 03 00 00         movss      dword ptr [edx + ecx + 0x37c], xmm0
00470c96  b8 08 00 00 00                     mov        eax, 8
00470c9b  d1 e0                              shl        eax, 1
00470c9d  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00470ca0  f3 0f 10 45 e8                     movss      xmm0, dword ptr [ebp - 0x18]
00470ca5  f3 0f 11 84 01 7c 03 00 00         movss      dword ptr [ecx + eax + 0x37c], xmm0
00470cae  f3 0f 2a 45 10                     cvtsi2ss   xmm0, dword ptr [ebp + 0x10]
00470cb3  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
00470cb6  f3 0f 5e 42 20                     divss      xmm0, dword ptr [edx + 0x20]
00470cbb  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00470cbe  f3 0f 11 80 98 03 00 00            movss      dword ptr [eax + 0x398], xmm0
00470cc6  f3 0f 2a 45 14                     cvtsi2ss   xmm0, dword ptr [ebp + 0x14]
00470ccb  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
00470cce  f3 0f 5e 41 1c                     divss      xmm0, dword ptr [ecx + 0x1c]
00470cd3  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00470cd6  f3 0f 11 82 9c 03 00 00            movss      dword ptr [edx + 0x39c], xmm0
00470cde  f3 0f 2a 45 10                     cvtsi2ss   xmm0, dword ptr [ebp + 0x10]
00470ce3  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00470ce6  f3 0f 11 40 70                     movss      dword ptr [eax + 0x70], xmm0
00470ceb  f3 0f 2a 45 14                     cvtsi2ss   xmm0, dword ptr [ebp + 0x14]
00470cf0  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00470cf3  f3 0f 11 41 74                     movss      dword ptr [ecx + 0x74], xmm0
00470cf8  8b 7d fc                           mov        edi, dword ptr [ebp - 4]
00470cfb  81 c7 b8 03 00 00                  add        edi, 0x3b8
00470d01  b9 10 00 00 00                     mov        ecx, 0x10
00470d06  be 08 f3 56 00                     mov        esi, 0x56f308
00470d0b  f3 a5                              rep movsd  dword ptr es:[edi], dword ptr [esi]
00470d0d  8b 7d fc                           mov        edi, dword ptr [ebp - 4]
00470d10  81 c7 f8 03 00 00                  add        edi, 0x3f8
00470d16  b9 10 00 00 00                     mov        ecx, 0x10
00470d1b  be 08 f3 56 00                     mov        esi, 0x56f308
00470d20  f3 a5                              rep movsd  dword ptr es:[edi], dword ptr [esi]
00470d22  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00470d25  f3 0f 10 42 70                     movss      xmm0, dword ptr [edx + 0x70]
00470d2a  f3 0f 5e 05 a0 cd 56 00            divss      xmm0, dword ptr [0x56cda0]
00470d32  b8 10 00 00 00                     mov        eax, 0x10
00470d37  6b c8 00                           imul       ecx, eax, 0
00470d3a  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00470d3d  8d 84 0a b8 03 00 00               lea        eax, [edx + ecx + 0x3b8]
00470d44  b9 04 00 00 00                     mov        ecx, 4
00470d49  6b d1 00                           imul       edx, ecx, 0
00470d4c  f3 0f 11 04 10                     movss      dword ptr [eax + edx], xmm0
00470d51  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00470d54  f3 0f 10 40 74                     movss      xmm0, dword ptr [eax + 0x74]
00470d59  f3 0f 5e 05 a0 cd 56 00            divss      xmm0, dword ptr [0x56cda0]
00470d61  b9 10 00 00 00                     mov        ecx, 0x10
00470d66  c1 e1 00                           shl        ecx, 0
00470d69  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00470d6c  8d 84 0a b8 03 00 00               lea        eax, [edx + ecx + 0x3b8]
00470d73  b9 04 00 00 00                     mov        ecx, 4
00470d78  c1 e1 00                           shl        ecx, 0
00470d7b  f3 0f 11 04 08                     movss      dword ptr [eax + ecx], xmm0
00470d80  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00470d83  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
00470d86  f3 0f 10 42 70                     movss      xmm0, dword ptr [edx + 0x70]
00470d8b  f3 0f 5e 40 20                     divss      xmm0, dword ptr [eax + 0x20]
00470d90  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
00470d93  f3 0f 59 41 50                     mulss      xmm0, dword ptr [ecx + 0x50]
00470d98  ba 10 00 00 00                     mov        edx, 0x10
00470d9d  6b c2 00                           imul       eax, edx, 0
00470da0  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00470da3  8d 94 01 f8 03 00 00               lea        edx, [ecx + eax + 0x3f8]
00470daa  b8 04 00 00 00                     mov        eax, 4
00470daf  6b c8 00                           imul       ecx, eax, 0
00470db2  f3 0f 11 04 0a                     movss      dword ptr [edx + ecx], xmm0
00470db7  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00470dba  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
00470dbd  f3 0f 10 42 74                     movss      xmm0, dword ptr [edx + 0x74]
00470dc2  f3 0f 5e 40 1c                     divss      xmm0, dword ptr [eax + 0x1c]
00470dc7  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
00470dca  f3 0f 59 41 54                     mulss      xmm0, dword ptr [ecx + 0x54]
00470dcf  ba 10 00 00 00                     mov        edx, 0x10
00470dd4  c1 e2 00                           shl        edx, 0
00470dd7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00470dda  8d 8c 10 f8 03 00 00               lea        ecx, [eax + edx + 0x3f8]
00470de1  ba 04 00 00 00                     mov        edx, 4
00470de6  c1 e2 00                           shl        edx, 0
00470de9  f3 0f 11 04 11                     movss      dword ptr [ecx + edx], xmm0
00470dee  8b 75 fc                           mov        esi, dword ptr [ebp - 4]
00470df1  81 c6 b8 03 00 00                  add        esi, 0x3b8
00470df7  8b 7d fc                           mov        edi, dword ptr [ebp - 4]
00470dfa  81 c7 7c 05 00 00                  add        edi, 0x57c
00470e00  b9 10 00 00 00                     mov        ecx, 0x10
00470e05  f3 a5                              rep movsd  dword ptr es:[edi], dword ptr [esi]
00470e07  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00470e0a  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00470e0d  89 88 a8 03 00 00                  mov        dword ptr [eax + 0x3a8], ecx
00470e13  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00470e16  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
00470e19  89 82 ac 03 00 00                  mov        dword ptr [edx + 0x3ac], eax
00470e1f  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00470e22  03 4d 10                           add        ecx, dword ptr [ebp + 0x10]
00470e25  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00470e28  89 8a b0 03 00 00                  mov        dword ptr [edx + 0x3b0], ecx
00470e2e  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
00470e31  03 45 14                           add        eax, dword ptr [ebp + 0x14]
00470e34  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00470e37  89 81 b4 03 00 00                  mov        dword ptr [ecx + 0x3b4], eax
00470e3d  5f                                 pop        edi
00470e3e  5e                                 pop        esi
00470e3f  8b e5                              mov        esp, ebp
00470e41  5d                                 pop        ebp
00470e42  c2 10 00                           ret        0x10
