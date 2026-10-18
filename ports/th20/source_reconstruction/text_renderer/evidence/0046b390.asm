0046b390  55                                 push       ebp
0046b391  8b ec                              mov        ebp, esp
0046b393  6a ff                              push       -1
0046b395  68 8d 7a 56 00                     push       0x567a8d
0046b39a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0046b3a0  50                                 push       eax
0046b3a1  83 ec 38                           sub        esp, 0x38
0046b3a4  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0046b3a9  33 c5                              xor        eax, ebp
0046b3ab  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0046b3ae  50                                 push       eax
0046b3af  8d 45 f4                           lea        eax, [ebp - 0xc]
0046b3b2  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0046b3b8  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
0046b3bb  6a 12                              push       0x12
0046b3bd  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0046b3c2  e8 49 0f fa ff                     call       0x40c310
0046b3c7  50                                 push       eax
0046b3c8  8d 4d ec                           lea        ecx, [ebp - 0x14]
0046b3cb  e8 60 03 fa ff                     call       0x40b730
0046b3d0  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0046b3d7  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
0046b3da  8b 08                              mov        ecx, dword ptr [eax]
0046b3dc  81 c1 80 06 00 00                  add        ecx, 0x680
0046b3e2  e8 49 fe ff ff                     call       0x46b230
0046b3e7  0f b6 c8                           movzx      ecx, al
0046b3ea  85 c9                              test       ecx, ecx
0046b3ec  74 26                              je         0x46b414
0046b3ee  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
0046b3f1  8b 02                              mov        eax, dword ptr [edx]
0046b3f3  50                                 push       eax
0046b3f4  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0046b3fa  e8 01 ed ff ff                     call       0x46a100
0046b3ff  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0046b406  8d 4d ec                           lea        ecx, [ebp - 0x14]
0046b409  e8 f2 04 fa ff                     call       0x40b900
0046b40e  90                                 nop        
0046b40f  e9 b1 00 00 00                     jmp        0x46b4c5
0046b414  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0046b417  83 c1 04                           add        ecx, 4
0046b41a  e8 d1 06 fe ff                     call       0x44baf0
0046b41f  89 45 e0                           mov        dword ptr [ebp - 0x20], eax
0046b422  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0046b425  8b 11                              mov        edx, dword ptr [ecx]
0046b427  81 c2 78 06 00 00                  add        edx, 0x678
0046b42d  89 55 dc                           mov        dword ptr [ebp - 0x24], edx
0046b430  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
0046b433  8b 08                              mov        ecx, dword ptr [eax]
0046b435  8b 91 30 06 00 00                  mov        edx, dword ptr [ecx + 0x630]
0046b43b  89 55 d8                           mov        dword ptr [ebp - 0x28], edx
0046b43e  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
0046b441  8b 08                              mov        ecx, dword ptr [eax]
0046b443  8b 91 2c 06 00 00                  mov        edx, dword ptr [ecx + 0x62c]
0046b449  89 55 d4                           mov        dword ptr [ebp - 0x2c], edx
0046b44c  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
0046b44f  8b 08                              mov        ecx, dword ptr [eax]
0046b451  8b 91 54 06 00 00                  mov        edx, dword ptr [ecx + 0x654]
0046b457  89 55 d0                           mov        dword ptr [ebp - 0x30], edx
0046b45a  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
0046b45d  8b 08                              mov        ecx, dword ptr [eax]
0046b45f  8b 91 50 06 00 00                  mov        edx, dword ptr [ecx + 0x650]
0046b465  89 55 cc                           mov        dword ptr [ebp - 0x34], edx
0046b468  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
0046b46b  8b 08                              mov        ecx, dword ptr [eax]
0046b46d  83 c1 14                           add        ecx, 0x14
0046b470  89 4d c8                           mov        dword ptr [ebp - 0x38], ecx
0046b473  8b 55 e0                           mov        edx, dword ptr [ebp - 0x20]
0046b476  52                                 push       edx
0046b477  83 ec 28                           sub        esp, 0x28
0046b47a  8b cc                              mov        ecx, esp
0046b47c  89 65 c4                           mov        dword ptr [ebp - 0x3c], esp
0046b47f  8d 45 e7                           lea        eax, [ebp - 0x19]
0046b482  50                                 push       eax
0046b483  e8 98 c9 ff ff                     call       0x467e20
0046b488  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
0046b48b  51                                 push       ecx
0046b48c  8b 55 d8                           mov        edx, dword ptr [ebp - 0x28]
0046b48f  52                                 push       edx
0046b490  6a 00                              push       0
0046b492  8b 45 d4                           mov        eax, dword ptr [ebp - 0x2c]
0046b495  50                                 push       eax
0046b496  8b 4d d0                           mov        ecx, dword ptr [ebp - 0x30]
0046b499  51                                 push       ecx
0046b49a  8b 55 cc                           mov        edx, dword ptr [ebp - 0x34]
0046b49d  52                                 push       edx
0046b49e  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046b4a1  50                                 push       eax
0046b4a2  8d 4d bc                           lea        ecx, [ebp - 0x44]
0046b4a5  51                                 push       ecx
0046b4a6  8b 15 28 00 5c 00                  mov        edx, dword ptr [0x5c0028]
0046b4ac  52                                 push       edx
0046b4ad  e8 6e f7 fd ff                     call       0x44ac20
0046b4b2  83 c4 50                           add        esp, 0x50
0046b4b5  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0046b4bc  8d 4d ec                           lea        ecx, [ebp - 0x14]
0046b4bf  e8 3c 04 fa ff                     call       0x40b900
0046b4c4  90                                 nop        
0046b4c5  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0046b4c8  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0046b4cf  59                                 pop        ecx
0046b4d0  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046b4d3  33 cd                              xor        ecx, ebp
0046b4d5  e8 14 75 0d 00                     call       0x5429ee
0046b4da  8b e5                              mov        esp, ebp
0046b4dc  5d                                 pop        ebp
0046b4dd  c3                                 ret        
