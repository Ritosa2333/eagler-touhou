0041df90  55                                 push       ebp
0041df91  8b ec                              mov        ebp, esp
0041df93  51                                 push       ecx
0041df94  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041df97  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0041df9a  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041df9d  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0041dfa0  89 54 81 40                        mov        dword ptr [ecx + eax*4 + 0x40], edx
0041dfa4  8b e5                              mov        esp, ebp
0041dfa6  5d                                 pop        ebp
0041dfa7  c2 08 00                           ret        8
