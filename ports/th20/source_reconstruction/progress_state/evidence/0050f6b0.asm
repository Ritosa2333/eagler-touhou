0050f6b0  55                                 push       ebp
0050f6b1  8b ec                              mov        ebp, esp
0050f6b3  6a ff                              push       -1
0050f6b5  68 a5 a9 56 00                     push       0x56a9a5
0050f6ba  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0050f6c0  50                                 push       eax
0050f6c1  81 ec 84 00 00 00                  sub        esp, 0x84
0050f6c7  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0050f6cc  33 c5                              xor        eax, ebp
0050f6ce  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0050f6d1  50                                 push       eax
0050f6d2  8d 45 f4                           lea        eax, [ebp - 0xc]
0050f6d5  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0050f6db  89 8d 70 ff ff ff                  mov        dword ptr [ebp - 0x90], ecx
0050f6e1  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0050f6e4  83 78 04 00                        cmp        dword ptr [eax + 4], 0
0050f6e8  75 08                              jne        0x50f6f2
0050f6ea  83 c8 ff                           or         eax, 0xffffffff
0050f6ed  e9 51 04 00 00                     jmp        0x50fb43
0050f6f2  68 a8 38 57 00                     push       0x5738a8 ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\game\\score.cpp:252 char'
0050f6f7  68 00 00 20 00                     push       0x200000
0050f6fc  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0050f702  e8 09 ff f0 ff                     call       0x41f610
0050f707  89 45 a0                           mov        dword ptr [ebp - 0x60], eax
0050f70a  c7 45 a4 00 00 00 00               mov        dword ptr [ebp - 0x5c], 0
0050f711  6a 2c                              push       0x2c
0050f713  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
0050f716  8b 51 04                           mov        edx, dword ptr [ecx + 4]
0050f719  52                                 push       edx
0050f71a  8b 45 a0                           mov        eax, dword ptr [ebp - 0x60]
0050f71d  03 45 a4                           add        eax, dword ptr [ebp - 0x5c]
0050f720  50                                 push       eax
0050f721  e8 fa 46 03 00                     call       0x543e20
0050f726  83 c4 0c                           add        esp, 0xc
0050f729  8b 4d a4                           mov        ecx, dword ptr [ebp - 0x5c]
0050f72c  83 c1 2c                           add        ecx, 0x2c
0050f72f  89 4d a4                           mov        dword ptr [ebp - 0x5c], ecx
0050f732  c7 45 e4 00 00 00 00               mov        dword ptr [ebp - 0x1c], 0
0050f739  8d 55 e4                           lea        edx, [ebp - 0x1c]
0050f73c  89 55 90                           mov        dword ptr [ebp - 0x70], edx
0050f73f  8b 45 90                           mov        eax, dword ptr [ebp - 0x70]
0050f742  8b 08                              mov        ecx, dword ptr [eax]
0050f744  51                                 push       ecx
0050f745  8d 55 ec                           lea        edx, [ebp - 0x14]
0050f748  52                                 push       edx
0050f749  e8 c2 1d f5 ff                     call       0x461510
0050f74e  83 c4 08                           add        esp, 8
0050f751  8b 45 90                           mov        eax, dword ptr [ebp - 0x70]
0050f754  8b 08                              mov        ecx, dword ptr [eax]
0050f756  51                                 push       ecx
0050f757  8d 55 e0                           lea        edx, [ebp - 0x20]
0050f75a  52                                 push       edx
0050f75b  e8 00 b7 ff ff                     call       0x50ae60
0050f760  83 c4 08                           add        esp, 8
0050f763  eb 09                              jmp        0x50f76e
0050f765  8d 4d ec                           lea        ecx, [ebp - 0x14]
0050f768  e8 f3 20 f5 ff                     call       0x461860
0050f76d  90                                 nop        
0050f76e  8d 45 e0                           lea        eax, [ebp - 0x20]
0050f771  50                                 push       eax
0050f772  8d 4d ec                           lea        ecx, [ebp - 0x14]
0050f775  e8 e6 9c f3 ff                     call       0x449460
0050f77a  0f b6 c8                           movzx      ecx, al
0050f77d  85 c9                              test       ecx, ecx
0050f77f  0f 84 47 01 00 00                  je         0x50f8cc
0050f785  8d 4d ec                           lea        ecx, [ebp - 0x14]
0050f788  e8 73 cb ef ff                     call       0x40c300
0050f78d  89 45 98                           mov        dword ptr [ebp - 0x68], eax
0050f790  c7 45 dc 00 00 00 00               mov        dword ptr [ebp - 0x24], 0
0050f797  8d 55 dc                           lea        edx, [ebp - 0x24]
0050f79a  89 55 8c                           mov        dword ptr [ebp - 0x74], edx
0050f79d  8b 45 8c                           mov        eax, dword ptr [ebp - 0x74]
0050f7a0  8b 08                              mov        ecx, dword ptr [eax]
0050f7a2  51                                 push       ecx
0050f7a3  8d 55 e8                           lea        edx, [ebp - 0x18]
0050f7a6  52                                 push       edx
0050f7a7  e8 64 1d f5 ff                     call       0x461510
0050f7ac  83 c4 08                           add        esp, 8
0050f7af  8b 45 8c                           mov        eax, dword ptr [ebp - 0x74]
0050f7b2  8b 08                              mov        ecx, dword ptr [eax]
0050f7b4  51                                 push       ecx
0050f7b5  8d 55 d8                           lea        edx, [ebp - 0x28]
0050f7b8  52                                 push       edx
0050f7b9  e8 c2 b6 ff ff                     call       0x50ae80
0050f7be  83 c4 08                           add        esp, 8
0050f7c1  eb 09                              jmp        0x50f7cc
0050f7c3  8d 4d e8                           lea        ecx, [ebp - 0x18]
0050f7c6  e8 95 20 f5 ff                     call       0x461860
0050f7cb  90                                 nop        
0050f7cc  8d 45 d8                           lea        eax, [ebp - 0x28]
0050f7cf  50                                 push       eax
0050f7d0  8d 4d e8                           lea        ecx, [ebp - 0x18]
0050f7d3  e8 88 9c f3 ff                     call       0x449460
0050f7d8  0f b6 c8                           movzx      ecx, al
0050f7db  85 c9                              test       ecx, ecx
0050f7dd  0f 84 e4 00 00 00                  je         0x50f8c7
0050f7e3  8d 4d e8                           lea        ecx, [ebp - 0x18]
0050f7e6  e8 15 cb ef ff                     call       0x40c300
0050f7eb  89 45 94                           mov        dword ptr [ebp - 0x6c], eax
0050f7ee  69 55 98 28 52 04 00               imul       edx, dword ptr [ebp - 0x68], 0x45228
0050f7f5  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0050f7f8  8d 4c 10 10                        lea        ecx, [eax + edx + 0x10]
0050f7fc  69 55 94 e8 7a 00 00               imul       edx, dword ptr [ebp - 0x6c], 0x7ae8
0050f803  0f b7 04 11                        movzx      eax, word ptr [ecx + edx]
0050f807  3d 43 52 00 00                     cmp        eax, 0x5243
0050f80c  0f 85 b0 00 00 00                  jne        0x50f8c2
0050f812  69 4d 98 28 52 04 00               imul       ecx, dword ptr [ebp - 0x68], 0x45228
0050f819  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0050f81c  8d 44 0a 10                        lea        eax, [edx + ecx + 0x10]
0050f820  69 4d 94 e8 7a 00 00               imul       ecx, dword ptr [ebp - 0x6c], 0x7ae8
0050f827  8b 55 98                           mov        edx, dword ptr [ebp - 0x68]
0050f82a  89 54 08 0c                        mov        dword ptr [eax + ecx + 0xc], edx
0050f82e  69 45 98 28 52 04 00               imul       eax, dword ptr [ebp - 0x68], 0x45228
0050f835  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
0050f838  8d 54 01 10                        lea        edx, [ecx + eax + 0x10]
0050f83c  69 45 94 e8 7a 00 00               imul       eax, dword ptr [ebp - 0x6c], 0x7ae8
0050f843  8b 4d 94                           mov        ecx, dword ptr [ebp - 0x6c]
0050f846  89 4c 02 10                        mov        dword ptr [edx + eax + 0x10], ecx
0050f84a  69 55 98 28 52 04 00               imul       edx, dword ptr [ebp - 0x68], 0x45228
0050f851  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0050f854  8d 4c 10 10                        lea        ecx, [eax + edx + 0x10]
0050f858  69 55 94 e8 7a 00 00               imul       edx, dword ptr [ebp - 0x6c], 0x7ae8
0050f85f  03 ca                              add        ecx, edx
0050f861  89 4d 88                           mov        dword ptr [ebp - 0x78], ecx
0050f864  68 e8 7a 00 00                     push       0x7ae8
0050f869  8b 4d 88                           mov        ecx, dword ptr [ebp - 0x78]
0050f86c  e8 1f 04 00 00                     call       0x50fc90
0050f871  69 4d 98 28 52 04 00               imul       ecx, dword ptr [ebp - 0x68], 0x45228
0050f878  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0050f87b  8d 4c 0a 10                        lea        ecx, [edx + ecx + 0x10]
0050f87f  69 55 94 e8 7a 00 00               imul       edx, dword ptr [ebp - 0x6c], 0x7ae8
0050f886  89 44 11 04                        mov        dword ptr [ecx + edx + 4], eax
0050f88a  68 e8 7a 00 00                     push       0x7ae8
0050f88f  69 45 98 28 52 04 00               imul       eax, dword ptr [ebp - 0x68], 0x45228
0050f896  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
0050f899  8d 54 01 10                        lea        edx, [ecx + eax + 0x10]
0050f89d  69 45 94 e8 7a 00 00               imul       eax, dword ptr [ebp - 0x6c], 0x7ae8
0050f8a4  03 d0                              add        edx, eax
0050f8a6  52                                 push       edx
0050f8a7  8b 4d a0                           mov        ecx, dword ptr [ebp - 0x60]
0050f8aa  03 4d a4                           add        ecx, dword ptr [ebp - 0x5c]
0050f8ad  51                                 push       ecx
0050f8ae  e8 6d 45 03 00                     call       0x543e20
0050f8b3  83 c4 0c                           add        esp, 0xc
0050f8b6  8b 55 a4                           mov        edx, dword ptr [ebp - 0x5c]
0050f8b9  81 c2 e8 7a 00 00                  add        edx, 0x7ae8
0050f8bf  89 55 a4                           mov        dword ptr [ebp - 0x5c], edx
0050f8c2  e9 fc fe ff ff                     jmp        0x50f7c3
0050f8c7  e9 99 fe ff ff                     jmp        0x50f765
0050f8cc  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0050f8cf  c7 80 6c a4 08 00 02 00 00 00      mov        dword ptr [eax + 0x8a46c], 2
0050f8d9  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
0050f8dc  81 c1 60 a4 08 00                  add        ecx, 0x8a460
0050f8e2  89 4d 84                           mov        dword ptr [ebp - 0x7c], ecx
0050f8e5  68 e8 7a 00 00                     push       0x7ae8
0050f8ea  8b 4d 84                           mov        ecx, dword ptr [ebp - 0x7c]
0050f8ed  e8 9e 03 00 00                     call       0x50fc90
0050f8f2  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0050f8f5  89 82 64 a4 08 00                  mov        dword ptr [edx + 0x8a464], eax
0050f8fb  68 e8 7a 00 00                     push       0x7ae8
0050f900  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0050f903  05 60 a4 08 00                     add        eax, 0x8a460
0050f908  50                                 push       eax
0050f909  8b 4d a0                           mov        ecx, dword ptr [ebp - 0x60]
0050f90c  03 4d a4                           add        ecx, dword ptr [ebp - 0x5c]
0050f90f  51                                 push       ecx
0050f910  e8 0b 45 03 00                     call       0x543e20
0050f915  83 c4 0c                           add        esp, 0xc
0050f918  8b 55 a4                           mov        edx, dword ptr [ebp - 0x5c]
0050f91b  81 c2 e8 7a 00 00                  add        edx, 0x7ae8
0050f921  89 55 a4                           mov        dword ptr [ebp - 0x5c], edx
0050f924  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0050f927  05 48 1f 09 00                     add        eax, 0x91f48
0050f92c  89 45 80                           mov        dword ptr [ebp - 0x80], eax
0050f92f  68 f8 01 00 00                     push       0x1f8
0050f934  8b 4d 80                           mov        ecx, dword ptr [ebp - 0x80]
0050f937  e8 54 03 00 00                     call       0x50fc90
0050f93c  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
0050f93f  89 81 4c 1f 09 00                  mov        dword ptr [ecx + 0x91f4c], eax
0050f945  68 f8 01 00 00                     push       0x1f8
0050f94a  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0050f94d  81 c2 48 1f 09 00                  add        edx, 0x91f48
0050f953  52                                 push       edx
0050f954  8b 45 a0                           mov        eax, dword ptr [ebp - 0x60]
0050f957  03 45 a4                           add        eax, dword ptr [ebp - 0x5c]
0050f95a  50                                 push       eax
0050f95b  e8 c0 44 03 00                     call       0x543e20
0050f960  83 c4 0c                           add        esp, 0xc
0050f963  8b 4d a4                           mov        ecx, dword ptr [ebp - 0x5c]
0050f966  81 c1 f8 01 00 00                  add        ecx, 0x1f8
0050f96c  89 4d a4                           mov        dword ptr [ebp - 0x5c], ecx
0050f96f  8b 55 a0                           mov        edx, dword ptr [ebp - 0x60]
0050f972  89 95 7c ff ff ff                  mov        dword ptr [ebp - 0x84], edx
0050f978  8b 45 a4                           mov        eax, dword ptr [ebp - 0x5c]
0050f97b  83 e8 2c                           sub        eax, 0x2c
0050f97e  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
0050f981  8b 51 04                           mov        edx, dword ptr [ecx + 4]
0050f984  89 42 28                           mov        dword ptr [edx + 0x28], eax
0050f987  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0050f98a  8b 48 04                           mov        ecx, dword ptr [eax + 4]
0050f98d  83 c1 24                           add        ecx, 0x24
0050f990  51                                 push       ecx
0050f991  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0050f994  8b 42 04                           mov        eax, dword ptr [edx + 4]
0050f997  8b 48 28                           mov        ecx, dword ptr [eax + 0x28]
0050f99a  51                                 push       ecx
0050f99b  ba 2c 00 00 00                     mov        edx, 0x2c
0050f9a0  c1 e2 00                           shl        edx, 0
0050f9a3  03 95 7c ff ff ff                  add        edx, dword ptr [ebp - 0x84]
0050f9a9  52                                 push       edx
0050f9aa  e8 a1 9b 02 00                     call       0x539550
0050f9af  83 c4 0c                           add        esp, 0xc
0050f9b2  89 45 9c                           mov        dword ptr [ebp - 0x64], eax
0050f9b5  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0050f9b8  8b 48 04                           mov        ecx, dword ptr [eax + 4]
0050f9bb  8b 51 24                           mov        edx, dword ptr [ecx + 0x24]
0050f9be  83 c2 2c                           add        edx, 0x2c
0050f9c1  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0050f9c4  8b 48 04                           mov        ecx, dword ptr [eax + 4]
0050f9c7  89 51 04                           mov        dword ptr [ecx + 4], edx
0050f9ca  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0050f9cd  8b 42 04                           mov        eax, dword ptr [edx + 4]
0050f9d0  8b 48 24                           mov        ecx, dword ptr [eax + 0x24]
0050f9d3  51                                 push       ecx
0050f9d4  6a 10                              push       0x10
0050f9d6  6a 35                              push       0x35
0050f9d8  68 ac 00 00 00                     push       0xac
0050f9dd  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0050f9e0  8b 42 04                           mov        eax, dword ptr [edx + 4]
0050f9e3  8b 48 24                           mov        ecx, dword ptr [eax + 0x24]
0050f9e6  51                                 push       ecx
0050f9e7  8b 55 9c                           mov        edx, dword ptr [ebp - 0x64]
0050f9ea  52                                 push       edx
0050f9eb  e8 d0 09 f0 ff                     call       0x4103c0
0050f9f0  83 c4 18                           add        esp, 0x18
0050f9f3  6a 00                              push       0
0050f9f5  68 e1 67 5b 00                     push       0x5b67e1
0050f9fa  8d 4d c0                           lea        ecx, [ebp - 0x40]
0050f9fd  e8 ce 82 f0 ff                     call       0x417cd0
0050fa02  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0050fa09  8d 45 08                           lea        eax, [ebp + 8]
0050fa0c  50                                 push       eax
0050fa0d  8d 4d c0                           lea        ecx, [ebp - 0x40]
0050fa10  e8 0b 8c fc ff                     call       0x4d8620
0050fa15  8d 4d a8                           lea        ecx, [ebp - 0x58]
0050fa18  51                                 push       ecx
0050fa19  8d 4d c0                           lea        ecx, [ebp - 0x40]
0050fa1c  e8 6f ed f0 ff                     call       0x41e790
0050fa21  c6 45 fc 01                        mov        byte ptr [ebp - 4], 1
0050fa25  8d 4d a8                           lea        ecx, [ebp - 0x58]
0050fa28  e8 03 05 f0 ff                     call       0x40ff30
0050fa2d  89 85 78 ff ff ff                  mov        dword ptr [ebp - 0x88], eax
0050fa33  8b 95 78 ff ff ff                  mov        edx, dword ptr [ebp - 0x88]
0050fa39  52                                 push       edx
0050fa3a  e8 01 0e f0 ff                     call       0x410840
0050fa3f  83 c4 04                           add        esp, 4
0050fa42  85 c0                              test       eax, eax
0050fa44  74 7a                              je         0x50fac0
0050fa46  68 e4 38 57 00                     push       0x5738e4 ; string='error : '
0050fa4b  68 78 06 5c 00                     push       0x5c0678
0050fa50  e8 db 47 f4 ff                     call       0x454230
0050fa55  83 c4 08                           add        esp, 8
0050fa58  83 7d 9c 00                        cmp        dword ptr [ebp - 0x64], 0
0050fa5c  74 16                              je         0x50fa74
0050fa5e  8b 45 9c                           mov        eax, dword ptr [ebp - 0x64]
0050fa61  50                                 push       eax
0050fa62  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0050fa68  e8 03 fc f0 ff                     call       0x41f670
0050fa6d  c7 45 9c 00 00 00 00               mov        dword ptr [ebp - 0x64], 0
0050fa74  83 7d a0 00                        cmp        dword ptr [ebp - 0x60], 0
0050fa78  74 16                              je         0x50fa90
0050fa7a  8b 4d a0                           mov        ecx, dword ptr [ebp - 0x60]
0050fa7d  51                                 push       ecx
0050fa7e  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0050fa84  e8 e7 fb f0 ff                     call       0x41f670
0050fa89  c7 45 a0 00 00 00 00               mov        dword ptr [ebp - 0x60], 0
0050fa90  c7 85 74 ff ff ff ff ff ff ff      mov        dword ptr [ebp - 0x8c], 0xffffffff
0050fa9a  c6 45 fc 00                        mov        byte ptr [ebp - 4], 0
0050fa9e  8d 4d a8                           lea        ecx, [ebp - 0x58]
0050faa1  e8 fa ea ef ff                     call       0x40e5a0
0050faa6  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0050faad  8d 4d c0                           lea        ecx, [ebp - 0x40]
0050fab0  e8 0b ec ef ff                     call       0x40e6c0
0050fab5  8b 85 74 ff ff ff                  mov        eax, dword ptr [ebp - 0x8c]
0050fabb  e9 83 00 00 00                     jmp        0x50fb43
0050fac0  6a 2c                              push       0x2c
0050fac2  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0050fac5  8b 42 04                           mov        eax, dword ptr [edx + 4]
0050fac8  50                                 push       eax
0050fac9  e8 92 15 f0 ff                     call       0x411060
0050face  83 c4 08                           add        esp, 8
0050fad1  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
0050fad4  8b 51 04                           mov        edx, dword ptr [ecx + 4]
0050fad7  8b 42 24                           mov        eax, dword ptr [edx + 0x24]
0050fada  50                                 push       eax
0050fadb  8b 4d 9c                           mov        ecx, dword ptr [ebp - 0x64]
0050fade  51                                 push       ecx
0050fadf  e8 7c 15 f0 ff                     call       0x411060
0050fae4  83 c4 08                           add        esp, 8
0050fae7  e8 f4 0b f0 ff                     call       0x4106e0
0050faec  90                                 nop        
0050faed  83 7d 9c 00                        cmp        dword ptr [ebp - 0x64], 0
0050faf1  74 16                              je         0x50fb09
0050faf3  8b 55 9c                           mov        edx, dword ptr [ebp - 0x64]
0050faf6  52                                 push       edx
0050faf7  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0050fafd  e8 6e fb f0 ff                     call       0x41f670
0050fb02  c7 45 9c 00 00 00 00               mov        dword ptr [ebp - 0x64], 0
0050fb09  c6 45 fc 00                        mov        byte ptr [ebp - 4], 0
0050fb0d  8d 4d a8                           lea        ecx, [ebp - 0x58]
0050fb10  e8 8b ea ef ff                     call       0x40e5a0
0050fb15  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0050fb1c  8d 4d c0                           lea        ecx, [ebp - 0x40]
0050fb1f  e8 9c eb ef ff                     call       0x40e6c0
0050fb24  90                                 nop        
0050fb25  83 7d a0 00                        cmp        dword ptr [ebp - 0x60], 0
0050fb29  74 16                              je         0x50fb41
0050fb2b  8b 45 a0                           mov        eax, dword ptr [ebp - 0x60]
0050fb2e  50                                 push       eax
0050fb2f  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0050fb35  e8 36 fb f0 ff                     call       0x41f670
0050fb3a  c7 45 a0 00 00 00 00               mov        dword ptr [ebp - 0x60], 0
0050fb41  33 c0                              xor        eax, eax
0050fb43  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0050fb46  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0050fb4d  59                                 pop        ecx
0050fb4e  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050fb51  33 cd                              xor        ecx, ebp
0050fb53  e8 96 2e 03 00                     call       0x5429ee
0050fb58  8b e5                              mov        esp, ebp
0050fb5a  5d                                 pop        ebp
0050fb5b  c2 08 00                           ret        8
