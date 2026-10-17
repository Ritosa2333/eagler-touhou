004989f0  55                                 push       ebp
004989f1  8b ec                              mov        ebp, esp
004989f3  51                                 push       ecx
004989f4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004989f7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004989fa  05 88 00 00 00                     add        eax, 0x88
004989ff  8b e5                              mov        esp, ebp
00498a01  5d                                 pop        ebp
00498a02  c3                                 ret        
