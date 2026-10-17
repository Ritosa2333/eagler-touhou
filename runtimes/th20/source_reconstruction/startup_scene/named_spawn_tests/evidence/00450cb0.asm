00450cb0  55                                 push       ebp
00450cb1  8b ec                              mov        ebp, esp
00450cb3  6a ff                              push       -1
00450cb5  68 2d 87 56 00                     push       0x56872d
00450cba  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
00450cc0  50                                 push       eax
00450cc1  83 ec 40                           sub        esp, 0x40
00450cc4  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
00450cc9  33 c5                              xor        eax, ebp
00450ccb  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
00450cce  50                                 push       eax
00450ccf  8d 45 f4                           lea        eax, [ebp - 0xc]
00450cd2  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
00450cd8  89 4d d4                           mov        dword ptr [ebp - 0x2c], ecx
00450cdb  6a 09                              push       9
00450cdd  b9 40 02 5c 00                     mov        ecx, 0x5c0240
00450ce2  e8 29 b6 fb ff                     call       0x40c310
00450ce7  50                                 push       eax
00450ce8  8d 4d ec                           lea        ecx, [ebp - 0x14]
00450ceb  e8 40 aa fb ff                     call       0x40b730
00450cf0  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
00450cf7  8b 45 d4                           mov        eax, dword ptr [ebp - 0x2c]
00450cfa  8b 48 68                           mov        ecx, dword ptr [eax + 0x68]
00450cfd  83 c1 01                           add        ecx, 1
00450d00  8b 55 d4                           mov        edx, dword ptr [ebp - 0x2c]
00450d03  89 4a 68                           mov        dword ptr [edx + 0x68], ecx
00450d06  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
00450d0c  e8 9f bc ff ff                     call       0x44c9b0
00450d11  89 45 dc                           mov        dword ptr [ebp - 0x24], eax
00450d14  83 7d 24 00                        cmp        dword ptr [ebp + 0x24], 0
00450d18  74 08                              je         0x450d22
00450d1a  8b 45 24                           mov        eax, dword ptr [ebp + 0x24]
00450d1d  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
00450d20  89 08                              mov        dword ptr [eax], ecx
00450d22  83 7d 0c 00                        cmp        dword ptr [ebp + 0xc], 0
00450d26  74 27                              je         0x450d4f
00450d28  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
00450d2b  52                                 push       edx
00450d2c  8b 45 d4                           mov        eax, dword ptr [ebp - 0x2c]
00450d2f  83 c0 20                           add        eax, 0x20
00450d32  50                                 push       eax
00450d33  e8 a8 60 ff ff                     call       0x446de0
00450d38  83 c4 08                           add        esp, 8
00450d3b  0f b6 c8                           movzx      ecx, al
00450d3e  85 c9                              test       ecx, ecx
00450d40  75 0d                              jne        0x450d4f
00450d42  68 84 e5 56 00                     push       0x56e584
00450d47  e8 64 b9 fb ff                     call       0x40c6b0
00450d4c  83 c4 04                           add        esp, 4
00450d4f  6a 00                              push       0
00450d51  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
00450d57  e8 64 bb ff ff                     call       0x44c8c0
00450d5c  89 45 d8                           mov        dword ptr [ebp - 0x28], eax
00450d5f  c7 45 d0 00 00 00 00               mov        dword ptr [ebp - 0x30], 0
00450d66  8b 55 20                           mov        edx, dword ptr [ebp + 0x20]
00450d69  83 e2 10                           and        edx, 0x10
00450d6c  75 0f                              jne        0x450d7d
00450d6e  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
00450d74  e8 27 bb ff ff                     call       0x44c8a0
00450d79  85 c0                              test       eax, eax
00450d7b  74 17                              je         0x450d94
00450d7d  c7 45 d0 01 00 00 00               mov        dword ptr [ebp - 0x30], 1
00450d84  6a 01                              push       1
00450d86  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
00450d8c  e8 2f bb ff ff                     call       0x44c8c0
00450d91  89 45 d8                           mov        dword ptr [ebp - 0x28], eax
00450d94  8b 45 10                           mov        eax, dword ptr [ebp + 0x10]
00450d97  50                                 push       eax
00450d98  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
00450d9b  51                                 push       ecx
00450d9c  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
00450d9f  e8 cc 7d fe ff                     call       0x438b70
00450da4  8b 55 dc                           mov        edx, dword ptr [ebp - 0x24]
00450da7  8b 45 d8                           mov        eax, dword ptr [ebp - 0x28]
00450daa  89 82 e8 04 00 00                  mov        dword ptr [edx + 0x4e8], eax
00450db0  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
00450db3  8b 91 9c 04 00 00                  mov        edx, dword ptr [ecx + 0x49c]
00450db9  81 ca 00 02 00 00                  or         edx, 0x200
00450dbf  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
00450dc2  89 90 9c 04 00 00                  mov        dword ptr [eax + 0x49c], edx
00450dc8  83 7d 1c 00                        cmp        dword ptr [ebp + 0x1c], 0
00450dcc  7c 27                              jl         0x450df5
00450dce  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
00450dd1  8b 55 1c                           mov        edx, dword ptr [ebp + 0x1c]
00450dd4  89 51 14                           mov        dword ptr [ecx + 0x14], edx
00450dd7  83 7d 1c 17                        cmp        dword ptr [ebp + 0x1c], 0x17
00450ddb  7f 18                              jg         0x450df5
00450ddd  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
00450de0  8a 88 a3 04 00 00                  mov        cl, byte ptr [eax + 0x4a3]
00450de6  80 e1 fc                           and        cl, 0xfc
00450de9  80 c9 01                           or         cl, 1
00450dec  8b 55 dc                           mov        edx, dword ptr [ebp - 0x24]
00450def  88 8a a3 04 00 00                  mov        byte ptr [edx + 0x4a3], cl
00450df5  83 7d 14 00                        cmp        dword ptr [ebp + 0x14], 0
00450df9  75 0c                              jne        0x450e07
00450dfb  8b 45 20                           mov        eax, dword ptr [ebp + 0x20]
00450dfe  83 e0 08                           and        eax, 8
00450e01  0f 84 9b 00 00 00                  je         0x450ea2
00450e07  8b 4d 20                           mov        ecx, dword ptr [ebp + 0x20]
00450e0a  83 e1 01                           and        ecx, 1
00450e0d  74 70                              je         0x450e7f
00450e0f  83 7d 14 00                        cmp        dword ptr [ebp + 0x14], 0
00450e13  74 68                              je         0x450e7d
00450e15  f3 0f 10 05 ac cd 56 00            movss      xmm0, dword ptr [0x56cdac]
00450e1d  f3 0f 5c 05 a4 cd 56 00            subss      xmm0, dword ptr [0x56cda4]
00450e25  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
00450e2d  f3 0f 10 0d a4 cd 56 00            movss      xmm1, dword ptr [0x56cda4]
00450e35  f3 0f 5e 0d d0 c8 56 00            divss      xmm1, dword ptr [0x56c8d0]
00450e3d  f3 0f 58 c1                        addss      xmm0, xmm1
00450e41  8b 55 14                           mov        edx, dword ptr [ebp + 0x14]
00450e44  f3 0f 58 02                        addss      xmm0, dword ptr [edx]
00450e48  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
00450e4b  f3 0f 11 80 bc 05 00 00            movss      dword ptr [eax + 0x5bc], xmm0
00450e53  8b 4d 14                           mov        ecx, dword ptr [ebp + 0x14]
00450e56  f3 0f 10 41 04                     movss      xmm0, dword ptr [ecx + 4]
00450e5b  f3 0f 58 05 90 cd 56 00            addss      xmm0, dword ptr [0x56cd90]
00450e63  8b 55 dc                           mov        edx, dword ptr [ebp - 0x24]
00450e66  f3 0f 11 82 c0 05 00 00            movss      dword ptr [edx + 0x5c0], xmm0
00450e6e  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
00450e71  8b 4d 14                           mov        ecx, dword ptr [ebp + 0x14]
00450e74  8b 51 08                           mov        edx, dword ptr [ecx + 8]
00450e77  89 90 c4 05 00 00                  mov        dword ptr [eax + 0x5c4], edx
00450e7d  eb 21                              jmp        0x450ea0
00450e7f  83 7d 14 00                        cmp        dword ptr [ebp + 0x14], 0
00450e83  74 1b                              je         0x450ea0
00450e85  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
00450e88  05 bc 05 00 00                     add        eax, 0x5bc
00450e8d  8b 4d 14                           mov        ecx, dword ptr [ebp + 0x14]
00450e90  8b 11                              mov        edx, dword ptr [ecx]
00450e92  89 10                              mov        dword ptr [eax], edx
00450e94  8b 51 04                           mov        edx, dword ptr [ecx + 4]
00450e97  89 50 04                           mov        dword ptr [eax + 4], edx
00450e9a  8b 49 08                           mov        ecx, dword ptr [ecx + 8]
00450e9d  89 48 08                           mov        dword ptr [eax + 8], ecx
00450ea0  eb 5b                              jmp        0x450efd
00450ea2  51                                 push       ecx
00450ea3  0f 57 c0                           xorps      xmm0, xmm0
00450ea6  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
00450eab  51                                 push       ecx
00450eac  0f 57 c0                           xorps      xmm0, xmm0
00450eaf  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
00450eb4  51                                 push       ecx
00450eb5  0f 57 c0                           xorps      xmm0, xmm0
00450eb8  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
00450ebd  8d 4d b4                           lea        ecx, [ebp - 0x4c]
00450ec0  e8 0b 1f fd ff                     call       0x422dd0
00450ec5  8b 55 dc                           mov        edx, dword ptr [ebp - 0x24]
00450ec8  81 c2 bc 05 00 00                  add        edx, 0x5bc
00450ece  8b 08                              mov        ecx, dword ptr [eax]
00450ed0  89 0a                              mov        dword ptr [edx], ecx
00450ed2  8b 48 04                           mov        ecx, dword ptr [eax + 4]
00450ed5  89 4a 04                           mov        dword ptr [edx + 4], ecx
00450ed8  8b 40 08                           mov        eax, dword ptr [eax + 8]
00450edb  89 42 08                           mov        dword ptr [edx + 8], eax
00450ede  83 7d 14 00                        cmp        dword ptr [ebp + 0x14], 0
00450ee2  74 19                              je         0x450efd
00450ee4  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
00450ee7  83 c1 2c                           add        ecx, 0x2c
00450eea  8b 55 14                           mov        edx, dword ptr [ebp + 0x14]
00450eed  8b 02                              mov        eax, dword ptr [edx]
00450eef  89 01                              mov        dword ptr [ecx], eax
00450ef1  8b 42 04                           mov        eax, dword ptr [edx + 4]
00450ef4  89 41 04                           mov        dword ptr [ecx + 4], eax
00450ef7  8b 52 08                           mov        edx, dword ptr [edx + 8]
00450efa  89 51 08                           mov        dword ptr [ecx + 8], edx
00450efd  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
00450f00  f3 0f 10 45 18                     movss      xmm0, dword ptr [ebp + 0x18]
00450f05  f3 0f 11 40 40                     movss      dword ptr [eax + 0x40], xmm0
00450f0a  8b 4d 20                           mov        ecx, dword ptr [ebp + 0x20]
00450f0d  83 e1 04                           and        ecx, 4
00450f10  74 19                              je         0x450f2b
00450f12  8b 55 dc                           mov        edx, dword ptr [ebp - 0x24]
00450f15  8b 82 9c 04 00 00                  mov        eax, dword ptr [edx + 0x49c]
00450f1b  0d 00 00 04 00                     or         eax, 0x40000
00450f20  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
00450f23  89 81 9c 04 00 00                  mov        dword ptr [ecx + 0x49c], eax
00450f29  eb 17                              jmp        0x450f42
00450f2b  8b 55 dc                           mov        edx, dword ptr [ebp - 0x24]
00450f2e  8b 82 9c 04 00 00                  mov        eax, dword ptr [edx + 0x49c]
00450f34  25 ff ff fb ff                     and        eax, 0xfffbffff
00450f39  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
00450f3c  89 81 9c 04 00 00                  mov        dword ptr [ecx + 0x49c], eax
00450f42  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
00450f45  e8 86 a6 fd ff                     call       0x42b5d0
00450f4a  8d 4d e0                           lea        ecx, [ebp - 0x20]
00450f4d  e8 be 1e fd ff                     call       0x422e10
00450f52  8d 55 e0                           lea        edx, [ebp - 0x20]
00450f55  52                                 push       edx
00450f56  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
00450f59  e8 82 67 fe ff                     call       0x4376e0
00450f5e  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00450f61  e8 5a 4d fd ff                     call       0x425cc0
00450f66  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
00450f69  8b 4d 20                           mov        ecx, dword ptr [ebp + 0x20]
00450f6c  89 88 74 05 00 00                  mov        dword ptr [eax + 0x574], ecx
00450f72  8b 55 20                           mov        edx, dword ptr [ebp + 0x20]
00450f75  83 e2 04                           and        edx, 4
00450f78  74 35                              je         0x450faf
00450f7a  8b 45 20                           mov        eax, dword ptr [ebp + 0x20]
00450f7d  83 e0 02                           and        eax, 2
00450f80  74 2d                              je         0x450faf
00450f82  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
00450f85  51                                 push       ecx
00450f86  8d 55 cc                           lea        edx, [ebp - 0x34]
00450f89  52                                 push       edx
00450f8a  8b 4d d8                           mov        ecx, dword ptr [ebp - 0x28]
00450f8d  e8 fe a8 ff ff                     call       0x44b890
00450f92  8b 00                              mov        eax, dword ptr [eax]
00450f94  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00450f97  89 01                              mov        dword ptr [ecx], eax
00450f99  8b 55 dc                           mov        edx, dword ptr [ebp - 0x24]
00450f9c  8a 82 a0 04 00 00                  mov        al, byte ptr [edx + 0x4a0]
00450fa2  24 fc                              and        al, 0xfc
00450fa4  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
00450fa7  88 81 a0 04 00 00                  mov        byte ptr [ecx + 0x4a0], al
00450fad  eb 6e                              jmp        0x45101d
00450faf  8b 55 20                           mov        edx, dword ptr [ebp + 0x20]
00450fb2  83 e2 04                           and        edx, 4
00450fb5  74 2e                              je         0x450fe5
00450fb7  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
00450fba  50                                 push       eax
00450fbb  8d 4d c8                           lea        ecx, [ebp - 0x38]
00450fbe  51                                 push       ecx
00450fbf  8b 4d d8                           mov        ecx, dword ptr [ebp - 0x28]
00450fc2  e8 19 a9 ff ff                     call       0x44b8e0
00450fc7  8b 10                              mov        edx, dword ptr [eax]
00450fc9  8b 45 08                           mov        eax, dword ptr [ebp + 8]
00450fcc  89 10                              mov        dword ptr [eax], edx
00450fce  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
00450fd1  8a 91 a0 04 00 00                  mov        dl, byte ptr [ecx + 0x4a0]
00450fd7  80 e2 fc                           and        dl, 0xfc
00450fda  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
00450fdd  88 90 a0 04 00 00                  mov        byte ptr [eax + 0x4a0], dl
00450fe3  eb 38                              jmp        0x45101d
00450fe5  8b 4d 20                           mov        ecx, dword ptr [ebp + 0x20]
00450fe8  83 e1 02                           and        ecx, 2
00450feb  74 19                              je         0x451006
00450fed  8b 55 dc                           mov        edx, dword ptr [ebp - 0x24]
00450ff0  52                                 push       edx
00450ff1  8d 45 c4                           lea        eax, [ebp - 0x3c]
00450ff4  50                                 push       eax
00450ff5  8b 4d d8                           mov        ecx, dword ptr [ebp - 0x28]
00450ff8  e8 43 a8 ff ff                     call       0x44b840
00450ffd  8b 08                              mov        ecx, dword ptr [eax]
00450fff  8b 55 08                           mov        edx, dword ptr [ebp + 8]
00451002  89 0a                              mov        dword ptr [edx], ecx
00451004  eb 17                              jmp        0x45101d
00451006  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
00451009  50                                 push       eax
0045100a  8d 4d c0                           lea        ecx, [ebp - 0x40]
0045100d  51                                 push       ecx
0045100e  8b 4d d8                           mov        ecx, dword ptr [ebp - 0x28]
00451011  e8 da a7 ff ff                     call       0x44b7f0
00451016  8b 10                              mov        edx, dword ptr [eax]
00451018  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0045101b  89 10                              mov        dword ptr [eax], edx
0045101d  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
00451024  8d 4d ec                           lea        ecx, [ebp - 0x14]
00451027  e8 d4 a8 fb ff                     call       0x40b900
0045102c  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0045102f  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
00451032  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
00451039  59                                 pop        ecx
0045103a  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0045103d  33 cd                              xor        ecx, ebp
0045103f  e8 aa 19 0f 00                     call       0x5429ee
00451044  8b e5                              mov        esp, ebp
00451046  5d                                 pop        ebp
00451047  c2 20 00                           ret        0x20
