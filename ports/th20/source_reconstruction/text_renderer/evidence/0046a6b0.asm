0046a6b0  55                                 push       ebp
0046a6b1  8b ec                              mov        ebp, esp
0046a6b3  6a ff                              push       -1
0046a6b5  68 00 76 56 00                     push       0x567600
0046a6ba  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0046a6c0  50                                 push       eax
0046a6c1  51                                 push       ecx
0046a6c2  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0046a6c7  33 c5                              xor        eax, ebp
0046a6c9  50                                 push       eax
0046a6ca  8d 45 f4                           lea        eax, [ebp - 0xc]
0046a6cd  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0046a6d3  89 4d f0                           mov        dword ptr [ebp - 0x10], ecx
0046a6d6  68 a0 ab 46 00                     push       0x46aba0
0046a6db  68 40 01 00 00                     push       0x140
0046a6e0  68 40 01 00 00                     push       0x140
0046a6e5  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0046a6e8  50                                 push       eax
0046a6e9  e8 32 15 fa ff                     call       0x40bc20
0046a6ee  90                                 nop        
0046a6ef  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0046a6f2  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0046a6f5  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0046a6fc  59                                 pop        ecx
0046a6fd  8b e5                              mov        esp, ebp
0046a6ff  5d                                 pop        ebp
0046a700  c3                                 ret        
