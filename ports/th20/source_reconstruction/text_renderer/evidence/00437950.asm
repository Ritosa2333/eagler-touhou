00437950  55                                 push       ebp
00437951  8b ec                              mov        ebp, esp
00437953  51                                 push       ecx
00437954  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00437957  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0043795a  8b 80 44 a2 01 00                  mov        eax, dword ptr [eax + 0x1a244]
00437960  8b e5                              mov        esp, ebp
00437962  5d                                 pop        ebp
00437963  c3                                 ret        
