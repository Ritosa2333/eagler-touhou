0046a170  55                                 push       ebp
0046a171  8b ec                              mov        ebp, esp
0046a173  83 ec 0c                           sub        esp, 0xc
0046a176  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
0046a179  68 60 a3 01 00                     push       0x1a360
0046a17e  e8 da 85 0d 00                     call       0x54275d
0046a183  83 c4 04                           add        esp, 4
0046a186  89 45 fc                           mov        dword ptr [ebp - 4], eax
0046a189  83 7d fc 00                        cmp        dword ptr [ebp - 4], 0
0046a18d  74 1a                              je         0x46a1a9
0046a18f  68 60 a3 01 00                     push       0x1a360
0046a194  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046a197  e8 e4 1e fa ff                     call       0x40c080
0046a19c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046a19f  e8 3c 07 00 00                     call       0x46a8e0
0046a1a4  89 45 f8                           mov        dword ptr [ebp - 8], eax
0046a1a7  eb 07                              jmp        0x46a1b0
0046a1a9  c7 45 f8 00 00 00 00               mov        dword ptr [ebp - 8], 0
0046a1b0  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
0046a1b3  8b e5                              mov        esp, ebp
0046a1b5  5d                                 pop        ebp
0046a1b6  c2 04 00                           ret        4
