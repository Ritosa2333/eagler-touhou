0041dfd0  55                                 push       ebp
0041dfd1  8b ec                              mov        ebp, esp
0041dfd3  51                                 push       ecx
0041dfd4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041dfd7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041dfda  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0041dfdd  89 48 60                           mov        dword ptr [eax + 0x60], ecx
0041dfe0  8b e5                              mov        esp, ebp
0041dfe2  5d                                 pop        ebp
0041dfe3  c2 04 00                           ret        4
