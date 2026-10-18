0041c1a0  55                                 push       ebp
0041c1a1  8b ec                              mov        ebp, esp
0041c1a3  83 ec 30                           sub        esp, 0x30
0041c1a6  a1 88 58 5c 00                     mov        eax, dword ptr [0x5c5888]
0041c1ab  83 e0 fe                           and        eax, 0xfffffffe
0041c1ae  a3 88 58 5c 00                     mov        dword ptr [0x5c5888], eax
0041c1b3  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041c1b8  e8 e3 0e 00 00                     call       0x41d0a0
0041c1bd  85 c0                              test       eax, eax
0041c1bf  74 0a                              je         0x41c1cb
0041c1c1  e9 bd 00 00 00                     jmp        0x41c283
0041c1cb  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041c1d0  e8 bb 3d ff ff                     call       0x40ff90
0041c1d5  89 45 fc                           mov        dword ptr [ebp - 4], eax
0041c1d8  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041c1db  8b 11                              mov        edx, dword ptr [ecx]
0041c1dd  8b 42 40                           mov        eax, dword ptr [edx + 0x40]
0041c1e0  89 45 e8                           mov        dword ptr [ebp - 0x18], eax
0041c1e3  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041c1e8  e8 83 08 00 00                     call       0x41ca70
0041c1ed  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0041c1f0  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041c1f5  e8 06 01 ff ff                     call       0x40c300
0041c1fa  89 45 ec                           mov        dword ptr [ebp - 0x14], eax
0041c1fd  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0041c200  51                                 push       ecx
0041c201  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0041c204  52                                 push       edx
0041c205  6a 40                              push       0x40
0041c207  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
0041c20a  50                                 push       eax
0041c20b  6a 01                              push       1
0041c20d  6a 00                              push       0
0041c20f  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041c212  51                                 push       ecx
0041c213  ff 55 e8                           call       dword ptr [ebp - 0x18]
0041c216  85 c0                              test       eax, eax
0041c218  75 16                              jne        0x41c230
0041c21a  8b 15 88 58 5c 00                  mov        edx, dword ptr [0x5c5888]
0041c220  83 ca 01                           or         edx, 1
0041c223  89 15 88 58 5c 00                  mov        dword ptr [0x5c5888], edx
0041c229  33 c0                              xor        eax, eax
0041c22b  e9 ab 00 00 00                     jmp        0x41c2db
0041c230  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041c235  e8 56 3d ff ff                     call       0x40ff90
0041c23a  89 45 f8                           mov        dword ptr [ebp - 8], eax
0041c23d  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
0041c240  8b 08                              mov        ecx, dword ptr [eax]
0041c242  8b 51 40                           mov        edx, dword ptr [ecx + 0x40]
0041c245  89 55 dc                           mov        dword ptr [ebp - 0x24], edx
0041c248  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041c24d  e8 1e 08 00 00                     call       0x41ca70
0041c252  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
0041c255  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041c25a  e8 a1 00 ff ff                     call       0x40c300
0041c25f  89 45 e0                           mov        dword ptr [ebp - 0x20], eax
0041c262  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
0041c265  50                                 push       eax
0041c266  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0041c269  51                                 push       ecx
0041c26a  6a 20                              push       0x20
0041c26c  8b 55 e0                           mov        edx, dword ptr [ebp - 0x20]
0041c26f  52                                 push       edx
0041c270  6a 01                              push       1
0041c272  6a 00                              push       0
0041c274  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
0041c277  50                                 push       eax
0041c278  ff 55 dc                           call       dword ptr [ebp - 0x24]
0041c27b  85 c0                              test       eax, eax
0041c27d  75 04                              jne        0x41c283
0041c27f  33 c0                              xor        eax, eax
0041c281  eb 58                              jmp        0x41c2db
0041c283  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041c288  e8 03 3d ff ff                     call       0x40ff90
0041c28d  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
0041c290  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0041c293  8b 11                              mov        edx, dword ptr [ecx]
0041c295  8b 42 40                           mov        eax, dword ptr [edx + 0x40]
0041c298  89 45 d0                           mov        dword ptr [ebp - 0x30], eax
0041c29b  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041c2a0  e8 cb 07 00 00                     call       0x41ca70
0041c2a5  89 45 d8                           mov        dword ptr [ebp - 0x28], eax
0041c2a8  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041c2ad  e8 4e 00 ff ff                     call       0x40c300
0041c2b2  89 45 d4                           mov        dword ptr [ebp - 0x2c], eax
0041c2b5  8b 4d d8                           mov        ecx, dword ptr [ebp - 0x28]
0041c2b8  51                                 push       ecx
0041c2b9  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0041c2bc  52                                 push       edx
0041c2bd  6a 20                              push       0x20
0041c2bf  8b 45 d4                           mov        eax, dword ptr [ebp - 0x2c]
0041c2c2  50                                 push       eax
0041c2c3  6a 02                              push       2
0041c2c5  6a 00                              push       0
0041c2c7  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0041c2ca  51                                 push       ecx
0041c2cb  ff 55 d0                           call       dword ptr [ebp - 0x30]
0041c2ce  85 c0                              test       eax, eax
0041c2d0  75 04                              jne        0x41c2d6
0041c2d2  33 c0                              xor        eax, eax
0041c2d4  eb 05                              jmp        0x41c2db
0041c2d6  b8 01 00 00 00                     mov        eax, 1
0041c2db  8b e5                              mov        esp, ebp
0041c2dd  5d                                 pop        ebp
0041c2de  c3                                 ret        
