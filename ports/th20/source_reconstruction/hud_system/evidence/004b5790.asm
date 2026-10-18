004b5790  55                                 push       ebp
004b5791  8b ec                              mov        ebp, esp
004b5793  51                                 push       ecx
004b5794  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004b5797  6a 04                              push       4
004b5799  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004b579e  e8 3d 85 f6 ff                     call       0x41dce0
004b57a3  6a 00                              push       0
004b57a5  e8 86 b0 fa ff                     call       0x460830
004b57aa  83 c4 04                           add        esp, 4
004b57ad  8b c8                              mov        ecx, eax
004b57af  e8 ac 52 ff ff                     call       0x4aaa60
004b57b4  8b c8                              mov        ecx, eax
004b57b6  e8 85 30 04 00                     call       0x4f8840
004b57bb  b8 01 00 00 00                     mov        eax, 1
004b57c0  8b e5                              mov        esp, ebp
004b57c2  5d                                 pop        ebp
004b57c3  c3                                 ret        
