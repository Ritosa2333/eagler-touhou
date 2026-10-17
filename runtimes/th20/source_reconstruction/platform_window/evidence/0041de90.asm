0041de90  55                                 push       ebp
0041de91  8b ec                              mov        ebp, esp
0041de93  51                                 push       ecx
0041de94  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041de97  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041de9a  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0041de9d  89 48 18                           mov        dword ptr [eax + 0x18], ecx
0041dea0  8b e5                              mov        esp, ebp
0041dea2  5d                                 pop        ebp
0041dea3  c2 04 00                           ret        4
