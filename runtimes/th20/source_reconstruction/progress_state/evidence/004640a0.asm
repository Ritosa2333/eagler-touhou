004640a0  55                                 push       ebp
004640a1  8b ec                              mov        ebp, esp
004640a3  51                                 push       ecx
004640a4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004640a7  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004640aa  81 c1 88 00 00 00                  add        ecx, 0x88
004640b0  e8 0b 00 00 00                     call       0x4640c0
004640b5  8b e5                              mov        esp, ebp
004640b7  5d                                 pop        ebp
004640b8  c3                                 ret        
