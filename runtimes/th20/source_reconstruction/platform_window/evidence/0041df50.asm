0041df50  55                                 push       ebp
0041df51  8b ec                              mov        ebp, esp
0041df53  51                                 push       ecx
0041df54  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041df57  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041df5a  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0041df5d  89 48 24                           mov        dword ptr [eax + 0x24], ecx
0041df60  8b e5                              mov        esp, ebp
0041df62  5d                                 pop        ebp
0041df63  c2 04 00                           ret        4
