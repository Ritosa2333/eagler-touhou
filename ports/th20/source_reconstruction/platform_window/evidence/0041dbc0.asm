0041dbc0  55                                 push       ebp
0041dbc1  8b ec                              mov        ebp, esp
0041dbc3  51                                 push       ecx
0041dbc4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041dbc7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041dbca  c6 80 10 0e 00 06 ff               mov        byte ptr [eax + 0x6000e10], 0xff
0041dbd1  8b e5                              mov        esp, ebp
0041dbd3  5d                                 pop        ebp
0041dbd4  c3                                 ret        
