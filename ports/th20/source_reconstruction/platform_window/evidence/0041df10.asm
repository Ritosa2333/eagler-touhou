0041df10  55                                 push       ebp
0041df11  8b ec                              mov        ebp, esp
0041df13  51                                 push       ecx
0041df14  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041df17  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0041df1a  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041df1d  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0041df20  89 54 81 30                        mov        dword ptr [ecx + eax*4 + 0x30], edx
0041df24  8b e5                              mov        esp, ebp
0041df26  5d                                 pop        ebp
0041df27  c2 08 00                           ret        8
