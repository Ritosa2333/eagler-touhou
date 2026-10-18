00464250  55                                 push       ebp
00464251  8b ec                              mov        ebp, esp
00464253  51                                 push       ecx
00464254  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00464257  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046425a  8b 40 2c                           mov        eax, dword ptr [eax + 0x2c]
0046425d  8b e5                              mov        esp, ebp
0046425f  5d                                 pop        ebp
00464260  c3                                 ret        
