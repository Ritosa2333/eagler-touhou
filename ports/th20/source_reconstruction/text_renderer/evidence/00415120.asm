00415120  55                                 push       ebp
00415121  8b ec                              mov        ebp, esp
00415123  81 ec a8 00 00 00                  sub        esp, 0xa8
00415129  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0041512e  33 c5                              xor        eax, ebp
00415130  89 45 fc                           mov        dword ptr [ebp - 4], eax
00415133  89 4d 88                           mov        dword ptr [ebp - 0x78], ecx
00415136  8b 4d 88                           mov        ecx, dword ptr [ebp - 0x78]
00415139  e8 22 12 00 00                     call       0x416360
0041513e  6a 6c                              push       0x6c
00415140  6a 00                              push       0
00415142  8d 45 90                           lea        eax, [ebp - 0x70]
00415145  50                                 push       eax
00415146  e8 15 f6 12 00                     call       0x544760
0041514b  83 c4 0c                           add        esp, 0xc
0041514e  8b 4d 10                           mov        ecx, dword ptr [ebp + 0x10]
00415151  51                                 push       ecx
00415152  8b 4d 88                           mov        ecx, dword ptr [ebp - 0x78]
00415155  e8 e6 04 00 00                     call       0x415640
0041515a  89 45 84                           mov        dword ptr [ebp - 0x7c], eax
0041515d  83 7d 84 00                        cmp        dword ptr [ebp - 0x7c], 0
00415161  75 07                              jne        0x41516a
00415163  32 c0                              xor        al, al
00415165  e9 4f 02 00 00                     jmp        0x4153b9
0041516a  8b 55 84                           mov        edx, dword ptr [ebp - 0x7c]
0041516d  8b 45 08                           mov        eax, dword ptr [ebp + 8]
00415170  0f af 42 04                        imul       eax, dword ptr [edx + 4]
00415174  99                                 cdq        
00415175  83 e2 07                           and        edx, 7
00415178  03 c2                              add        eax, edx
0041517a  c1 f8 03                           sar        eax, 3
0041517d  83 c0 03                           add        eax, 3
00415180  99                                 cdq        
00415181  83 e2 03                           and        edx, 3
00415184  03 c2                              add        eax, edx
00415186  c1 f8 02                           sar        eax, 2
00415189  c1 e0 02                           shl        eax, 2
0041518c  89 85 64 ff ff ff                  mov        dword ptr [ebp - 0x9c], eax
00415192  c7 45 90 6c 00 00 00               mov        dword ptr [ebp - 0x70], 0x6c
00415199  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0041519c  89 45 94                           mov        dword ptr [ebp - 0x6c], eax
0041519f  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
004151a2  83 c1 01                           add        ecx, 1
004151a5  f7 d9                              neg        ecx
004151a7  89 4d 98                           mov        dword ptr [ebp - 0x68], ecx
004151aa  ba 01 00 00 00                     mov        edx, 1
004151af  66 89 55 9c                        mov        word ptr [ebp - 0x64], dx
004151b3  8b 45 84                           mov        eax, dword ptr [ebp - 0x7c]
004151b6  66 8b 48 04                        mov        cx, word ptr [eax + 4]
004151ba  66 89 4d 9e                        mov        word ptr [ebp - 0x62], cx
004151be  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
004151c1  0f af 95 64 ff ff ff               imul       edx, dword ptr [ebp - 0x9c]
004151c8  89 55 a4                           mov        dword ptr [ebp - 0x5c], edx
004151cb  83 7d 10 18                        cmp        dword ptr [ebp + 0x10], 0x18
004151cf  74 31                              je         0x415202
004151d1  83 7d 10 16                        cmp        dword ptr [ebp + 0x10], 0x16
004151d5  74 2b                              je         0x415202
004151d7  c7 45 a0 03 00 00 00               mov        dword ptr [ebp - 0x60], 3
004151de  8b 45 84                           mov        eax, dword ptr [ebp - 0x7c]
004151e1  8b 48 0c                           mov        ecx, dword ptr [eax + 0xc]
004151e4  89 4d b8                           mov        dword ptr [ebp - 0x48], ecx
004151e7  8b 55 84                           mov        edx, dword ptr [ebp - 0x7c]
004151ea  8b 42 10                           mov        eax, dword ptr [edx + 0x10]
004151ed  89 45 bc                           mov        dword ptr [ebp - 0x44], eax
004151f0  8b 4d 84                           mov        ecx, dword ptr [ebp - 0x7c]
004151f3  8b 51 14                           mov        edx, dword ptr [ecx + 0x14]
004151f6  89 55 c0                           mov        dword ptr [ebp - 0x40], edx
004151f9  8b 45 84                           mov        eax, dword ptr [ebp - 0x7c]
004151fc  8b 48 08                           mov        ecx, dword ptr [eax + 8]
004151ff  89 4d c4                           mov        dword ptr [ebp - 0x3c], ecx
00415202  c7 45 8c 00 00 00 00               mov        dword ptr [ebp - 0x74], 0
00415209  6a 00                              push       0
0041520b  6a 00                              push       0
0041520d  8d 55 8c                           lea        edx, [ebp - 0x74]
00415210  52                                 push       edx
00415211  6a 00                              push       0
00415213  8d 45 90                           lea        eax, [ebp - 0x70]
00415216  50                                 push       eax
00415217  6a 00                              push       0
00415219  ff 15 20 c0 56 00                  call       dword ptr [0x56c020] ; GDI32.dll!CreateDIBSection
0041521f  89 85 7c ff ff ff                  mov        dword ptr [ebp - 0x84], eax
00415225  83 bd 7c ff ff ff 00               cmp        dword ptr [ebp - 0x84], 0
0041522c  75 07                              jne        0x415235
0041522e  32 c0                              xor        al, al
00415230  e9 84 01 00 00                     jmp        0x4153b9
00415235  6a 00                              push       0
00415237  ff 15 44 c0 56 00                  call       dword ptr [0x56c044] ; GDI32.dll!CreateCompatibleDC
0041523d  89 85 68 ff ff ff                  mov        dword ptr [ebp - 0x98], eax
00415243  8b 8d 7c ff ff ff                  mov        ecx, dword ptr [ebp - 0x84]
00415249  51                                 push       ecx
0041524a  8b 95 68 ff ff ff                  mov        edx, dword ptr [ebp - 0x98]
00415250  52                                 push       edx
00415251  ff 15 2c c0 56 00                  call       dword ptr [0x56c02c] ; GDI32.dll!SelectObject
00415257  89 85 5c ff ff ff                  mov        dword ptr [ebp - 0xa4], eax
0041525d  8b 45 88                           mov        eax, dword ptr [ebp - 0x78]
00415260  8b 8d 68 ff ff ff                  mov        ecx, dword ptr [ebp - 0x98]
00415266  89 88 14 01 00 00                  mov        dword ptr [eax + 0x114], ecx
0041526c  8b 55 88                           mov        edx, dword ptr [ebp - 0x78]
0041526f  8b 85 7c ff ff ff                  mov        eax, dword ptr [ebp - 0x84]
00415275  89 82 1c 01 00 00                  mov        dword ptr [edx + 0x11c], eax
0041527b  8b 4d 88                           mov        ecx, dword ptr [ebp - 0x78]
0041527e  8b 55 8c                           mov        edx, dword ptr [ebp - 0x74]
00415281  89 91 20 01 00 00                  mov        dword ptr [ecx + 0x120], edx
00415287  8b 45 88                           mov        eax, dword ptr [ebp - 0x78]
0041528a  8b 4d a4                           mov        ecx, dword ptr [ebp - 0x5c]
0041528d  89 88 0c 01 00 00                  mov        dword ptr [eax + 0x10c], ecx
00415293  8b 55 88                           mov        edx, dword ptr [ebp - 0x78]
00415296  8b 85 5c ff ff ff                  mov        eax, dword ptr [ebp - 0xa4]
0041529c  89 82 18 01 00 00                  mov        dword ptr [edx + 0x118], eax
004152a2  8b 4d 88                           mov        ecx, dword ptr [ebp - 0x78]
004152a5  8b 55 08                           mov        edx, dword ptr [ebp + 8]
004152a8  89 91 04 01 00 00                  mov        dword ptr [ecx + 0x104], edx
004152ae  8b 45 88                           mov        eax, dword ptr [ebp - 0x78]
004152b1  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
004152b4  89 88 08 01 00 00                  mov        dword ptr [eax + 0x108], ecx
004152ba  8b 55 88                           mov        edx, dword ptr [ebp - 0x78]
004152bd  8b 45 10                           mov        eax, dword ptr [ebp + 0x10]
004152c0  89 82 00 01 00 00                  mov        dword ptr [edx + 0x100], eax
004152c6  8b 4d 88                           mov        ecx, dword ptr [ebp - 0x78]
004152c9  8b 95 64 ff ff ff                  mov        edx, dword ptr [ebp - 0x9c]
004152cf  89 91 10 01 00 00                  mov        dword ptr [ecx + 0x110], edx
004152d5  8b 45 88                           mov        eax, dword ptr [ebp - 0x78]
004152d8  8b 88 00 01 00 00                  mov        ecx, dword ptr [eax + 0x100]
004152de  89 8d 60 ff ff ff                  mov        dword ptr [ebp - 0xa0], ecx
004152e4  83 bd 60 ff ff ff 15               cmp        dword ptr [ebp - 0xa0], 0x15
004152eb  74 0e                              je         0x4152fb
004152ed  83 bd 60 ff ff ff 1a               cmp        dword ptr [ebp - 0xa0], 0x1a
004152f4  74 65                              je         0x41535b
004152f6  e9 bc 00 00 00                     jmp        0x4153b7
004152fb  8b 55 8c                           mov        edx, dword ptr [ebp - 0x74]
004152fe  89 95 74 ff ff ff                  mov        dword ptr [ebp - 0x8c], edx
00415304  c7 85 58 ff ff ff 00 00 00 ff      mov        dword ptr [ebp - 0xa8], 0xff000000
0041530e  c7 85 78 ff ff ff 00 00 00 00      mov        dword ptr [ebp - 0x88], 0
00415318  eb 1e                              jmp        0x415338
0041531a  8b 85 78 ff ff ff                  mov        eax, dword ptr [ebp - 0x88]
00415320  83 c0 04                           add        eax, 4
00415323  89 85 78 ff ff ff                  mov        dword ptr [ebp - 0x88], eax
00415329  8b 8d 74 ff ff ff                  mov        ecx, dword ptr [ebp - 0x8c]
0041532f  83 c1 04                           add        ecx, 4
00415332  89 8d 74 ff ff ff                  mov        dword ptr [ebp - 0x8c], ecx
00415338  8b 55 88                           mov        edx, dword ptr [ebp - 0x78]
0041533b  8b 85 78 ff ff ff                  mov        eax, dword ptr [ebp - 0x88]
00415341  3b 82 0c 01 00 00                  cmp        eax, dword ptr [edx + 0x10c]
00415347  7d 10                              jge        0x415359
00415349  8b 8d 74 ff ff ff                  mov        ecx, dword ptr [ebp - 0x8c]
0041534f  8b 95 58 ff ff ff                  mov        edx, dword ptr [ebp - 0xa8]
00415355  89 11                              mov        dword ptr [ecx], edx
00415357  eb c1                              jmp        0x41531a
00415359  eb 5c                              jmp        0x4153b7
0041535b  8b 45 8c                           mov        eax, dword ptr [ebp - 0x74]
0041535e  89 85 6c ff ff ff                  mov        dword ptr [ebp - 0x94], eax
00415364  b9 00 f0 00 00                     mov        ecx, 0xf000
00415369  66 89 4d 80                        mov        word ptr [ebp - 0x80], cx
0041536d  c7 85 70 ff ff ff 00 00 00 00      mov        dword ptr [ebp - 0x90], 0
00415377  eb 1e                              jmp        0x415397
00415379  8b 95 70 ff ff ff                  mov        edx, dword ptr [ebp - 0x90]
0041537f  83 c2 02                           add        edx, 2
00415382  89 95 70 ff ff ff                  mov        dword ptr [ebp - 0x90], edx
00415388  8b 85 6c ff ff ff                  mov        eax, dword ptr [ebp - 0x94]
0041538e  83 c0 02                           add        eax, 2
00415391  89 85 6c ff ff ff                  mov        dword ptr [ebp - 0x94], eax
00415397  8b 4d 88                           mov        ecx, dword ptr [ebp - 0x78]
0041539a  8b 95 70 ff ff ff                  mov        edx, dword ptr [ebp - 0x90]
004153a0  3b 91 0c 01 00 00                  cmp        edx, dword ptr [ecx + 0x10c]
004153a6  7d 0f                              jge        0x4153b7
004153a8  8b 85 6c ff ff ff                  mov        eax, dword ptr [ebp - 0x94]
004153ae  66 8b 4d 80                        mov        cx, word ptr [ebp - 0x80]
004153b2  66 89 08                           mov        word ptr [eax], cx
004153b5  eb c2                              jmp        0x415379
004153b7  b0 01                              mov        al, 1
004153b9  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004153bc  33 cd                              xor        ecx, ebp
004153be  e8 2b d6 12 00                     call       0x5429ee
004153c3  8b e5                              mov        esp, ebp
004153c5  5d                                 pop        ebp
004153c6  c2 0c 00                           ret        0xc
