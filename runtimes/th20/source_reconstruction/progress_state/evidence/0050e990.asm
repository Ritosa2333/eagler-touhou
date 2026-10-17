0050e990  55                                 push       ebp
0050e991  8b ec                              mov        ebp, esp
0050e993  6a ff                              push       -1
0050e995  68 00 76 56 00                     push       0x567600
0050e99a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0050e9a0  50                                 push       eax
0050e9a1  51                                 push       ecx
0050e9a2  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0050e9a7  33 c5                              xor        eax, ebp
0050e9a9  50                                 push       eax
0050e9aa  8d 45 f4                           lea        eax, [ebp - 0xc]
0050e9ad  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0050e9b3  89 4d f0                           mov        dword ptr [ebp - 0x10], ecx
0050e9b6  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050e9b9  e8 a2 0c 00 00                     call       0x50f660
0050e9be  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050e9c1  81 c1 c4 42 12 00                  add        ecx, 0x1242c4
0050e9c7  e8 24 d3 ef ff                     call       0x40bcf0
0050e9cc  90                                 nop        
0050e9cd  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0050e9d0  83 78 04 00                        cmp        dword ptr [eax + 4], 0
0050e9d4  74 25                              je         0x50e9fb
0050e9d6  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050e9d9  83 79 04 00                        cmp        dword ptr [ecx + 4], 0
0050e9dd  74 1c                              je         0x50e9fb
0050e9df  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0050e9e2  8b 42 04                           mov        eax, dword ptr [edx + 4]
0050e9e5  50                                 push       eax
0050e9e6  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0050e9ec  e8 7f 0c f1 ff                     call       0x41f670
0050e9f1  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050e9f4  c7 41 04 00 00 00 00               mov        dword ptr [ecx + 4], 0
0050e9fb  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0050e9fe  83 7a 08 00                        cmp        dword ptr [edx + 8], 0
0050ea02  74 25                              je         0x50ea29
0050ea04  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0050ea07  83 78 08 00                        cmp        dword ptr [eax + 8], 0
0050ea0b  74 1c                              je         0x50ea29
0050ea0d  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050ea10  8b 51 08                           mov        edx, dword ptr [ecx + 8]
0050ea13  52                                 push       edx
0050ea14  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0050ea1a  e8 51 0c f1 ff                     call       0x41f670
0050ea1f  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0050ea22  c7 40 08 00 00 00 00               mov        dword ptr [eax + 8], 0
0050ea29  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050ea2c  83 b9 44 21 09 00 00               cmp        dword ptr [ecx + 0x92144], 0
0050ea33  74 2e                              je         0x50ea63
0050ea35  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0050ea38  83 ba 44 21 09 00 00               cmp        dword ptr [edx + 0x92144], 0
0050ea3f  74 22                              je         0x50ea63
0050ea41  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0050ea44  8b 88 44 21 09 00                  mov        ecx, dword ptr [eax + 0x92144]
0050ea4a  51                                 push       ecx
0050ea4b  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0050ea51  e8 1a 0c f1 ff                     call       0x41f670
0050ea56  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0050ea59  c7 82 44 21 09 00 00 00 00 00      mov        dword ptr [edx + 0x92144], 0
0050ea63  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0050ea66  83 b8 48 21 09 00 00               cmp        dword ptr [eax + 0x92148], 0
0050ea6d  74 2e                              je         0x50ea9d
0050ea6f  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050ea72  83 b9 48 21 09 00 00               cmp        dword ptr [ecx + 0x92148], 0
0050ea79  74 22                              je         0x50ea9d
0050ea7b  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0050ea7e  8b 82 48 21 09 00                  mov        eax, dword ptr [edx + 0x92148]
0050ea84  50                                 push       eax
0050ea85  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0050ea8b  e8 e0 0b f1 ff                     call       0x41f670
0050ea90  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050ea93  c7 81 48 21 09 00 00 00 00 00      mov        dword ptr [ecx + 0x92148], 0
0050ea9d  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050eaa0  81 c1 c4 42 12 00                  add        ecx, 0x1242c4
0050eaa6  e8 d5 ce ef ff                     call       0x40b980
0050eaab  90                                 nop        
0050eaac  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0050eaaf  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0050eab6  59                                 pop        ecx
0050eab7  8b e5                              mov        esp, ebp
0050eab9  5d                                 pop        ebp
0050eaba  c3                                 ret        
