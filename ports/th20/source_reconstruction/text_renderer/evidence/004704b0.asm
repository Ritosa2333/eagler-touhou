004704b0  55                                 push       ebp
004704b1  8b ec                              mov        ebp, esp
004704b3  51                                 push       ecx
004704b4  6a 00                              push       0
004704b6  6a 02                              push       2
004704b8  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004704bd  e8 5e f2 fd ff                     call       0x44f720
004704c2  6a 02                              push       2
004704c4  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004704c7  e8 54 b5 ff ff                     call       0x46ba20
004704cc  89 45 fc                           mov        dword ptr [ebp - 4], eax
004704cf  6a 02                              push       2
004704d1  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004704d6  e8 05 d8 fa ff                     call       0x41dce0
004704db  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004704de  8b e5                              mov        esp, ebp
004704e0  5d                                 pop        ebp
004704e1  c3                                 ret        
