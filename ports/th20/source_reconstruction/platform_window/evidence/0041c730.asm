0041c730  55                                 push       ebp
0041c731  8b ec                              mov        ebp, esp
0041c733  83 ec 6c                           sub        esp, 0x6c
0041c736  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0041c73b  33 c5                              xor        eax, ebp
0041c73d  89 45 fc                           mov        dword ptr [ebp - 4], eax
0041c740  56                                 push       esi
0041c741  57                                 push       edi
0041c742  c7 45 b4 00 00 00 00               mov        dword ptr [ebp - 0x4c], 0
0041c749  b9 0e 00 00 00                     mov        ecx, 0xe
0041c74e  be 24 4e 5c 00                     mov        esi, 0x5c4e24
0041c753  8d 7d c4                           lea        edi, [ebp - 0x3c]
0041c756  f3 a5                              rep movsd  dword ptr es:[edi], dword ptr [esi]
0041c758  0f b6 05 89 4f 5c 00               movzx      eax, byte ptr [0x5c4f89]
0041c75f  83 f8 03                           cmp        eax, 3
0041c762  75 18                              jne        0x41c77c
0041c764  8b 0d e8 87 5b 00                  mov        ecx, dword ptr [0x5b87e8]
0041c76a  83 e1 fb                           and        ecx, 0xfffffffb
0041c76d  89 0d e8 87 5b 00                  mov        dword ptr [0x5b87e8], ecx
0041c773  c7 45 f8 00 00 00 80               mov        dword ptr [ebp - 8], 0x80000000
0041c77a  eb 07                              jmp        0x41c783
0041c77c  c7 45 f8 01 00 00 00               mov        dword ptr [ebp - 8], 1
0041c783  c7 45 c0 00 00 00 00               mov        dword ptr [ebp - 0x40], 0
0041c78a  eb 09                              jmp        0x41c795
0041c78c  8b 55 c0                           mov        edx, dword ptr [ebp - 0x40]
0041c78f  83 c2 01                           add        edx, 1
0041c792  89 55 c0                           mov        dword ptr [ebp - 0x40], edx
0041c795  83 7d c0 10                        cmp        dword ptr [ebp - 0x40], 0x10
0041c799  0f 83 47 01 00 00                  jae        0x41c8e6
0041c79f  83 7d c0 00                        cmp        dword ptr [ebp - 0x40], 0
0041c7a3  75 33                              jne        0x41c7d8
0041c7a5  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041c7aa  e8 c1 04 00 00                     call       0x41cc70
0041c7af  83 f8 08                           cmp        eax, 8
0041c7b2  74 0f                              je         0x41c7c3
0041c7b4  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041c7b9  e8 b2 04 00 00                     call       0x41cc70
0041c7be  83 f8 09                           cmp        eax, 9
0041c7c1  75 15                              jne        0x41c7d8
0041c7c3  a1 10 88 5b 00                     mov        eax, dword ptr [0x5b8810]
0041c7c8  89 45 c4                           mov        dword ptr [ebp - 0x3c], eax
0041c7cb  8b 0d 14 88 5b 00                  mov        ecx, dword ptr [0x5b8814]
0041c7d1  89 4d c8                           mov        dword ptr [ebp - 0x38], ecx
0041c7d4  eb 77                              jmp        0x41c84d
0041c7d8  ba 04 00 00 00                     mov        edx, 4
0041c7dd  6b c2 00                           imul       eax, edx, 0
0041c7e0  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
0041c7e3  8b 94 c8 50 e1 5a 00               mov        edx, dword ptr [eax + ecx*8 + 0x5ae150]
0041c7ea  3b 15 f8 87 5b 00                  cmp        edx, dword ptr [0x5b87f8]
0041c7f0  7c 1a                              jl         0x41c80c
0041c7f2  b8 04 00 00 00                     mov        eax, 4
0041c7f7  c1 e0 00                           shl        eax, 0
0041c7fa  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
0041c7fd  8b 94 c8 50 e1 5a 00               mov        edx, dword ptr [eax + ecx*8 + 0x5ae150]
0041c804  3b 15 fc 87 5b 00                  cmp        edx, dword ptr [0x5b87fc]
0041c80a  7d 05                              jge        0x41c811
0041c80c  e9 7b ff ff ff                     jmp        0x41c78c
0041c811  b8 04 00 00 00                     mov        eax, 4
0041c816  6b c8 00                           imul       ecx, eax, 0
0041c819  8b 55 c0                           mov        edx, dword ptr [ebp - 0x40]
0041c81c  8b 84 d1 50 e1 5a 00               mov        eax, dword ptr [ecx + edx*8 + 0x5ae150]
0041c823  89 45 c4                           mov        dword ptr [ebp - 0x3c], eax
0041c826  b9 04 00 00 00                     mov        ecx, 4
0041c82b  c1 e1 00                           shl        ecx, 0
0041c82e  8b 55 c0                           mov        edx, dword ptr [ebp - 0x40]
0041c831  8b 84 d1 50 e1 5a 00               mov        eax, dword ptr [ecx + edx*8 + 0x5ae150]
0041c838  89 45 c8                           mov        dword ptr [ebp - 0x38], eax
0041c83b  8b 4d c4                           mov        ecx, dword ptr [ebp - 0x3c]
0041c83e  89 0d 10 88 5b 00                  mov        dword ptr [0x5b8810], ecx
0041c844  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0041c847  89 15 14 88 5b 00                  mov        dword ptr [0x5b8814], edx
0041c84d  83 7d 08 00                        cmp        dword ptr [ebp + 8], 0
0041c851  75 22                              jne        0x41c875
0041c853  8d 45 c4                           lea        eax, [ebp - 0x3c]
0041c856  50                                 push       eax
0041c857  e8 44 f9 ff ff                     call       0x41c1a0
0041c85c  83 c4 04                           add        esp, 4
0041c85f  85 c0                              test       eax, eax
0041c861  74 10                              je         0x41c873
0041c863  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041c868  e8 d3 d9 ff ff                     call       0x41a240
0041c86d  90                                 nop        
0041c86e  e9 19 ff ff ff                     jmp        0x41c78c
0041c873  eb 31                              jmp        0x41c8a6
0041c875  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041c87a  e8 b1 5e ff ff                     call       0x412730
0041c87f  89 45 bc                           mov        dword ptr [ebp - 0x44], eax
0041c882  8b 4d bc                           mov        ecx, dword ptr [ebp - 0x44]
0041c885  8b 11                              mov        edx, dword ptr [ecx]
0041c887  8b 42 40                           mov        eax, dword ptr [edx + 0x40]
0041c88a  89 45 a8                           mov        dword ptr [ebp - 0x58], eax
0041c88d  8d 4d c4                           lea        ecx, [ebp - 0x3c]
0041c890  51                                 push       ecx
0041c891  8b 55 bc                           mov        edx, dword ptr [ebp - 0x44]
0041c894  52                                 push       edx
0041c895  ff 55 a8                           call       dword ptr [ebp - 0x58]
0041c898  89 45 a4                           mov        dword ptr [ebp - 0x5c], eax
0041c89b  83 7d a4 00                        cmp        dword ptr [ebp - 0x5c], 0
0041c89f  74 05                              je         0x41c8a6
0041c8a1  e9 e6 fe ff ff                     jmp        0x41c78c
0041c8a6  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041c8ab  e8 50 fa fe ff                     call       0x40c300
0041c8b0  50                                 push       eax
0041c8b1  ff 15 ac c2 56 00                  call       dword ptr [0x56c2ac] ; USER32.dll!GetDC
0041c8b7  89 45 b8                           mov        dword ptr [ebp - 0x48], eax
0041c8ba  6a 74                              push       0x74
0041c8bc  8b 45 b8                           mov        eax, dword ptr [ebp - 0x48]
0041c8bf  50                                 push       eax
0041c8c0  ff 15 14 c0 56 00                  call       dword ptr [0x56c014] ; GDI32.dll!GetDeviceCaps
0041c8c6  89 45 a0                           mov        dword ptr [ebp - 0x60], eax
0041c8c9  8b 4d b8                           mov        ecx, dword ptr [ebp - 0x48]
0041c8cc  51                                 push       ecx
0041c8cd  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041c8d2  e8 29 fa fe ff                     call       0x40c300
0041c8d7  50                                 push       eax
0041c8d8  ff 15 b0 c2 56 00                  call       dword ptr [0x56c2b0] ; USER32.dll!ReleaseDC
0041c8de  90                                 nop        
0041c8df  eb 05                              jmp        0x41c8e6
0041c8e6  83 7d c0 10                        cmp        dword ptr [ebp - 0x40], 0x10
0041c8ea  72 3d                              jb         0x41c929
0041c8ec  83 7d b4 00                        cmp        dword ptr [ebp - 0x4c], 0
0041c8f0  75 0c                              jne        0x41c8fe
0041c8f2  c7 45 b4 01 00 00 00               mov        dword ptr [ebp - 0x4c], 1
0041c8f9  e9 85 fe ff ff                     jmp        0x41c783
0041c8fe  83 7d 08 00                        cmp        dword ptr [ebp + 8], 0
0041c902  75 1d                              jne        0x41c921
0041c904  68 a4 cb 56 00                     push       0x56cba4 ; string='Direct3D '
0041c909  68 78 06 5c 00                     push       0x5c0678
0041c90e  e8 1d 79 03 00                     call       0x454230
0041c913  83 c4 08                           add        esp, 8
0041c916  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041c91b  e8 e0 d8 ff ff                     call       0x41a200
0041c920  90                                 nop        
0041c921  83 c8 ff                           or         eax, 0xffffffff
0041c924  e9 b0 00 00 00                     jmp        0x41c9d9
0041c929  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041c92e  e8 cd f9 fe ff                     call       0x40c300
0041c933  50                                 push       eax
0041c934  ff 15 ac c2 56 00                  call       dword ptr [0x56c2ac] ; USER32.dll!GetDC
0041c93a  89 45 b0                           mov        dword ptr [ebp - 0x50], eax
0041c93d  6a 74                              push       0x74
0041c93f  8b 55 b0                           mov        edx, dword ptr [ebp - 0x50]
0041c942  52                                 push       edx
0041c943  ff 15 14 c0 56 00                  call       dword ptr [0x56c014] ; GDI32.dll!GetDeviceCaps
0041c949  89 45 9c                           mov        dword ptr [ebp - 0x64], eax
0041c94c  8b 45 b0                           mov        eax, dword ptr [ebp - 0x50]
0041c94f  50                                 push       eax
0041c950  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041c955  e8 a6 f9 fe ff                     call       0x40c300
0041c95a  50                                 push       eax
0041c95b  ff 15 b0 c2 56 00                  call       dword ptr [0x56c2b0] ; USER32.dll!ReleaseDC
0041c961  90                                 nop        
0041c962  83 7d 9c 3c                        cmp        dword ptr [ebp - 0x64], 0x3c
0041c966  74 16                              je         0x41c97e
0041c968  8b 0d e8 87 5b 00                  mov        ecx, dword ptr [0x5b87e8]
0041c96e  83 e1 fb                           and        ecx, 0xfffffffb
0041c971  89 0d e8 87 5b 00                  mov        dword ptr [0x5b87e8], ecx
0041c977  c7 45 f8 00 00 00 80               mov        dword ptr [ebp - 8], 0x80000000
0041c97e  b9 0e 00 00 00                     mov        ecx, 0xe
0041c983  8d 75 c4                           lea        esi, [ebp - 0x3c]
0041c986  bf 24 4e 5c 00                     mov        edi, 0x5c4e24
0041c98b  f3 a5                              rep movsd  dword ptr es:[edi], dword ptr [esi]
0041c98d  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041c992  e8 99 5d ff ff                     call       0x412730
0041c997  89 45 ac                           mov        dword ptr [ebp - 0x54], eax
0041c99a  8b 55 ac                           mov        edx, dword ptr [ebp - 0x54]
0041c99d  8b 02                              mov        eax, dword ptr [edx]
0041c99f  8b 48 40                           mov        ecx, dword ptr [eax + 0x40]
0041c9a2  89 4d 98                           mov        dword ptr [ebp - 0x68], ecx
0041c9a5  8d 55 c4                           lea        edx, [ebp - 0x3c]
0041c9a8  52                                 push       edx
0041c9a9  8b 45 ac                           mov        eax, dword ptr [ebp - 0x54]
0041c9ac  50                                 push       eax
0041c9ad  ff 55 98                           call       dword ptr [ebp - 0x68]
0041c9b0  89 45 94                           mov        dword ptr [ebp - 0x6c], eax
0041c9b3  83 7d 94 00                        cmp        dword ptr [ebp - 0x6c], 0
0041c9b7  74 04                              je         0x41c9bd
0041c9b9  33 c0                              xor        eax, eax
0041c9bb  eb 1c                              jmp        0x41c9d9
0041c9bd  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0041c9c0  51                                 push       ecx
0041c9c1  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0041c9c4  52                                 push       edx
0041c9c5  68 dc cb 56 00                     push       0x56cbdc
0041c9ca  68 78 06 5c 00                     push       0x5c0678
0041c9cf  e8 7c 77 03 00                     call       0x454150
0041c9d4  83 c4 10                           add        esp, 0x10
0041c9d7  33 c0                              xor        eax, eax
0041c9d9  5f                                 pop        edi
0041c9da  5e                                 pop        esi
0041c9db  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041c9de  33 cd                              xor        ecx, ebp
0041c9e0  e8 09 60 12 00                     call       0x5429ee
0041c9e5  8b e5                              mov        esp, ebp
0041c9e7  5d                                 pop        ebp
0041c9e8  c3                                 ret        
