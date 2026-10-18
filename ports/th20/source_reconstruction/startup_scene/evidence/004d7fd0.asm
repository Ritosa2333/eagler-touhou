004d7fd0  55                                 push       ebp
004d7fd1  8b ec                              mov        ebp, esp
004d7fd3  6a ff                              push       -1
004d7fd5  68 00 76 56 00                     push       0x567600
004d7fda  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
004d7fe0  50                                 push       eax
004d7fe1  51                                 push       ecx
004d7fe2  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
004d7fe7  33 c5                              xor        eax, ebp
004d7fe9  50                                 push       eax
004d7fea  8d 45 f4                           lea        eax, [ebp - 0xc]
004d7fed  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
004d7ff3  89 4d f0                           mov        dword ptr [ebp - 0x10], ecx
004d7ff6  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
004d7ff9  c7 00 98 17 57 00                  mov        dword ptr [eax], 0x571798
004d7fff  68 b0 18 57 00                     push       0x5718b0 ; string='shutdown LoadingInf'
004d8004  e8 a7 46 f3 ff                     call       0x40c6b0
004d8009  83 c4 04                           add        esp, 4
004d800c  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004d800f  83 c1 10                           add        ecx, 0x10
004d8012  e8 d9 3c f3 ff                     call       0x40bcf0
004d8017  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004d801a  8b 51 08                           mov        edx, dword ptr [ecx + 8]
004d801d  52                                 push       edx
004d801e  8b 0d d8 66 5b 00                  mov        ecx, dword ptr [0x5b66d8]
004d8024  e8 87 a4 f3 ff                     call       0x4124b0
004d8029  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
004d802c  8b 48 0c                           mov        ecx, dword ptr [eax + 0xc]
004d802f  51                                 push       ecx
004d8030  8b 0d d8 66 5b 00                  mov        ecx, dword ptr [0x5b66d8]
004d8036  e8 75 a4 f3 ff                     call       0x4124b0
004d803b  e8 20 05 00 00                     call       0x4d8560
004d8040  6a 01                              push       1
004d8042  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004d8048  e8 e3 43 f7 ff                     call       0x44c430
004d804d  c7 05 2c 4d 5c 00 00 00 00 00      mov        dword ptr [0x5c4d2c], 0
004d8057  8b 15 98 06 5c 00                  mov        edx, dword ptr [0x5c0698]
004d805d  52                                 push       edx
004d805e  e8 5d 97 f4 ff                     call       0x4217c0
004d8063  83 c4 04                           add        esp, 4
004d8066  6a 00                              push       0
004d8068  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004d806e  e8 bd 43 f7 ff                     call       0x44c430
004d8073  e8 98 7b 03 00                     call       0x50fc10
004d8078  90                                 nop        
004d8079  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004d807c  83 c1 24                           add        ecx, 0x24
004d807f  e8 ec 12 f7 ff                     call       0x449370
004d8084  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004d8087  83 c1 10                           add        ecx, 0x10
004d808a  e8 f1 38 f3 ff                     call       0x40b980
004d808f  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004d8092  e8 59 7d f4 ff                     call       0x41fdf0
004d8097  90                                 nop        
004d8098  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
004d809b  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
004d80a2  59                                 pop        ecx
004d80a3  8b e5                              mov        esp, ebp
004d80a5  5d                                 pop        ebp
004d80a6  c3                                 ret        
