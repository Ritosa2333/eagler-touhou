0040aa10  55                                 push       ebp
0040aa11  8b ec                              mov        ebp, esp
0040aa13  68 e8 0d 00 00                     push       0xde8
0040aa18  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0040aa1d  e8 5e 16 00 00                     call       0x40c080
0040aa22  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0040aa27  e8 64 df 0c 00                     call       0x4d8990
0040aa2c  68 f0 b3 56 00                     push       0x56b3f0
0040aa31  e8 a3 7f 13 00                     call       0x5429d9
0040aa36  83 c4 04                           add        esp, 4
0040aa39  5d                                 pop        ebp
0040aa3a  c3                                 ret        
