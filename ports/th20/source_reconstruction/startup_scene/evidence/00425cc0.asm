00425cc0  55                                 push       ebp
00425cc1  8b ec                              mov        ebp, esp
00425cc3  51                                 push       ecx
00425cc4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00425cc7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00425cca  c7 00 00 00 00 00                  mov        dword ptr [eax], 0
00425cd0  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00425cd3  8b e5                              mov        esp, ebp
00425cd5  5d                                 pop        ebp
00425cd6  c3                                 ret        
