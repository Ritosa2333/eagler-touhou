00419c00  55                                 push       ebp
00419c01  8b ec                              mov        ebp, esp
00419c03  51                                 push       ecx
00419c04  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00419c07  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00419c0a  8b 40 10                           mov        eax, dword ptr [eax + 0x10]
00419c0d  23 45 08                           and        eax, dword ptr [ebp + 8]
00419c10  8b e5                              mov        esp, ebp
00419c12  5d                                 pop        ebp
00419c13  c2 04 00                           ret        4
