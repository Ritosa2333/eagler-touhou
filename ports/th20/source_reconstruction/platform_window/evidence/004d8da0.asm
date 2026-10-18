004d8da0  55                                 push       ebp
004d8da1  8b ec                              mov        ebp, esp
004d8da3  51                                 push       ecx
004d8da4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004d8da7  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004d8daa  81 c1 a0 0d 00 00                  add        ecx, 0xda0
004d8db0  e8 cb 2b f3 ff                     call       0x40b980
004d8db5  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004d8db8  81 c1 90 0d 00 00                  add        ecx, 0xd90
004d8dbe  e8 bd 2b f3 ff                     call       0x40b980
004d8dc3  90                                 nop        
004d8dc4  8b e5                              mov        esp, ebp
004d8dc6  5d                                 pop        ebp
004d8dc7  c3                                 ret        
