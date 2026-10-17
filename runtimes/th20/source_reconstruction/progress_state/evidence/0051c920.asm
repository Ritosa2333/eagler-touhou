0051c921  8b ec                              mov        ebp, esp
0051c923  6a ff                              push       -1
0051c925  68 2d 76 56 00                     push       0x56762d
0051c92a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0051c930  50                                 push       eax
0051c931  83 ec 0c                           sub        esp, 0xc
0051c934  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0051c939  33 c5                              xor        eax, ebp
0051c93b  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0051c93e  50                                 push       eax
0051c93f  8d 45 f4                           lea        eax, [ebp - 0xc]
0051c942  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0051c948  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
0051c94b  6a 14                              push       0x14
0051c94d  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0051c952  e8 b9 f9 ee ff                     call       0x40c310
0051c957  50                                 push       eax
0051c958  8d 4d ec                           lea        ecx, [ebp - 0x14]
0051c95b  e8 d0 ed ee ff                     call       0x40b730
0051c960  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0051c967  83 7d 08 00                        cmp        dword ptr [ebp + 8], 0
0051c96b  7c 1e                              jl         0x51c98b
0051c96d  83 7d 08 04                        cmp        dword ptr [ebp + 8], 4
0051c971  7d 18                              jge        0x51c98b
0051c973  83 7d 0c 00                        cmp        dword ptr [ebp + 0xc], 0
0051c977  7c 12                              jl         0x51c98b
0051c979  83 7d 0c 02                        cmp        dword ptr [ebp + 0xc], 2
0051c97d  7d 0c                              jge        0x51c98b
0051c97f  83 7d 10 00                        cmp        dword ptr [ebp + 0x10], 0
0051c983  7c 06                              jl         0x51c98b
0051c985  83 7d 10 09                        cmp        dword ptr [ebp + 0x10], 9
0051c989  7c 12                              jl         0x51c99d
0051c98b  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0051c992  8d 4d ec                           lea        ecx, [ebp - 0x14]
0051c995  e8 66 ef ee ff                     call       0x40b900
0051c99a  90                                 nop        
0051c99b  eb 65                              jmp        0x51ca02
0051c99d  b9 68 a5 5b 00                     mov        ecx, 0x5ba568
0051c9a2  e8 f9 76 f4 ff                     call       0x4640a0
0051c9a7  83 f8 04                           cmp        eax, 4
0051c9aa  75 20                              jne        0x51c9cc
0051c9ac  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0051c9af  e8 2c 77 f4 ff                     call       0x4640e0
0051c9b4  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
0051c9b7  c1 e1 04                           shl        ecx, 4
0051c9ba  8d 94 08 48 01 00 00               lea        edx, [eax + ecx + 0x148]
0051c9c1  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0051c9c4  8b 4d 10                           mov        ecx, dword ptr [ebp + 0x10]
0051c9c7  89 0c 82                           mov        dword ptr [edx + eax*4], ecx
0051c9ca  eb 1e                              jmp        0x51c9ea
0051c9cc  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0051c9cf  e8 0c 77 f4 ff                     call       0x4640e0
0051c9d4  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0051c9d7  c1 e2 04                           shl        edx, 4
0051c9da  8d 84 10 28 01 00 00               lea        eax, [eax + edx + 0x128]
0051c9e1  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0051c9e4  8b 55 10                           mov        edx, dword ptr [ebp + 0x10]
0051c9e7  89 14 88                           mov        dword ptr [eax + ecx*4], edx
0051c9ea  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0051c9ed  e8 4e 21 fa ff                     call       0x4beb40
0051c9f2  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0051c9f9  8d 4d ec                           lea        ecx, [ebp - 0x14]
0051c9fc  e8 ff ee ee ff                     call       0x40b900
0051ca01  90                                 nop        
0051ca02  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0051ca05  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0051ca0c  59                                 pop        ecx
0051ca0d  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0051ca10  33 cd                              xor        ecx, ebp
0051ca12  e8 d7 5f 02 00                     call       0x5429ee
0051ca17  8b e5                              mov        esp, ebp
0051ca19  5d                                 pop        ebp
0051ca1a  c2 0c 00                           ret        0xc
