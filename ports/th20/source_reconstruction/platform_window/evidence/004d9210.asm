004d9210  55                                 push       ebp
004d9211  8b ec                              mov        ebp, esp
004d9213  83 ec 38                           sub        esp, 0x38
004d9216  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
004d921b  33 c5                              xor        eax, ebp
004d921d  89 45 fc                           mov        dword ptr [ebp - 4], eax
004d9220  68 00 1c 57 00                     push       0x571c00 ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\game\\mother.cpp:501 BITMAPINFO'
004d9225  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004d922b  e8 10 f5 ff ff                     call       0x4d8740
004d9230  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
004d9233  33 c0                              xor        eax, eax
004d9235  89 45 ec                           mov        dword ptr [ebp - 0x14], eax
004d9238  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
004d923b  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
004d923e  66 89 45 f8                        mov        word ptr [ebp - 8], ax
004d9242  6a 0e                              push       0xe
004d9244  6a 00                              push       0
004d9246  8d 4d ec                           lea        ecx, [ebp - 0x14]
004d9249  51                                 push       ecx
004d924a  e8 11 b5 06 00                     call       0x544760
004d924f  83 c4 0c                           add        esp, 0xc
004d9252  ba 42 4d 00 00                     mov        edx, 0x4d42
004d9257  66 89 55 ec                        mov        word ptr [ebp - 0x14], dx
004d925b  c7 45 f6 36 00 00 00               mov        dword ptr [ebp - 0xa], 0x36
004d9262  8b 45 f6                           mov        eax, dword ptr [ebp - 0xa]
004d9265  89 45 ee                           mov        dword ptr [ebp - 0x12], eax
004d9268  6b 0d f8 87 5b 00 03               imul       ecx, dword ptr [0x5b87f8], 3
004d926f  89 4d c8                           mov        dword ptr [ebp - 0x38], ecx
004d9272  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
004d9275  0f af 15 fc 87 5b 00               imul       edx, dword ptr [0x5b87fc]
004d927c  03 55 ee                           add        edx, dword ptr [ebp - 0x12]
004d927f  89 55 ee                           mov        dword ptr [ebp - 0x12], edx
004d9282  b8 18 00 00 00                     mov        eax, 0x18
004d9287  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
004d928a  66 89 41 0e                        mov        word ptr [ecx + 0xe], ax
004d928e  8b 55 e4                           mov        edx, dword ptr [ebp - 0x1c]
004d9291  c7 02 28 00 00 00                  mov        dword ptr [edx], 0x28
004d9297  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
004d929a  8b 0d f8 87 5b 00                  mov        ecx, dword ptr [0x5b87f8]
004d92a0  89 48 04                           mov        dword ptr [eax + 4], ecx
004d92a3  8b 55 e4                           mov        edx, dword ptr [ebp - 0x1c]
004d92a6  a1 fc 87 5b 00                     mov        eax, dword ptr [0x5b87fc]
004d92ab  89 42 08                           mov        dword ptr [edx + 8], eax
004d92ae  b9 01 00 00 00                     mov        ecx, 1
004d92b3  8b 55 e4                           mov        edx, dword ptr [ebp - 0x1c]
004d92b6  66 89 4a 0c                        mov        word ptr [edx + 0xc], cx
004d92ba  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
004d92bd  c7 40 10 00 00 00 00               mov        dword ptr [eax + 0x10], 0
004d92c4  8b 0d c4 59 5c 00                  mov        ecx, dword ptr [0x5c59c4]
004d92ca  89 4d d8                           mov        dword ptr [ebp - 0x28], ecx
004d92cd  6a 00                              push       0
004d92cf  ff 15 ac c2 56 00                  call       dword ptr [0x56c2ac] ; USER32.dll!GetDC
004d92d5  89 45 cc                           mov        dword ptr [ebp - 0x34], eax
004d92d8  6a 00                              push       0
004d92da  6a 00                              push       0
004d92dc  8d 55 e8                           lea        edx, [ebp - 0x18]
004d92df  52                                 push       edx
004d92e0  6a 00                              push       0
004d92e2  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
004d92e5  50                                 push       eax
004d92e6  8b 4d cc                           mov        ecx, dword ptr [ebp - 0x34]
004d92e9  51                                 push       ecx
004d92ea  ff 15 20 c0 56 00                  call       dword ptr [0x56c020] ; GDI32.dll!CreateDIBSection
004d92f0  89 45 d0                           mov        dword ptr [ebp - 0x30], eax
004d92f3  ba 6c 01 00 00                     mov        edx, 0x16c
004d92f8  d1 e2                              shl        edx, 1
004d92fa  8b 82 b4 50 5c 00                  mov        eax, dword ptr [edx + 0x5c50b4]
004d9300  8b 4d d8                           mov        ecx, dword ptr [ebp - 0x28]
004d9303  8d 14 81                           lea        edx, [ecx + eax*4]
004d9306  b8 6c 01 00 00                     mov        eax, 0x16c
004d930b  d1 e0                              shl        eax, 1
004d930d  8b 88 b8 50 5c 00                  mov        ecx, dword ptr [eax + 0x5c50b8]
004d9313  0f af 0d c8 59 5c 00               imul       ecx, dword ptr [0x5c59c8]
004d931a  03 d1                              add        edx, ecx
004d931c  89 55 d8                           mov        dword ptr [ebp - 0x28], edx
004d931f  8b 15 fc 87 5b 00                  mov        edx, dword ptr [0x5b87fc]
004d9325  83 ea 01                           sub        edx, 1
004d9328  89 55 dc                           mov        dword ptr [ebp - 0x24], edx
004d932b  eb 09                              jmp        0x4d9336
004d932d  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
004d9330  83 e8 01                           sub        eax, 1
004d9333  89 45 dc                           mov        dword ptr [ebp - 0x24], eax
004d9336  83 7d dc ff                        cmp        dword ptr [ebp - 0x24], -1
004d933a  7e 65                              jle        0x4d93a1
004d933c  8b 0d c8 59 5c 00                  mov        ecx, dword ptr [0x5c59c8]
004d9342  0f af 4d dc                        imul       ecx, dword ptr [ebp - 0x24]
004d9346  03 4d d8                           add        ecx, dword ptr [ebp - 0x28]
004d9349  89 4d e0                           mov        dword ptr [ebp - 0x20], ecx
004d934c  c7 45 d4 00 00 00 00               mov        dword ptr [ebp - 0x2c], 0
004d9353  eb 09                              jmp        0x4d935e
004d9355  8b 55 d4                           mov        edx, dword ptr [ebp - 0x2c]
004d9358  83 c2 01                           add        edx, 1
004d935b  89 55 d4                           mov        dword ptr [ebp - 0x2c], edx
004d935e  8b 45 d4                           mov        eax, dword ptr [ebp - 0x2c]
004d9361  3b 05 f8 87 5b 00                  cmp        eax, dword ptr [0x5b87f8]
004d9367  7d 36                              jge        0x4d939f
004d9369  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004d936c  8b 55 e0                           mov        edx, dword ptr [ebp - 0x20]
004d936f  8a 42 02                           mov        al, byte ptr [edx + 2]
004d9372  88 41 02                           mov        byte ptr [ecx + 2], al
004d9375  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004d9378  8b 55 e0                           mov        edx, dword ptr [ebp - 0x20]
004d937b  8a 42 01                           mov        al, byte ptr [edx + 1]
004d937e  88 41 01                           mov        byte ptr [ecx + 1], al
004d9381  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004d9384  8b 55 e0                           mov        edx, dword ptr [ebp - 0x20]
004d9387  8a 02                              mov        al, byte ptr [edx]
004d9389  88 01                              mov        byte ptr [ecx], al
004d938b  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
004d938e  83 c1 04                           add        ecx, 4
004d9391  89 4d e0                           mov        dword ptr [ebp - 0x20], ecx
004d9394  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
004d9397  83 c2 03                           add        edx, 3
004d939a  89 55 e8                           mov        dword ptr [ebp - 0x18], edx
004d939d  eb b6                              jmp        0x4d9355
004d939f  eb 8c                              jmp        0x4d932d
004d93a1  68 cc 59 5c 00                     push       0x5c59cc
004d93a6  8b 45 d0                           mov        eax, dword ptr [ebp - 0x30]
004d93a9  50                                 push       eax
004d93aa  e8 91 08 00 00                     call       0x4d9c40
004d93af  83 c4 08                           add        esp, 8
004d93b2  8b 4d d0                           mov        ecx, dword ptr [ebp - 0x30]
004d93b5  51                                 push       ecx
004d93b6  ff 15 38 c0 56 00                  call       dword ptr [0x56c038] ; GDI32.dll!DeleteObject
004d93bc  90                                 nop        
004d93bd  83 7d e4 00                        cmp        dword ptr [ebp - 0x1c], 0
004d93c1  74 16                              je         0x4d93d9
004d93c3  8b 55 e4                           mov        edx, dword ptr [ebp - 0x1c]
004d93c6  52                                 push       edx
004d93c7  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004d93cd  e8 9e 62 f4 ff                     call       0x41f670
004d93d2  c7 45 e4 00 00 00 00               mov        dword ptr [ebp - 0x1c], 0
004d93d9  83 3d c4 59 5c 00 00               cmp        dword ptr [0x5c59c4], 0
004d93e0  74 1b                              je         0x4d93fd
004d93e2  a1 c4 59 5c 00                     mov        eax, dword ptr [0x5c59c4]
004d93e7  50                                 push       eax
004d93e8  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004d93ee  e8 7d 62 f4 ff                     call       0x41f670
004d93f3  c7 05 c4 59 5c 00 00 00 00 00      mov        dword ptr [0x5c59c4], 0
004d93fd  8b 4d cc                           mov        ecx, dword ptr [ebp - 0x34]
004d9400  51                                 push       ecx
004d9401  6a 00                              push       0
004d9403  ff 15 b0 c2 56 00                  call       dword ptr [0x56c2b0] ; USER32.dll!ReleaseDC
004d9409  33 c0                              xor        eax, eax
004d940b  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004d940e  33 cd                              xor        ecx, ebp
004d9410  e8 d9 95 06 00                     call       0x5429ee
004d9415  8b e5                              mov        esp, ebp
004d9417  5d                                 pop        ebp
004d9418  c3                                 ret        
