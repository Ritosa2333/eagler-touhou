004dc5f0  55                                 push       ebp
004dc5f1  8b ec                              mov        ebp, esp
004dc5f3  83 ec 2c                           sub        esp, 0x2c
004dc5f6  83 3d dc 4e 5c 00 00               cmp        dword ptr [0x5c4edc], 0
004dc5fd  0f 84 c5 00 00 00                  je         0x4dc6c8
004dc603  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004dc608  e8 23 61 f3 ff                     call       0x412730
004dc60d  89 45 fc                           mov        dword ptr [ebp - 4], eax
004dc610  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dc613  8b 08                              mov        ecx, dword ptr [eax]
004dc615  8b 91 ac 00 00 00                  mov        edx, dword ptr [ecx + 0xac]
004dc61b  89 55 ec                           mov        dword ptr [ebp - 0x14], edx
004dc61e  6a 00                              push       0
004dc620  51                                 push       ecx
004dc621  f3 0f 10 05 cc c8 56 00            movss      xmm0, dword ptr [0x56c8cc]
004dc629  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dc62e  6a ff                              push       -1
004dc630  6a 03                              push       3
004dc632  6a 00                              push       0
004dc634  6a 00                              push       0
004dc636  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dc639  50                                 push       eax
004dc63a  ff 55 ec                           call       dword ptr [ebp - 0x14]
004dc63d  90                                 nop        
004dc63e  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004dc643  e8 e8 60 f3 ff                     call       0x412730
004dc648  89 45 f8                           mov        dword ptr [ebp - 8], eax
004dc64b  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004dc64e  8b 11                              mov        edx, dword ptr [ecx]
004dc650  8b 82 94 00 00 00                  mov        eax, dword ptr [edx + 0x94]
004dc656  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
004dc659  8b 0d dc 4e 5c 00                  mov        ecx, dword ptr [0x5c4edc]
004dc65f  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
004dc662  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
004dc665  52                                 push       edx
004dc666  6a 00                              push       0
004dc668  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
004dc66b  50                                 push       eax
004dc66c  ff 55 e4                           call       dword ptr [ebp - 0x1c]
004dc66f  85 c0                              test       eax, eax
004dc671  74 0d                              je         0x4dc680
004dc673  68 84 1e 57 00                     push       0x571e84 ; string='error SetRenderTarget'
004dc678  e8 33 00 f3 ff                     call       0x40c6b0
004dc67d  83 c4 04                           add        esp, 4
004dc680  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004dc685  e8 a6 60 f3 ff                     call       0x412730
004dc68a  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
004dc68d  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
004dc690  8b 11                              mov        edx, dword ptr [ecx]
004dc692  8b 82 ac 00 00 00                  mov        eax, dword ptr [edx + 0xac]
004dc698  89 45 dc                           mov        dword ptr [ebp - 0x24], eax
004dc69b  8b 0d 20 5b 5c 00                  mov        ecx, dword ptr [0x5c5b20]
004dc6a1  89 4d e0                           mov        dword ptr [ebp - 0x20], ecx
004dc6a4  6a 00                              push       0
004dc6a6  51                                 push       ecx
004dc6a7  f3 0f 10 05 cc c8 56 00            movss      xmm0, dword ptr [0x56c8cc]
004dc6af  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dc6b4  8b 55 e0                           mov        edx, dword ptr [ebp - 0x20]
004dc6b7  52                                 push       edx
004dc6b8  6a 03                              push       3
004dc6ba  6a 00                              push       0
004dc6bc  6a 00                              push       0
004dc6be  8b 45 f4                           mov        eax, dword ptr [ebp - 0xc]
004dc6c1  50                                 push       eax
004dc6c2  ff 55 dc                           call       dword ptr [ebp - 0x24]
004dc6c5  90                                 nop        
004dc6c6  eb 46                              jmp        0x4dc70e
004dc6c8  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004dc6cd  e8 5e 60 f3 ff                     call       0x412730
004dc6d2  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
004dc6d5  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004dc6d8  8b 11                              mov        edx, dword ptr [ecx]
004dc6da  8b 82 ac 00 00 00                  mov        eax, dword ptr [edx + 0xac]
004dc6e0  89 45 d4                           mov        dword ptr [ebp - 0x2c], eax
004dc6e3  8b 0d 20 5b 5c 00                  mov        ecx, dword ptr [0x5c5b20]
004dc6e9  89 4d d8                           mov        dword ptr [ebp - 0x28], ecx
004dc6ec  6a 00                              push       0
004dc6ee  51                                 push       ecx
004dc6ef  f3 0f 10 05 cc c8 56 00            movss      xmm0, dword ptr [0x56c8cc]
004dc6f7  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dc6fc  8b 55 d8                           mov        edx, dword ptr [ebp - 0x28]
004dc6ff  52                                 push       edx
004dc700  6a 03                              push       3
004dc702  6a 00                              push       0
004dc704  6a 00                              push       0
004dc706  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
004dc709  50                                 push       eax
004dc70a  ff 55 d4                           call       dword ptr [ebp - 0x2c]
004dc70d  90                                 nop        
004dc70e  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004dc714  e8 67 79 f4 ff                     call       0x424080
004dc719  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004dc71f  e8 7c 14 f4 ff                     call       0x41dba0
004dc724  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004dc72a  e8 31 14 f4 ff                     call       0x41db60
004dc72f  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004dc735  e8 06 14 f4 ff                     call       0x41db40
004dc73a  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004dc740  e8 5b 79 f4 ff                     call       0x4240a0
004dc745  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004dc74b  e8 70 14 f4 ff                     call       0x41dbc0
004dc750  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004dc756  e8 c5 89 f9 ff                     call       0x475120
004dc75b  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004dc761  e8 ba 10 00 00                     call       0x4dd820
004dc766  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004dc76c  e8 df 89 f9 ff                     call       0x475150
004dc771  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004dc777  0f 57 c0                           xorps      xmm0, xmm0
004dc77a  f3 0f 11 81 cc 00 00 00            movss      dword ptr [ecx + 0xcc], xmm0
004dc782  8b 15 28 00 5c 00                  mov        edx, dword ptr [0x5c0028]
004dc788  0f 57 c0                           xorps      xmm0, xmm0
004dc78b  f3 0f 11 82 c8 00 00 00            movss      dword ptr [edx + 0xc8], xmm0
004dc793  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004dc799  e8 e2 13 f4 ff                     call       0x41db80
004dc79e  6a 02                              push       2
004dc7a0  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004dc7a3  e8 38 15 f4 ff                     call       0x41dce0
004dc7a8  b8 01 00 00 00                     mov        eax, 1
004dc7ad  8b e5                              mov        esp, ebp
004dc7af  5d                                 pop        ebp
004dc7b0  c3                                 ret        
