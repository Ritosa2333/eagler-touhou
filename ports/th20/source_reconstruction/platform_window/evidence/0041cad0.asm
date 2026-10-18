0041cad0  55                                 push       ebp
0041cad1  8b ec                              mov        ebp, esp
0041cad3  51                                 push       ecx
0041cad4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041cad7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041cada  8b 40 20                           mov        eax, dword ptr [eax + 0x20]
0041cadd  8b e5                              mov        esp, ebp
0041cadf  5d                                 pop        ebp
0041cae0  c3                                 ret        
