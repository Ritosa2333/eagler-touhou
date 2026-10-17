0046b6e0  55                                 push       ebp
0046b6e1  8b ec                              mov        ebp, esp
0046b6e3  6a ff                              push       -1
0046b6e5  68 8d 8d 56 00                     push       0x568d8d
0046b6ea  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0046b6f0  50                                 push       eax
0046b6f1  83 ec 44                           sub        esp, 0x44
0046b6f4  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0046b6f9  33 c5                              xor        eax, ebp
0046b6fb  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0046b6fe  50                                 push       eax
0046b6ff  8d 45 f4                           lea        eax, [ebp - 0xc]
0046b702  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0046b708  89 4d c4                           mov        dword ptr [ebp - 0x3c], ecx
0046b70b  8b 4d c4                           mov        ecx, dword ptr [ebp - 0x3c]
0046b70e  e8 dd 00 00 00                     call       0x46b7f0
0046b713  8b 4d c4                           mov        ecx, dword ptr [ebp - 0x3c]
0046b716  e8 c5 2e fa ff                     call       0x40e5e0
0046b71b  90                                 nop        
0046b71c  8b 4d c4                           mov        ecx, dword ptr [ebp - 0x3c]
0046b71f  81 c1 34 a2 01 00                  add        ecx, 0x1a234
0046b725  e8 66 48 fa ff                     call       0x40ff90
0046b72a  85 c0                              test       eax, eax
0046b72c  74 7d                              je         0x46b7ab
0046b72e  8d 4d c8                           lea        ecx, [ebp - 0x38]
0046b731  e8 1a 8a fa ff                     call       0x414150
0046b736  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0046b73d  8b 4d c4                           mov        ecx, dword ptr [ebp - 0x3c]
0046b740  81 c1 34 a2 01 00                  add        ecx, 0x1a234
0046b746  e8 b5 4e 00 00                     call       0x470600
0046b74b  50                                 push       eax
0046b74c  8d 4d c8                           lea        ecx, [ebp - 0x38]
0046b74f  e8 ec 8d fa ff                     call       0x414540
0046b754  90                                 nop        
0046b755  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046b758  05 34 a2 01 00                     add        eax, 0x1a234
0046b75d  89 45 b8                           mov        dword ptr [ebp - 0x48], eax
0046b760  8b 4d c4                           mov        ecx, dword ptr [ebp - 0x3c]
0046b763  81 c1 34 a2 01 00                  add        ecx, 0x1a234
0046b769  89 4d c0                           mov        dword ptr [ebp - 0x40], ecx
0046b76c  8d 55 b4                           lea        edx, [ebp - 0x4c]
0046b76f  52                                 push       edx
0046b770  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
0046b773  e8 f8 4a 00 00                     call       0x470270
0046b778  8b 00                              mov        eax, dword ptr [eax]
0046b77a  89 45 bc                           mov        dword ptr [ebp - 0x44], eax
0046b77d  8b 4d bc                           mov        ecx, dword ptr [ebp - 0x44]
0046b780  51                                 push       ecx
0046b781  8d 55 b0                           lea        edx, [ebp - 0x50]
0046b784  52                                 push       edx
0046b785  8b 4d b8                           mov        ecx, dword ptr [ebp - 0x48]
0046b788  e8 13 4e 00 00                     call       0x4705a0
0046b78d  90                                 nop        
0046b78e  8d 4d c8                           lea        ecx, [ebp - 0x38]
0046b791  e8 aa 8f fa ff                     call       0x414740
0046b796  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0046b79d  8d 4d c8                           lea        ecx, [ebp - 0x38]
0046b7a0  e8 5b 8c fa ff                     call       0x414400
0046b7a5  90                                 nop        
0046b7a6  e9 71 ff ff ff                     jmp        0x46b71c
0046b7ab  e8 90 a9 fa ff                     call       0x416140
0046b7b0  8b 45 c4                           mov        eax, dword ptr [ebp - 0x3c]
0046b7b3  8b 88 40 a2 01 00                  mov        ecx, dword ptr [eax + 0x1a240]
0046b7b9  83 c1 01                           add        ecx, 1
0046b7bc  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046b7bf  89 8a 40 a2 01 00                  mov        dword ptr [edx + 0x1a240], ecx
0046b7c5  b8 01 00 00 00                     mov        eax, 1
0046b7ca  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0046b7cd  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0046b7d4  59                                 pop        ecx
0046b7d5  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046b7d8  33 cd                              xor        ecx, ebp
0046b7da  e8 0f 72 0d 00                     call       0x5429ee
0046b7df  8b e5                              mov        esp, ebp
0046b7e1  5d                                 pop        ebp
0046b7e2  c3                                 ret        
