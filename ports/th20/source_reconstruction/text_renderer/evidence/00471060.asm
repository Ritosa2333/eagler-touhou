00471060  55                                 push       ebp
00471061  8b ec                              mov        ebp, esp
00471063  51                                 push       ecx
00471064  68 3c f6 56 00                     push       0x56f63c ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\game\\ascii.cpp:111 AsciiInf'
00471069  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0047106f  e8 fc 90 ff ff                     call       0x46a170
00471074  89 45 fc                           mov        dword ptr [ebp - 4], eax
00471077  83 7d fc 00                        cmp        dword ptr [ebp - 4], 0
0047107b  75 04                              jne        0x471081
0047107d  33 c0                              xor        eax, eax
0047107f  eb 33                              jmp        0x4710b4
00471081  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00471084  e8 f7 af ff ff                     call       0x46c080
00471089  85 c0                              test       eax, eax
0047108b  74 04                              je         0x471091
0047108d  eb 07                              jmp        0x471096
00471091  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00471094  eb 1e                              jmp        0x4710b4
00471096  83 7d fc 00                        cmp        dword ptr [ebp - 4], 0
0047109a  74 16                              je         0x4710b2
0047109c  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0047109f  50                                 push       eax
004710a0  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004710a6  e8 15 e7 fa ff                     call       0x41f7c0
004710ab  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
004710b2  33 c0                              xor        eax, eax
004710b4  8b e5                              mov        esp, ebp
004710b6  5d                                 pop        ebp
004710b7  c3                                 ret        
