0041d0f0  55                                 push       ebp
0041d0f1  8b ec                              mov        ebp, esp
0041d0f3  83 ec 08                           sub        esp, 8
0041d0f6  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041d0f9  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041d0fc  83 b8 40 02 00 00 08               cmp        dword ptr [eax + 0x240], 8
0041d103  74 15                              je         0x41d11a
0041d105  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041d108  83 b9 40 02 00 00 09               cmp        dword ptr [ecx + 0x240], 9
0041d10f  74 09                              je         0x41d11a
0041d111  c7 45 f8 00 00 00 00               mov        dword ptr [ebp - 8], 0
0041d118  eb 07                              jmp        0x41d121
0041d11a  c7 45 f8 01 00 00 00               mov        dword ptr [ebp - 8], 1
0041d121  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
0041d124  8b e5                              mov        esp, ebp
0041d126  5d                                 pop        ebp
0041d127  c3                                 ret        
