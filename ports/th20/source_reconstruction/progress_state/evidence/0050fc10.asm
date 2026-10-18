0050fc10  55                                 push       ebp
0050fc11  8b ec                              mov        ebp, esp
0050fc13  83 3d 08 61 5c 00 00               cmp        dword ptr [0x5c6108], 0
0050fc1a  74 1b                              je         0x50fc37
0050fc1c  a1 08 61 5c 00                     mov        eax, dword ptr [0x5c6108]
0050fc21  50                                 push       eax
0050fc22  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0050fc28  e8 23 b1 ff ff                     call       0x50ad50
0050fc2d  c7 05 08 61 5c 00 00 00 00 00      mov        dword ptr [0x5c6108], 0
0050fc37  c7 05 08 61 5c 00 00 00 00 00      mov        dword ptr [0x5c6108], 0
0050fc41  5d                                 pop        ebp
0050fc42  c3                                 ret        
