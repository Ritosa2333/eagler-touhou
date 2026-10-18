00449490  55                                 push       ebp
00449491  8b ec                              mov        ebp, esp
00449493  51                                 push       ecx
00449494  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00449497  6b 45 08 48                        imul       eax, dword ptr [ebp + 8], 0x48
0044949b  03 45 fc                           add        eax, dword ptr [ebp - 4]
0044949e  8b e5                              mov        esp, ebp
004494a0  5d                                 pop        ebp
004494a1  c2 04 00                           ret        4
