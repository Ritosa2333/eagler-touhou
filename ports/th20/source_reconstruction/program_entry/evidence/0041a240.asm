; release Direct3D device
; source SHA-256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
0041a240  55                                 push       ebp
0041a241  8b ec                              mov        ebp, esp
0041a243  51                                 push       ecx
0041a244  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041a247  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041a24a  83 78 08 00                        cmp        dword ptr [eax + 8], 0
0041a24e  74 1e                              je         0x41a26e
0041a250  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041a253  8b 51 08                           mov        edx, dword ptr [ecx + 8]
0041a256  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041a259  8b 48 08                           mov        ecx, dword ptr [eax + 8]
0041a25c  8b 12                              mov        edx, dword ptr [edx]
0041a25e  51                                 push       ecx
0041a25f  8b 42 08                           mov        eax, dword ptr [edx + 8]
0041a262  ff d0                              call       eax
0041a264  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041a267  c7 41 08 00 00 00 00               mov        dword ptr [ecx + 8], 0
0041a26e  8b e5                              mov        esp, ebp
0041a270  5d                                 pop        ebp
0041a271  c3                                 ret
