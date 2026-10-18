004d8740  55                                 push       ebp
004d8741  8b ec                              mov        ebp, esp
004d8743  83 ec 0c                           sub        esp, 0xc
004d8746  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
004d8749  6a 2c                              push       0x2c
004d874b  e8 0d a0 06 00                     call       0x54275d
004d8750  83 c4 04                           add        esp, 4
004d8753  89 45 fc                           mov        dword ptr [ebp - 4], eax
004d8756  83 7d fc 00                        cmp        dword ptr [ebp - 4], 0
004d875a  74 18                              je         0x4d8774
004d875c  6a 2c                              push       0x2c
004d875e  6a 00                              push       0
004d8760  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d8763  50                                 push       eax
004d8764  e8 f7 bf 06 00                     call       0x544760
004d8769  83 c4 0c                           add        esp, 0xc
004d876c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004d876f  89 4d f8                           mov        dword ptr [ebp - 8], ecx
004d8772  eb 07                              jmp        0x4d877b
004d8774  c7 45 f8 00 00 00 00               mov        dword ptr [ebp - 8], 0
004d877b  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
004d877e  8b e5                              mov        esp, ebp
004d8780  5d                                 pop        ebp
004d8781  c2 04 00                           ret        4
