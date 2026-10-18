004d97f0  55                                 push       ebp
004d97f1  8b ec                              mov        ebp, esp
004d97f3  83 ec 14                           sub        esp, 0x14
004d97f6  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
004d97fb  33 c5                              xor        eax, ebp
004d97fd  89 45 fc                           mov        dword ptr [ebp - 4], eax
004d9800  c7 45 f8 00 00 00 00               mov        dword ptr [ebp - 8], 0
004d9807  c7 45 f4 00 00 00 00               mov        dword ptr [ebp - 0xc], 0
004d980e  8d 45 f4                           lea        eax, [ebp - 0xc]
004d9811  50                                 push       eax
004d9812  8d 4d f8                           lea        ecx, [ebp - 8]
004d9815  51                                 push       ecx
004d9816  e8 05 01 00 00                     call       0x4d9920
004d981b  83 c4 08                           add        esp, 8
004d981e  83 7d f4 00                        cmp        dword ptr [ebp - 0xc], 0
004d9822  75 07                              jne        0x4d982b
004d9824  33 c0                              xor        eax, eax
004d9826  e9 c5 00 00 00                     jmp        0x4d98f0
004d982b  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
004d982e  52                                 push       edx
004d982f  68 b0 1b 57 00                     push       0x571bb0 ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\game\\mother.cpp:483 Gdiplus::ImageCodecInfo'
004d9834  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004d983a  e8 61 ee ff ff                     call       0x4d86a0
004d983f  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
004d9842  83 7d f0 00                        cmp        dword ptr [ebp - 0x10], 0
004d9846  75 07                              jne        0x4d984f
004d9848  33 c0                              xor        eax, eax
004d984a  e9 a1 00 00 00                     jmp        0x4d98f0
004d984f  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
004d9852  50                                 push       eax
004d9853  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
004d9856  51                                 push       ecx
004d9857  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
004d985a  52                                 push       edx
004d985b  e8 a0 00 00 00                     call       0x4d9900
004d9860  83 c4 0c                           add        esp, 0xc
004d9863  c7 45 ec 00 00 00 00               mov        dword ptr [ebp - 0x14], 0
004d986a  eb 09                              jmp        0x4d9875
004d986c  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
004d986f  83 c0 01                           add        eax, 1
004d9872  89 45 ec                           mov        dword ptr [ebp - 0x14], eax
004d9875  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
004d9878  3b 4d f8                           cmp        ecx, dword ptr [ebp - 8]
004d987b  73 5b                              jae        0x4d98d8
004d987d  8b 55 08                           mov        edx, dword ptr [ebp + 8]
004d9880  52                                 push       edx
004d9881  6b 45 ec 4c                        imul       eax, dword ptr [ebp - 0x14], 0x4c
004d9885  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004d9888  8b 54 01 30                        mov        edx, dword ptr [ecx + eax + 0x30]
004d988c  52                                 push       edx
004d988d  e8 1e 7b 07 00                     call       0x5513b0
004d9892  83 c4 08                           add        esp, 8
004d9895  85 c0                              test       eax, eax
004d9897  75 3d                              jne        0x4d98d6
004d9899  6b 45 ec 4c                        imul       eax, dword ptr [ebp - 0x14], 0x4c
004d989d  03 45 f0                           add        eax, dword ptr [ebp - 0x10]
004d98a0  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
004d98a3  8b 10                              mov        edx, dword ptr [eax]
004d98a5  89 11                              mov        dword ptr [ecx], edx
004d98a7  8b 50 04                           mov        edx, dword ptr [eax + 4]
004d98aa  89 51 04                           mov        dword ptr [ecx + 4], edx
004d98ad  8b 50 08                           mov        edx, dword ptr [eax + 8]
004d98b0  89 51 08                           mov        dword ptr [ecx + 8], edx
004d98b3  8b 40 0c                           mov        eax, dword ptr [eax + 0xc]
004d98b6  89 41 0c                           mov        dword ptr [ecx + 0xc], eax
004d98b9  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004d98bc  51                                 push       ecx
004d98bd  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004d98c3  e8 78 3f f3 ff                     call       0x40d840
004d98c8  c7 45 f0 00 00 00 00               mov        dword ptr [ebp - 0x10], 0
004d98cf  b8 01 00 00 00                     mov        eax, 1
004d98d4  eb 1a                              jmp        0x4d98f0
004d98d6  eb 94                              jmp        0x4d986c
004d98d8  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
004d98db  52                                 push       edx
004d98dc  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004d98e2  e8 59 3f f3 ff                     call       0x40d840
004d98e7  c7 45 f0 00 00 00 00               mov        dword ptr [ebp - 0x10], 0
004d98ee  33 c0                              xor        eax, eax
004d98f0  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004d98f3  33 cd                              xor        ecx, ebp
004d98f5  e8 f4 90 06 00                     call       0x5429ee
004d98fa  8b e5                              mov        esp, ebp
004d98fc  5d                                 pop        ebp
004d98fd  c3                                 ret        
