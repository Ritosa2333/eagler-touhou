0050f3b0  55                                 push       ebp
0050f3b1  8b ec                              mov        ebp, esp
0050f3b3  6a ff                              push       -1
0050f3b5  68 5d a9 56 00                     push       0x56a95d
0050f3ba  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0050f3c0  50                                 push       eax
0050f3c1  83 ec 6c                           sub        esp, 0x6c
0050f3c4  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0050f3c9  33 c5                              xor        eax, ebp
0050f3cb  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0050f3ce  50                                 push       eax
0050f3cf  8d 45 f4                           lea        eax, [ebp - 0xc]
0050f3d2  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0050f3d8  89 4d b8                           mov        dword ptr [ebp - 0x48], ecx
0050f3db  6a 14                              push       0x14
0050f3dd  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0050f3e2  e8 29 cf ef ff                     call       0x40c310
0050f3e7  50                                 push       eax
0050f3e8  8d 4d ec                           lea        ecx, [ebp - 0x14]
0050f3eb  e8 40 c3 ef ff                     call       0x40b730
0050f3f0  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0050f3f7  6a 00                              push       0
0050f3f9  68 e1 67 5b 00                     push       0x5b67e1
0050f3fe  8d 4d d4                           lea        ecx, [ebp - 0x2c]
0050f401  e8 ca 88 f0 ff                     call       0x417cd0
0050f406  c6 45 fc 01                        mov        byte ptr [ebp - 4], 1
0050f40a  68 1c 37 57 00                     push       0x57371c ; string='scoreth20bak.dat'
0050f40f  8d 4d d4                           lea        ecx, [ebp - 0x2c]
0050f412  e8 29 8a f0 ff                     call       0x417e40
0050f417  8d 45 bc                           lea        eax, [ebp - 0x44]
0050f41a  50                                 push       eax
0050f41b  8d 4d d4                           lea        ecx, [ebp - 0x2c]
0050f41e  e8 6d f3 f0 ff                     call       0x41e790
0050f423  c6 45 fc 02                        mov        byte ptr [ebp - 4], 2
0050f427  8d 4d bc                           lea        ecx, [ebp - 0x44]
0050f42a  e8 01 0b f0 ff                     call       0x40ff30
0050f42f  89 45 a4                           mov        dword ptr [ebp - 0x5c], eax
0050f432  6a 01                              push       1
0050f434  8b 4d b8                           mov        ecx, dword ptr [ebp - 0x48]
0050f437  81 c1 40 21 09 00                  add        ecx, 0x92140
0050f43d  51                                 push       ecx
0050f43e  8b 55 a4                           mov        edx, dword ptr [ebp - 0x5c]
0050f441  52                                 push       edx
0050f442  e8 59 16 f0 ff                     call       0x410aa0
0050f447  83 c4 0c                           add        esp, 0xc
0050f44a  8b 4d b8                           mov        ecx, dword ptr [ebp - 0x48]
0050f44d  89 81 44 21 09 00                  mov        dword ptr [ecx + 0x92144], eax
0050f453  8b 4d b8                           mov        ecx, dword ptr [ebp - 0x48]
0050f456  81 c1 88 40 12 00                  add        ecx, 0x124088
0050f45c  e8 2f fc ff ff                     call       0x50f090
0050f461  90                                 nop        
0050f462  c7 45 b4 00 00 00 00               mov        dword ptr [ebp - 0x4c], 0
0050f469  eb 09                              jmp        0x50f474
0050f46b  8b 55 b4                           mov        edx, dword ptr [ebp - 0x4c]
0050f46e  83 c2 01                           add        edx, 1
0050f471  89 55 b4                           mov        dword ptr [ebp - 0x4c], edx
0050f474  83 7d b4 02                        cmp        dword ptr [ebp - 0x4c], 2
0050f478  7d 3c                              jge        0x50f4b6
0050f47a  c7 45 b0 00 00 00 00               mov        dword ptr [ebp - 0x50], 0
0050f481  eb 09                              jmp        0x50f48c
0050f483  8b 45 b0                           mov        eax, dword ptr [ebp - 0x50]
0050f486  83 c0 01                           add        eax, 1
0050f489  89 45 b0                           mov        dword ptr [ebp - 0x50], eax
0050f48c  83 7d b0 0a                        cmp        dword ptr [ebp - 0x50], 0xa
0050f490  7d 22                              jge        0x50f4b4
0050f492  69 4d b4 28 52 04 00               imul       ecx, dword ptr [ebp - 0x4c], 0x45228
0050f499  8b 55 b8                           mov        edx, dword ptr [ebp - 0x48]
0050f49c  8d 8c 0a 50 21 09 00               lea        ecx, [edx + ecx + 0x92150]
0050f4a3  69 45 b0 e8 7a 00 00               imul       eax, dword ptr [ebp - 0x50], 0x7ae8
0050f4aa  03 c8                              add        ecx, eax
0050f4ac  e8 4f fa ff ff                     call       0x50ef00
0050f4b1  90                                 nop        
0050f4b2  eb cf                              jmp        0x50f483
0050f4b4  eb b5                              jmp        0x50f46b
0050f4b6  8b 4d b8                           mov        ecx, dword ptr [ebp - 0x48]
0050f4b9  81 c1 40 21 09 00                  add        ecx, 0x92140
0050f4bf  51                                 push       ecx
0050f4c0  8b 4d b8                           mov        ecx, dword ptr [ebp - 0x48]
0050f4c3  e8 a8 f6 ff ff                     call       0x50eb70
0050f4c8  68 e1 67 5b 00                     push       0x5b67e1
0050f4cd  8d 4d d4                           lea        ecx, [ebp - 0x2c]
0050f4d0  e8 1b 89 f0 ff                     call       0x417df0
0050f4d5  68 30 37 57 00                     push       0x573730 ; string='scoreth20.dat'
0050f4da  8d 4d d4                           lea        ecx, [ebp - 0x2c]
0050f4dd  e8 5e 89 f0 ff                     call       0x417e40
0050f4e2  90                                 nop        
0050f4e3  8d 55 88                           lea        edx, [ebp - 0x78]
0050f4e6  52                                 push       edx
0050f4e7  8d 4d d4                           lea        ecx, [ebp - 0x2c]
0050f4ea  e8 a1 f2 f0 ff                     call       0x41e790
0050f4ef  89 45 a0                           mov        dword ptr [ebp - 0x60], eax
0050f4f2  8b 45 a0                           mov        eax, dword ptr [ebp - 0x60]
0050f4f5  50                                 push       eax
0050f4f6  8d 4d bc                           lea        ecx, [ebp - 0x44]
0050f4f9  e8 b2 99 f0 ff                     call       0x418eb0
0050f4fe  8d 4d 88                           lea        ecx, [ebp - 0x78]
0050f501  e8 9a f0 ef ff                     call       0x40e5a0
0050f506  90                                 nop        
0050f507  8d 4d bc                           lea        ecx, [ebp - 0x44]
0050f50a  e8 21 0a f0 ff                     call       0x40ff30
0050f50f  89 45 a4                           mov        dword ptr [ebp - 0x5c], eax
0050f512  6a 01                              push       1
0050f514  8b 4d b8                           mov        ecx, dword ptr [ebp - 0x48]
0050f517  51                                 push       ecx
0050f518  8b 55 a4                           mov        edx, dword ptr [ebp - 0x5c]
0050f51b  52                                 push       edx
0050f51c  e8 7f 15 f0 ff                     call       0x410aa0
0050f521  83 c4 0c                           add        esp, 0xc
0050f524  8b 4d b8                           mov        ecx, dword ptr [ebp - 0x48]
0050f527  89 41 04                           mov        dword ptr [ecx + 4], eax
0050f52a  8b 4d b8                           mov        ecx, dword ptr [ebp - 0x48]
0050f52d  81 c1 48 1f 09 00                  add        ecx, 0x91f48
0050f533  e8 58 fb ff ff                     call       0x50f090
0050f538  90                                 nop        
0050f539  c7 45 ac 00 00 00 00               mov        dword ptr [ebp - 0x54], 0
0050f540  eb 09                              jmp        0x50f54b
0050f542  8b 55 ac                           mov        edx, dword ptr [ebp - 0x54]
0050f545  83 c2 01                           add        edx, 1
0050f548  89 55 ac                           mov        dword ptr [ebp - 0x54], edx
0050f54b  83 7d ac 02                        cmp        dword ptr [ebp - 0x54], 2
0050f54f  7d 39                              jge        0x50f58a
0050f551  c7 45 a8 00 00 00 00               mov        dword ptr [ebp - 0x58], 0
0050f558  eb 09                              jmp        0x50f563
0050f55a  8b 45 a8                           mov        eax, dword ptr [ebp - 0x58]
0050f55d  83 c0 01                           add        eax, 1
0050f560  89 45 a8                           mov        dword ptr [ebp - 0x58], eax
0050f563  83 7d a8 0a                        cmp        dword ptr [ebp - 0x58], 0xa
0050f567  7d 1f                              jge        0x50f588
0050f569  69 4d ac 28 52 04 00               imul       ecx, dword ptr [ebp - 0x54], 0x45228
0050f570  8b 55 b8                           mov        edx, dword ptr [ebp - 0x48]
0050f573  8d 4c 0a 10                        lea        ecx, [edx + ecx + 0x10]
0050f577  69 45 a8 e8 7a 00 00               imul       eax, dword ptr [ebp - 0x58], 0x7ae8
0050f57e  03 c8                              add        ecx, eax
0050f580  e8 7b f9 ff ff                     call       0x50ef00
0050f585  90                                 nop        
0050f586  eb d2                              jmp        0x50f55a
0050f588  eb b8                              jmp        0x50f542
0050f58a  8b 4d b8                           mov        ecx, dword ptr [ebp - 0x48]
0050f58d  e8 5e 00 00 00                     call       0x50f5f0
0050f592  68 40 37 57 00                     push       0x573740 ; string='Score.LoadThread end'
0050f597  e8 14 d1 ef ff                     call       0x40c6b0
0050f59c  83 c4 04                           add        esp, 4
0050f59f  c6 45 fc 01                        mov        byte ptr [ebp - 4], 1
0050f5a3  8d 4d bc                           lea        ecx, [ebp - 0x44]
0050f5a6  e8 f5 ef ef ff                     call       0x40e5a0
0050f5ab  c6 45 fc 00                        mov        byte ptr [ebp - 4], 0
0050f5af  8d 4d d4                           lea        ecx, [ebp - 0x2c]
0050f5b2  e8 09 f1 ef ff                     call       0x40e6c0
0050f5b7  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0050f5be  8d 4d ec                           lea        ecx, [ebp - 0x14]
0050f5c1  e8 3a c3 ef ff                     call       0x40b900
0050f5c6  90                                 nop        
0050f5c7  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0050f5ca  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0050f5d1  59                                 pop        ecx
0050f5d2  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050f5d5  33 cd                              xor        ecx, ebp
0050f5d7  e8 12 34 03 00                     call       0x5429ee
0050f5dc  8b e5                              mov        esp, ebp
0050f5de  5d                                 pop        ebp
0050f5df  c2 04 00                           ret        4
