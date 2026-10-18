0041e010  55                                 push       ebp
0041e011  8b ec                              mov        ebp, esp
0041e013  51                                 push       ecx
0041e014  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041e017  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041e01a  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0041e01d  89 48 64                           mov        dword ptr [eax + 0x64], ecx
0041e020  8b e5                              mov        esp, ebp
0041e022  5d                                 pop        ebp
0041e023  c2 04 00                           ret        4
