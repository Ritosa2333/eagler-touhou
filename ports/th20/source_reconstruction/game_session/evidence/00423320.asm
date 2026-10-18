00423320  55                                 push       ebp
00423321  8b ec                              mov        ebp, esp
00423323  51                                 push       ecx
00423324  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00423327  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0042332a  c7 00 00 00 00 00                  mov        dword ptr [eax], 0
00423330  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00423333  c7 41 04 00 00 00 00               mov        dword ptr [ecx + 4], 0
0042333a  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0042333d  c7 42 08 00 00 00 00               mov        dword ptr [edx + 8], 0
00423344  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00423347  c7 40 0c 00 00 00 00               mov        dword ptr [eax + 0xc], 0
0042334e  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00423351  c7 41 10 00 00 00 00               mov        dword ptr [ecx + 0x10], 0
00423358  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0042335b  c7 42 14 00 00 00 00               mov        dword ptr [edx + 0x14], 0
00423362  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00423365  c7 40 18 00 00 00 00               mov        dword ptr [eax + 0x18], 0
0042336c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0042336f  c7 41 1c 00 00 00 00               mov        dword ptr [ecx + 0x1c], 0
00423376  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00423379  c7 42 20 00 00 00 00               mov        dword ptr [edx + 0x20], 0
00423380  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00423383  c7 40 24 00 00 00 00               mov        dword ptr [eax + 0x24], 0
0042338a  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0042338d  c7 41 28 00 00 00 00               mov        dword ptr [ecx + 0x28], 0
00423394  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00423397  c7 42 2c 00 00 00 00               mov        dword ptr [edx + 0x2c], 0
0042339e  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004233a1  8b e5                              mov        esp, ebp
004233a3  5d                                 pop        ebp
004233a4  c3                                 ret        
