0041def0  55                                 push       ebp
0041def1  8b ec                              mov        ebp, esp
0041def3  51                                 push       ecx
0041def4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041def7  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0041defa  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041defd  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0041df00  89 54 81 50                        mov        dword ptr [ecx + eax*4 + 0x50], edx
0041df04  8b e5                              mov        esp, ebp
0041df06  5d                                 pop        ebp
0041df07  c2 08 00                           ret        8
