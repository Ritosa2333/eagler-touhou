004640c0  55                                 push       ebp
004640c1  8b ec                              mov        ebp, esp
004640c3  51                                 push       ecx
004640c4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004640c7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004640ca  8b 80 e0 01 00 00                  mov        eax, dword ptr [eax + 0x1e0]
004640d0  8b e5                              mov        esp, ebp
004640d2  5d                                 pop        ebp
004640d3  c3                                 ret        
