004d9e30  55                                 push       ebp
004d9e31  8b ec                              mov        ebp, esp
004d9e33  51                                 push       ecx
004d9e34  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004d9e37  68 48 1e 57 00                     push       0x571e48 ; string='SyncCloseSubThread'
004d9e3c  e8 6f 28 f3 ff                     call       0x40c6b0
004d9e41  83 c4 04                           add        esp, 4
004d9e44  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004d9e47  81 c1 90 0d 00 00                  add        ecx, 0xd90
004d9e4d  e8 5e 0a fe ff                     call       0x4ba8b0
004d9e52  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004d9e55  81 c1 90 0d 00 00                  add        ecx, 0xd90
004d9e5b  e8 90 1e f3 ff                     call       0x40bcf0
004d9e60  90                                 nop        
004d9e61  8b e5                              mov        esp, ebp
004d9e63  5d                                 pop        ebp
004d9e64  c3                                 ret        
