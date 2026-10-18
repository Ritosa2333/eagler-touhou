0046b7f0  53                                 push       ebx
0046b7f1  8b dc                              mov        ebx, esp
0046b7f3  83 ec 08                           sub        esp, 8
0046b7f6  83 e4 f8                           and        esp, 0xfffffff8
0046b7f9  83 c4 04                           add        esp, 4
0046b7fc  55                                 push       ebp
0046b7fd  8b 6b 04                           mov        ebp, dword ptr [ebx + 4]
0046b800  89 6c 24 04                        mov        dword ptr [esp + 4], ebp
0046b804  8b ec                              mov        ebp, esp
0046b806  83 ec 60                           sub        esp, 0x60
0046b809  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0046b80e  33 c5                              xor        eax, ebp
0046b810  89 45 fc                           mov        dword ptr [ebp - 4], eax
0046b813  89 4d dc                           mov        dword ptr [ebp - 0x24], ecx
0046b816  c7 45 d8 00 00 00 00               mov        dword ptr [ebp - 0x28], 0
0046b81d  c7 45 d0 ff ff ff ff               mov        dword ptr [ebp - 0x30], 0xffffffff
0046b824  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
0046b827  05 bc 11 00 00                     add        eax, 0x11bc
0046b82c  50                                 push       eax
0046b82d  8d 4d ac                           lea        ecx, [ebp - 0x54]
0046b830  51                                 push       ecx
0046b831  e8 ba c7 ff ff                     call       0x467ff0
0046b836  83 c4 08                           add        esp, 8
0046b839  8b 10                              mov        edx, dword ptr [eax]
0046b83b  89 55 c4                           mov        dword ptr [ebp - 0x3c], edx
0046b83e  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046b841  89 45 c0                           mov        dword ptr [ebp - 0x40], eax
0046b844  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
0046b847  81 c1 bc 11 00 00                  add        ecx, 0x11bc
0046b84d  e8 fe 57 00 00                     call       0x471050
0046b852  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
0046b855  39 81 bc a1 01 00                  cmp        dword ptr [ecx + 0x1a1bc], eax
0046b85b  7e 13                              jle        0x46b870
0046b85d  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
0046b860  81 c1 bc 11 00 00                  add        ecx, 0x11bc
0046b866  e8 e5 57 00 00                     call       0x471050
0046b86b  89 45 cc                           mov        dword ptr [ebp - 0x34], eax
0046b86e  eb 0c                              jmp        0x46b87c
0046b870  8b 55 dc                           mov        edx, dword ptr [ebp - 0x24]
0046b873  8b 82 bc a1 01 00                  mov        eax, dword ptr [edx + 0x1a1bc]
0046b879  89 45 cc                           mov        dword ptr [ebp - 0x34], eax
0046b87c  8b 4d cc                           mov        ecx, dword ptr [ebp - 0x34]
0046b87f  51                                 push       ecx
0046b880  8b 55 c0                           mov        edx, dword ptr [ebp - 0x40]
0046b883  52                                 push       edx
0046b884  8d 4d a4                           lea        ecx, [ebp - 0x5c]
0046b887  e8 84 3a ff ff                     call       0x45f310
0046b88c  8b 08                              mov        ecx, dword ptr [eax]
0046b88e  8b 50 04                           mov        edx, dword ptr [eax + 4]
0046b891  89 4d b0                           mov        dword ptr [ebp - 0x50], ecx
0046b894  89 55 b4                           mov        dword ptr [ebp - 0x4c], edx
0046b897  a0 48 f3 56 00                     mov        al, byte ptr [0x56f348]
0046b89c  88 45 e3                           mov        byte ptr [ebp - 0x1d], al
0046b89f  8a 4d e3                           mov        cl, byte ptr [ebp - 0x1d]
0046b8a2  88 4d e2                           mov        byte ptr [ebp - 0x1e], cl
0046b8a5  0f b6 55 e2                        movzx      edx, byte ptr [ebp - 0x1e]
0046b8a9  52                                 push       edx
0046b8aa  8b 45 b4                           mov        eax, dword ptr [ebp - 0x4c]
0046b8ad  50                                 push       eax
0046b8ae  8b 4d b0                           mov        ecx, dword ptr [ebp - 0x50]
0046b8b1  51                                 push       ecx
0046b8b2  8d 4d e4                           lea        ecx, [ebp - 0x1c]
0046b8b5  e8 96 c6 ff ff                     call       0x467f50
0046b8ba  8d 55 e4                           lea        edx, [ebp - 0x1c]
0046b8bd  89 55 c8                           mov        dword ptr [ebp - 0x38], edx
0046b8c0  6a 08                              push       8
0046b8c2  8d 4d f0                           lea        ecx, [ebp - 0x10]
0046b8c5  e8 b6 07 fa ff                     call       0x40c080
0046b8ca  8d 45 f0                           lea        eax, [ebp - 0x10]
0046b8cd  50                                 push       eax
0046b8ce  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046b8d1  e8 4a 5d fe ff                     call       0x451620
0046b8d6  8d 4d fb                           lea        ecx, [ebp - 5]
0046b8d9  51                                 push       ecx
0046b8da  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0046b8dd  e8 5e 4c 00 00                     call       0x470540
0046b8e2  90                                 nop        
0046b8e3  eb 09                              jmp        0x46b8ee
0046b8e5  8d 4d f0                           lea        ecx, [ebp - 0x10]
0046b8e8  e8 c3 f9 ff ff                     call       0x46b2b0
0046b8ed  90                                 nop        
0046b8ee  0f b6 55 fb                        movzx      edx, byte ptr [ebp - 5]
0046b8f2  52                                 push       edx
0046b8f3  8d 45 f0                           lea        eax, [ebp - 0x10]
0046b8f6  50                                 push       eax
0046b8f7  e8 e4 f8 ff ff                     call       0x46b1e0
0046b8fc  83 c4 08                           add        esp, 8
0046b8ff  0f b6 c8                           movzx      ecx, al
0046b902  85 c9                              test       ecx, ecx
0046b904  75 75                              jne        0x46b97b
0046b906  8d 4d f0                           lea        ecx, [ebp - 0x10]
0046b909  e8 42 f9 ff ff                     call       0x46b250
0046b90e  89 45 d4                           mov        dword ptr [ebp - 0x2c], eax
0046b911  8b 55 d0                           mov        edx, dword ptr [ebp - 0x30]
0046b914  83 c2 01                           add        edx, 1
0046b917  89 55 d0                           mov        dword ptr [ebp - 0x30], edx
0046b91a  8b 45 d4                           mov        eax, dword ptr [ebp - 0x2c]
0046b91d  8b 88 34 01 00 00                  mov        ecx, dword ptr [eax + 0x134]
0046b923  83 e9 01                           sub        ecx, 1
0046b926  8b 55 d4                           mov        edx, dword ptr [ebp - 0x2c]
0046b929  89 8a 34 01 00 00                  mov        dword ptr [edx + 0x134], ecx
0046b92f  8b 45 d4                           mov        eax, dword ptr [ebp - 0x2c]
0046b932  83 b8 34 01 00 00 00               cmp        dword ptr [eax + 0x134], 0
0046b939  7c 3b                              jl         0x46b976
0046b93b  8b 4d d8                           mov        ecx, dword ptr [ebp - 0x28]
0046b93e  3b 4d d0                           cmp        ecx, dword ptr [ebp - 0x30]
0046b941  74 2a                              je         0x46b96d
0046b943  8b 55 dc                           mov        edx, dword ptr [ebp - 0x24]
0046b946  81 c2 bc 11 00 00                  add        edx, 0x11bc
0046b94c  89 55 bc                           mov        dword ptr [ebp - 0x44], edx
0046b94f  68 40 01 00 00                     push       0x140
0046b954  8b 45 d4                           mov        eax, dword ptr [ebp - 0x2c]
0046b957  50                                 push       eax
0046b958  8b 4d d8                           mov        ecx, dword ptr [ebp - 0x28]
0046b95b  51                                 push       ecx
0046b95c  8b 4d bc                           mov        ecx, dword ptr [ebp - 0x44]
0046b95f  e8 ac f8 ff ff                     call       0x46b210
0046b964  50                                 push       eax
0046b965  e8 b6 84 0d 00                     call       0x543e20
0046b96a  83 c4 0c                           add        esp, 0xc
0046b96d  8b 55 d8                           mov        edx, dword ptr [ebp - 0x28]
0046b970  83 c2 01                           add        edx, 1
0046b973  89 55 d8                           mov        dword ptr [ebp - 0x28], edx
0046b976  e9 6a ff ff ff                     jmp        0x46b8e5
0046b97b  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
0046b97e  8b 4d d8                           mov        ecx, dword ptr [ebp - 0x28]
0046b981  89 88 bc a1 01 00                  mov        dword ptr [eax + 0x1a1bc], ecx
0046b987  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046b98a  33 cd                              xor        ecx, ebp
0046b98c  e8 5d 70 0d 00                     call       0x5429ee
0046b991  8b e5                              mov        esp, ebp
0046b993  5d                                 pop        ebp
0046b994  8b e3                              mov        esp, ebx
0046b996  5b                                 pop        ebx
0046b997  c3                                 ret        
