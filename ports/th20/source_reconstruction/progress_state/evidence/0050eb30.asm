0050eb30  55                                 push       ebp
0050eb31  8b ec                              mov        ebp, esp
0050eb33  51                                 push       ecx
0050eb34  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0050eb37  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050eb3a  e8 51 fe ff ff                     call       0x50e990
0050eb3f  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0050eb42  83 e0 01                           and        eax, 1
0050eb45  74 11                              je         0x50eb58
0050eb47  68 d8 42 12 00                     push       0x1242d8
0050eb4c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050eb4f  51                                 push       ecx
0050eb50  e8 38 3c 03 00                     call       0x54278d
0050eb55  83 c4 08                           add        esp, 8
0050eb58  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050eb5b  8b e5                              mov        esp, ebp
0050eb5d  5d                                 pop        ebp
0050eb5e  c2 04 00                           ret        4
