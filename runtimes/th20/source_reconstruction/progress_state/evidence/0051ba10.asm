0051ba10  55                                 push       ebp
0051ba11  8b ec                              mov        ebp, esp
0051ba13  83 ec 08                           sub        esp, 8
0051ba16  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0051ba19  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0051ba1c  05 8c 01 00 00                     add        eax, 0x18c
0051ba21  89 45 f8                           mov        dword ptr [ebp - 8], eax
0051ba24  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0051ba27  51                                 push       ecx
0051ba28  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0051ba2b  e8 50 16 fa ff                     call       0x4bd080
0051ba30  8b 00                              mov        eax, dword ptr [eax]
0051ba32  8b e5                              mov        esp, ebp
0051ba34  5d                                 pop        ebp
0051ba35  c2 04 00                           ret        4
