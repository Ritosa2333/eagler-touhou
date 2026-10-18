0041e050  55                                 push       ebp
0041e051  8b ec                              mov        ebp, esp
0041e053  83 ec 24                           sub        esp, 0x24
0041e056  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041e059  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041e05e  e8 0d ec ff ff                     call       0x41cc70
0041e063  83 f8 09                           cmp        eax, 9
0041e066  74 13                              je         0x41e07b
0041e068  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041e06d  e8 fe eb ff ff                     call       0x41cc70
0041e072  83 f8 08                           cmp        eax, 8
0041e075  0f 85 41 03 00 00                  jne        0x41e3bc
0041e07b  a1 08 88 5b 00                     mov        eax, dword ptr [0x5b8808]
0041e080  a3 00 88 5b 00                     mov        dword ptr [0x5b8800], eax
0041e085  8b 0d 0c 88 5b 00                  mov        ecx, dword ptr [0x5b880c]
0041e08b  89 0d 04 88 5b 00                  mov        dword ptr [0x5b8804], ecx
0041e091  83 7d 08 00                        cmp        dword ptr [ebp + 8], 0
0041e095  0f 84 de 00 00 00                  je         0x41e179
0041e09b  81 3d 00 88 5b 00 00 0a 00 00      cmp        dword ptr [0x5b8800], 0xa00
0041e0a5  7c 28                              jl         0x41e0cf
0041e0a7  81 3d 04 88 5b 00 80 07 00 00      cmp        dword ptr [0x5b8804], 0x780
0041e0b1  7c 1c                              jl         0x41e0cf
0041e0b3  f3 0f 10 05 d0 c8 56 00            movss      xmm0, dword ptr [0x56c8d0]
0041e0bb  f3 0f 11 05 18 88 5b 00            movss      dword ptr [0x5b8818], xmm0
0041e0c3  c6 05 8a 4f 5c 00 04               mov        byte ptr [0x5c4f8a], 4
0041e0ca  e9 aa 00 00 00                     jmp        0x41e179
0041e0cf  81 3d 00 88 5b 00 80 07 00 00      cmp        dword ptr [0x5b8800], 0x780
0041e0d9  7c 25                              jl         0x41e100
0041e0db  81 3d 04 88 5b 00 a0 05 00 00      cmp        dword ptr [0x5b8804], 0x5a0
0041e0e5  7c 19                              jl         0x41e100
0041e0e7  f3 0f 10 05 d0 c8 56 00            movss      xmm0, dword ptr [0x56c8d0]
0041e0ef  f3 0f 11 05 18 88 5b 00            movss      dword ptr [0x5b8818], xmm0
0041e0f7  c6 05 8a 4f 5c 00 03               mov        byte ptr [0x5c4f8a], 3
0041e0fe  eb 79                              jmp        0x41e179
0041e100  81 3d 00 88 5b 00 00 05 00 00      cmp        dword ptr [0x5b8800], 0x500
0041e10a  7c 25                              jl         0x41e131
0041e10c  81 3d 04 88 5b 00 c0 03 00 00      cmp        dword ptr [0x5b8804], 0x3c0
0041e116  7c 19                              jl         0x41e131
0041e118  f3 0f 10 05 d0 c8 56 00            movss      xmm0, dword ptr [0x56c8d0]
0041e120  f3 0f 11 05 18 88 5b 00            movss      dword ptr [0x5b8818], xmm0
0041e128  c6 05 8a 4f 5c 00 02               mov        byte ptr [0x5c4f8a], 2
0041e12f  eb 48                              jmp        0x41e179
0041e131  81 3d 00 88 5b 00 c0 03 00 00      cmp        dword ptr [0x5b8800], 0x3c0
0041e13b  7c 25                              jl         0x41e162
0041e13d  81 3d 04 88 5b 00 d0 02 00 00      cmp        dword ptr [0x5b8804], 0x2d0
0041e147  7c 19                              jl         0x41e162
0041e149  f3 0f 10 05 68 cd 56 00            movss      xmm0, dword ptr [0x56cd68]
0041e151  f3 0f 11 05 18 88 5b 00            movss      dword ptr [0x5b8818], xmm0
0041e159  c6 05 8a 4f 5c 00 01               mov        byte ptr [0x5c4f8a], 1
0041e160  eb 17                              jmp        0x41e179
0041e162  c6 05 8a 4f 5c 00 00               mov        byte ptr [0x5c4f8a], 0
0041e169  f3 0f 10 05 cc c8 56 00            movss      xmm0, dword ptr [0x56c8cc]
0041e171  f3 0f 11 05 18 88 5b 00            movss      dword ptr [0x5b8818], xmm0
0041e179  f3 0f 10 05 ac cd 56 00            movss      xmm0, dword ptr [0x56cdac]
0041e181  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0041e189  f3 0f 2c d0                        cvttss2si  edx, xmm0
0041e18d  89 15 f8 87 5b 00                  mov        dword ptr [0x5b87f8], edx
0041e193  f3 0f 10 05 a8 cd 56 00            movss      xmm0, dword ptr [0x56cda8]
0041e19b  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0041e1a3  f3 0f 2c c0                        cvttss2si  eax, xmm0
0041e1a7  a3 fc 87 5b 00                     mov        dword ptr [0x5b87fc], eax
0041e1ac  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041e1b1  e8 ba ea ff ff                     call       0x41cc70
0041e1b6  83 f8 09                           cmp        eax, 9
0041e1b9  0f 85 aa 00 00 00                  jne        0x41e269
0041e1bf  f3 0f 2a 05 00 88 5b 00            cvtsi2ss   xmm0, dword ptr [0x5b8800]
0041e1c7  f3 0f 2a 0d 04 88 5b 00            cvtsi2ss   xmm1, dword ptr [0x5b8804]
0041e1cf  f3 0f 5e c1                        divss      xmm0, xmm1
0041e1d3  f3 0f 2a 0d f8 87 5b 00            cvtsi2ss   xmm1, dword ptr [0x5b87f8]
0041e1db  f3 0f 2a 15 fc 87 5b 00            cvtsi2ss   xmm2, dword ptr [0x5b87fc]
0041e1e3  f3 0f 5e ca                        divss      xmm1, xmm2
0041e1e7  0f 2f c1                           comiss     xmm0, xmm1
0041e1ea  72 3e                              jb         0x41e22a
0041e1ec  f3 0f 2a 05 00 88 5b 00            cvtsi2ss   xmm0, dword ptr [0x5b8800]
0041e1f4  f3 0f 2a 0d 04 88 5b 00            cvtsi2ss   xmm1, dword ptr [0x5b8804]
0041e1fc  f3 0f 5e c1                        divss      xmm0, xmm1
0041e200  f3 0f 11 45 f0                     movss      dword ptr [ebp - 0x10], xmm0
0041e205  f3 0f 2a 05 fc 87 5b 00            cvtsi2ss   xmm0, dword ptr [0x5b87fc]
0041e20d  f3 0f 59 45 f0                     mulss      xmm0, dword ptr [ebp - 0x10]
0041e212  f3 0f 2c c8                        cvttss2si  ecx, xmm0
0041e216  89 0d 10 88 5b 00                  mov        dword ptr [0x5b8810], ecx
0041e21c  8b 15 fc 87 5b 00                  mov        edx, dword ptr [0x5b87fc]
0041e222  89 15 14 88 5b 00                  mov        dword ptr [0x5b8814], edx
0041e228  eb 3a                              jmp        0x41e264
0041e22a  f3 0f 2a 05 04 88 5b 00            cvtsi2ss   xmm0, dword ptr [0x5b8804]
0041e232  f3 0f 2a 0d 00 88 5b 00            cvtsi2ss   xmm1, dword ptr [0x5b8800]
0041e23a  f3 0f 5e c1                        divss      xmm0, xmm1
0041e23e  f3 0f 11 45 ec                     movss      dword ptr [ebp - 0x14], xmm0
0041e243  a1 f8 87 5b 00                     mov        eax, dword ptr [0x5b87f8]
0041e248  a3 10 88 5b 00                     mov        dword ptr [0x5b8810], eax
0041e24d  f3 0f 2a 05 f8 87 5b 00            cvtsi2ss   xmm0, dword ptr [0x5b87f8]
0041e255  f3 0f 59 45 ec                     mulss      xmm0, dword ptr [ebp - 0x14]
0041e25a  f3 0f 2c c8                        cvttss2si  ecx, xmm0
0041e25e  89 0d 14 88 5b 00                  mov        dword ptr [0x5b8814], ecx
0041e264  e9 4e 01 00 00                     jmp        0x41e3b7
0041e269  f3 0f 2a 05 00 88 5b 00            cvtsi2ss   xmm0, dword ptr [0x5b8800]
0041e271  f3 0f 2a 0d 04 88 5b 00            cvtsi2ss   xmm1, dword ptr [0x5b8804]
0041e279  f3 0f 5e c1                        divss      xmm0, xmm1
0041e27d  f3 0f 2a 0d f8 87 5b 00            cvtsi2ss   xmm1, dword ptr [0x5b87f8]
0041e285  f3 0f 2a 15 fc 87 5b 00            cvtsi2ss   xmm2, dword ptr [0x5b87fc]
0041e28d  f3 0f 5e ca                        divss      xmm1, xmm2
0041e291  0f 2f c1                           comiss     xmm0, xmm1
0041e294  0f 82 91 00 00 00                  jb         0x41e32b
0041e29a  f3 0f 2a 05 00 88 5b 00            cvtsi2ss   xmm0, dword ptr [0x5b8800]
0041e2a2  f3 0f 2a 0d 04 88 5b 00            cvtsi2ss   xmm1, dword ptr [0x5b8804]
0041e2aa  f3 0f 5e c1                        divss      xmm0, xmm1
0041e2ae  f3 0f 11 45 e4                     movss      dword ptr [ebp - 0x1c], xmm0
0041e2b3  8b 15 fc 87 5b 00                  mov        edx, dword ptr [0x5b87fc]
0041e2b9  89 55 f8                           mov        dword ptr [ebp - 8], edx
0041e2bc  a1 fc 87 5b 00                     mov        eax, dword ptr [0x5b87fc]
0041e2c1  99                                 cdq        
0041e2c2  2b c2                              sub        eax, edx
0041e2c4  d1 f8                              sar        eax, 1
0041e2c6  03 45 f8                           add        eax, dword ptr [ebp - 8]
0041e2c9  3b 05 04 88 5b 00                  cmp        eax, dword ptr [0x5b8804]
0041e2cf  7d 12                              jge        0x41e2e3
0041e2d1  a1 fc 87 5b 00                     mov        eax, dword ptr [0x5b87fc]
0041e2d6  99                                 cdq        
0041e2d7  2b c2                              sub        eax, edx
0041e2d9  d1 f8                              sar        eax, 1
0041e2db  03 45 f8                           add        eax, dword ptr [ebp - 8]
0041e2de  89 45 f8                           mov        dword ptr [ebp - 8], eax
0041e2e1  eb d9                              jmp        0x41e2bc
0041e2e3  f3 0f 2a 05 04 88 5b 00            cvtsi2ss   xmm0, dword ptr [0x5b8804]
0041e2eb  f3 0f 2a 4d f8                     cvtsi2ss   xmm1, dword ptr [ebp - 8]
0041e2f0  f3 0f 5e c1                        divss      xmm0, xmm1
0041e2f4  f3 0f 11 45 e8                     movss      dword ptr [ebp - 0x18], xmm0
0041e2f9  f3 0f 2a 05 fc 87 5b 00            cvtsi2ss   xmm0, dword ptr [0x5b87fc]
0041e301  f3 0f 59 45 e8                     mulss      xmm0, dword ptr [ebp - 0x18]
0041e306  f3 0f 2c c0                        cvttss2si  eax, xmm0
0041e30a  a3 14 88 5b 00                     mov        dword ptr [0x5b8814], eax
0041e30f  f3 0f 2a 05 14 88 5b 00            cvtsi2ss   xmm0, dword ptr [0x5b8814]
0041e317  f3 0f 59 45 e4                     mulss      xmm0, dword ptr [ebp - 0x1c]
0041e31c  f3 0f 2c c8                        cvttss2si  ecx, xmm0
0041e320  89 0d 10 88 5b 00                  mov        dword ptr [0x5b8810], ecx
0041e326  e9 8c 00 00 00                     jmp        0x41e3b7
0041e32b  f3 0f 2a 05 04 88 5b 00            cvtsi2ss   xmm0, dword ptr [0x5b8804]
0041e333  f3 0f 2a 0d 00 88 5b 00            cvtsi2ss   xmm1, dword ptr [0x5b8800]
0041e33b  f3 0f 5e c1                        divss      xmm0, xmm1
0041e33f  f3 0f 11 45 dc                     movss      dword ptr [ebp - 0x24], xmm0
0041e344  8b 15 f8 87 5b 00                  mov        edx, dword ptr [0x5b87f8]
0041e34a  89 55 f4                           mov        dword ptr [ebp - 0xc], edx
0041e34d  a1 f8 87 5b 00                     mov        eax, dword ptr [0x5b87f8]
0041e352  99                                 cdq        
0041e353  2b c2                              sub        eax, edx
0041e355  d1 f8                              sar        eax, 1
0041e357  03 45 f4                           add        eax, dword ptr [ebp - 0xc]
0041e35a  3b 05 00 88 5b 00                  cmp        eax, dword ptr [0x5b8800]
0041e360  7d 12                              jge        0x41e374
0041e362  a1 f8 87 5b 00                     mov        eax, dword ptr [0x5b87f8]
0041e367  99                                 cdq        
0041e368  2b c2                              sub        eax, edx
0041e36a  d1 f8                              sar        eax, 1
0041e36c  03 45 f4                           add        eax, dword ptr [ebp - 0xc]
0041e36f  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
0041e372  eb d9                              jmp        0x41e34d
0041e374  f3 0f 2a 05 00 88 5b 00            cvtsi2ss   xmm0, dword ptr [0x5b8800]
0041e37c  f3 0f 2a 4d f4                     cvtsi2ss   xmm1, dword ptr [ebp - 0xc]
0041e381  f3 0f 5e c1                        divss      xmm0, xmm1
0041e385  f3 0f 11 45 e0                     movss      dword ptr [ebp - 0x20], xmm0
0041e38a  f3 0f 2a 05 fc 87 5b 00            cvtsi2ss   xmm0, dword ptr [0x5b87fc]
0041e392  f3 0f 59 45 e0                     mulss      xmm0, dword ptr [ebp - 0x20]
0041e397  f3 0f 2c c0                        cvttss2si  eax, xmm0
0041e39b  a3 10 88 5b 00                     mov        dword ptr [0x5b8810], eax
0041e3a0  f3 0f 2a 05 14 88 5b 00            cvtsi2ss   xmm0, dword ptr [0x5b8814]
0041e3a8  f3 0f 59 45 dc                     mulss      xmm0, dword ptr [ebp - 0x24]
0041e3ad  f3 0f 2c c8                        cvttss2si  ecx, xmm0
0041e3b1  89 0d 14 88 5b 00                  mov        dword ptr [0x5b8814], ecx
0041e3b7  e9 1c 01 00 00                     jmp        0x41e4d8
0041e3bc  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041e3c1  e8 aa e8 ff ff                     call       0x41cc70
0041e3c6  83 f8 07                           cmp        eax, 7
0041e3c9  75 29                              jne        0x41e3f4
0041e3cb  f3 0f 10 05 d0 c8 56 00            movss      xmm0, dword ptr [0x56c8d0]
0041e3d3  f3 0f 11 05 18 88 5b 00            movss      dword ptr [0x5b8818], xmm0
0041e3db  c7 05 00 88 5b 00 00 0a 00 00      mov        dword ptr [0x5b8800], 0xa00
0041e3e5  c7 05 04 88 5b 00 80 07 00 00      mov        dword ptr [0x5b8804], 0x780
0041e3ef  e9 e4 00 00 00                     jmp        0x41e4d8
0041e3f4  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041e3f9  e8 72 e8 ff ff                     call       0x41cc70
0041e3fe  83 f8 06                           cmp        eax, 6
0041e401  75 29                              jne        0x41e42c
0041e403  f3 0f 10 05 d0 c8 56 00            movss      xmm0, dword ptr [0x56c8d0]
0041e40b  f3 0f 11 05 18 88 5b 00            movss      dword ptr [0x5b8818], xmm0
0041e413  c7 05 00 88 5b 00 80 07 00 00      mov        dword ptr [0x5b8800], 0x780
0041e41d  c7 05 04 88 5b 00 a0 05 00 00      mov        dword ptr [0x5b8804], 0x5a0
0041e427  e9 ac 00 00 00                     jmp        0x41e4d8
0041e42c  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041e431  e8 3a e8 ff ff                     call       0x41cc70
0041e436  83 f8 02                           cmp        eax, 2
0041e439  74 0f                              je         0x41e44a
0041e43b  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041e440  e8 2b e8 ff ff                     call       0x41cc70
0041e445  83 f8 05                           cmp        eax, 5
0041e448  75 26                              jne        0x41e470
0041e44a  f3 0f 10 05 d0 c8 56 00            movss      xmm0, dword ptr [0x56c8d0]
0041e452  f3 0f 11 05 18 88 5b 00            movss      dword ptr [0x5b8818], xmm0
0041e45a  c7 05 00 88 5b 00 00 05 00 00      mov        dword ptr [0x5b8800], 0x500
0041e464  c7 05 04 88 5b 00 c0 03 00 00      mov        dword ptr [0x5b8804], 0x3c0
0041e46e  eb 68                              jmp        0x41e4d8
0041e470  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041e475  e8 f6 e7 ff ff                     call       0x41cc70
0041e47a  83 f8 01                           cmp        eax, 1
0041e47d  74 0f                              je         0x41e48e
0041e47f  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041e484  e8 e7 e7 ff ff                     call       0x41cc70
0041e489  83 f8 04                           cmp        eax, 4
0041e48c  75 26                              jne        0x41e4b4
0041e48e  f3 0f 10 05 68 cd 56 00            movss      xmm0, dword ptr [0x56cd68]
0041e496  f3 0f 11 05 18 88 5b 00            movss      dword ptr [0x5b8818], xmm0
0041e49e  c7 05 00 88 5b 00 c0 03 00 00      mov        dword ptr [0x5b8800], 0x3c0
0041e4a8  c7 05 04 88 5b 00 d0 02 00 00      mov        dword ptr [0x5b8804], 0x2d0
0041e4b2  eb 24                              jmp        0x41e4d8
0041e4b4  f3 0f 10 05 cc c8 56 00            movss      xmm0, dword ptr [0x56c8cc]
0041e4bc  f3 0f 11 05 18 88 5b 00            movss      dword ptr [0x5b8818], xmm0
0041e4c4  c7 05 00 88 5b 00 80 02 00 00      mov        dword ptr [0x5b8800], 0x280
0041e4ce  c7 05 04 88 5b 00 e0 01 00 00      mov        dword ptr [0x5b8804], 0x1e0
0041e4d8  f3 0f 10 05 ac cd 56 00            movss      xmm0, dword ptr [0x56cdac]
0041e4e0  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0041e4e8  f3 0f 2c d0                        cvttss2si  edx, xmm0
0041e4ec  89 15 f8 87 5b 00                  mov        dword ptr [0x5b87f8], edx
0041e4f2  f3 0f 10 05 a8 cd 56 00            movss      xmm0, dword ptr [0x56cda8]
0041e4fa  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0041e502  f3 0f 2c c0                        cvttss2si  eax, xmm0
0041e506  a3 fc 87 5b 00                     mov        dword ptr [0x5b87fc], eax
0041e50b  68 80 02 00 00                     push       0x280
0041e510  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e513  e8 88 48 ff ff                     call       0x412da0
0041e518  68 e0 01 00 00                     push       0x1e0
0041e51d  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e520  e8 2b f9 ff ff                     call       0x41de50
0041e525  68 80 01 00 00                     push       0x180
0041e52a  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e52d  e8 5e f9 ff ff                     call       0x41de90
0041e532  68 c0 01 00 00                     push       0x1c0
0041e537  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e53a  e8 31 f9 ff ff                     call       0x41de70
0041e53f  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e542  e8 b9 31 ff ff                     call       0x411700
0041e547  8b 0d f8 87 5b 00                  mov        ecx, dword ptr [0x5b87f8]
0041e54d  2b c8                              sub        ecx, eax
0041e54f  8b c1                              mov        eax, ecx
0041e551  99                                 cdq        
0041e552  2b c2                              sub        eax, edx
0041e554  d1 f8                              sar        eax, 1
0041e556  50                                 push       eax
0041e557  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e55a  e8 d1 f9 ff ff                     call       0x41df30
0041e55f  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e562  e8 29 e5 ff ff                     call       0x41ca90
0041e567  8b 15 fc 87 5b 00                  mov        edx, dword ptr [0x5b87fc]
0041e56d  2b d0                              sub        edx, eax
0041e56f  8b c2                              mov        eax, edx
0041e571  99                                 cdq        
0041e572  2b c2                              sub        eax, edx
0041e574  d1 f8                              sar        eax, 1
0041e576  50                                 push       eax
0041e577  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e57a  e8 d1 f9 ff ff                     call       0x41df50
0041e57f  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e582  e8 49 e5 ff ff                     call       0x41cad0
0041e587  83 c0 20                           add        eax, 0x20
0041e58a  50                                 push       eax
0041e58b  6a 00                              push       0
0041e58d  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e590  e8 3b f9 ff ff                     call       0x41ded0
0041e595  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e598  e8 33 e5 ff ff                     call       0x41cad0
0041e59d  83 c0 20                           add        eax, 0x20
0041e5a0  50                                 push       eax
0041e5a1  6a 01                              push       1
0041e5a3  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e5a6  e8 25 f9 ff ff                     call       0x41ded0
0041e5ab  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e5ae  e8 1d e5 ff ff                     call       0x41cad0
0041e5b3  83 c0 10                           add        eax, 0x10
0041e5b6  50                                 push       eax
0041e5b7  6a 00                              push       0
0041e5b9  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e5bc  e8 4f f9 ff ff                     call       0x41df10
0041e5c1  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e5c4  e8 07 e5 ff ff                     call       0x41cad0
0041e5c9  83 c0 10                           add        eax, 0x10
0041e5cc  50                                 push       eax
0041e5cd  6a 01                              push       1
0041e5cf  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e5d2  e8 39 f9 ff ff                     call       0x41df10
0041e5d7  f3 0f 10 05 94 cd 56 00            movss      xmm0, dword ptr [0x56cd94]
0041e5df  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0041e5e7  f3 0f 2c c0                        cvttss2si  eax, xmm0
0041e5eb  50                                 push       eax
0041e5ec  6a 00                              push       0
0041e5ee  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e5f1  e8 ba f8 ff ff                     call       0x41deb0
0041e5f6  f3 0f 10 05 94 cd 56 00            movss      xmm0, dword ptr [0x56cd94]
0041e5fe  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0041e606  f3 0f 2c c8                        cvttss2si  ecx, xmm0
0041e60a  51                                 push       ecx
0041e60b  6a 01                              push       1
0041e60d  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e610  e8 9b f8 ff ff                     call       0x41deb0
0041e615  f3 0f 10 05 90 cd 56 00            movss      xmm0, dword ptr [0x56cd90]
0041e61d  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0041e625  f3 0f 2c d0                        cvttss2si  edx, xmm0
0041e629  52                                 push       edx
0041e62a  6a 00                              push       0
0041e62c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e62f  e8 bc f8 ff ff                     call       0x41def0
0041e634  f3 0f 10 05 90 cd 56 00            movss      xmm0, dword ptr [0x56cd90]
0041e63c  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0041e644  f3 0f 2c c0                        cvttss2si  eax, xmm0
0041e648  50                                 push       eax
0041e649  6a 01                              push       1
0041e64b  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e64e  e8 9d f8 ff ff                     call       0x41def0
0041e653  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e656  e8 75 e4 ff ff                     call       0x41cad0
0041e65b  f3 0f 10 05 ac cd 56 00            movss      xmm0, dword ptr [0x56cdac]
0041e663  f3 0f 5c 05 a4 cd 56 00            subss      xmm0, dword ptr [0x56cda4]
0041e66b  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
0041e673  f3 0f 10 0d a4 cd 56 00            movss      xmm1, dword ptr [0x56cda4]
0041e67b  f3 0f 5e 0d d0 c8 56 00            divss      xmm1, dword ptr [0x56c8d0]
0041e683  f3 0f 58 c1                        addss      xmm0, xmm1
0041e687  f3 0f 2c c8                        cvttss2si  ecx, xmm0
0041e68b  03 c1                              add        eax, ecx
0041e68d  50                                 push       eax
0041e68e  6a 00                              push       0
0041e690  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e693  e8 d8 f8 ff ff                     call       0x41df70
0041e698  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e69b  e8 30 e4 ff ff                     call       0x41cad0
0041e6a0  f3 0f 10 05 ac cd 56 00            movss      xmm0, dword ptr [0x56cdac]
0041e6a8  f3 0f 5c 05 a4 cd 56 00            subss      xmm0, dword ptr [0x56cda4]
0041e6b0  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
0041e6b8  f3 0f 10 0d a4 cd 56 00            movss      xmm1, dword ptr [0x56cda4]
0041e6c0  f3 0f 5e 0d d0 c8 56 00            divss      xmm1, dword ptr [0x56c8d0]
0041e6c8  f3 0f 58 c1                        addss      xmm0, xmm1
0041e6cc  f3 0f 2c d0                        cvttss2si  edx, xmm0
0041e6d0  03 c2                              add        eax, edx
0041e6d2  50                                 push       eax
0041e6d3  6a 01                              push       1
0041e6d5  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e6d8  e8 93 f8 ff ff                     call       0x41df70
0041e6dd  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e6e0  e8 0b e4 ff ff                     call       0x41caf0
0041e6e5  83 c0 10                           add        eax, 0x10
0041e6e8  50                                 push       eax
0041e6e9  6a 00                              push       0
0041e6eb  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e6ee  e8 9d f8 ff ff                     call       0x41df90
0041e6f3  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e6f6  e8 f5 e3 ff ff                     call       0x41caf0
0041e6fb  83 c0 10                           add        eax, 0x10
0041e6fe  50                                 push       eax
0041e6ff  6a 01                              push       1
0041e701  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e704  e8 87 f8 ff ff                     call       0x41df90
0041e709  a1 f8 87 5b 00                     mov        eax, dword ptr [0x5b87f8]
0041e70e  99                                 cdq        
0041e70f  2b c2                              sub        eax, edx
0041e711  d1 f8                              sar        eax, 1
0041e713  50                                 push       eax
0041e714  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e717  e8 94 f8 ff ff                     call       0x41dfb0
0041e71c  a1 fc 87 5b 00                     mov        eax, dword ptr [0x5b87fc]
0041e721  2d c0 01 00 00                     sub        eax, 0x1c0
0041e726  99                                 cdq        
0041e727  2b c2                              sub        eax, edx
0041e729  d1 f8                              sar        eax, 1
0041e72b  50                                 push       eax
0041e72c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e72f  e8 bc f8 ff ff                     call       0x41dff0
0041e734  a1 f8 87 5b 00                     mov        eax, dword ptr [0x5b87f8]
0041e739  99                                 cdq        
0041e73a  2b c2                              sub        eax, edx
0041e73c  d1 f8                              sar        eax, 1
0041e73e  50                                 push       eax
0041e73f  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e742  e8 89 f8 ff ff                     call       0x41dfd0
0041e747  f3 0f 10 05 90 cd 56 00            movss      xmm0, dword ptr [0x56cd90]
0041e74f  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0041e757  f3 0f 2c c0                        cvttss2si  eax, xmm0
0041e75b  50                                 push       eax
0041e75c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041e75f  e8 ac f8 ff ff                     call       0x41e010
0041e764  90                                 nop        
0041e765  8b e5                              mov        esp, ebp
0041e767  5d                                 pop        ebp
0041e768  c2 04 00                           ret        4
