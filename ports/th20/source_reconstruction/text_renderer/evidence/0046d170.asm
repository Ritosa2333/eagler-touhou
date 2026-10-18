0046d170  55                                 push       ebp
0046d171  8b ec                              mov        ebp, esp
0046d173  83 ec 10                           sub        esp, 0x10
0046d176  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0046d179  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046d17c  83 b8 f0 a1 01 00 00               cmp        dword ptr [eax + 0x1a1f0], 0
0046d183  75 09                              jne        0x46d18e
0046d185  c7 45 f8 01 00 00 00               mov        dword ptr [ebp - 8], 1
0046d18c  eb 0c                              jmp        0x46d19a
0046d18e  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046d191  8b 91 f0 a1 01 00                  mov        edx, dword ptr [ecx + 0x1a1f0]
0046d197  89 55 f8                           mov        dword ptr [ebp - 8], edx
0046d19a  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0046d19d  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0046d1a0  89 88 20 06 00 00                  mov        dword ptr [eax + 0x620], ecx
0046d1a6  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046d1a9  f3 0f 10 02                        movss      xmm0, dword ptr [edx]
0046d1ad  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046d1b5  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0046d1b8  f3 0f 11 80 14 06 00 00            movss      dword ptr [eax + 0x614], xmm0
0046d1c0  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046d1c3  f3 0f 10 41 04                     movss      xmm0, dword ptr [ecx + 4]
0046d1c8  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046d1d0  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0046d1d3  f3 0f 11 82 18 06 00 00            movss      dword ptr [edx + 0x618], xmm0
0046d1db  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0046d1de  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046d1e1  8b 91 c0 a1 01 00                  mov        edx, dword ptr [ecx + 0x1a1c0]
0046d1e7  89 90 4c 06 00 00                  mov        dword ptr [eax + 0x64c], edx
0046d1ed  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0046d1f0  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046d1f3  8b 91 c8 a1 01 00                  mov        edx, dword ptr [ecx + 0x1a1c8]
0046d1f9  89 90 50 06 00 00                  mov        dword ptr [eax + 0x650], edx
0046d1ff  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0046d202  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046d205  8b 91 c4 a1 01 00                  mov        edx, dword ptr [ecx + 0x1a1c4]
0046d20b  89 90 54 06 00 00                  mov        dword ptr [eax + 0x654], edx
0046d211  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0046d214  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046d217  8b 91 ec a1 01 00                  mov        edx, dword ptr [ecx + 0x1a1ec]
0046d21d  89 90 24 06 00 00                  mov        dword ptr [eax + 0x624], edx
0046d223  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0046d226  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046d229  8b 91 cc a1 01 00                  mov        edx, dword ptr [ecx + 0x1a1cc]
0046d22f  89 90 58 06 00 00                  mov        dword ptr [eax + 0x658], edx
0046d235  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0046d238  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046d23b  8b 91 d0 a1 01 00                  mov        edx, dword ptr [ecx + 0x1a1d0]
0046d241  89 90 5c 06 00 00                  mov        dword ptr [eax + 0x65c], edx
0046d247  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0046d24a  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046d24d  8b 91 f4 a1 01 00                  mov        edx, dword ptr [ecx + 0x1a1f4]
0046d253  89 90 60 06 00 00                  mov        dword ptr [eax + 0x660], edx
0046d259  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0046d25c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046d25f  8b 91 f8 a1 01 00                  mov        edx, dword ptr [ecx + 0x1a1f8]
0046d265  89 90 64 06 00 00                  mov        dword ptr [eax + 0x664], edx
0046d26b  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046d26e  8b 88 08 a2 01 00                  mov        ecx, dword ptr [eax + 0x1a208]
0046d274  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0046d277  89 8a 6c 06 00 00                  mov        dword ptr [edx + 0x66c], ecx
0046d27d  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0046d280  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046d283  8b 91 dc a1 01 00                  mov        edx, dword ptr [ecx + 0x1a1dc]
0046d289  89 90 70 06 00 00                  mov        dword ptr [eax + 0x670], edx
0046d28f  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0046d292  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046d295  8b 91 f0 a1 01 00                  mov        edx, dword ptr [ecx + 0x1a1f0]
0046d29b  89 90 74 06 00 00                  mov        dword ptr [eax + 0x674], edx
0046d2a1  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0046d2a4  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046d2a7  8b 91 e8 a1 01 00                  mov        edx, dword ptr [ecx + 0x1a1e8]
0046d2ad  89 90 30 06 00 00                  mov        dword ptr [eax + 0x630], edx
0046d2b3  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046d2b6  8a 88 b4 04 00 00                  mov        cl, byte ptr [eax + 0x4b4]
0046d2bc  80 e1 00                           and        cl, 0
0046d2bf  80 c9 06                           or         cl, 6
0046d2c2  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046d2c5  88 8a b4 04 00 00                  mov        byte ptr [edx + 0x4b4], cl
0046d2cb  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0046d2ce  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
0046d2d1  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
0046d2d4  51                                 push       ecx
0046d2d5  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0046d2d8  e8 93 d3 fd ff                     call       0x44a670
0046d2dd  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046d2e0  81 c2 0c a2 01 00                  add        edx, 0x1a20c
0046d2e6  89 55 f0                           mov        dword ptr [ebp - 0x10], edx
0046d2e9  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0046d2ec  50                                 push       eax
0046d2ed  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046d2f0  e8 ab 4b fa ff                     call       0x411ea0
0046d2f5  90                                 nop        
0046d2f6  8b e5                              mov        esp, ebp
0046d2f8  5d                                 pop        ebp
0046d2f9  c2 08 00                           ret        8
