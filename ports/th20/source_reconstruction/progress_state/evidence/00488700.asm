00488700  55                                 push       ebp
00488701  8b ec                              mov        ebp, esp
00488703  51                                 push       ecx
00488704  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00488707  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0048870a  05 60 a4 08 00                     add        eax, 0x8a460
0048870f  8b e5                              mov        esp, ebp
00488711  5d                                 pop        ebp
00488712  c3                                 ret        
