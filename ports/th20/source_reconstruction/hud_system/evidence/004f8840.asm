004f8840  55                                 push       ebp
004f8841  8b ec                              mov        ebp, esp
004f8843  83 ec 20                           sub        esp, 0x20
004f8846  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
004f884b  33 c5                              xor        eax, ebp
004f884d  89 45 fc                           mov        dword ptr [ebp - 4], eax
004f8850  89 4d ec                           mov        dword ptr [ebp - 0x14], ecx
004f8853  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
004f8856  83 78 30 01                        cmp        dword ptr [eax + 0x30], 1
004f885a  7c 34                              jl         0x4f8890
004f885c  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
004f885f  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
004f8862  8b 41 34                           mov        eax, dword ptr [ecx + 0x34]
004f8865  3b 42 30                           cmp        eax, dword ptr [edx + 0x30]
004f8868  7d 26                              jge        0x4f8890
004f886a  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
004f886d  e8 6e 6f 00 00                     call       0x4ff7e0
004f8872  85 c0                              test       eax, eax
004f8874  74 1a                              je         0x4f8890
004f8876  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
004f8879  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
004f887c  8b 42 30                           mov        eax, dword ptr [edx + 0x30]
004f887f  89 41 34                           mov        dword ptr [ecx + 0x34], eax
004f8882  6a 78                              push       0x78
004f8884  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
004f8887  83 c1 10                           add        ecx, 0x10
004f888a  e8 91 ac f2 ff                     call       0x423520
004f888f  90                                 nop        
004f8890  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
004f8893  83 79 34 00                        cmp        dword ptr [ecx + 0x34], 0
004f8897  0f 84 40 02 00 00                  je         0x4f8add
004f889d  51                                 push       ecx
004f889e  0f 57 c0                           xorps      xmm0, xmm0
004f88a1  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004f88a6  51                                 push       ecx
004f88a7  f3 0f 10 05 68 31 57 00            movss      xmm0, dword ptr [0x573168]
004f88af  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004f88b4  51                                 push       ecx
004f88b5  f3 0f 10 05 04 fc 56 00            movss      xmm0, dword ptr [0x56fc04]
004f88bd  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004f88c2  8d 4d f0                           lea        ecx, [ebp - 0x10]
004f88c5  e8 06 a5 f2 ff                     call       0x422dd0
004f88ca  68 00 ff ff ff                     push       0xffffff00
004f88cf  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004f88d5  e8 f6 81 f7 ff                     call       0x470ad0
004f88da  90                                 nop        
004f88db  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
004f88de  e8 fd 6e 00 00                     call       0x4ff7e0
004f88e3  85 c0                              test       eax, eax
004f88e5  75 3c                              jne        0x4f8923
004f88e7  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
004f88ea  83 c2 10                           add        edx, 0x10
004f88ed  89 55 e0                           mov        dword ptr [ebp - 0x20], edx
004f88f0  6a 06                              push       6
004f88f2  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
004f88f5  e8 46 97 f7 ff                     call       0x472040
004f88fa  83 f8 03                           cmp        eax, 3
004f88fd  7d 13                              jge        0x4f8912
004f88ff  68 00 ff ff ff                     push       0xffffff00
004f8904  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004f890a  e8 c1 81 f7 ff                     call       0x470ad0
004f890f  90                                 nop        
004f8910  eb 11                              jmp        0x4f8923
004f8912  68 80 80 80 80                     push       0x80808080
004f8917  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004f891d  e8 ae 81 f7 ff                     call       0x470ad0
004f8922  90                                 nop        
004f8923  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
004f8926  0f b6 48 50                        movzx      ecx, byte ptr [eax + 0x50]
004f892a  85 c9                              test       ecx, ecx
004f892c  74 09                              je         0x4f8937
004f892e  c7 45 e8 40 00 00 00               mov        dword ptr [ebp - 0x18], 0x40
004f8935  eb 07                              jmp        0x4f893e
004f8937  c7 45 e8 ff 00 00 00               mov        dword ptr [ebp - 0x18], 0xff
004f893e  0f b6 55 e8                        movzx      edx, byte ptr [ebp - 0x18]
004f8942  52                                 push       edx
004f8943  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004f8949  e8 f2 ff f8 ff                     call       0x488940
004f894e  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
004f8951  0f b6 48 50                        movzx      ecx, byte ptr [eax + 0x50]
004f8955  85 c9                              test       ecx, ecx
004f8957  74 09                              je         0x4f8962
004f8959  c7 45 e4 40 00 00 00               mov        dword ptr [ebp - 0x1c], 0x40
004f8960  eb 07                              jmp        0x4f8969
004f8962  c7 45 e4 ff 00 00 00               mov        dword ptr [ebp - 0x1c], 0xff
004f8969  0f b6 55 e4                        movzx      edx, byte ptr [ebp - 0x1c]
004f896d  52                                 push       edx
004f896e  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004f8974  e8 87 fc fb ff                     call       0x4b8600
004f8979  6a 06                              push       6
004f897b  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004f8981  e8 7a 01 f9 ff                     call       0x488b00
004f8986  6a 00                              push       0
004f8988  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004f898e  e8 6d 00 f9 ff                     call       0x488a00
004f8993  51                                 push       ecx
004f8994  f3 0f 10 05 cc c8 56 00            movss      xmm0, dword ptr [0x56c8cc]
004f899c  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004f89a1  51                                 push       ecx
004f89a2  f3 0f 10 05 cc c8 56 00            movss      xmm0, dword ptr [0x56c8cc]
004f89aa  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004f89af  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004f89b5  e8 d6 03 fc ff                     call       0x4b8d90
004f89ba  6a 02                              push       2
004f89bc  6a 02                              push       2
004f89be  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004f89c4  e8 57 fc fb ff                     call       0x4b8620
004f89c9  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
004f89cc  8b 48 34                           mov        ecx, dword ptr [eax + 0x34]
004f89cf  51                                 push       ecx
004f89d0  8d 55 f0                           lea        edx, [ebp - 0x10]
004f89d3  52                                 push       edx
004f89d4  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004f89da  e8 a1 42 f7 ff                     call       0x46cc80
004f89df  6a 02                              push       2
004f89e1  6a 01                              push       1
004f89e3  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004f89e9  e8 32 fc fb ff                     call       0x4b8620
004f89ee  51                                 push       ecx
004f89ef  f3 0f 10 05 60 31 57 00            movss      xmm0, dword ptr [0x573160]
004f89f7  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004f89fc  51                                 push       ecx
004f89fd  f3 0f 10 05 60 31 57 00            movss      xmm0, dword ptr [0x573160]
004f8a05  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004f8a0a  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004f8a10  e8 7b 03 fc ff                     call       0x4b8d90
004f8a15  90                                 nop        
004f8a16  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
004f8a19  83 78 34 01                        cmp        dword ptr [eax + 0x34], 1
004f8a1d  7e 21                              jle        0x4f8a40
004f8a1f  83 ec 08                           sub        esp, 8
004f8a22  8b cc                              mov        ecx, esp
004f8a24  68 90 2c 57 00                     push       0x572c90
004f8a29  e8 a2 53 f1 ff                     call       0x40ddd0
004f8a2e  8d 4d f0                           lea        ecx, [ebp - 0x10]
004f8a31  51                                 push       ecx
004f8a32  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004f8a38  e8 53 44 f7 ff                     call       0x46ce90
004f8a3d  90                                 nop        
004f8a3e  eb 1f                              jmp        0x4f8a5f
004f8a40  83 ec 08                           sub        esp, 8
004f8a43  8b cc                              mov        ecx, esp
004f8a45  68 98 2c 57 00                     push       0x572c98
004f8a4a  e8 81 53 f1 ff                     call       0x40ddd0
004f8a4f  8d 55 f0                           lea        edx, [ebp - 0x10]
004f8a52  52                                 push       edx
004f8a53  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004f8a59  e8 32 44 f7 ff                     call       0x46ce90
004f8a5e  90                                 nop        
004f8a5f  51                                 push       ecx
004f8a60  f3 0f 10 05 cc c8 56 00            movss      xmm0, dword ptr [0x56c8cc]
004f8a68  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004f8a6d  51                                 push       ecx
004f8a6e  f3 0f 10 05 cc c8 56 00            movss      xmm0, dword ptr [0x56c8cc]
004f8a76  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004f8a7b  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004f8a81  e8 0a 03 fc ff                     call       0x4b8d90
004f8a86  6a ff                              push       -1
004f8a88  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004f8a8e  e8 3d 80 f7 ff                     call       0x470ad0
004f8a93  68 ff 00 00 00                     push       0xff
004f8a98  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004f8a9e  e8 9d fe f8 ff                     call       0x488940
004f8aa3  68 ff 00 00 00                     push       0xff
004f8aa8  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004f8aae  e8 4d fb fb ff                     call       0x4b8600
004f8ab3  6a 00                              push       0
004f8ab5  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004f8abb  e8 40 ff f8 ff                     call       0x488a00
004f8ac0  6a 01                              push       1
004f8ac2  6a 01                              push       1
004f8ac4  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004f8aca  e8 51 fb fb ff                     call       0x4b8620
004f8acf  6a 00                              push       0
004f8ad1  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004f8ad7  e8 24 00 f9 ff                     call       0x488b00
004f8adc  90                                 nop        
004f8add  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004f8ae0  33 cd                              xor        ecx, ebp
004f8ae2  e8 07 9f 04 00                     call       0x5429ee
004f8ae7  8b e5                              mov        esp, ebp
004f8ae9  5d                                 pop        ebp
004f8aea  c3                                 ret        
