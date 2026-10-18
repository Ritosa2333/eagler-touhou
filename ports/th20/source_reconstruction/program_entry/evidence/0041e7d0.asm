; WinMain, corrected four-argument WINAPI ABI
; source SHA-256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
0041e7d0  55                                 push       ebp
0041e7d1  8b ec                              mov        ebp, esp
0041e7d3  b8 64 10 00 00                     mov        eax, 0x1064
0041e7d8  e8 63 44 12 00                     call       0x542c40
0041e7dd  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0041e7e2  33 c5                              xor        eax, ebp
0041e7e4  89 45 fc                           mov        dword ptr [ebp - 4], eax
0041e7e7  56                                 push       esi
0041e7e8  c7 85 d8 ef ff ff 00 00 00 00      mov        dword ptr [ebp - 0x1028], 0
0041e7f2  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0041e7f5  a3 64 67 5b 00                     mov        dword ptr [0x5b6764], eax
0041e7fa  c7 05 c4 67 5b 00 00 00 00 00      mov        dword ptr [0x5b67c4], 0
0041e804  6a 01                              push       1
0041e806  ff 15 d8 c2 56 00                  call       dword ptr [0x56c2d8] ; WINMM.dll!timeBeginPeriod
0041e80c  6a 00                              push       0
0041e80e  e8 cd bc 12 00                     call       0x54a4e0
0041e813  83 c4 04                           add        esp, 4
0041e816  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0041e81b  e8 a0 e4 ff ff                     call       0x41ccc0
0041e820  90                                 nop
0041e821  6a 08                              push       8
0041e823  e8 35 3f 12 00                     call       0x54275d
0041e828  83 c4 04                           add        esp, 4
0041e82b  89 85 d0 ef ff ff                  mov        dword ptr [ebp - 0x1030], eax
0041e831  83 bd d0 ef ff ff 00               cmp        dword ptr [ebp - 0x1030], 0
0041e838  74 13                              je         0x41e84d
0041e83a  8b 8d d0 ef ff ff                  mov        ecx, dword ptr [ebp - 0x1030]
0041e840  e8 6b 0d 00 00                     call       0x41f5b0
0041e845  89 85 cc ef ff ff                  mov        dword ptr [ebp - 0x1034], eax
0041e84b  eb 0a                              jmp        0x41e857
0041e84d  c7 85 cc ef ff ff 00 00 00 00      mov        dword ptr [ebp - 0x1034], 0
0041e857  8b 8d cc ef ff ff                  mov        ecx, dword ptr [ebp - 0x1034]
0041e85d  89 8d b4 ef ff ff                  mov        dword ptr [ebp - 0x104c], ecx
0041e863  8b 95 b4 ef ff ff                  mov        edx, dword ptr [ebp - 0x104c]
0041e869  89 15 94 88 5b 00                  mov        dword ptr [0x5b8894], edx
0041e86f  8d 8d f8 ef ff ff                  lea        ecx, [ebp - 0x1008]
0041e875  e8 36 a5 ff ff                     call       0x418db0
0041e87a  8d 85 f8 ef ff ff                  lea        eax, [ebp - 0x1008]
0041e880  50                                 push       eax
0041e881  e8 ea fe ff ff                     call       0x41e770
0041e886  83 c4 04                           add        esp, 4
0041e889  68 48 c9 56 00                     push       0x56c948
0041e88e  68 78 06 5c 00                     push       0x5c0678
0041e893  e8 b8 58 03 00                     call       0x454150
0041e898  83 c4 08                           add        esp, 8
0041e89b  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0041e89e  51                                 push       ecx
0041e89f  e8 7c d7 ff ff                     call       0x41c020
0041e8a4  83 c4 04                           add        esp, 4
0041e8a7  83 f8 ff                           cmp        eax, -1
0041e8aa  75 05                              jne        0x41e8b1
0041e8ac  e9 ad 06 00 00                     jmp        0x41ef5e
0041e8b1  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0041e8b4  52                                 push       edx
0041e8b5  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041e8ba  e8 e1 2e ff ff                     call       0x4117a0
0041e8bf  90                                 nop
0041e8c0  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041e8c5  e8 26 cc ff ff                     call       0x41b4f0
0041e8ca  85 c0                              test       eax, eax
0041e8cc  74 17                              je         0x41e8e5
0041e8ce  68 88 c9 56 00                     push       0x56c988
0041e8d3  68 78 06 5c 00                     push       0x5c0678
0041e8d8  e8 53 59 03 00                     call       0x454230
0041e8dd  83 c4 08                           add        esp, 8
0041e8e0  e9 79 06 00 00                     jmp        0x41ef5e
0041e8e5  68 a8 c9 56 00                     push       0x56c9a8 ; string='th20.cfg'
0041e8ea  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041e8ef  e8 cc d8 0b 00                     call       0x4dc1c0
0041e8f4  85 c0                              test       eax, eax
0041e8f6  74 05                              je         0x41e8fd
0041e8f8  e9 61 06 00 00                     jmp        0x41ef5e
0041e8fd  e8 7e 26 00 00                     call       0x420f80
0041e902  e8 39 27 00 00                     call       0x421040
0041e907  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0041e90a  50                                 push       eax
0041e90b  e8 60 c5 ff ff                     call       0x41ae70
0041e910  83 c4 04                           add        esp, 4
0041e913  8b 0d e8 87 5b 00                  mov        ecx, dword ptr [0x5b87e8]
0041e919  c1 e9 03                           shr        ecx, 3
0041e91c  83 e1 03                           and        ecx, 3
0041e91f  74 05                              je         0x41e926
0041e921  e9 38 06 00 00                     jmp        0x41ef5e
0041e926  8b 15 80 4f 5c 00                  mov        edx, dword ptr [0x5c4f80]
0041e92c  89 15 e4 87 5b 00                  mov        dword ptr [0x5b87e4], edx
0041e932  e8 a9 28 ff ff                     call       0x4111e0
0041e937  e8 e4 83 ff ff                     call       0x416d20
0041e93c  90                                 nop
0041e93d  e8 de d9 ff ff                     call       0x41c320
0041e942  85 c0                              test       eax, eax
0041e944  74 05                              je         0x41e94b
0041e946  e9 13 06 00 00                     jmp        0x41ef5e
0041e94b  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0041e94e  50                                 push       eax
0041e94f  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041e954  e8 97 e3 ff ff                     call       0x41ccf0
0041e959  85 c0                              test       eax, eax
0041e95b  74 05                              je         0x41e962
0041e95d  e9 fc 05 00 00                     jmp        0x41ef5e
0041e962  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041e967  e8 74 da ff ff                     call       0x41c3e0
0041e96c  85 c0                              test       eax, eax
0041e96e  74 05                              je         0x41e975
0041e970  e9 e9 05 00 00                     jmp        0x41ef5e
0041e975  68 b4 c9 56 00                     push       0x56c9b4 ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\core\\main.cpp:172 FuncCtrlInf'
0041e97a  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0041e980  e8 1b 9e ff ff                     call       0x4187a0
0041e985  a3 d8 66 5b 00                     mov        dword ptr [0x5b66d8], eax
0041e98a  68 f4 c9 56 00                     push       0x56c9f4 ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\core\\main.cpp:173 SprtCtrlInf'
0041e98f  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0041e995  e8 96 9e ff ff                     call       0x418830
0041e99a  a3 28 00 5c 00                     mov        dword ptr [0x5c0028], eax
0041e99f  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041e9a4  e8 57 6e ff ff                     call       0x415800
0041e9a9  85 c0                              test       eax, eax
0041e9ab  74 0e                              je         0x41e9bb
0041e9ad  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041e9b2  e8 39 e7 ff ff                     call       0x41d0f0
0041e9b7  85 c0                              test       eax, eax
0041e9b9  74 1f                              je         0x41e9da
0041e9bb  6a 00                              push       0
0041e9bd  6a 00                              push       0
0041e9bf  e8 96 1c 12 00                     call       0x54065a
0041e9c4  90                                 nop
0041e9c5  6a 00                              push       0
0041e9c7  ff 15 68 c2 56 00                  call       dword ptr [0x56c268] ; USER32.dll!ShowCursor
0041e9cd  85 c0                              test       eax, eax
0041e9cf  7d f4                              jge        0x41e9c5
0041e9d1  6a 00                              push       0
0041e9d3  ff 15 6c c2 56 00                  call       dword ptr [0x56c26c] ; USER32.dll!SetCursor
0041e9d9  90                                 nop
0041e9da  0f 57 c0                           xorps      xmm0, xmm0
0041e9dd  f2 0f 11 05 38 88 5b 00            movsd      qword ptr [0x5b8838], xmm0
0041e9e5  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041e9ea  e8 21 e1 ff ff                     call       0x41cb10
0041e9ef  dd 1d 30 88 5b 00                  fstp       qword ptr [0x5b8830]
0041e9f5  f2 0f 10 05 30 88 5b 00            movsd      xmm0, qword ptr [0x5b8830]
0041e9fd  f2 0f 11 05 20 88 5b 00            movsd      qword ptr [0x5b8820], xmm0
0041ea05  f2 0f 10 05 20 88 5b 00            movsd      xmm0, qword ptr [0x5b8820]
0041ea0d  f2 0f 11 05 28 88 5b 00            movsd      qword ptr [0x5b8828], xmm0
0041ea15  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041ea1a  e8 f1 e0 ff ff                     call       0x41cb10
0041ea1f  dd 1d 48 88 5b 00                  fstp       qword ptr [0x5b8848]
0041ea25  f2 0f 10 05 48 88 5b 00            movsd      xmm0, qword ptr [0x5b8848]
0041ea2d  f2 0f 11 05 40 88 5b 00            movsd      qword ptr [0x5b8840], xmm0
0041ea35  8b 0d 58 67 5b 00                  mov        ecx, dword ptr [0x5b6758]
0041ea3b  51                                 push       ecx
0041ea3c  e8 3f ca ff ff                     call       0x41b480
0041ea41  83 c4 04                           add        esp, 4
0041ea44  e8 a7 f7 0b 00                     call       0x4de1f0
0041ea49  89 85 d8 ef ff ff                  mov        dword ptr [ebp - 0x1028], eax
0041ea4f  83 bd d8 ef ff ff 00               cmp        dword ptr [ebp - 0x1028], 0
0041ea56  74 1f                              je         0x41ea77
0041ea58  83 bd d8 ef ff ff ff               cmp        dword ptr [ebp - 0x1028], -1
0041ea5f  75 07                              jne        0x41ea68
0041ea61  e9 8f 04 00 00                     jmp        0x41eef5
0041ea68  c7 85 d8 ef ff ff 02 00 00 00      mov        dword ptr [ebp - 0x1028], 2
0041ea72  e9 7e 04 00 00                     jmp        0x41eef5
0041ea77  c7 85 d8 ef ff ff 00 00 00 00      mov        dword ptr [ebp - 0x1028], 0
0041ea81  6a fc                              push       -4
0041ea83  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041ea88  e8 33 f2 ff ff                     call       0x41dcc0
0041ea8d  8b 15 e8 87 5b 00                  mov        edx, dword ptr [0x5b87e8]
0041ea93  83 ca 01                           or         edx, 1
0041ea96  89 15 e8 87 5b 00                  mov        dword ptr [0x5b87e8], edx
0041ea9c  83 3d 60 67 5b 00 00               cmp        dword ptr [0x5b6760], 0
0041eaa3  0f 85 4c 04 00 00                  jne        0x41eef5
0041eaa9  6a 01                              push       1
0041eaab  6a 00                              push       0
0041eaad  6a 00                              push       0
0041eaaf  6a 00                              push       0
0041eab1  8d 85 dc ef ff ff                  lea        eax, [ebp - 0x1024]
0041eab7  50                                 push       eax
0041eab8  ff 15 d0 c2 56 00                  call       dword ptr [0x56c2d0] ; USER32.dll!PeekMessageW
0041eabe  85 c0                              test       eax, eax
0041eac0  74 20                              je         0x41eae2
0041eac2  8d 8d dc ef ff ff                  lea        ecx, [ebp - 0x1024]
0041eac8  51                                 push       ecx
0041eac9  ff 15 b4 c2 56 00                  call       dword ptr [0x56c2b4] ; USER32.dll!TranslateMessage
0041eacf  8d 95 dc ef ff ff                  lea        edx, [ebp - 0x1024]
0041ead5  52                                 push       edx
0041ead6  ff 15 b8 c2 56 00                  call       dword ptr [0x56c2b8] ; USER32.dll!DispatchMessageW
0041eadc  90                                 nop
0041eadd  e9 0e 04 00 00                     jmp        0x41eef0
0041eae2  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041eae7  e8 44 3c ff ff                     call       0x412730
0041eaec  85 c0                              test       eax, eax
0041eaee  75 02                              jne        0x41eaf2
0041eaf0  eb aa                              jmp        0x41ea9c
0041eaf2  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041eaf7  e8 34 3c ff ff                     call       0x412730
0041eafc  89 85 c8 ef ff ff                  mov        dword ptr [ebp - 0x1038], eax
0041eb02  8b 85 c8 ef ff ff                  mov        eax, dword ptr [ebp - 0x1038]
0041eb08  8b 08                              mov        ecx, dword ptr [eax]
0041eb0a  8b 95 c8 ef ff ff                  mov        edx, dword ptr [ebp - 0x1038]
0041eb10  52                                 push       edx
0041eb11  8b 41 0c                           mov        eax, dword ptr [ecx + 0xc]
0041eb14  ff d0                              call       eax
0041eb16  89 85 c4 ef ff ff                  mov        dword ptr [ebp - 0x103c], eax
0041eb1c  83 bd c4 ef ff ff 00               cmp        dword ptr [ebp - 0x103c], 0
0041eb23  0f 85 a5 00 00 00                  jne        0x41ebce
0041eb29  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041eb2e  e8 4d e5 ff ff                     call       0x41d080
0041eb33  85 c0                              test       eax, eax
0041eb35  74 05                              je         0x41eb3c
0041eb37  e9 a2 00 00 00                     jmp        0x41ebde
0041eb3c  8b 0d e8 87 5b 00                  mov        ecx, dword ptr [0x5b87e8]
0041eb42  c1 e9 02                           shr        ecx, 2
0041eb45  83 e1 01                           and        ecx, 1
0041eb48  74 20                              je         0x41eb6a
0041eb4a  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041eb4f  e8 cc b0 ff ff                     call       0x419c20
0041eb54  89 85 d8 ef ff ff                  mov        dword ptr [ebp - 0x1028], eax
0041eb5a  83 bd d8 ef ff ff 00               cmp        dword ptr [ebp - 0x1028], 0
0041eb61  74 05                              je         0x41eb68
0041eb63  e9 8d 03 00 00                     jmp        0x41eef5
0041eb68  eb 52                              jmp        0x41ebbc
0041eb6a  83 3d 58 4e 5c 00 01               cmp        dword ptr [0x5c4e58], 1
0041eb71  75 2b                              jne        0x41eb9e
0041eb73  0f b6 15 84 4f 5c 00               movzx      edx, byte ptr [0x5c4f84]
0041eb7a  85 d2                              test       edx, edx
0041eb7c  75 20                              jne        0x41eb9e
0041eb7e  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041eb83  e8 a8 b4 ff ff                     call       0x41a030
0041eb88  89 85 d8 ef ff ff                  mov        dword ptr [ebp - 0x1028], eax
0041eb8e  83 bd d8 ef ff ff 00               cmp        dword ptr [ebp - 0x1028], 0
0041eb95  74 05                              je         0x41eb9c
0041eb97  e9 59 03 00 00                     jmp        0x41eef5
0041eb9c  eb 1e                              jmp        0x41ebbc
0041eb9e  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041eba3  e8 38 b2 ff ff                     call       0x419de0
0041eba8  89 85 d8 ef ff ff                  mov        dword ptr [ebp - 0x1028], eax
0041ebae  83 bd d8 ef ff ff 00               cmp        dword ptr [ebp - 0x1028], 0
0041ebb5  74 05                              je         0x41ebbc
0041ebb7  e9 39 03 00 00                     jmp        0x41eef5
0041ebbc  a1 88 58 5c 00                     mov        eax, dword ptr [0x5c5888]
0041ebc1  83 e0 f7                           and        eax, 0xfffffff7
0041ebc4  a3 88 58 5c 00                     mov        dword ptr [0x5c5888], eax
0041ebc9  e9 22 03 00 00                     jmp        0x41eef0
0041ebce  81 bd c4 ef ff ff 69 08 76 88      cmp        dword ptr [ebp - 0x103c], 0x88760869
0041ebd8  0f 85 12 03 00 00                  jne        0x41eef0
0041ebde  6a 0a                              push       0xa
0041ebe0  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041ebe5  e8 46 f2 ff ff                     call       0x41de30
0041ebea  90                                 nop
0041ebeb  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041ebf0  e8 8b e4 ff ff                     call       0x41d080
0041ebf5  85 c0                              test       eax, eax
0041ebf7  74 79                              je         0x41ec72
0041ebf9  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041ebfe  e8 6d e0 ff ff                     call       0x41cc70
0041ec03  83 f8 02                           cmp        eax, 2
0041ec06  7f 3f                              jg         0x41ec47
0041ec08  68 4c 4d 5c 00                     push       0x5c4d4c
0041ec0d  8b 0d 58 67 5b 00                  mov        ecx, dword ptr [0x5b6758]
0041ec13  51                                 push       ecx
0041ec14  ff 15 60 c2 56 00                  call       dword ptr [0x56c260] ; USER32.dll!GetWindowRect
0041ec1a  c7 05 44 4e 5c 00 00 00 00 00      mov        dword ptr [0x5c4e44], 0
0041ec24  0f b6 15 7c 4f 5c 00               movzx      edx, byte ptr [0x5c4f7c]
0041ec2b  85 d2                              test       edx, edx
0041ec2d  75 0c                              jne        0x41ec3b
0041ec2f  c7 05 2c 4e 5c 00 15 00 00 00      mov        dword ptr [0x5c4e2c], 0x15
0041ec39  eb 0a                              jmp        0x41ec45
0041ec3b  c7 05 2c 4e 5c 00 17 00 00 00      mov        dword ptr [0x5c4e2c], 0x17
0041ec45  eb 1e                              jmp        0x41ec65
0041ec47  a1 d8 4e 5c 00                     mov        eax, dword ptr [0x5c4ed8]
0041ec4c  a3 2c 4e 5c 00                     mov        dword ptr [0x5c4e2c], eax
0041ec51  c7 05 54 4e 5c 00 00 00 00 00      mov        dword ptr [0x5c4e54], 0
0041ec5b  c7 05 44 4e 5c 00 01 00 00 00      mov        dword ptr [0x5c4e44], 1
0041ec65  6a 00                              push       0
0041ec67  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041ec6c  e8 df f3 ff ff                     call       0x41e050
0041ec71  90                                 nop
0041ec72  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041ec77  e8 c4 eb 0b 00                     call       0x4dd840
0041ec7c  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
0041ec82  e8 59 ef ff ff                     call       0x41dbe0
0041ec87  90                                 nop
0041ec88  6a 01                              push       1
0041ec8a  e8 a1 da ff ff                     call       0x41c730
0041ec8f  83 c4 04                           add        esp, 4
0041ec92  85 c0                              test       eax, eax
0041ec94  74 05                              je         0x41ec9b
0041ec96  e9 5a 02 00 00                     jmp        0x41eef5
0041ec9b  e8 20 b6 ff ff                     call       0x41a2c0
0041eca0  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
0041eca6  e8 45 ed ff ff                     call       0x41d9f0
0041ecab  c7 05 6c 58 5c 00 03 00 00 00      mov        dword ptr [0x5c586c], 3
0041ecb5  8b 0d 88 58 5c 00                  mov        ecx, dword ptr [0x5c5888]
0041ecbb  83 c9 08                           or         ecx, 8
0041ecbe  89 0d 88 58 5c 00                  mov        dword ptr [0x5c5888], ecx
0041ecc4  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041ecc9  e8 b2 e3 ff ff                     call       0x41d080
0041ecce  85 c0                              test       eax, eax
0041ecd0  0f 84 f1 01 00 00                  je         0x41eec7
0041ecd6  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041ecdb  e8 00 d0 0b 00                     call       0x4dbce0
0041ece0  90                                 nop
0041ece1  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041ece6  e8 85 df ff ff                     call       0x41cc70
0041eceb  83 f8 08                           cmp        eax, 8
0041ecee  74 13                              je         0x41ed03
0041ecf0  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041ecf5  e8 76 df ff ff                     call       0x41cc70
0041ecfa  83 f8 09                           cmp        eax, 9
0041ecfd  0f 85 9f 00 00 00                  jne        0x41eda2
0041ed03  6a 00                              push       0
0041ed05  6a f0                              push       -0x10
0041ed07  8b 15 58 67 5b 00                  mov        edx, dword ptr [0x5b6758]
0041ed0d  52                                 push       edx
0041ed0e  ff 15 74 c2 56 00                  call       dword ptr [0x56c274] ; USER32.dll!SetWindowLongW
0041ed14  6a 05                              push       5
0041ed16  a1 58 67 5b 00                     mov        eax, dword ptr [0x5b6758]
0041ed1b  50                                 push       eax
0041ed1c  ff 15 28 c2 56 00                  call       dword ptr [0x56c228] ; USER32.dll!ShowWindow
0041ed22  8b 0d 00 88 5b 00                  mov        ecx, dword ptr [0x5b8800]
0041ed28  89 8d bc ef ff ff                  mov        dword ptr [ebp - 0x1044], ecx
0041ed2e  8b 15 04 88 5b 00                  mov        edx, dword ptr [0x5b8804]
0041ed34  89 95 c0 ef ff ff                  mov        dword ptr [ebp - 0x1040], edx
0041ed3a  8b 85 c0 ef ff ff                  mov        eax, dword ptr [ebp - 0x1040]
0041ed40  50                                 push       eax
0041ed41  8b 8d bc ef ff ff                  mov        ecx, dword ptr [ebp - 0x1044]
0041ed47  51                                 push       ecx
0041ed48  68 34 ca 56 00                     push       0x56ca34 ; string='%d, %d'
0041ed4d  e8 5e d9 fe ff                     call       0x40c6b0
0041ed52  83 c4 0c                           add        esp, 0xc
0041ed55  6a 40                              push       0x40
0041ed57  8b 95 c0 ef ff ff                  mov        edx, dword ptr [ebp - 0x1040]
0041ed5d  52                                 push       edx
0041ed5e  8b 85 bc ef ff ff                  mov        eax, dword ptr [ebp - 0x1044]
0041ed64  50                                 push       eax
0041ed65  6a 00                              push       0
0041ed67  6a 00                              push       0
0041ed69  6a 00                              push       0
0041ed6b  8b 0d 58 67 5b 00                  mov        ecx, dword ptr [0x5b6758]
0041ed71  51                                 push       ecx
0041ed72  ff 15 2c c2 56 00                  call       dword ptr [0x56c22c] ; USER32.dll!SetWindowPos
0041ed78  6a 00                              push       0
0041ed7a  6a 00                              push       0
0041ed7c  e8 d9 18 12 00                     call       0x54065a
0041ed81  90                                 nop
0041ed82  6a 00                              push       0
0041ed84  ff 15 68 c2 56 00                  call       dword ptr [0x56c268] ; USER32.dll!ShowCursor
0041ed8a  85 c0                              test       eax, eax
0041ed8c  7d f4                              jge        0x41ed82
0041ed8e  6a 00                              push       0
0041ed90  ff 15 6c c2 56 00                  call       dword ptr [0x56c26c] ; USER32.dll!SetCursor
0041ed96  c6 05 c1 67 5b 00 00               mov        byte ptr [0x5b67c1], 0
0041ed9d  e9 25 01 00 00                     jmp        0x41eec7
0041eda2  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041eda7  e8 c4 de ff ff                     call       0x41cc70
0041edac  83 f8 03                           cmp        eax, 3
0041edaf  0f 85 9f 00 00 00                  jne        0x41ee54
0041edb5  68 00 00 cb 10                     push       0x10cb0000
0041edba  6a f0                              push       -0x10
0041edbc  8b 15 58 67 5b 00                  mov        edx, dword ptr [0x5b6758]
0041edc2  52                                 push       edx
0041edc3  ff 15 74 c2 56 00                  call       dword ptr [0x56c274] ; USER32.dll!SetWindowLongW
0041edc9  6a 07                              push       7
0041edcb  ff 15 48 c2 56 00                  call       dword ptr [0x56c248] ; USER32.dll!GetSystemMetrics
0041edd1  8b 0d 00 88 5b 00                  mov        ecx, dword ptr [0x5b8800]
0041edd7  8d 14 41                           lea        edx, [ecx + eax*2]
0041edda  89 95 ac ef ff ff                  mov        dword ptr [ebp - 0x1054], edx
0041ede0  6a 08                              push       8
0041ede2  ff 15 48 c2 56 00                  call       dword ptr [0x56c248] ; USER32.dll!GetSystemMetrics
0041ede8  8b 0d 04 88 5b 00                  mov        ecx, dword ptr [0x5b8804]
0041edee  8d 34 41                           lea        esi, [ecx + eax*2]
0041edf1  6a 04                              push       4
0041edf3  ff 15 48 c2 56 00                  call       dword ptr [0x56c248] ; USER32.dll!GetSystemMetrics
0041edf9  03 f0                              add        esi, eax
0041edfb  89 b5 b0 ef ff ff                  mov        dword ptr [ebp - 0x1050], esi
0041ee01  6a 60                              push       0x60
0041ee03  8b 95 b0 ef ff ff                  mov        edx, dword ptr [ebp - 0x1050]
0041ee09  52                                 push       edx
0041ee0a  8b 85 ac ef ff ff                  mov        eax, dword ptr [ebp - 0x1054]
0041ee10  50                                 push       eax
0041ee11  8b 0d 50 4d 5c 00                  mov        ecx, dword ptr [0x5c4d50]
0041ee17  51                                 push       ecx
0041ee18  8b 15 4c 4d 5c 00                  mov        edx, dword ptr [0x5c4d4c]
0041ee1e  52                                 push       edx
0041ee1f  6a 00                              push       0
0041ee21  a1 58 67 5b 00                     mov        eax, dword ptr [0x5b6758]
0041ee26  50                                 push       eax
0041ee27  ff 15 2c c2 56 00                  call       dword ptr [0x56c22c] ; USER32.dll!SetWindowPos
0041ee2d  6a 01                              push       1
0041ee2f  8b 0d 58 67 5b 00                  mov        ecx, dword ptr [0x5b6758]
0041ee35  51                                 push       ecx
0041ee36  ff 15 28 c2 56 00                  call       dword ptr [0x56c228] ; USER32.dll!ShowWindow
0041ee3c  6a 01                              push       1
0041ee3e  6a 00                              push       0
0041ee40  e8 15 18 12 00                     call       0x54065a
0041ee45  90                                 nop
0041ee46  6a 01                              push       1
0041ee48  ff 15 68 c2 56 00                  call       dword ptr [0x56c268] ; USER32.dll!ShowCursor
0041ee4e  85 c0                              test       eax, eax
0041ee50  7c f4                              jl         0x41ee46
0041ee52  eb 73                              jmp        0x41eec7
0041ee54  8b 15 f8 87 5b 00                  mov        edx, dword ptr [0x5b87f8]
0041ee5a  89 95 a4 ef ff ff                  mov        dword ptr [ebp - 0x105c], edx
0041ee60  a1 fc 87 5b 00                     mov        eax, dword ptr [0x5b87fc]
0041ee65  89 85 a8 ef ff ff                  mov        dword ptr [ebp - 0x1058], eax
0041ee6b  68 00 00 00 90                     push       0x90000000
0041ee70  6a f0                              push       -0x10
0041ee72  8b 0d 58 67 5b 00                  mov        ecx, dword ptr [0x5b6758]
0041ee78  51                                 push       ecx
0041ee79  ff 15 74 c2 56 00                  call       dword ptr [0x56c274] ; USER32.dll!SetWindowLongW
0041ee7f  6a 20                              push       0x20
0041ee81  8b 95 a8 ef ff ff                  mov        edx, dword ptr [ebp - 0x1058]
0041ee87  52                                 push       edx
0041ee88  8b 85 a4 ef ff ff                  mov        eax, dword ptr [ebp - 0x105c]
0041ee8e  50                                 push       eax
0041ee8f  6a 00                              push       0
0041ee91  6a 00                              push       0
0041ee93  6a 00                              push       0
0041ee95  8b 0d 58 67 5b 00                  mov        ecx, dword ptr [0x5b6758]
0041ee9b  51                                 push       ecx
0041ee9c  ff 15 2c c2 56 00                  call       dword ptr [0x56c22c] ; USER32.dll!SetWindowPos
0041eea2  6a 00                              push       0
0041eea4  6a 00                              push       0
0041eea6  e8 af 17 12 00                     call       0x54065a
0041eeab  90                                 nop
0041eeac  6a 00                              push       0
0041eeae  ff 15 68 c2 56 00                  call       dword ptr [0x56c268] ; USER32.dll!ShowCursor
0041eeb4  85 c0                              test       eax, eax
0041eeb6  7d f4                              jge        0x41eeac
0041eeb8  6a 00                              push       0
0041eeba  ff 15 6c c2 56 00                  call       dword ptr [0x56c26c] ; USER32.dll!SetCursor
0041eec0  c6 05 c1 67 5b 00 00               mov        byte ptr [0x5b67c1], 0
0041eec7  68 4c 4d 5c 00                     push       0x5c4d4c
0041eecc  8b 15 58 67 5b 00                  mov        edx, dword ptr [0x5b6758]
0041eed2  52                                 push       edx
0041eed3  ff 15 60 c2 56 00                  call       dword ptr [0x56c260] ; USER32.dll!GetWindowRect
0041eed9  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041eede  e8 8d ce 0b 00                     call       0x4dbd70
0041eee3  6a 00                              push       0
0041eee5  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041eeea  e8 11 ef ff ff                     call       0x41de00
0041eeef  90                                 nop
0041eef0  e9 a7 fb ff ff                     jmp        0x41ea9c
0041eef5  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041eefa  e8 71 dd ff ff                     call       0x41cc70
0041eeff  a3 80 4f 5c 00                     mov        dword ptr [0x5c4f80], eax
0041ef04  83 3d 80 4f 5c 00 03               cmp        dword ptr [0x5c4f80], 3
0041ef0b  7c 29                              jl         0x41ef36
0041ef0d  68 4c 4d 5c 00                     push       0x5c4d4c
0041ef12  a1 58 67 5b 00                     mov        eax, dword ptr [0x5b6758]
0041ef17  50                                 push       eax
0041ef18  ff 15 60 c2 56 00                  call       dword ptr [0x56c260] ; USER32.dll!GetWindowRect
0041ef1e  8b 0d 4c 4d 5c 00                  mov        ecx, dword ptr [0x5c4d4c]
0041ef24  89 0d 90 4f 5c 00                  mov        dword ptr [0x5c4f90], ecx
0041ef2a  8b 15 50 4d 5c 00                  mov        edx, dword ptr [0x5c4d50]
0041ef30  89 15 94 4f 5c 00                  mov        dword ptr [0x5c4f94], edx
0041ef36  68 40 4d 5c 00                     push       0x5c4d40
0041ef3b  e8 50 e5 0b 00                     call       0x4dd490
0041ef40  83 c4 04                           add        esp, 4
0041ef43  a1 d8 66 5b 00                     mov        eax, dword ptr [0x5b66d8]
0041ef48  50                                 push       eax
0041ef49  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0041ef4f  e8 6c 97 ff ff                     call       0x4186c0
0041ef54  c7 05 d8 66 5b 00 00 00 00 00      mov        dword ptr [0x5b66d8], 0
0041ef5e  b9 30 a8 5b 00                     mov        ecx, 0x5ba830
0041ef63  e8 08 72 00 00                     call       0x426170
0041ef68  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
0041ef6e  51                                 push       ecx
0041ef6f  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0041ef75  e8 b6 97 ff ff                     call       0x418730
0041ef7a  c7 05 28 00 5c 00 00 00 00 00      mov        dword ptr [0x5c0028], 0
0041ef84  83 3d dc 4e 5c 00 00               cmp        dword ptr [0x5c4edc], 0
0041ef8b  74 1e                              je         0x41efab
0041ef8d  8b 15 dc 4e 5c 00                  mov        edx, dword ptr [0x5c4edc]
0041ef93  8b 02                              mov        eax, dword ptr [edx]
0041ef95  8b 0d dc 4e 5c 00                  mov        ecx, dword ptr [0x5c4edc]
0041ef9b  51                                 push       ecx
0041ef9c  8b 50 08                           mov        edx, dword ptr [eax + 8]
0041ef9f  ff d2                              call       edx
0041efa1  c7 05 dc 4e 5c 00 00 00 00 00      mov        dword ptr [0x5c4edc], 0
0041efab  83 3d e0 4e 5c 00 00               cmp        dword ptr [0x5c4ee0], 0
0041efb2  74 1d                              je         0x41efd1
0041efb4  a1 e0 4e 5c 00                     mov        eax, dword ptr [0x5c4ee0]
0041efb9  8b 08                              mov        ecx, dword ptr [eax]
0041efbb  8b 15 e0 4e 5c 00                  mov        edx, dword ptr [0x5c4ee0]
0041efc1  52                                 push       edx
0041efc2  8b 41 08                           mov        eax, dword ptr [ecx + 8]
0041efc5  ff d0                              call       eax
0041efc7  c7 05 e0 4e 5c 00 00 00 00 00      mov        dword ptr [0x5c4ee0], 0
0041efd1  83 3d e4 4e 5c 00 00               cmp        dword ptr [0x5c4ee4], 0
0041efd8  74 1d                              je         0x41eff7
0041efda  8b 0d e4 4e 5c 00                  mov        ecx, dword ptr [0x5c4ee4]
0041efe0  8b 11                              mov        edx, dword ptr [ecx]
0041efe2  a1 e4 4e 5c 00                     mov        eax, dword ptr [0x5c4ee4]
0041efe7  50                                 push       eax
0041efe8  8b 4a 08                           mov        ecx, dword ptr [edx + 8]
0041efeb  ff d1                              call       ecx
0041efed  c7 05 e4 4e 5c 00 00 00 00 00      mov        dword ptr [0x5c4ee4], 0
0041eff7  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041effc  e8 3f b2 ff ff                     call       0x41a240
0041f001  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041f006  e8 f5 b1 ff ff                     call       0x41a200
0041f00b  90                                 nop
0041f00c  83 3d 58 67 5b 00 00               cmp        dword ptr [0x5b6758], 0
0041f013  74 25                              je         0x41f03a
0041f015  6a 00                              push       0
0041f017  8b 15 58 67 5b 00                  mov        edx, dword ptr [0x5b6758]
0041f01d  52                                 push       edx
0041f01e  ff 15 28 c2 56 00                  call       dword ptr [0x56c228] ; USER32.dll!ShowWindow
0041f024  a1 58 67 5b 00                     mov        eax, dword ptr [0x5b6758]
0041f029  50                                 push       eax
0041f02a  ff 15 24 c2 56 00                  call       dword ptr [0x56c224] ; USER32.dll!DestroyWindow
0041f030  c7 05 58 67 5b 00 00 00 00 00      mov        dword ptr [0x5b6758], 0
0041f03a  6a 01                              push       1
0041f03c  ff 15 68 c2 56 00                  call       dword ptr [0x56c268] ; USER32.dll!ShowCursor
0041f042  85 c0                              test       eax, eax
0041f044  7c f4                              jl         0x41f03a
0041f046  83 bd d8 ef ff ff 02               cmp        dword ptr [ebp - 0x1028], 2
0041f04d  0f 85 a0 00 00 00                  jne        0x41f0f3
0041f053  b9 78 06 5c 00                     mov        ecx, 0x5c0678
0041f058  e8 83 ab ff ff                     call       0x419be0
0041f05d  68 3c ca 56 00                     push       0x56ca3c
0041f062  68 78 06 5c 00                     push       0x5c0678
0041f067  e8 e4 50 03 00                     call       0x454150
0041f06c  83 c4 08                           add        esp, 8
0041f06f  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041f074  e8 87 67 ff ff                     call       0x415800
0041f079  85 c0                              test       eax, eax
0041f07b  75 0a                              jne        0x41f087
0041f07d  6a 01                              push       1
0041f07f  6a 00                              push       0
0041f081  e8 d4 15 12 00                     call       0x54065a
0041f086  90                                 nop
0041f087  c7 85 d4 ef ff ff 00 00 00 00      mov        dword ptr [ebp - 0x102c], 0
0041f091  83 bd d4 ef ff ff 3c               cmp        dword ptr [ebp - 0x102c], 0x3c
0041f098  7d 45                              jge        0x41f0df
0041f09a  6a 01                              push       1
0041f09c  6a 00                              push       0
0041f09e  6a 00                              push       0
0041f0a0  6a 00                              push       0
0041f0a2  8d 8d dc ef ff ff                  lea        ecx, [ebp - 0x1024]
0041f0a8  51                                 push       ecx
0041f0a9  ff 15 d0 c2 56 00                  call       dword ptr [0x56c2d0] ; USER32.dll!PeekMessageW
0041f0af  85 c0                              test       eax, eax
0041f0b1  74 1b                              je         0x41f0ce
0041f0b3  8d 95 dc ef ff ff                  lea        edx, [ebp - 0x1024]
0041f0b9  52                                 push       edx
0041f0ba  ff 15 b4 c2 56 00                  call       dword ptr [0x56c2b4] ; USER32.dll!TranslateMessage
0041f0c0  8d 85 dc ef ff ff                  lea        eax, [ebp - 0x1024]
0041f0c6  50                                 push       eax
0041f0c7  ff 15 b8 c2 56 00                  call       dword ptr [0x56c2b8] ; USER32.dll!DispatchMessageW
0041f0cd  90                                 nop
0041f0ce  8b 8d d4 ef ff ff                  mov        ecx, dword ptr [ebp - 0x102c]
0041f0d4  83 c1 01                           add        ecx, 1
0041f0d7  89 8d d4 ef ff ff                  mov        dword ptr [ebp - 0x102c], ecx
0041f0dd  eb b2                              jmp        0x41f091
0041f0df  8b 15 88 58 5c 00                  mov        edx, dword ptr [0x5c5888]
0041f0e5  83 e2 9f                           and        edx, 0xffffff9f
0041f0e8  89 15 88 58 5c 00                  mov        dword ptr [0x5c5888], edx
0041f0ee  e9 4a f8 ff ff                     jmp        0x41e93d
0041f0f3  b9 08 4f 5c 00                     mov        ecx, 0x5c4f08
0041f0f8  e8 73 b9 ff ff                     call       0x41aa70
0041f0fd  6a 01                              push       1
0041f0ff  ff 15 dc c2 56 00                  call       dword ptr [0x56c2dc] ; WINMM.dll!timeEndPeriod
0041f105  90                                 nop
0041f106  68 e1 67 5b 00                     push       0x5b67e1
0041f10b  68 00 10 00 00                     push       0x1000
0041f110  8d 85 fc ef ff ff                  lea        eax, [ebp - 0x1004]
0041f116  50                                 push       eax
0041f117  e8 f4 94 12 00                     call       0x548610
0041f11c  83 c4 0c                           add        esp, 0xc
0041f11f  68 74 ca 56 00                     push       0x56ca74 ; string='log.txt'
0041f124  68 00 10 00 00                     push       0x1000
0041f129  8d 8d fc ef ff ff                  lea        ecx, [ebp - 0x1004]
0041f12f  51                                 push       ecx
0041f130  e8 6b 95 12 00                     call       0x5486a0
0041f135  83 c4 0c                           add        esp, 0xc
0041f138  b9 78 06 5c 00                     mov        ecx, 0x5c0678
0041f13d  e8 de 40 03 00                     call       0x453220
0041f142  90                                 nop
0041f143  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041f148  e8 43 c3 ff ff                     call       0x41b490
0041f14d  90                                 nop
0041f14e  8b 15 94 88 5b 00                  mov        edx, dword ptr [0x5b8894]
0041f154  89 95 b8 ef ff ff                  mov        dword ptr [ebp - 0x1048], edx
0041f15a  83 bd b8 ef ff ff 00               cmp        dword ptr [ebp - 0x1048], 0
0041f161  74 15                              je         0x41f178
0041f163  6a 01                              push       1
0041f165  8b 8d b8 ef ff ff                  mov        ecx, dword ptr [ebp - 0x1048]
0041f16b  e8 b0 a1 ff ff                     call       0x419320
0041f170  89 85 a0 ef ff ff                  mov        dword ptr [ebp - 0x1060], eax
0041f176  eb 0a                              jmp        0x41f182
0041f178  c7 85 a0 ef ff ff 00 00 00 00      mov        dword ptr [ebp - 0x1060], 0
0041f182  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0041f187  e8 a4 d8 ff ff                     call       0x41ca30
0041f18c  90                                 nop
0041f18d  c7 85 9c ef ff ff 00 00 00 00      mov        dword ptr [ebp - 0x1064], 0
0041f197  8d 8d f8 ef ff ff                  lea        ecx, [ebp - 0x1008]
0041f19d  e8 ee 9c ff ff                     call       0x418e90
0041f1a2  8b 85 9c ef ff ff                  mov        eax, dword ptr [ebp - 0x1064]
0041f1a8  5e                                 pop        esi
0041f1a9  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041f1ac  33 cd                              xor        ecx, ebp
0041f1ae  e8 3b 38 12 00                     call       0x5429ee
0041f1b3  8b e5                              mov        esp, ebp
0041f1b5  5d                                 pop        ebp
0041f1b6  c2 10 00                           ret        0x10
