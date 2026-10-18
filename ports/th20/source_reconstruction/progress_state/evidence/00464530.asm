00464530  55                                 push       ebp
00464531  8b ec                              mov        ebp, esp
00464533  51                                 push       ecx
00464534  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00464537  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046453a  c7 40 08 fe ff ff ff               mov        dword ptr [eax + 8], 0xfffffffe
00464541  8b e5                              mov        esp, ebp
00464543  5d                                 pop        ebp
00464544  c3                                 ret        
