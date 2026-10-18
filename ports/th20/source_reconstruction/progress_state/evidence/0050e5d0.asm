0050e5d0  55                                 push       ebp
0050e5d1  8b ec                              mov        ebp, esp
0050e5d3  6a ff                              push       -1
0050e5d5  68 23 a9 56 00                     push       0x56a923
0050e5da  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0050e5e0  50                                 push       eax
0050e5e1  83 ec 0c                           sub        esp, 0xc
0050e5e4  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0050e5e9  33 c5                              xor        eax, ebp
0050e5eb  50                                 push       eax
0050e5ec  8d 45 f4                           lea        eax, [ebp - 0xc]
0050e5ef  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0050e5f5  89 4d f0                           mov        dword ptr [ebp - 0x10], ecx
0050e5f8  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050e5fb  e8 40 ff ff ff                     call       0x50e540
0050e600  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050e603  81 c1 40 21 09 00                  add        ecx, 0x92140
0050e609  e8 32 ff ff ff                     call       0x50e540
0050e60e  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0050e611  c7 80 80 42 12 00 00 00 00 00      mov        dword ptr [eax + 0x124280], 0
0050e61b  6a 40                              push       0x40
0050e61d  6a 00                              push       0
0050e61f  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050e622  81 c1 84 42 12 00                  add        ecx, 0x124284
0050e628  51                                 push       ecx
0050e629  e8 32 61 03 00                     call       0x544760
0050e62e  83 c4 0c                           add        esp, 0xc
0050e631  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050e634  81 c1 c4 42 12 00                  add        ecx, 0x1242c4
0050e63a  e8 41 d1 ef ff                     call       0x40b780
0050e63f  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0050e646  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050e649  81 c1 c4 42 12 00                  add        ecx, 0x1242c4
0050e64f  e8 9c d6 ef ff                     call       0x40bcf0
0050e654  90                                 nop        
0050e655  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0050e658  81 c2 c4 42 12 00                  add        edx, 0x1242c4
0050e65e  89 55 e8                           mov        dword ptr [ebp - 0x18], edx
0050e661  c7 45 ec 00 00 00 00               mov        dword ptr [ebp - 0x14], 0
0050e668  8d 45 ec                           lea        eax, [ebp - 0x14]
0050e66b  50                                 push       eax
0050e66c  68 b0 f3 50 00                     push       0x50f3b0
0050e671  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050e674  51                                 push       ecx
0050e675  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0050e678  e8 93 c4 ff ff                     call       0x50ab10
0050e67d  90                                 nop        
0050e67e  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0050e685  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0050e688  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0050e68b  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0050e692  59                                 pop        ecx
0050e693  8b e5                              mov        esp, ebp
0050e695  5d                                 pop        ebp
0050e696  c3                                 ret        
