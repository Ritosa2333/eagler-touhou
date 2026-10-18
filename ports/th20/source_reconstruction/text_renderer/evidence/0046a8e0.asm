0046a8e0  55                                 push       ebp
0046a8e1  8b ec                              mov        ebp, esp
0046a8e3  6a ff                              push       -1
0046a8e5  68 a0 79 56 00                     push       0x5679a0
0046a8ea  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0046a8f0  50                                 push       eax
0046a8f1  83 ec 08                           sub        esp, 8
0046a8f4  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0046a8f9  33 c5                              xor        eax, ebp
0046a8fb  50                                 push       eax
0046a8fc  8d 45 f4                           lea        eax, [ebp - 0xc]
0046a8ff  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0046a905  89 4d f0                           mov        dword ptr [ebp - 0x10], ecx
0046a908  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046a90b  e8 10 54 fb ff                     call       0x41fd20
0046a910  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0046a913  c7 00 c0 f5 56 00                  mov        dword ptr [eax], 0x56f5c0
0046a919  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046a91c  83 c1 10                           add        ecx, 0x10
0046a91f  e8 1c e2 fd ff                     call       0x448b40
0046a924  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046a927  81 c1 f4 05 00 00                  add        ecx, 0x5f4
0046a92d  e8 0e e2 fd ff                     call       0x448b40
0046a932  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046a935  81 c1 d8 0b 00 00                  add        ecx, 0xbd8
0046a93b  e8 00 e2 fd ff                     call       0x448b40
0046a940  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046a943  81 c1 bc 11 00 00                  add        ecx, 0x11bc
0046a949  e8 62 fd ff ff                     call       0x46a6b0
0046a94e  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046a951  c7 81 bc a1 01 00 00 00 00 00      mov        dword ptr [ecx + 0x1a1bc], 0
0046a95b  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0046a95e  c7 82 c0 a1 01 00 ff ff ff ff      mov        dword ptr [edx + 0x1a1c0], 0xffffffff
0046a968  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0046a96b  c7 80 c4 a1 01 00 00 00 00 ff      mov        dword ptr [eax + 0x1a1c4], 0xff000000
0046a975  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046a978  c7 81 c8 a1 01 00 ff ff ff ff      mov        dword ptr [ecx + 0x1a1c8], 0xffffffff
0046a982  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0046a985  f3 0f 10 05 cc c8 56 00            movss      xmm0, dword ptr [0x56c8cc]
0046a98d  f3 0f 11 82 cc a1 01 00            movss      dword ptr [edx + 0x1a1cc], xmm0
0046a995  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0046a998  f3 0f 10 05 cc c8 56 00            movss      xmm0, dword ptr [0x56c8cc]
0046a9a0  f3 0f 11 80 d0 a1 01 00            movss      dword ptr [eax + 0x1a1d0], xmm0
0046a9a8  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046a9ab  c7 81 d4 a1 01 00 00 00 00 00      mov        dword ptr [ecx + 0x1a1d4], 0
0046a9b5  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0046a9b8  c7 82 d8 a1 01 00 00 00 00 00      mov        dword ptr [edx + 0x1a1d8], 0
0046a9c2  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0046a9c5  c7 80 dc a1 01 00 00 00 00 00      mov        dword ptr [eax + 0x1a1dc], 0
0046a9cf  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046a9d2  c7 81 e0 a1 01 00 00 00 00 00      mov        dword ptr [ecx + 0x1a1e0], 0
0046a9dc  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0046a9df  c7 82 e4 a1 01 00 02 00 00 00      mov        dword ptr [edx + 0x1a1e4], 2
0046a9e9  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0046a9ec  c7 80 e8 a1 01 00 00 00 00 00      mov        dword ptr [eax + 0x1a1e8], 0
0046a9f6  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046a9f9  c7 81 ec a1 01 00 00 00 00 00      mov        dword ptr [ecx + 0x1a1ec], 0
0046aa03  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0046aa06  c7 82 f0 a1 01 00 00 00 00 00      mov        dword ptr [edx + 0x1a1f0], 0
0046aa10  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0046aa13  c7 80 f4 a1 01 00 01 00 00 00      mov        dword ptr [eax + 0x1a1f4], 1
0046aa1d  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046aa20  c7 81 f8 a1 01 00 01 00 00 00      mov        dword ptr [ecx + 0x1a1f8], 1
0046aa2a  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0046aa2d  c7 82 fc a1 01 00 00 00 00 00      mov        dword ptr [edx + 0x1a1fc], 0
0046aa37  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046aa3a  81 c1 00 a2 01 00                  add        ecx, 0x1a200
0046aa40  e8 eb 95 fa ff                     call       0x414030
0046aa45  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0046aa48  05 08 a2 01 00                     add        eax, 0x1a208
0046aa4d  89 45 ec                           mov        dword ptr [ebp - 0x14], eax
0046aa50  51                                 push       ecx
0046aa51  0f 57 c0                           xorps      xmm0, xmm0
0046aa54  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
0046aa59  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
0046aa5c  e8 af e7 fb ff                     call       0x429210
0046aa61  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046aa64  81 c1 0c a2 01 00                  add        ecx, 0x1a20c
0046aa6a  e8 e1 df fa ff                     call       0x418a50
0046aa6f  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046aa72  81 c1 24 a2 01 00                  add        ecx, 0x1a224
0046aa78  e8 03 0d fa ff                     call       0x40b780
0046aa7d  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046aa80  81 c1 34 a2 01 00                  add        ecx, 0x1a234
0046aa86  e8 25 fe ff ff                     call       0x46a8b0
0046aa8b  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046aa8e  c7 81 3c a2 01 00 09 00 00 00      mov        dword ptr [ecx + 0x1a23c], 9
0046aa98  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0046aa9b  c7 82 40 a2 01 00 00 00 00 00      mov        dword ptr [edx + 0x1a240], 0
0046aaa5  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0046aaa8  c7 80 44 a2 01 00 00 00 00 00      mov        dword ptr [eax + 0x1a244], 0
0046aab2  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046aab5  81 c1 48 a2 01 00                  add        ecx, 0x1a248
0046aabb  e8 00 b2 fb ff                     call       0x425cc0
0046aac0  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046aac3  81 c1 4c a2 01 00                  add        ecx, 0x1a24c
0046aac9  e8 f2 b1 fb ff                     call       0x425cc0
0046aace  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046aad1  c7 81 50 a2 01 00 00 00 00 00      mov        dword ptr [ecx + 0x1a250], 0
0046aadb  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0046aade  c7 82 54 a2 01 00 00 00 00 00      mov        dword ptr [edx + 0x1a254], 0
0046aae8  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0046aaeb  c7 80 58 a2 01 00 00 00 00 00      mov        dword ptr [eax + 0x1a258], 0
0046aaf5  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046aaf8  c7 81 5c a2 01 00 00 00 00 00      mov        dword ptr [ecx + 0x1a25c], 0
0046ab02  68 00 01 00 00                     push       0x100
0046ab07  6a 00                              push       0
0046ab09  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0046ab0c  81 c2 60 a2 01 00                  add        edx, 0x1a260
0046ab12  52                                 push       edx
0046ab13  e8 48 9c 0d 00                     call       0x544760
0046ab18  83 c4 0c                           add        esp, 0xc
0046ab1b  68 cc f5 56 00                     push       0x56f5cc ; string='initialize AsciiInf'
0046ab20  e8 8b 1b fa ff                     call       0x40c6b0
0046ab25  83 c4 04                           add        esp, 4
0046ab28  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0046ab2b  a3 98 06 5c 00                     mov        dword ptr [0x5c0698], eax
0046ab30  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046ab33  c7 81 c0 a1 01 00 ff ff ff ff      mov        dword ptr [ecx + 0x1a1c0], 0xffffffff
0046ab3d  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0046ab40  c7 82 c4 a1 01 00 00 00 00 ff      mov        dword ptr [edx + 0x1a1c4], 0xff000000
0046ab4a  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0046ab4d  f3 0f 10 05 cc c8 56 00            movss      xmm0, dword ptr [0x56c8cc]
0046ab55  f3 0f 11 80 cc a1 01 00            movss      dword ptr [eax + 0x1a1cc], xmm0
0046ab5d  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046ab60  f3 0f 10 05 cc c8 56 00            movss      xmm0, dword ptr [0x56c8cc]
0046ab68  f3 0f 11 81 d0 a1 01 00            movss      dword ptr [ecx + 0x1a1d0], xmm0
0046ab70  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0046ab73  c7 82 d8 a1 01 00 00 00 00 00      mov        dword ptr [edx + 0x1a1d8], 0
0046ab7d  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0046ab80  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0046ab83  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0046ab8a  59                                 pop        ecx
0046ab8b  8b e5                              mov        esp, ebp
0046ab8d  5d                                 pop        ebp
0046ab8e  c3                                 ret        
