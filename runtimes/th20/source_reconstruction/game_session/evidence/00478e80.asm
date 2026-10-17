00478e80  55                                 push       ebp
00478e81  8b ec                              mov        ebp, esp
00478e83  8b 45 08                           mov        eax, dword ptr [ebp + 8]
00478e86  50                                 push       eax
00478e87  b9 68 a5 5b 00                     mov        ecx, 0x5ba568
00478e8c  e8 2f 2d f9 ff                     call       0x40bbc0
00478e91  8b c8                              mov        ecx, eax
00478e93  e8 68 34 f9 ff                     call       0x40c300
00478e98  5d                                 pop        ebp
00478e99  c3                                 ret        
