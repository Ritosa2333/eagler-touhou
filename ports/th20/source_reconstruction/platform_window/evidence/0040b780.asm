0040b780  55                                 push       ebp
0040b781  8b ec                              mov        ebp, esp
0040b783  83 ec 08                           sub        esp, 8
0040b786  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0040b789  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0040b78c  e8 7f 00 00 00                     call       0x40b810
0040b791  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0040b794  83 c0 0c                           add        eax, 0xc
0040b797  89 45 f8                           mov        dword ptr [ebp - 8], eax
0040b79a  6a 00                              push       0
0040b79c  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0040b79f  e8 6c ff ff ff                     call       0x40b710
0040b7a4  90                                 nop        
0040b7a5  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0040b7a8  8b e5                              mov        esp, ebp
0040b7aa  5d                                 pop        ebp
0040b7ab  c3                                 ret        
