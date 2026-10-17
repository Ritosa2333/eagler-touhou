0041dfb0  55                                 push       ebp
0041dfb1  8b ec                              mov        ebp, esp
0041dfb3  51                                 push       ecx
0041dfb4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041dfb7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041dfba  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0041dfbd  89 48 58                           mov        dword ptr [eax + 0x58], ecx
0041dfc0  8b e5                              mov        esp, ebp
0041dfc2  5d                                 pop        ebp
0041dfc3  c2 04 00                           ret        4
