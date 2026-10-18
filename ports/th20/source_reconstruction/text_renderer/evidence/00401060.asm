00401060  55                                 push       ebp
00401061  8b ec                              mov        ebp, esp
00401063  6a 0c                              push       0xc
00401065  b9 e0 66 5b 00                     mov        ecx, 0x5b66e0
0040106a  e8 11 b0 00 00                     call       0x40c080
0040106f  b9 e0 66 5b 00                     mov        ecx, 0x5b66e0
00401074  e8 f7 30 01 00                     call       0x414170
00401079  68 80 b3 56 00                     push       0x56b380
0040107e  e8 56 19 14 00                     call       0x5429d9
00401083  83 c4 04                           add        esp, 4
00401086  5d                                 pop        ebp
00401087  c3                                 ret        
