0044c8c0  55                                 push       ebp
0044c8c1  8b ec                              mov        ebp, esp
0044c8c3  83 ec 08                           sub        esp, 8
0044c8c6  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0044c8c9  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0044c8cc  05 c8 06 00 00                     add        eax, 0x6c8
0044c8d1  89 45 f8                           mov        dword ptr [ebp - 8], eax
0044c8d4  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0044c8d7  51                                 push       ecx
0044c8d8  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0044c8db  e8 b0 cb ff ff                     call       0x449490
0044c8e0  8b e5                              mov        esp, ebp
0044c8e2  5d                                 pop        ebp
0044c8e3  c2 04 00                           ret        4
