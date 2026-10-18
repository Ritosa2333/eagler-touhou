00470670  55                                 push       ebp
00470671  8b ec                              mov        ebp, esp
00470673  51                                 push       ecx
00470674  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00470677  a1 80 58 5c 00                     mov        eax, dword ptr [0x5c5880]
0047067c  8b e5                              mov        esp, ebp
0047067e  5d                                 pop        ebp
0047067f  c3                                 ret        
