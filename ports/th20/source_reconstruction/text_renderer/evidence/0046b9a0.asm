0046b9a0  55                                 push       ebp
0046b9a1  8b ec                              mov        ebp, esp
0046b9a3  51                                 push       ecx
0046b9a4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0046b9a7  6a 00                              push       0
0046b9a9  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0046b9ae  e8 2d 23 fb ff                     call       0x41dce0
0046b9b3  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
0046b9b9  e8 02 9c fd ff                     call       0x4455c0
0046b9be  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046b9c1  8a 88 b3 04 00 00                  mov        cl, byte ptr [eax + 0x4b3]
0046b9c7  80 e1 fc                           and        cl, 0xfc
0046b9ca  80 c9 02                           or         cl, 2
0046b9cd  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046b9d0  88 8a b3 04 00 00                  mov        byte ptr [edx + 0x4b3], cl
0046b9d6  6a 01                              push       1
0046b9d8  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046b9db  e8 40 00 00 00                     call       0x46ba20
0046b9e0  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046b9e3  8a 88 b3 04 00 00                  mov        cl, byte ptr [eax + 0x4b3]
0046b9e9  80 e1 fc                           and        cl, 0xfc
0046b9ec  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046b9ef  88 8a b3 04 00 00                  mov        byte ptr [edx + 0x4b3], cl
0046b9f5  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
0046b9fb  e8 c0 9b fd ff                     call       0x4455c0
0046ba00  6a 02                              push       2
0046ba02  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0046ba07  e8 d4 22 fb ff                     call       0x41dce0
0046ba0c  b8 01 00 00 00                     mov        eax, 1
0046ba11  8b e5                              mov        esp, ebp
0046ba13  5d                                 pop        ebp
0046ba14  c3                                 ret        
