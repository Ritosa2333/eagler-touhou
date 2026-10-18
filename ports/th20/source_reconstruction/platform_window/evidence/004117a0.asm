004117a0  55                                 push       ebp
004117a1  8b ec                              mov        ebp, esp
004117a3  51                                 push       ecx
004117a4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004117a7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004117aa  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004117ad  89 08                              mov        dword ptr [eax], ecx
004117af  8b e5                              mov        esp, ebp
004117b1  5d                                 pop        ebp
004117b2  c2 04 00                           ret        4
