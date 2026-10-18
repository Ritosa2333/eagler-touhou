00488720  55                                 push       ebp
00488721  8b ec                              mov        ebp, esp
00488723  51                                 push       ecx
00488724  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00488727  69 45 08 f0 00 00 00               imul       eax, dword ptr [ebp + 8], 0xf0
0048872e  03 45 fc                           add        eax, dword ptr [ebp - 4]
00488731  8b e5                              mov        esp, ebp
00488733  5d                                 pop        ebp
00488734  c2 04 00                           ret        4
