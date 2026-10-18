004dd2e0  55                                 push       ebp
004dd2e1  8b ec                              mov        ebp, esp
004dd2e3  83 ec 2c                           sub        esp, 0x2c
004dd2e6  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
004dd2eb  33 c5                              xor        eax, ebp
004dd2ed  89 45 fc                           mov        dword ptr [ebp - 4], eax
004dd2f0  83 3d dc 4e 5c 00 00               cmp        dword ptr [0x5c4edc], 0
004dd2f7  0f 84 e3 00 00 00                  je         0x4dd3e0
004dd2fd  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004dd303  e8 b8 82 f6 ff                     call       0x4455c0
004dd308  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004dd30d  e8 1e 54 f3 ff                     call       0x412730
004dd312  89 45 e8                           mov        dword ptr [ebp - 0x18], eax
004dd315  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
004dd318  8b 08                              mov        ecx, dword ptr [eax]
004dd31a  8b 91 94 00 00 00                  mov        edx, dword ptr [ecx + 0x94]
004dd320  89 55 dc                           mov        dword ptr [ebp - 0x24], edx
004dd323  a1 e0 4e 5c 00                     mov        eax, dword ptr [0x5c4ee0]
004dd328  89 45 e0                           mov        dword ptr [ebp - 0x20], eax
004dd32b  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
004dd32e  51                                 push       ecx
004dd32f  6a 00                              push       0
004dd331  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
004dd334  52                                 push       edx
004dd335  ff 55 dc                           call       dword ptr [ebp - 0x24]
004dd338  b8 6c 01 00 00                     mov        eax, 0x16c
004dd33d  c1 e0 00                           shl        eax, 0
004dd340  8b 88 98 50 5c 00                  mov        ecx, dword ptr [eax + 0x5c5098]
004dd346  89 4d ec                           mov        dword ptr [ebp - 0x14], ecx
004dd349  ba 6c 01 00 00                     mov        edx, 0x16c
004dd34e  c1 e2 00                           shl        edx, 0
004dd351  8b 82 9c 50 5c 00                  mov        eax, dword ptr [edx + 0x5c509c]
004dd357  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
004dd35a  b9 6c 01 00 00                     mov        ecx, 0x16c
004dd35f  c1 e1 00                           shl        ecx, 0
004dd362  ba 6c 01 00 00                     mov        edx, 0x16c
004dd367  c1 e2 00                           shl        edx, 0
004dd36a  8b 81 98 50 5c 00                  mov        eax, dword ptr [ecx + 0x5c5098]
004dd370  03 82 a0 50 5c 00                  add        eax, dword ptr [edx + 0x5c50a0]
004dd376  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
004dd379  b9 6c 01 00 00                     mov        ecx, 0x16c
004dd37e  c1 e1 00                           shl        ecx, 0
004dd381  ba 6c 01 00 00                     mov        edx, 0x16c
004dd386  c1 e2 00                           shl        edx, 0
004dd389  8b 81 9c 50 5c 00                  mov        eax, dword ptr [ecx + 0x5c509c]
004dd38f  03 82 a4 50 5c 00                  add        eax, dword ptr [edx + 0x5c50a4]
004dd395  89 45 f8                           mov        dword ptr [ebp - 8], eax
004dd398  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004dd39d  e8 8e 53 f3 ff                     call       0x412730
004dd3a2  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
004dd3a5  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
004dd3a8  8b 11                              mov        edx, dword ptr [ecx]
004dd3aa  8b 82 ac 00 00 00                  mov        eax, dword ptr [edx + 0xac]
004dd3b0  89 45 d4                           mov        dword ptr [ebp - 0x2c], eax
004dd3b3  8b 0d 20 5b 5c 00                  mov        ecx, dword ptr [0x5c5b20]
004dd3b9  89 4d d8                           mov        dword ptr [ebp - 0x28], ecx
004dd3bc  6a 00                              push       0
004dd3be  51                                 push       ecx
004dd3bf  f3 0f 10 05 cc c8 56 00            movss      xmm0, dword ptr [0x56c8cc]
004dd3c7  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004dd3cc  8b 55 d8                           mov        edx, dword ptr [ebp - 0x28]
004dd3cf  52                                 push       edx
004dd3d0  6a 02                              push       2
004dd3d2  8d 45 ec                           lea        eax, [ebp - 0x14]
004dd3d5  50                                 push       eax
004dd3d6  6a 01                              push       1
004dd3d8  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
004dd3db  51                                 push       ecx
004dd3dc  ff 55 d4                           call       dword ptr [ebp - 0x2c]
004dd3df  90                                 nop        
004dd3e0  b8 01 00 00 00                     mov        eax, 1
004dd3e5  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dd3e8  33 cd                              xor        ecx, ebp
004dd3ea  e8 ff 55 06 00                     call       0x5429ee
004dd3ef  8b e5                              mov        esp, ebp
004dd3f1  5d                                 pop        ebp
004dd3f2  c3                                 ret        
