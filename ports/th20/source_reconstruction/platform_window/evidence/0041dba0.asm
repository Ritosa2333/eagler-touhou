0041dba0  55                                 push       ebp
0041dba1  8b ec                              mov        ebp, esp
0041dba3  51                                 push       ecx
0041dba4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041dba7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041dbaa  c7 80 08 0e 00 06 ff ff ff ff      mov        dword ptr [eax + 0x6000e08], 0xffffffff
0041dbb4  8b e5                              mov        esp, ebp
0041dbb6  5d                                 pop        ebp
0041dbb7  c3                                 ret        
