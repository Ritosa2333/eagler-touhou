0041cc90  55                                 push       ebp
0041cc91  8b ec                              mov        ebp, esp
0041cc93  51                                 push       ecx
0041cc94  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041cc97  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041cc9a  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0041cc9d  89 08                              mov        dword ptr [eax], ecx
0041cc9f  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0041cca2  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0041cca5  89 42 04                           mov        dword ptr [edx + 4], eax
0041cca8  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041ccab  c7 41 08 00 00 00 00               mov        dword ptr [ecx + 8], 0
0041ccb2  8b e5                              mov        esp, ebp
0041ccb4  5d                                 pop        ebp
0041ccb5  c2 04 00                           ret        4
