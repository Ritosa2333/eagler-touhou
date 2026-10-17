0041cab0  55                                 push       ebp
0041cab1  8b ec                              mov        ebp, esp
0041cab3  51                                 push       ecx
0041cab4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041cab7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041caba  8b 40 0c                           mov        eax, dword ptr [eax + 0xc]
0041cabd  8b e5                              mov        esp, ebp
0041cabf  5d                                 pop        ebp
0041cac0  c3                                 ret        
