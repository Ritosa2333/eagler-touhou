00411570  55                                 push       ebp
00411571  8b ec                              mov        ebp, esp
00411573  51                                 push       ecx
00411574  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00411577  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041157a  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0041157d  89 48 04                           mov        dword ptr [eax + 4], ecx
00411580  8b e5                              mov        esp, ebp
00411582  5d                                 pop        ebp
00411583  c2 04 00                           ret        4
