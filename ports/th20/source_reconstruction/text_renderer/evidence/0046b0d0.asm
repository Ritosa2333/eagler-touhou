0046b0d0  55                                 push       ebp
0046b0d1  8b ec                              mov        ebp, esp
0046b0d3  6a ff                              push       -1
0046b0d5  68 00 76 56 00                     push       0x567600
0046b0da  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0046b0e0  50                                 push       eax
0046b0e1  51                                 push       ecx
0046b0e2  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0046b0e7  33 c5                              xor        eax, ebp
0046b0e9  50                                 push       eax
0046b0ea  8d 45 f4                           lea        eax, [ebp - 0xc]
0046b0ed  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0046b0f3  89 4d f0                           mov        dword ptr [ebp - 0x10], ecx
0046b0f6  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046b0f9  e8 e2 6b fa ff                     call       0x411ce0
0046b0fe  90                                 nop        
0046b0ff  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046b102  81 c1 f8 05 00 00                  add        ecx, 0x5f8
0046b108  e8 33 e1 fd ff                     call       0x449240
0046b10d  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046b110  83 c1 14                           add        ecx, 0x14
0046b113  e8 58 e2 fd ff                     call       0x449370
0046b118  90                                 nop        
0046b119  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0046b11c  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0046b123  59                                 pop        ecx
0046b124  8b e5                              mov        esp, ebp
0046b126  5d                                 pop        ebp
0046b127  c3                                 ret        
