004bd6b0  55                                 push       ebp
004bd6b1  8b ec                              mov        ebp, esp
004bd6b3  83 ec 08                           sub        esp, 8
004bd6b6  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004bd6b9  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004bd6bc  05 68 01 00 00                     add        eax, 0x168
004bd6c1  89 45 f8                           mov        dword ptr [ebp - 8], eax
004bd6c4  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004bd6c7  51                                 push       ecx
004bd6c8  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004bd6cb  e8 b0 6e f5 ff                     call       0x414580
004bd6d0  8b 00                              mov        eax, dword ptr [eax]
004bd6d2  8b e5                              mov        esp, ebp
004bd6d4  5d                                 pop        ebp
004bd6d5  c2 04 00                           ret        4
