00412da0  55                                 push       ebp
00412da1  8b ec                              mov        ebp, esp
00412da3  51                                 push       ecx
00412da4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00412da7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00412daa  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00412dad  89 48 10                           mov        dword ptr [eax + 0x10], ecx
00412db0  8b e5                              mov        esp, ebp
00412db2  5d                                 pop        ebp
00412db3  c2 04 00                           ret        4
