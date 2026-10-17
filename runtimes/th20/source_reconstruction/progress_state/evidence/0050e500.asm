0050e500  55                                 push       ebp
0050e501  8b ec                              mov        ebp, esp
0050e503  51                                 push       ecx
0050e504  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0050e507  33 c0                              xor        eax, eax
0050e509  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050e50c  66 89 01                           mov        word ptr [ecx], ax
0050e50f  33 d2                              xor        edx, edx
0050e511  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e514  66 89 50 02                        mov        word ptr [eax + 2], dx
0050e518  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050e51b  c7 41 04 00 00 00 00               mov        dword ptr [ecx + 4], 0
0050e522  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050e525  c7 42 08 00 00 00 00               mov        dword ptr [edx + 8], 0
0050e52c  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e52f  8b e5                              mov        esp, ebp
0050e531  5d                                 pop        ebp
0050e532  c3                                 ret        
