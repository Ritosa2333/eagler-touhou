0041db80  55                                 push       ebp
0041db81  8b ec                              mov        ebp, esp
0041db83  51                                 push       ecx
0041db84  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041db87  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041db8a  c6 80 0e 0e 00 06 ff               mov        byte ptr [eax + 0x6000e0e], 0xff
0041db91  8b e5                              mov        esp, ebp
0041db93  5d                                 pop        ebp
0041db94  c3                                 ret        
