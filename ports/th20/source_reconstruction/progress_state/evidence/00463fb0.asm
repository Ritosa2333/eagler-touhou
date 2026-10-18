00463fb0  55                                 push       ebp
00463fb1  8b ec                              mov        ebp, esp
00463fb3  51                                 push       ecx
00463fb4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00463fb7  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00463fba  e8 21 01 00 00                     call       0x4640e0
00463fbf  8b c8                              mov        ecx, eax
00463fc1  e8 1a 00 00 00                     call       0x463fe0
00463fc6  8b e5                              mov        esp, ebp
00463fc8  5d                                 pop        ebp
00463fc9  c3                                 ret        
