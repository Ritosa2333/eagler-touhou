0050ad50  55                                 push       ebp
0050ad51  8b ec                              mov        ebp, esp
0050ad53  83 ec 0c                           sub        esp, 0xc
0050ad56  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0050ad5b  33 c5                              xor        eax, ebp
0050ad5d  89 45 fc                           mov        dword ptr [ebp - 4], eax
0050ad60  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
0050ad63  83 7d 08 00                        cmp        dword ptr [ebp + 8], 0
0050ad67  75 02                              jne        0x50ad6b
0050ad69  eb 36                              jmp        0x50ada1
0050ad6b  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0050ad6e  50                                 push       eax
0050ad6f  e8 dc 00 00 00                     call       0x50ae50
0050ad74  83 c4 04                           add        esp, 4
0050ad77  6a 01                              push       1
0050ad79  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0050ad7e  e8 8d 15 f0 ff                     call       0x40c310
0050ad83  50                                 push       eax
0050ad84  8d 4d f8                           lea        ecx, [ebp - 8]
0050ad87  e8 a4 09 f0 ff                     call       0x40b730
0050ad8c  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0050ad8f  51                                 push       ecx
0050ad90  e8 67 7c 03 00                     call       0x5429fc
0050ad95  83 c4 04                           add        esp, 4
0050ad98  8d 4d f8                           lea        ecx, [ebp - 8]
0050ad9b  e8 60 0b f0 ff                     call       0x40b900
0050ada0  90                                 nop        
0050ada1  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050ada4  33 cd                              xor        ecx, ebp
0050ada6  e8 43 7c 03 00                     call       0x5429ee
0050adab  8b e5                              mov        esp, ebp
0050adad  5d                                 pop        ebp
0050adae  c2 04 00                           ret        4
