0046a100  55                                 push       ebp
0046a101  8b ec                              mov        ebp, esp
0046a103  83 ec 0c                           sub        esp, 0xc
0046a106  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0046a10b  33 c5                              xor        eax, ebp
0046a10d  89 45 fc                           mov        dword ptr [ebp - 4], eax
0046a110  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
0046a113  83 7d 08 00                        cmp        dword ptr [ebp + 8], 0
0046a117  75 02                              jne        0x46a11b
0046a119  eb 36                              jmp        0x46a151
0046a11b  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046a11e  50                                 push       eax
0046a11f  e8 ec 00 00 00                     call       0x46a210
0046a124  83 c4 04                           add        esp, 4
0046a127  6a 01                              push       1
0046a129  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0046a12e  e8 dd 21 fa ff                     call       0x40c310
0046a133  50                                 push       eax
0046a134  8d 4d f8                           lea        ecx, [ebp - 8]
0046a137  e8 f4 15 fa ff                     call       0x40b730
0046a13c  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046a13f  51                                 push       ecx
0046a140  e8 b7 88 0d 00                     call       0x5429fc
0046a145  83 c4 04                           add        esp, 4
0046a148  8d 4d f8                           lea        ecx, [ebp - 8]
0046a14b  e8 b0 17 fa ff                     call       0x40b900
0046a150  90                                 nop        
0046a151  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046a154  33 cd                              xor        ecx, ebp
0046a156  e8 93 88 0d 00                     call       0x5429ee
0046a15b  8b e5                              mov        esp, ebp
0046a15d  5d                                 pop        ebp
0046a15e  c2 04 00                           ret        4
