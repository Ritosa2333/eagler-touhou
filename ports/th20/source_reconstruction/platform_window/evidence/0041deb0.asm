0041deb0  55                                 push       ebp
0041deb1  8b ec                              mov        ebp, esp
0041deb3  51                                 push       ecx
0041deb4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041deb7  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0041deba  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041debd  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0041dec0  89 54 81 48                        mov        dword ptr [ecx + eax*4 + 0x48], edx
0041dec4  8b e5                              mov        esp, ebp
0041dec6  5d                                 pop        ebp
0041dec7  c2 08 00                           ret        8
