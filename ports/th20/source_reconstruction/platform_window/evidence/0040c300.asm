0040c300  55                                 push       ebp
0040c301  8b ec                              mov        ebp, esp
0040c303  51                                 push       ecx
0040c304  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0040c307  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0040c30a  8b 00                              mov        eax, dword ptr [eax]
0040c30c  8b e5                              mov        esp, ebp
0040c30e  5d                                 pop        ebp
0040c30f  c3                                 ret        
