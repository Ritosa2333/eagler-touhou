0046a1c0  55                                 push       ebp
0046a1c1  8b ec                              mov        ebp, esp
0046a1c3  83 ec 0c                           sub        esp, 0xc
0046a1c6  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
0046a1c9  68 84 06 00 00                     push       0x684
0046a1ce  e8 8a 85 0d 00                     call       0x54275d
0046a1d3  83 c4 04                           add        esp, 4
0046a1d6  89 45 fc                           mov        dword ptr [ebp - 4], eax
0046a1d9  83 7d fc 00                        cmp        dword ptr [ebp - 4], 0
0046a1dd  74 1a                              je         0x46a1f9
0046a1df  68 84 06 00 00                     push       0x684
0046a1e4  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046a1e7  e8 94 1e fa ff                     call       0x40c080
0046a1ec  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046a1ef  e8 8c 0a 00 00                     call       0x46ac80
0046a1f4  89 45 f8                           mov        dword ptr [ebp - 8], eax
0046a1f7  eb 07                              jmp        0x46a200
0046a1f9  c7 45 f8 00 00 00 00               mov        dword ptr [ebp - 8], 0
0046a200  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
0046a203  8b e5                              mov        esp, ebp
0046a205  5d                                 pop        ebp
0046a206  c2 04 00                           ret        4
