0041ca90  55                                 push       ebp
0041ca91  8b ec                              mov        ebp, esp
0041ca93  51                                 push       ecx
0041ca94  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041ca97  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041ca9a  8b 40 14                           mov        eax, dword ptr [eax + 0x14]
0041ca9d  8b e5                              mov        esp, ebp
0041ca9f  5d                                 pop        ebp
0041caa0  c3                                 ret        
