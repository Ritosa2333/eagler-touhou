0051b160  55                                 push       ebp
0051b161  8b ec                              mov        ebp, esp
0051b163  83 ec 14                           sub        esp, 0x14
0051b166  56                                 push       esi
0051b167  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0051b16a  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0051b16d  05 8c 01 00 00                     add        eax, 0x18c
0051b172  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0051b175  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0051b178  81 c1 68 01 00 00                  add        ecx, 0x168
0051b17e  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
0051b181  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0051b184  52                                 push       edx
0051b185  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0051b188  e8 f3 1e fa ff                     call       0x4bd080
0051b18d  8b f0                              mov        esi, eax
0051b18f  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0051b192  50                                 push       eax
0051b193  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0051b196  e8 e5 1e fa ff                     call       0x4bd080
0051b19b  8b 08                              mov        ecx, dword ptr [eax]
0051b19d  3b 0e                              cmp        ecx, dword ptr [esi]
0051b19f  73 28                              jae        0x51b1c9
0051b1a1  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0051b1a4  81 c2 8c 01 00 00                  add        edx, 0x18c
0051b1aa  89 55 ec                           mov        dword ptr [ebp - 0x14], edx
0051b1ad  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0051b1b0  50                                 push       eax
0051b1b1  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
0051b1b4  e8 c7 1e fa ff                     call       0x4bd080
0051b1b9  89 45 f8                           mov        dword ptr [ebp - 8], eax
0051b1bc  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0051b1bf  8b 11                              mov        edx, dword ptr [ecx]
0051b1c1  83 c2 01                           add        edx, 1
0051b1c4  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
0051b1c7  89 10                              mov        dword ptr [eax], edx
0051b1c9  5e                                 pop        esi
0051b1ca  8b e5                              mov        esp, ebp
0051b1cc  5d                                 pop        ebp
0051b1cd  c2 04 00                           ret        4
