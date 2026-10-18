0040ff90  55                                 push       ebp
0040ff91  8b ec                              mov        ebp, esp
0040ff93  51                                 push       ecx
0040ff94  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0040ff97  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0040ff9a  8b 40 04                           mov        eax, dword ptr [eax + 4]
0040ff9d  8b e5                              mov        esp, ebp
0040ff9f  5d                                 pop        ebp
0040ffa0  c3                                 ret        
