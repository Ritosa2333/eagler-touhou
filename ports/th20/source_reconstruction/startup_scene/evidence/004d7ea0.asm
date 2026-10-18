004d7ea0  55                                 push       ebp
004d7ea1  8b ec                              mov        ebp, esp
004d7ea3  83 ec 0c                           sub        esp, 0xc
004d7ea6  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
004d7ea9  68 1c 06 00 00                     push       0x61c
004d7eae  e8 aa a8 06 00                     call       0x54275d
004d7eb3  83 c4 04                           add        esp, 4
004d7eb6  89 45 fc                           mov        dword ptr [ebp - 4], eax
004d7eb9  83 7d fc 00                        cmp        dword ptr [ebp - 4], 0
004d7ebd  74 1a                              je         0x4d7ed9
004d7ebf  68 1c 06 00 00                     push       0x61c
004d7ec4  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004d7ec7  e8 b4 41 f3 ff                     call       0x40c080
004d7ecc  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004d7ecf  e8 1c 00 00 00                     call       0x4d7ef0
004d7ed4  89 45 f8                           mov        dword ptr [ebp - 8], eax
004d7ed7  eb 07                              jmp        0x4d7ee0
004d7ed9  c7 45 f8 00 00 00 00               mov        dword ptr [ebp - 8], 0
004d7ee0  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
004d7ee3  8b e5                              mov        esp, ebp
004d7ee5  5d                                 pop        ebp
004d7ee6  c2 04 00                           ret        4
