00486e10  55                                 push       ebp
00486e11  8b ec                              mov        ebp, esp
00486e13  51                                 push       ecx
00486e14  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00486e17  69 45 08 e0 00 00 00               imul       eax, dword ptr [ebp + 8], 0xe0
00486e1e  03 45 fc                           add        eax, dword ptr [ebp - 4]
00486e21  8b e5                              mov        esp, ebp
00486e23  5d                                 pop        ebp
00486e24  c2 04 00                           ret        4
