004193c0  55                                 push       ebp
004193c1  8b ec                              mov        ebp, esp
004193c3  83 3d 00 4a 5c 00 00               cmp        dword ptr [0x5c4a00], 0
004193ca  74 0c                              je         0x4193d8
004193cc  8b 0d 00 4a 5c 00                  mov        ecx, dword ptr [0x5c4a00]
004193d2  e8 79 29 09 00                     call       0x4abd50
004193d7  90                                 nop        
004193d8  e8 83 e1 06 00                     call       0x487560
004193dd  90                                 nop        
004193de  5d                                 pop        ebp
004193df  c3                                 ret        
