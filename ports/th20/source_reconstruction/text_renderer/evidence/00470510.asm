00470510  55                                 push       ebp
00470511  8b ec                              mov        ebp, esp
00470513  6a 02                              push       2
00470515  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0047051a  e8 c1 d7 fa ff                     call       0x41dce0
0047051f  6a 04                              push       4
00470521  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00470524  e8 f7 b4 ff ff                     call       0x46ba20
00470529  5d                                 pop        ebp
0047052a  c3                                 ret        
