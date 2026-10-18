00411700  55                                 push       ebp
00411701  8b ec                              mov        ebp, esp
00411703  51                                 push       ecx
00411704  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00411707  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041170a  8b 40 10                           mov        eax, dword ptr [eax + 0x10]
0041170d  8b e5                              mov        esp, ebp
0041170f  5d                                 pop        ebp
00411710  c3                                 ret        
