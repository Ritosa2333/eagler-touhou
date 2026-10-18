00464100  55                                 push       ebp
00464101  8b ec                              mov        ebp, esp
00464103  6a ff                              push       -1
00464105  68 8d 80 56 00                     push       0x56808d
0046410a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
00464110  50                                 push       eax
00464111  83 ec 14                           sub        esp, 0x14
00464114  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
00464119  33 c5                              xor        eax, ebp
0046411b  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0046411e  50                                 push       eax
0046411f  8d 45 f4                           lea        eax, [ebp - 0xc]
00464122  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
00464128  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
0046412b  6a 14                              push       0x14
0046412d  b9 40 02 5c 00                     mov        ecx, 0x5c0240
00464132  e8 d9 81 fa ff                     call       0x40c310
00464137  50                                 push       eax
00464138  8d 4d ec                           lea        ecx, [ebp - 0x14]
0046413b  e8 f0 75 fa ff                     call       0x40b730
00464140  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
00464147  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0046414a  e8 61 fe ff ff                     call       0x463fb0
0046414f  90                                 nop        
00464150  b9 68 a5 5b 00                     mov        ecx, 0x5ba568
00464155  e8 46 ff ff ff                     call       0x4640a0
0046415a  83 f8 04                           cmp        eax, 4
0046415d  75 20                              jne        0x46417f
0046415f  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
00464162  e8 79 ff ff ff                     call       0x4640e0
00464167  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
0046416a  c1 e1 04                           shl        ecx, 4
0046416d  8d 94 08 48 01 00 00               lea        edx, [eax + ecx + 0x148]
00464174  8b 45 08                           mov        eax, dword ptr [ebp + 8]
00464177  8b 0c 82                           mov        ecx, dword ptr [edx + eax*4]
0046417a  89 4d e4                           mov        dword ptr [ebp - 0x1c], ecx
0046417d  eb 1e                              jmp        0x46419d
0046417f  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
00464182  e8 59 ff ff ff                     call       0x4640e0
00464187  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0046418a  c1 e2 04                           shl        edx, 4
0046418d  8d 84 10 28 01 00 00               lea        eax, [eax + edx + 0x128]
00464194  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00464197  8b 14 88                           mov        edx, dword ptr [eax + ecx*4]
0046419a  89 55 e4                           mov        dword ptr [ebp - 0x1c], edx
0046419d  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
004641a0  89 45 e0                           mov        dword ptr [ebp - 0x20], eax
004641a3  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
004641aa  8d 4d ec                           lea        ecx, [ebp - 0x14]
004641ad  e8 4e 77 fa ff                     call       0x40b900
004641b2  8b 45 e0                           mov        eax, dword ptr [ebp - 0x20]
004641b5  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
004641b8  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
004641bf  59                                 pop        ecx
004641c0  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004641c3  33 cd                              xor        ecx, ebp
004641c5  e8 24 e8 0d 00                     call       0x5429ee
004641ca  8b e5                              mov        esp, ebp
004641cc  5d                                 pop        ebp
004641cd  c2 08 00                           ret        8
