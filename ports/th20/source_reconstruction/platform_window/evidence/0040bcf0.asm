0040bcf0  55                                 push       ebp
0040bcf1  8b ec                              mov        ebp, esp
0040bcf3  6a ff                              push       -1
0040bcf5  68 2d 76 56 00                     push       0x56762d
0040bcfa  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0040bd00  50                                 push       eax
0040bd01  83 ec 0c                           sub        esp, 0xc
0040bd04  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0040bd09  33 c5                              xor        eax, ebp
0040bd0b  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0040bd0e  50                                 push       eax
0040bd0f  8d 45 f4                           lea        eax, [ebp - 0xc]
0040bd12  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0040bd18  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
0040bd1b  6a 06                              push       6
0040bd1d  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0040bd22  e8 e9 05 00 00                     call       0x40c310
0040bd27  50                                 push       eax
0040bd28  8d 4d ec                           lea        ecx, [ebp - 0x14]
0040bd2b  e8 00 fa ff ff                     call       0x40b730
0040bd30  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0040bd37  6a 01                              push       1
0040bd39  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0040bd3c  83 c1 0c                           add        ecx, 0xc
0040bd3f  e8 5c fd ff ff                     call       0x40baa0
0040bd44  90                                 nop        
0040bd45  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0040bd48  e8 83 06 00 00                     call       0x40c3d0
0040bd4d  0f b6 c0                           movzx      eax, al
0040bd50  85 c0                              test       eax, eax
0040bd52  74 09                              je         0x40bd5d
0040bd54  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0040bd57  e8 e4 05 00 00                     call       0x40c340
0040bd5c  90                                 nop        
0040bd5d  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0040bd64  8d 4d ec                           lea        ecx, [ebp - 0x14]
0040bd67  e8 94 fb ff ff                     call       0x40b900
0040bd6c  90                                 nop        
0040bd6d  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0040bd70  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0040bd77  59                                 pop        ecx
0040bd78  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0040bd7b  33 cd                              xor        ecx, ebp
0040bd7d  e8 6c 6c 13 00                     call       0x5429ee
0040bd82  8b e5                              mov        esp, ebp
0040bd84  5d                                 pop        ebp
0040bd85  c3                                 ret        
