00413c50  55                                 push       ebp
00413c51  8b ec                              mov        ebp, esp
00413c53  83 ec 0c                           sub        esp, 0xc
00413c56  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
00413c59  68 24 01 00 00                     push       0x124
00413c5e  e8 fa ea 12 00                     call       0x54275d
00413c63  83 c4 04                           add        esp, 4
00413c66  89 45 fc                           mov        dword ptr [ebp - 4], eax
00413c69  83 7d fc 00                        cmp        dword ptr [ebp - 4], 0
00413c6d  74 1a                              je         0x413c89
00413c6f  68 24 01 00 00                     push       0x124
00413c74  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00413c77  e8 04 84 ff ff                     call       0x40c080
00413c7c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00413c7f  e8 7c 05 00 00                     call       0x414200
00413c84  89 45 f8                           mov        dword ptr [ebp - 8], eax
00413c87  eb 07                              jmp        0x413c90
00413c89  c7 45 f8 00 00 00 00               mov        dword ptr [ebp - 8], 0
00413c90  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
00413c93  8b e5                              mov        esp, ebp
00413c95  5d                                 pop        ebp
00413c96  c2 04 00                           ret        4
