005399a0  55                                 push       ebp
005399a1  8b ec                              mov        ebp, esp
005399a3  68 00 20 00 00                     push       0x2000
005399a8  6a 00                              push       0
005399aa  68 38 6b 5c 00                     push       0x5c6b38
005399af  e8 ac ad 00 00                     call       0x544760
005399b4  83 c4 0c                           add        esp, 0xc
005399b7  68 0c 80 01 00                     push       0x1800c
005399bc  6a 00                              push       0
005399be  68 38 8b 5c 00                     push       0x5c8b38
005399c3  e8 98 ad 00 00                     call       0x544760
005399c8  83 c4 0c                           add        esp, 0xc
005399cb  5d                                 pop        ebp
005399cc  c3                                 ret        
