0044c8a0  55                                 push       ebp
0044c8a1  8b ec                              mov        ebp, esp
0044c8a3  51                                 push       ecx
0044c8a4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0044c8a7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0044c8aa  8b 80 c4 06 00 00                  mov        eax, dword ptr [eax + 0x6c4]
0044c8b0  8b e5                              mov        esp, ebp
0044c8b2  5d                                 pop        ebp
0044c8b3  c3                                 ret        
