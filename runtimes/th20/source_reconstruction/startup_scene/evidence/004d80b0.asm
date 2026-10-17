004d80b0  55                                 push       ebp
004d80b1  8b ec                              mov        ebp, esp
004d80b3  51                                 push       ecx
004d80b4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004d80b7  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004d80ba  e8 11 ff ff ff                     call       0x4d7fd0
004d80bf  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004d80c2  83 e0 01                           and        eax, 1
004d80c5  74 11                              je         0x4d80d8
004d80c7  68 1c 06 00 00                     push       0x61c
004d80cc  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004d80cf  51                                 push       ecx
004d80d0  e8 b8 a6 06 00                     call       0x54278d
004d80d5  83 c4 08                           add        esp, 8
004d80d8  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d80db  8b e5                              mov        esp, ebp
004d80dd  5d                                 pop        ebp
004d80de  c2 04 00                           ret        4
