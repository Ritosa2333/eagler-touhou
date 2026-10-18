0041d0a0  55                                 push       ebp
0041d0a1  8b ec                              mov        ebp, esp
0041d0a3  51                                 push       ecx
0041d0a4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041d0a7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041d0aa  8b 80 4c 02 00 00                  mov        eax, dword ptr [eax + 0x24c]
0041d0b0  d1 e8                              shr        eax, 1
0041d0b2  83 e0 01                           and        eax, 1
0041d0b5  8b e5                              mov        esp, ebp
0041d0b7  5d                                 pop        ebp
0041d0b8  c3                                 ret        
