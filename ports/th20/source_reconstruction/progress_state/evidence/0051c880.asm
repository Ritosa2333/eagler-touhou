0051c880  55                                 push       ebp
0051c881  8b ec                              mov        ebp, esp
0051c883  83 ec 08                           sub        esp, 8
0051c886  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0051c889  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0051c88c  05 8c 01 00 00                     add        eax, 0x18c
0051c891  89 45 f8                           mov        dword ptr [ebp - 8], eax
0051c894  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0051c897  51                                 push       ecx
0051c898  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0051c89b  e8 e0 07 fa ff                     call       0x4bd080
0051c8a0  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0051c8a3  89 10                              mov        dword ptr [eax], edx
0051c8a5  8b e5                              mov        esp, ebp
0051c8a7  5d                                 pop        ebp
0051c8a8  c2 08 00                           ret        8
