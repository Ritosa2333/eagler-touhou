004704f0  55                                 push       ebp
004704f1  8b ec                              mov        ebp, esp
004704f3  6a 02                              push       2
004704f5  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004704fa  e8 e1 d7 fa ff                     call       0x41dce0
004704ff  6a 03                              push       3
00470501  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00470504  e8 17 b5 ff ff                     call       0x46ba20
00470509  5d                                 pop        ebp
0047050a  c3                                 ret        
