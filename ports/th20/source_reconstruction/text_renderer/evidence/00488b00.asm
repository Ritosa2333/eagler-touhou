00488b00  55                                 push       ebp
00488b01  8b ec                              mov        ebp, esp
00488b03  51                                 push       ecx
00488b04  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00488b07  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00488b0a  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00488b0d  89 88 e0 a1 01 00                  mov        dword ptr [eax + 0x1a1e0], ecx
00488b13  8b e5                              mov        esp, ebp
00488b15  5d                                 pop        ebp
00488b16  c2 04 00                           ret        4
