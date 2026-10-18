0041de70  55                                 push       ebp
0041de71  8b ec                              mov        ebp, esp
0041de73  51                                 push       ecx
0041de74  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041de77  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041de7a  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0041de7d  89 48 1c                           mov        dword ptr [eax + 0x1c], ecx
0041de80  8b e5                              mov        esp, ebp
0041de82  5d                                 pop        ebp
0041de83  c2 04 00                           ret        4
