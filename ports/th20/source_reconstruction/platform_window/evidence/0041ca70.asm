0041ca70  55                                 push       ebp
0041ca71  8b ec                              mov        ebp, esp
0041ca73  51                                 push       ecx
0041ca74  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041ca77  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041ca7a  83 c0 08                           add        eax, 8
0041ca7d  8b e5                              mov        esp, ebp
0041ca7f  5d                                 pop        ebp
0041ca80  c3                                 ret        
