00414820  55                                 push       ebp
00414821  8b ec                              mov        ebp, esp
00414823  a1 48 67 5b 00                     mov        eax, dword ptr [0x5b6748]
00414828  c6 00 01                           mov        byte ptr [eax], 1
0041482b  b8 01 00 00 00                     mov        eax, 1
00414830  5d                                 pop        ebp
00414831  c2 10 00                           ret        0x10
