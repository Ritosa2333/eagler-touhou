00445ab0  55                                 push       ebp
00445ab1  8b ec                              mov        ebp, esp
00445ab3  51                                 push       ecx
00445ab4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00445ab7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00445aba  8a 88 9a 04 00 00                  mov        cl, byte ptr [eax + 0x49a]
00445ac0  80 e1 01                           and        cl, 1
00445ac3  0f b6 c1                           movzx      eax, cl
00445ac6  8b e5                              mov        esp, ebp
00445ac8  5d                                 pop        ebp
00445ac9  c3                                 ret        
