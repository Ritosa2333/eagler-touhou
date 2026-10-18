00464230  55                                 push       ebp
00464231  8b ec                              mov        ebp, esp
00464233  8b 45 08                           mov        eax, dword ptr [ebp + 8]
00464236  50                                 push       eax
00464237  b9 68 a5 5b 00                     mov        ecx, 0x5ba568
0046423c  e8 7f 79 fa ff                     call       0x40bbc0
00464241  8b c8                              mov        ecx, eax
00464243  e8 08 00 00 00                     call       0x464250
00464248  5d                                 pop        ebp
00464249  c3                                 ret        
