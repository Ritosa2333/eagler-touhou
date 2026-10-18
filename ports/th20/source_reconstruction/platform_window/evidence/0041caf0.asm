0041caf0  55                                 push       ebp
0041caf1  8b ec                              mov        ebp, esp
0041caf3  51                                 push       ecx
0041caf4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041caf7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041cafa  8b 40 24                           mov        eax, dword ptr [eax + 0x24]
0041cafd  8b e5                              mov        esp, ebp
0041caff  5d                                 pop        ebp
0041cb00  c3                                 ret        
