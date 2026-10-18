0041de50  55                                 push       ebp
0041de51  8b ec                              mov        ebp, esp
0041de53  51                                 push       ecx
0041de54  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041de57  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041de5a  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0041de5d  89 48 14                           mov        dword ptr [eax + 0x14], ecx
0041de60  8b e5                              mov        esp, ebp
0041de62  5d                                 pop        ebp
0041de63  c2 04 00                           ret        4
