0041ded0  55                                 push       ebp
0041ded1  8b ec                              mov        ebp, esp
0041ded3  51                                 push       ecx
0041ded4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041ded7  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0041deda  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041dedd  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0041dee0  89 54 81 28                        mov        dword ptr [ecx + eax*4 + 0x28], edx
0041dee4  8b e5                              mov        esp, ebp
0041dee6  5d                                 pop        ebp
0041dee7  c2 08 00                           ret        8
