004193e0  55                                 push       ebp
004193e1  8b ec                              mov        ebp, esp
004193e3  83 ec 54                           sub        esp, 0x54
004193e6  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
004193eb  33 c5                              xor        eax, ebp
004193ed  89 45 fc                           mov        dword ptr [ebp - 4], eax
004193f0  56                                 push       esi
004193f1  89 4d f0                           mov        dword ptr [ebp - 0x10], ecx
004193f4  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004193f7  e8 14 37 00 00                     call       0x41cb10
004193fc  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
004193ff  dd 98 c8 20 00 00                  fstp       qword ptr [eax + 0x20c8]
00419405  f2 0f 10 05 68 c4 56 00            movsd      xmm0, qword ptr [0x56c468]
0041940d  f2 0f 5e 05 70 c4 56 00            divsd      xmm0, qword ptr [0x56c470]
00419415  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00419418  f2 0f 58 81 e8 20 00 00            addsd      xmm0, qword ptr [ecx + 0x20e8]
00419420  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00419423  f2 0f 5c 82 c8 20 00 00            subsd      xmm0, qword ptr [edx + 0x20c8]
0041942b  f2 0f 59 05 88 cd 56 00            mulsd      xmm0, qword ptr [0x56cd88]
00419433  f2 0f 5c 05 78 cd 56 00            subsd      xmm0, qword ptr [0x56cd78]
0041943b  f2 0f 2c c0                        cvttsd2si  eax, xmm0
0041943f  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00419442  89 81 fc 20 00 00                  mov        dword ptr [ecx + 0x20fc], eax
00419448  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0041944b  e8 00 36 00 00                     call       0x41ca50
00419450  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00419453  8b 40 04                           mov        eax, dword ptr [eax + 4]
00419456  3b 82 fc 20 00 00                  cmp        eax, dword ptr [edx + 0x20fc]
0041945c  7c 48                              jl         0x4194a6
0041945e  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00419461  83 b9 fc 20 00 00 00               cmp        dword ptr [ecx + 0x20fc], 0
00419468  7c 1c                              jl         0x419486
0041946a  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0041946d  8b 82 fc 20 00 00                  mov        eax, dword ptr [edx + 0x20fc]
00419473  89 45 d4                           mov        dword ptr [ebp - 0x2c], eax
00419476  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00419479  e8 d2 35 00 00                     call       0x41ca50
0041947e  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
00419481  89 48 04                           mov        dword ptr [eax + 4], ecx
00419484  eb 0f                              jmp        0x419495
00419486  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00419489  e8 c2 35 00 00                     call       0x41ca50
0041948e  c7 40 04 00 00 00 00               mov        dword ptr [eax + 4], 0
00419495  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00419498  e8 b3 35 00 00                     call       0x41ca50
0041949d  c7 40 08 00 00 00 00               mov        dword ptr [eax + 8], 0
004194a4  eb 6c                              jmp        0x419512
004194a6  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004194a9  e8 a2 35 00 00                     call       0x41ca50
004194ae  83 c0 08                           add        eax, 8
004194b1  89 45 ec                           mov        dword ptr [ebp - 0x14], eax
004194b4  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
004194b7  8b 02                              mov        eax, dword ptr [edx]
004194b9  83 c0 01                           add        eax, 1
004194bc  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
004194bf  89 01                              mov        dword ptr [ecx], eax
004194c1  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004194c4  e8 87 35 00 00                     call       0x41ca50
004194c9  83 78 08 0f                        cmp        dword ptr [eax + 8], 0xf
004194cd  7c 43                              jl         0x419512
004194cf  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004194d2  e8 79 35 00 00                     call       0x41ca50
004194d7  8b f0                              mov        esi, eax
004194d9  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004194dc  e8 6f 35 00 00                     call       0x41ca50
004194e1  8b 56 04                           mov        edx, dword ptr [esi + 4]
004194e4  3b 10                              cmp        edx, dword ptr [eax]
004194e6  7d 1b                              jge        0x419503
004194e8  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004194eb  e8 60 35 00 00                     call       0x41ca50
004194f0  83 c0 04                           add        eax, 4
004194f3  89 45 e8                           mov        dword ptr [ebp - 0x18], eax
004194f6  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
004194f9  8b 08                              mov        ecx, dword ptr [eax]
004194fb  83 c1 01                           add        ecx, 1
004194fe  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
00419501  89 0a                              mov        dword ptr [edx], ecx
00419503  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00419506  e8 45 35 00 00                     call       0x41ca50
0041950b  c7 40 08 00 00 00 00               mov        dword ptr [eax + 8], 0
00419512  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00419515  83 b8 fc 20 00 00 00               cmp        dword ptr [eax + 0x20fc], 0
0041951c  7d 0d                              jge        0x41952b
0041951e  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00419521  c7 81 fc 20 00 00 00 00 00 00      mov        dword ptr [ecx + 0x20fc], 0
0041952b  33 d2                              xor        edx, edx
0041952d  89 55 f4                           mov        dword ptr [ebp - 0xc], edx
00419530  89 55 f8                           mov        dword ptr [ebp - 8], edx
00419533  c7 45 f8 00 00 00 00               mov        dword ptr [ebp - 8], 0
0041953a  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0041953d  e8 ce 35 00 00                     call       0x41cb10
00419542  dd 5d bc                           fstp       qword ptr [ebp - 0x44]
00419545  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00419548  f2 0f 10 80 f0 20 00 00            movsd      xmm0, qword ptr [eax + 0x20f0]
00419550  66 0f 2f 45 bc                     comisd     xmm0, xmmword ptr [ebp - 0x44]
00419555  76 10                              jbe        0x419567
00419557  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0041955a  f2 0f 10 45 bc                     movsd      xmm0, qword ptr [ebp - 0x44]
0041955f  f2 0f 11 81 f0 20 00 00            movsd      qword ptr [ecx + 0x20f0], xmm0
00419567  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0041956a  f2 0f 10 45 bc                     movsd      xmm0, qword ptr [ebp - 0x44]
0041956f  f2 0f 5c 82 f0 20 00 00            subsd      xmm0, qword ptr [edx + 0x20f0]
00419577  f2 0f 10 0d 60 cd 56 00            movsd      xmm1, qword ptr [0x56cd60]
0041957f  66 0f 2f c8                        comisd     xmm1, xmm0
00419583  0f 86 9a 00 00 00                  jbe        0x419623
00419589  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041958e  e8 6d c2 ff ff                     call       0x415800
00419593  85 c0                              test       eax, eax
00419595  0f 85 86 00 00 00                  jne        0x419621
0041959b  c7 45 d0 00 00 00 00               mov        dword ptr [ebp - 0x30], 0
004195a2  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004195a5  e8 66 35 00 00                     call       0x41cb10
004195aa  dd 5d b4                           fstp       qword ptr [ebp - 0x4c]
004195ad  f2 0f 10 45 b4                     movsd      xmm0, qword ptr [ebp - 0x4c]
004195b2  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
004195b5  f2 0f 5c 80 f0 20 00 00            subsd      xmm0, qword ptr [eax + 0x20f0]
004195bd  f2 0f 10 0d 58 cd 56 00            movsd      xmm1, qword ptr [0x56cd58]
004195c5  66 0f 2f c8                        comisd     xmm1, xmm0
004195c9  76 0b                              jbe        0x4195d6
004195cb  6a 01                              push       1
004195cd  ff 15 6c c1 56 00                  call       dword ptr [0x56c16c] ; KERNEL32.dll!Sleep
004195d3  90                                 nop        
004195d4  eb cc                              jmp        0x4195a2
004195d6  c7 45 f4 00 00 00 00               mov        dword ptr [ebp - 0xc], 0
004195dd  b9 01 00 00 00                     mov        ecx, 1
004195e2  85 c9                              test       ecx, ecx
004195e4  74 3b                              je         0x419621
004195e6  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
004195e9  89 55 d0                           mov        dword ptr [ebp - 0x30], edx
004195ec  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004195f1  e8 3a 91 ff ff                     call       0x412730
004195f6  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
004195f9  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
004195fc  8b 08                              mov        ecx, dword ptr [eax]
004195fe  8b 51 4c                           mov        edx, dword ptr [ecx + 0x4c]
00419601  89 55 cc                           mov        dword ptr [ebp - 0x34], edx
00419604  8d 45 f4                           lea        eax, [ebp - 0xc]
00419607  50                                 push       eax
00419608  6a 00                              push       0
0041960a  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
0041960d  51                                 push       ecx
0041960e  ff 55 cc                           call       dword ptr [ebp - 0x34]
00419611  85 c0                              test       eax, eax
00419613  74 02                              je         0x419617
00419615  eb 0a                              jmp        0x419621
00419617  83 7d f4 00                        cmp        dword ptr [ebp - 0xc], 0
0041961b  74 02                              je         0x41961f
0041961d  eb 02                              jmp        0x419621
0041961f  eb bc                              jmp        0x4195dd
00419621  eb 29                              jmp        0x41964c
00419623  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00419626  e8 25 34 00 00                     call       0x41ca50
0041962b  83 78 04 00                        cmp        dword ptr [eax + 4], 0
0041962f  7e 1b                              jle        0x41964c
00419631  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00419634  e8 17 34 00 00                     call       0x41ca50
00419639  83 c0 04                           add        eax, 4
0041963c  89 45 e0                           mov        dword ptr [ebp - 0x20], eax
0041963f  8b 55 e0                           mov        edx, dword ptr [ebp - 0x20]
00419642  8b 02                              mov        eax, dword ptr [edx]
00419644  83 e8 01                           sub        eax, 1
00419647  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
0041964a  89 01                              mov        dword ptr [ecx], eax
0041964c  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0041964f  e8 bc 34 00 00                     call       0x41cb10
00419654  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00419657  dd 9a f0 20 00 00                  fstp       qword ptr [edx + 0x20f0]
0041965d  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00419660  e8 fb 00 00 00                     call       0x419760
00419665  90                                 nop        
00419666  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041966b  e8 c0 90 ff ff                     call       0x412730
00419670  89 45 dc                           mov        dword ptr [ebp - 0x24], eax
00419673  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
00419676  8b 08                              mov        ecx, dword ptr [eax]
00419678  8b 51 44                           mov        edx, dword ptr [ecx + 0x44]
0041967b  89 55 c8                           mov        dword ptr [ebp - 0x38], edx
0041967e  6a 00                              push       0
00419680  6a 00                              push       0
00419682  6a 00                              push       0
00419684  6a 00                              push       0
00419686  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
00419689  50                                 push       eax
0041968a  ff 55 c8                           call       dword ptr [ebp - 0x38]
0041968d  85 c0                              test       eax, eax
0041968f  7d 62                              jge        0x4196f3
00419691  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
00419696  e8 a5 41 0c 00                     call       0x4dd840
0041969b  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004196a1  e8 3a 45 00 00                     call       0x41dbe0
004196a6  90                                 nop        
004196a7  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004196ac  e8 7f 90 ff ff                     call       0x412730
004196b1  89 45 d8                           mov        dword ptr [ebp - 0x28], eax
004196b4  8b 4d d8                           mov        ecx, dword ptr [ebp - 0x28]
004196b7  8b 11                              mov        edx, dword ptr [ecx]
004196b9  8b 42 40                           mov        eax, dword ptr [edx + 0x40]
004196bc  89 45 c4                           mov        dword ptr [ebp - 0x3c], eax
004196bf  68 24 4e 5c 00                     push       0x5c4e24
004196c4  8b 4d d8                           mov        ecx, dword ptr [ebp - 0x28]
004196c7  51                                 push       ecx
004196c8  ff 55 c4                           call       dword ptr [ebp - 0x3c]
004196cb  85 c0                              test       eax, eax
004196cd  75 24                              jne        0x4196f3
004196cf  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004196d5  e8 16 43 00 00                     call       0x41d9f0
004196da  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004196df  e8 8c 26 0c 00                     call       0x4dbd70
004196e4  e8 d7 0b 00 00                     call       0x41a2c0
004196e9  c7 05 6c 58 5c 00 02 00 00 00      mov        dword ptr [0x5c586c], 2
004196f3  e8 c8 fc ff ff                     call       0x4193c0
004196f8  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004196fb  e8 10 34 00 00                     call       0x41cb10
00419700  dd 5d ac                           fstp       qword ptr [ebp - 0x54]
00419703  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00419706  e8 45 33 00 00                     call       0x41ca50
0041970b  83 78 04 00                        cmp        dword ptr [eax + 4], 0
0041970f  7e 23                              jle        0x419734
00419711  6a 01                              push       1
00419713  ff 15 d8 c2 56 00                  call       dword ptr [0x56c2d8] ; WINMM.dll!timeBeginPeriod
00419719  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0041971c  e8 2f 33 00 00                     call       0x41ca50
00419721  8b 50 04                           mov        edx, dword ptr [eax + 4]
00419724  52                                 push       edx
00419725  ff 15 6c c1 56 00                  call       dword ptr [0x56c16c] ; KERNEL32.dll!Sleep
0041972b  6a 01                              push       1
0041972d  ff 15 dc c2 56 00                  call       dword ptr [0x56c2dc] ; WINMM.dll!timeEndPeriod
00419733  90                                 nop        
00419734  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00419737  e8 d4 33 00 00                     call       0x41cb10
0041973c  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0041973f  dd 98 e8 20 00 00                  fstp       qword ptr [eax + 0x20e8]
00419745  5e                                 pop        esi
00419746  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00419749  33 cd                              xor        ecx, ebp
0041974b  e8 9e 92 12 00                     call       0x5429ee
00419750  8b e5                              mov        esp, ebp
00419752  5d                                 pop        ebp
00419753  c3                                 ret        
