004dd1c0  55                                 push       ebp
004dd1c1  8b ec                              mov        ebp, esp
004dd1c3  83 ec 2c                           sub        esp, 0x2c
004dd1c6  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
004dd1cb  33 c5                              xor        eax, ebp
004dd1cd  89 45 fc                           mov        dword ptr [ebp - 4], eax
004dd1d0  83 3d dc 4e 5c 00 00               cmp        dword ptr [0x5c4edc], 0
004dd1d7  0f 84 e3 00 00 00                  je         0x4dd2c0
004dd1dd  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004dd1e3  e8 d8 83 f6 ff                     call       0x4455c0
004dd1e8  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004dd1ed  e8 3e 55 f3 ff                     call       0x412730
004dd1f2  89 45 e8                           mov        dword ptr [ebp - 0x18], eax
004dd1f5  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
004dd1f8  8b 08                              mov        ecx, dword ptr [eax]
004dd1fa  8b 91 94 00 00 00                  mov        edx, dword ptr [ecx + 0x94]
004dd200  89 55 dc                           mov        dword ptr [ebp - 0x24], edx
004dd203  a1 dc 4e 5c 00                     mov        eax, dword ptr [0x5c4edc]
004dd208  89 45 e0                           mov        dword ptr [ebp - 0x20], eax
004dd20b  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
004dd20e  51                                 push       ecx
004dd20f  6a 00                              push       0
004dd211  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
004dd214  52                                 push       edx
004dd215  ff 55 dc                           call       dword ptr [ebp - 0x24]
004dd218  b8 6c 01 00 00                     mov        eax, 0x16c
004dd21d  6b c8 03                           imul       ecx, eax, 3
004dd220  8b 91 98 50 5c 00                  mov        edx, dword ptr [ecx + 0x5c5098]
004dd226  89 55 ec                           mov        dword ptr [ebp - 0x14], edx
004dd229  b8 6c 01 00 00                     mov        eax, 0x16c
004dd22e  6b c8 03                           imul       ecx, eax, 3
004dd231  8b 91 9c 50 5c 00                  mov        edx, dword ptr [ecx + 0x5c509c]
004dd237  89 55 f0                           mov        dword ptr [ebp - 0x10], edx
004dd23a  b8 6c 01 00 00                     mov        eax, 0x16c
004dd23f  6b c8 03                           imul       ecx, eax, 3
004dd242  ba 6c 01 00 00                     mov        edx, 0x16c
004dd247  6b c2 03                           imul       eax, edx, 3
004dd24a  8b 89 98 50 5c 00                  mov        ecx, dword ptr [ecx + 0x5c5098]
004dd250  03 88 a0 50 5c 00                  add        ecx, dword ptr [eax + 0x5c50a0]
004dd256  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
004dd259  ba 6c 01 00 00                     mov        edx, 0x16c
004dd25e  6b c2 03                           imul       eax, edx, 3
004dd261  b9 6c 01 00 00                     mov        ecx, 0x16c
004dd266  6b d1 03                           imul       edx, ecx, 3
004dd269  8b 80 9c 50 5c 00                  mov        eax, dword ptr [eax + 0x5c509c]
004dd26f  03 82 a4 50 5c 00                  add        eax, dword ptr [edx + 0x5c50a4]
004dd275  89 45 f8                           mov        dword ptr [ebp - 8], eax
004dd278  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004dd27d  e8 ae 54 f3 ff                     call       0x412730
004dd282  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
004dd285  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
004dd288  8b 11                              mov        edx, dword ptr [ecx]
004dd28a  8b 82 ac 00 00 00                  mov        eax, dword ptr [edx + 0xac]
004dd290  89 45 d4                           mov        dword ptr [ebp - 0x2c], eax
004dd293  8b 0d 20 5b 5c 00                  mov        ecx, dword ptr [0x5c5b20]
004dd299  89 4d d8                           mov        dword ptr [ebp - 0x28], ecx
004dd29c  6a 00                              push       0
004dd29e  51                                 push       ecx
004dd29f  f3 0f 10 05 cc c8 56 00            movss      xmm0, dword ptr [0x56c8cc]
004dd2a7  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dd2ac  8b 55 d8                           mov        edx, dword ptr [ebp - 0x28]
004dd2af  52                                 push       edx
004dd2b0  6a 02                              push       2
004dd2b2  8d 45 ec                           lea        eax, [ebp - 0x14]
004dd2b5  50                                 push       eax
004dd2b6  6a 01                              push       1
004dd2b8  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
004dd2bb  51                                 push       ecx
004dd2bc  ff 55 d4                           call       dword ptr [ebp - 0x2c]
004dd2bf  90                                 nop        
004dd2c0  b8 01 00 00 00                     mov        eax, 1
004dd2c5  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dd2c8  33 cd                              xor        ecx, ebp
004dd2ca  e8 1f 57 06 00                     call       0x5429ee
004dd2cf  8b e5                              mov        esp, ebp
004dd2d1  5d                                 pop        ebp
004dd2d2  c3                                 ret        
