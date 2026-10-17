00539060  55                                 push       ebp
00539061  8b ec                              mov        ebp, esp
00539063  83 ec 14                           sub        esp, 0x14
00539066  c7 45 ec 00 00 00 00               mov        dword ptr [ebp - 0x14], 0
0053906d  83 7d 08 00                        cmp        dword ptr [ebp + 8], 0
00539071  75 07                              jne        0x53907a
00539073  33 c0                              xor        eax, eax
00539075  e9 fe 00 00 00                     jmp        0x539178
0053907a  b8 0c 00 00 00                     mov        eax, 0xc
0053907f  c1 e0 0d                           shl        eax, 0xd
00539082  8b 88 40 8b 5c 00                  mov        ecx, dword ptr [eax + 0x5c8b40]
00539088  89 4d f8                           mov        dword ptr [ebp - 8], ecx
0053908b  c7 45 f4 00 00 00 00               mov        dword ptr [ebp - 0xc], 0
00539092  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
00539099  eb 09                              jmp        0x5390a4
0053909b  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0053909e  83 c2 01                           add        edx, 1
005390a1  89 55 fc                           mov        dword ptr [ebp - 4], edx
005390a4  83 7d fc 12                        cmp        dword ptr [ebp - 4], 0x12
005390a8  7d 30                              jge        0x5390da
005390aa  8b 45 08                           mov        eax, dword ptr [ebp + 8]
005390ad  03 45 fc                           add        eax, dword ptr [ebp - 4]
005390b0  25 ff 1f 00 00                     and        eax, 0x1fff
005390b5  0f b6 88 38 6b 5c 00               movzx      ecx, byte ptr [eax + 0x5c6b38]
005390bc  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
005390bf  03 55 fc                           add        edx, dword ptr [ebp - 4]
005390c2  81 e2 ff 1f 00 00                  and        edx, 0x1fff
005390c8  0f b6 82 38 6b 5c 00               movzx      eax, byte ptr [edx + 0x5c6b38]
005390cf  2b c8                              sub        ecx, eax
005390d1  89 4d ec                           mov        dword ptr [ebp - 0x14], ecx
005390d4  74 02                              je         0x5390d8
005390d6  eb 02                              jmp        0x5390da
005390d8  eb c1                              jmp        0x53909b
005390da  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
005390dd  3b 4d f4                           cmp        ecx, dword ptr [ebp - 0xc]
005390e0  7c 29                              jl         0x53910b
005390e2  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
005390e5  89 55 f4                           mov        dword ptr [ebp - 0xc], edx
005390e8  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
005390eb  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
005390ee  89 08                              mov        dword ptr [eax], ecx
005390f0  83 7d f4 12                        cmp        dword ptr [ebp - 0xc], 0x12
005390f4  7c 15                              jl         0x53910b
005390f6  8b 55 08                           mov        edx, dword ptr [ebp + 8]
005390f9  52                                 push       edx
005390fa  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
005390fd  50                                 push       eax
005390fe  e8 0d 09 00 00                     call       0x539a10
00539103  83 c4 08                           add        esp, 8
00539106  8b 45 f4                           mov        eax, dword ptr [ebp - 0xc]
00539109  eb 6d                              jmp        0x539178
0053910b  83 7d ec 00                        cmp        dword ptr [ebp - 0x14], 0
0053910f  7c 0f                              jl         0x539120
00539111  6b 4d f8 0c                        imul       ecx, dword ptr [ebp - 8], 0xc
00539115  81 c1 40 8b 5c 00                  add        ecx, 0x5c8b40
0053911b  89 4d f0                           mov        dword ptr [ebp - 0x10], ecx
0053911e  eb 0d                              jmp        0x53912d
00539120  6b 55 f8 0c                        imul       edx, dword ptr [ebp - 8], 0xc
00539124  81 c2 3c 8b 5c 00                  add        edx, 0x5c8b3c
0053912a  89 55 f0                           mov        dword ptr [ebp - 0x10], edx
0053912d  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00539130  83 38 00                           cmp        dword ptr [eax], 0
00539133  75 36                              jne        0x53916b
00539135  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00539138  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0053913b  89 11                              mov        dword ptr [ecx], edx
0053913d  6b 45 08 0c                        imul       eax, dword ptr [ebp + 8], 0xc
00539141  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
00539144  89 88 38 8b 5c 00                  mov        dword ptr [eax + 0x5c8b38], ecx
0053914a  6b 55 08 0c                        imul       edx, dword ptr [ebp + 8], 0xc
0053914e  c7 82 40 8b 5c 00 00 00 00 00      mov        dword ptr [edx + 0x5c8b40], 0
00539158  6b 45 08 0c                        imul       eax, dword ptr [ebp + 8], 0xc
0053915c  c7 80 3c 8b 5c 00 00 00 00 00      mov        dword ptr [eax + 0x5c8b3c], 0
00539166  8b 45 f4                           mov        eax, dword ptr [ebp - 0xc]
00539169  eb 0d                              jmp        0x539178
0053916b  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0053916e  8b 11                              mov        edx, dword ptr [ecx]
00539170  89 55 f8                           mov        dword ptr [ebp - 8], edx
00539173  e9 1a ff ff ff                     jmp        0x539092
00539178  8b e5                              mov        esp, ebp
0053917a  5d                                 pop        ebp
0053917b  c3                                 ret        
