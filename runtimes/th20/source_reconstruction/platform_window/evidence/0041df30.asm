0041df30  55                                 push       ebp
0041df31  8b ec                              mov        ebp, esp
0041df33  51                                 push       ecx
0041df34  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041df37  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041df3a  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0041df3d  89 48 20                           mov        dword ptr [eax + 0x20], ecx
0041df40  8b e5                              mov        esp, ebp
0041df42  5d                                 pop        ebp
0041df43  c2 04 00                           ret        4
