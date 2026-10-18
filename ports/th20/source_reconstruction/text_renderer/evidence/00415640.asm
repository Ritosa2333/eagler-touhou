00415640  55                                 push       ebp
00415641  8b ec                              mov        ebp, esp
00415643  83 ec 08                           sub        esp, 8
00415646  89 4d f8                           mov        dword ptr [ebp - 8], ecx
00415649  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
00415650  eb 09                              jmp        0x41565b
00415652  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00415655  83 c0 01                           add        eax, 1
00415658  89 45 fc                           mov        dword ptr [ebp - 4], eax
0041565b  6b 4d fc 18                        imul       ecx, dword ptr [ebp - 4], 0x18
0041565f  83 b9 78 e0 5a 00 ff               cmp        dword ptr [ecx + 0x5ae078], -1
00415666  74 11                              je         0x415679
00415668  6b 55 fc 18                        imul       edx, dword ptr [ebp - 4], 0x18
0041566c  8b 82 78 e0 5a 00                  mov        eax, dword ptr [edx + 0x5ae078]
00415672  3b 45 08                           cmp        eax, dword ptr [ebp + 8]
00415675  74 02                              je         0x415679
00415677  eb d9                              jmp        0x415652
00415679  83 7d 08 ff                        cmp        dword ptr [ebp + 8], -1
0041567d  75 04                              jne        0x415683
0041567f  33 c0                              xor        eax, eax
00415681  eb 09                              jmp        0x41568c
00415683  6b 45 fc 18                        imul       eax, dword ptr [ebp - 4], 0x18
00415687  05 78 e0 5a 00                     add        eax, 0x5ae078
0041568c  8b e5                              mov        esp, ebp
0041568e  5d                                 pop        ebp
0041568f  c2 04 00                           ret        4
