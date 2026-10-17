004de040  55                                 push       ebp
004de041  8b ec                              mov        ebp, esp
004de043  83 ec 28                           sub        esp, 0x28
004de046  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
004de04b  33 c5                              xor        eax, ebp
004de04d  89 45 fc                           mov        dword ptr [ebp - 4], eax
004de050  89 4d ec                           mov        dword ptr [ebp - 0x14], ecx
004de053  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
004de056  81 c1 a0 0d 00 00                  add        ecx, 0xda0
004de05c  e8 8f dc f2 ff                     call       0x40bcf0
004de061  c7 45 f8 00 00 00 00               mov        dword ptr [ebp - 8], 0
004de068  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004de06b  50                                 push       eax
004de06c  68 1c 1b 57 00                     push       0x571b1c ; string='SnapShot! %s'
004de071  e8 3a e6 f2 ff                     call       0x40c6b0
004de076  83 c4 08                           add        esp, 8
004de079  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
004de07c  8b 51 08                           mov        edx, dword ptr [ecx + 8]
004de07f  89 55 e4                           mov        dword ptr [ebp - 0x1c], edx
004de082  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
004de085  8b 48 08                           mov        ecx, dword ptr [eax + 8]
004de088  8b 11                              mov        edx, dword ptr [ecx]
004de08a  8b 42 48                           mov        eax, dword ptr [edx + 0x48]
004de08d  89 45 e0                           mov        dword ptr [ebp - 0x20], eax
004de090  8d 4d f8                           lea        ecx, [ebp - 8]
004de093  51                                 push       ecx
004de094  6a 00                              push       0
004de096  6a 00                              push       0
004de098  6a 00                              push       0
004de09a  8b 55 e4                           mov        edx, dword ptr [ebp - 0x1c]
004de09d  52                                 push       edx
004de09e  ff 55 e0                           call       dword ptr [ebp - 0x20]
004de0a1  90                                 nop        
004de0a2  83 7d f8 00                        cmp        dword ptr [ebp - 8], 0
004de0a6  75 08                              jne        0x4de0b0
004de0a8  83 c8 ff                           or         eax, 0xffffffff
004de0ab  e9 29 01 00 00                     jmp        0x4de1d9
004de0b0  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004de0b3  50                                 push       eax
004de0b4  68 04 01 00 00                     push       0x104
004de0b9  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
004de0bc  81 c1 8c 0c 00 00                  add        ecx, 0xc8c
004de0c2  51                                 push       ecx
004de0c3  e8 48 a5 06 00                     call       0x548610
004de0c8  83 c4 0c                           add        esp, 0xc
004de0cb  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
004de0ce  8b 82 ec 00 00 00                  mov        eax, dword ptr [edx + 0xec]
004de0d4  89 45 e8                           mov        dword ptr [ebp - 0x18], eax
004de0d7  83 7d e8 15                        cmp        dword ptr [ebp - 0x18], 0x15
004de0db  74 3a                              je         0x4de117
004de0dd  83 7d e8 16                        cmp        dword ptr [ebp - 0x18], 0x16
004de0e1  74 34                              je         0x4de117
004de0e3  83 7d e8 17                        cmp        dword ptr [ebp - 0x18], 0x17
004de0e7  74 05                              je         0x4de0ee
004de0e9  e9 b5 00 00 00                     jmp        0x4de1a3
004de0ee  68 2c 1b 57 00                     push       0x571b2c ; string='16bit '
004de0f3  e8 b8 e5 f2 ff                     call       0x40c6b0
004de0f8  83 c4 04                           add        esp, 4
004de0fb  68 2c 1b 57 00                     push       0x571b2c ; string='16bit '
004de100  68 78 06 5c 00                     push       0x5c0678
004de105  e8 46 60 f7 ff                     call       0x454150
004de10a  83 c4 08                           add        esp, 8
004de10d  e9 aa 00 00 00                     jmp        0x4de1bc
004de117  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004de11a  8b 11                              mov        edx, dword ptr [ecx]
004de11c  8b 42 34                           mov        eax, dword ptr [edx + 0x34]
004de11f  89 45 dc                           mov        dword ptr [ebp - 0x24], eax
004de122  6a 00                              push       0
004de124  6a 00                              push       0
004de126  8d 4d f0                           lea        ecx, [ebp - 0x10]
004de129  51                                 push       ecx
004de12a  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
004de12d  52                                 push       edx
004de12e  ff 55 dc                           call       dword ptr [ebp - 0x24]
004de131  68 44 1b 57 00                     push       0x571b44 ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\game\\mother.cpp:424 BYTE'
004de136  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
004de139  0f af 05 14 88 5b 00               imul       eax, dword ptr [0x5b8814]
004de140  50                                 push       eax
004de141  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004de147  e8 c4 14 f4 ff                     call       0x41f610
004de14c  a3 c4 59 5c 00                     mov        dword ptr [0x5c59c4], eax
004de151  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004de154  0f af 0d 14 88 5b 00               imul       ecx, dword ptr [0x5b8814]
004de15b  51                                 push       ecx
004de15c  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
004de15f  52                                 push       edx
004de160  a1 c4 59 5c 00                     mov        eax, dword ptr [0x5c59c4]
004de165  50                                 push       eax
004de166  e8 b5 5c 06 00                     call       0x543e20
004de16b  83 c4 0c                           add        esp, 0xc
004de16e  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004de171  89 0d c8 59 5c 00                  mov        dword ptr [0x5c59c8], ecx
004de177  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
004de17a  8b 02                              mov        eax, dword ptr [edx]
004de17c  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004de17f  51                                 push       ecx
004de180  8b 50 38                           mov        edx, dword ptr [eax + 0x38]
004de183  ff d2                              call       edx
004de185  90                                 nop        
004de186  c7 45 d8 00 00 00 00               mov        dword ptr [ebp - 0x28], 0
004de18d  8d 45 d8                           lea        eax, [ebp - 0x28]
004de190  50                                 push       eax
004de191  68 10 92 4d 00                     push       0x4d9210
004de196  b9 e0 5a 5c 00                     mov        ecx, 0x5c5ae0
004de19b  e8 30 d0 f2 ff                     call       0x40b1d0
004de1a0  90                                 nop        
004de1a1  eb 19                              jmp        0x4de1bc
004de1a3  68 80 1b 57 00                     push       0x571b80 ; string='error : snapShotScreen'
004de1a8  68 78 06 5c 00                     push       0x5c0678
004de1ad  e8 9e 5f f7 ff                     call       0x454150
004de1b2  83 c4 08                           add        esp, 8
004de1b5  b8 01 00 00 00                     mov        eax, 1
004de1ba  eb 1d                              jmp        0x4de1d9
004de1bc  83 7d f8 00                        cmp        dword ptr [ebp - 8], 0
004de1c0  74 15                              je         0x4de1d7
004de1c2  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004de1c5  8b 11                              mov        edx, dword ptr [ecx]
004de1c7  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
004de1ca  50                                 push       eax
004de1cb  8b 4a 08                           mov        ecx, dword ptr [edx + 8]
004de1ce  ff d1                              call       ecx
004de1d0  c7 45 f8 00 00 00 00               mov        dword ptr [ebp - 8], 0
004de1d7  33 c0                              xor        eax, eax
004de1d9  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004de1dc  33 cd                              xor        ecx, ebp
004de1de  e8 0b 48 06 00                     call       0x5429ee
004de1e3  8b e5                              mov        esp, ebp
004de1e5  5d                                 pop        ebp
004de1e6  c2 04 00                           ret        4
