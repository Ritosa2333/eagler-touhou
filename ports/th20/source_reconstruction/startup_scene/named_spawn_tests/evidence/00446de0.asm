00446de0  55                                 push       ebp
00446de1  8b ec                              mov        ebp, esp
00446de3  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
00446de6  50                                 push       eax
00446de7  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00446dea  e8 11 46 00 00                     call       0x44b400
00446def  5d                                 pop        ebp
00446df0  c3                                 ret        
