0046ba20  53                                 push       ebx
0046ba21  8b dc                              mov        ebx, esp
0046ba23  83 ec 08                           sub        esp, 8
0046ba26  83 e4 f8                           and        esp, 0xfffffff8
0046ba29  83 c4 04                           add        esp, 4
0046ba2c  55                                 push       ebp
0046ba2d  8b 6b 04                           mov        ebp, dword ptr [ebx + 4]
0046ba30  89 6c 24 04                        mov        dword ptr [esp + 4], ebp
0046ba34  8b ec                              mov        ebp, esp
0046ba36  83 ec 50                           sub        esp, 0x50
0046ba39  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0046ba3e  33 c5                              xor        eax, ebp
0046ba40  89 45 fc                           mov        dword ptr [ebp - 4], eax
0046ba43  89 4d dc                           mov        dword ptr [ebp - 0x24], ecx
0046ba46  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
0046ba49  05 bc 11 00 00                     add        eax, 0x11bc
0046ba4e  50                                 push       eax
0046ba4f  8d 4d bc                           lea        ecx, [ebp - 0x44]
0046ba52  51                                 push       ecx
0046ba53  e8 98 c5 ff ff                     call       0x467ff0
0046ba58  83 c4 08                           add        esp, 8
0046ba5b  8b 10                              mov        edx, dword ptr [eax]
0046ba5d  89 55 cc                           mov        dword ptr [ebp - 0x34], edx
0046ba60  8b 45 cc                           mov        eax, dword ptr [ebp - 0x34]
0046ba63  89 45 c8                           mov        dword ptr [ebp - 0x38], eax
0046ba66  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
0046ba69  81 c1 bc 11 00 00                  add        ecx, 0x11bc
0046ba6f  e8 dc 55 00 00                     call       0x471050
0046ba74  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
0046ba77  39 81 bc a1 01 00                  cmp        dword ptr [ecx + 0x1a1bc], eax
0046ba7d  7e 13                              jle        0x46ba92
0046ba7f  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
0046ba82  81 c1 bc 11 00 00                  add        ecx, 0x11bc
0046ba88  e8 c3 55 00 00                     call       0x471050
0046ba8d  89 45 d8                           mov        dword ptr [ebp - 0x28], eax
0046ba90  eb 0c                              jmp        0x46ba9e
0046ba92  8b 55 dc                           mov        edx, dword ptr [ebp - 0x24]
0046ba95  8b 82 bc a1 01 00                  mov        eax, dword ptr [edx + 0x1a1bc]
0046ba9b  89 45 d8                           mov        dword ptr [ebp - 0x28], eax
0046ba9e  8b 4d d8                           mov        ecx, dword ptr [ebp - 0x28]
0046baa1  51                                 push       ecx
0046baa2  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
0046baa5  52                                 push       edx
0046baa6  8d 4d b4                           lea        ecx, [ebp - 0x4c]
0046baa9  e8 62 38 ff ff                     call       0x45f310
0046baae  8b 08                              mov        ecx, dword ptr [eax]
0046bab0  8b 50 04                           mov        edx, dword ptr [eax + 4]
0046bab3  89 4d c0                           mov        dword ptr [ebp - 0x40], ecx
0046bab6  89 55 c4                           mov        dword ptr [ebp - 0x3c], edx
0046bab9  a0 48 f3 56 00                     mov        al, byte ptr [0x56f348]
0046babe  88 45 e3                           mov        byte ptr [ebp - 0x1d], al
0046bac1  8a 4d e3                           mov        cl, byte ptr [ebp - 0x1d]
0046bac4  88 4d e2                           mov        byte ptr [ebp - 0x1e], cl
0046bac7  0f b6 55 e2                        movzx      edx, byte ptr [ebp - 0x1e]
0046bacb  52                                 push       edx
0046bacc  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046bacf  50                                 push       eax
0046bad0  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
0046bad3  51                                 push       ecx
0046bad4  8d 4d e4                           lea        ecx, [ebp - 0x1c]
0046bad7  e8 74 c4 ff ff                     call       0x467f50
0046badc  8d 55 e4                           lea        edx, [ebp - 0x1c]
0046badf  89 55 d4                           mov        dword ptr [ebp - 0x2c], edx
0046bae2  6a 08                              push       8
0046bae4  8d 4d f0                           lea        ecx, [ebp - 0x10]
0046bae7  e8 94 05 fa ff                     call       0x40c080
0046baec  8d 45 f0                           lea        eax, [ebp - 0x10]
0046baef  50                                 push       eax
0046baf0  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046baf3  e8 28 5b fe ff                     call       0x451620
0046baf8  8d 4d fb                           lea        ecx, [ebp - 5]
0046bafb  51                                 push       ecx
0046bafc  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046baff  e8 3c 4a 00 00                     call       0x470540
0046bb04  90                                 nop        
0046bb05  eb 09                              jmp        0x46bb10
0046bb07  8d 4d f0                           lea        ecx, [ebp - 0x10]
0046bb0a  e8 a1 f7 ff ff                     call       0x46b2b0
0046bb0f  90                                 nop        
0046bb10  0f b6 55 fb                        movzx      edx, byte ptr [ebp - 5]
0046bb14  52                                 push       edx
0046bb15  8d 45 f0                           lea        eax, [ebp - 0x10]
0046bb18  50                                 push       eax
0046bb19  e8 c2 f6 ff ff                     call       0x46b1e0
0046bb1e  83 c4 08                           add        esp, 8
0046bb21  0f b6 c8                           movzx      ecx, al
0046bb24  85 c9                              test       ecx, ecx
0046bb26  75 2a                              jne        0x46bb52
0046bb28  8d 4d f0                           lea        ecx, [ebp - 0x10]
0046bb2b  e8 20 f7 ff ff                     call       0x46b250
0046bb30  89 45 d0                           mov        dword ptr [ebp - 0x30], eax
0046bb33  8b 55 d0                           mov        edx, dword ptr [ebp - 0x30]
0046bb36  8b 82 30 01 00 00                  mov        eax, dword ptr [edx + 0x130]
0046bb3c  3b 43 08                           cmp        eax, dword ptr [ebx + 8]
0046bb3f  74 02                              je         0x46bb43
0046bb41  eb c4                              jmp        0x46bb07
0046bb43  8b 4d d0                           mov        ecx, dword ptr [ebp - 0x30]
0046bb46  51                                 push       ecx
0046bb47  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
0046bb4a  e8 b1 17 00 00                     call       0x46d300
0046bb4f  90                                 nop        
0046bb50  eb b5                              jmp        0x46bb07
0046bb52  8b 53 08                           mov        edx, dword ptr [ebx + 8]
0046bb55  52                                 push       edx
0046bb56  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
0046bb59  e8 32 00 00 00                     call       0x46bb90
0046bb5e  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
0046bb64  e8 57 9a fd ff                     call       0x4455c0
0046bb69  6a 02                              push       2
0046bb6b  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0046bb70  e8 6b 21 fb ff                     call       0x41dce0
0046bb75  b8 01 00 00 00                     mov        eax, 1
0046bb7a  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046bb7d  33 cd                              xor        ecx, ebp
0046bb7f  e8 6a 6e 0d 00                     call       0x5429ee
0046bb84  8b e5                              mov        esp, ebp
0046bb86  5d                                 pop        ebp
0046bb87  8b e3                              mov        esp, ebx
0046bb89  5b                                 pop        ebx
0046bb8a  c2 04 00                           ret        4
