00418ac0  55                                 push       ebp
00418ac1  8b ec                              mov        ebp, esp
00418ac3  6a ff                              push       -1
00418ac5  68 00 76 56 00                     push       0x567600
00418aca  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
00418ad0  50                                 push       eax
00418ad1  51                                 push       ecx
00418ad2  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
00418ad7  33 c5                              xor        eax, ebp
00418ad9  50                                 push       eax
00418ada  8d 45 f4                           lea        eax, [ebp - 0xc]
00418add  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
00418ae3  89 4d f0                           mov        dword ptr [ebp - 0x10], ecx
00418ae6  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00418ae9  c7 00 00 00 00 00                  mov        dword ptr [eax], 0
00418aef  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00418af2  c7 41 04 00 00 00 00               mov        dword ptr [ecx + 4], 0
00418af9  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00418afc  c7 42 08 00 00 00 00               mov        dword ptr [edx + 8], 0
00418b03  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00418b06  c7 40 0c 00 00 00 00               mov        dword ptr [eax + 0xc], 0
00418b0d  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00418b10  c7 41 10 00 00 00 00               mov        dword ptr [ecx + 0x10], 0
00418b17  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00418b1a  c7 42 14 00 00 00 00               mov        dword ptr [edx + 0x14], 0
00418b21  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00418b24  c7 40 18 00 00 00 00               mov        dword ptr [eax + 0x18], 0
00418b2b  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00418b2e  c7 41 1c 00 00 00 00               mov        dword ptr [ecx + 0x1c], 0
00418b35  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00418b38  c7 42 20 00 00 00 00               mov        dword ptr [edx + 0x20], 0
00418b3f  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00418b42  c7 40 24 00 00 00 00               mov        dword ptr [eax + 0x24], 0
00418b49  33 c9                              xor        ecx, ecx
00418b4b  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00418b4e  83 c2 28                           add        edx, 0x28
00418b51  89 0a                              mov        dword ptr [edx], ecx
00418b53  89 4a 04                           mov        dword ptr [edx + 4], ecx
00418b56  33 c0                              xor        eax, eax
00418b58  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00418b5b  83 c1 30                           add        ecx, 0x30
00418b5e  89 01                              mov        dword ptr [ecx], eax
00418b60  89 41 04                           mov        dword ptr [ecx + 4], eax
00418b63  33 d2                              xor        edx, edx
00418b65  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00418b68  83 c0 38                           add        eax, 0x38
00418b6b  89 10                              mov        dword ptr [eax], edx
00418b6d  89 50 04                           mov        dword ptr [eax + 4], edx
00418b70  33 c9                              xor        ecx, ecx
00418b72  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00418b75  83 c2 40                           add        edx, 0x40
00418b78  89 0a                              mov        dword ptr [edx], ecx
00418b7a  89 4a 04                           mov        dword ptr [edx + 4], ecx
00418b7d  33 c0                              xor        eax, eax
00418b7f  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00418b82  83 c1 48                           add        ecx, 0x48
00418b85  89 01                              mov        dword ptr [ecx], eax
00418b87  89 41 04                           mov        dword ptr [ecx + 4], eax
00418b8a  33 d2                              xor        edx, edx
00418b8c  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00418b8f  83 c0 50                           add        eax, 0x50
00418b92  89 10                              mov        dword ptr [eax], edx
00418b94  89 50 04                           mov        dword ptr [eax + 4], edx
00418b97  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00418b9a  c7 41 58 00 00 00 00               mov        dword ptr [ecx + 0x58], 0
00418ba1  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00418ba4  c7 42 5c 00 00 00 00               mov        dword ptr [edx + 0x5c], 0
00418bab  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00418bae  c7 40 60 00 00 00 00               mov        dword ptr [eax + 0x60], 0
00418bb5  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00418bb8  c7 41 64 00 00 00 00               mov        dword ptr [ecx + 0x64], 0
00418bbf  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00418bc2  c6 42 68 00                        mov        byte ptr [edx + 0x68], 0
00418bc6  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00418bc9  c6 40 69 00                        mov        byte ptr [eax + 0x69], 0
00418bcd  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00418bd0  c7 41 6c 00 00 00 00               mov        dword ptr [ecx + 0x6c], 0
00418bd7  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00418bda  c6 42 70 00                        mov        byte ptr [edx + 0x70], 0
00418bde  33 c0                              xor        eax, eax
00418be0  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00418be3  83 c1 78                           add        ecx, 0x78
00418be6  89 01                              mov        dword ptr [ecx], eax
00418be8  89 41 04                           mov        dword ptr [ecx + 4], eax
00418beb  33 d2                              xor        edx, edx
00418bed  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00418bf0  05 80 00 00 00                     add        eax, 0x80
00418bf5  89 10                              mov        dword ptr [eax], edx
00418bf7  89 50 04                           mov        dword ptr [eax + 4], edx
00418bfa  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00418bfd  c6 81 88 00 00 00 00               mov        byte ptr [ecx + 0x88], 0
00418c04  68 00 10 00 00                     push       0x1000
00418c09  6a 00                              push       0
00418c0b  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00418c0e  81 c2 89 00 00 00                  add        edx, 0x89
00418c14  52                                 push       edx
00418c15  e8 46 bb 12 00                     call       0x544760
00418c1a  83 c4 0c                           add        esp, 0xc
00418c1d  68 00 10 00 00                     push       0x1000
00418c22  6a 00                              push       0
00418c24  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00418c27  05 89 10 00 00                     add        eax, 0x1089
00418c2c  50                                 push       eax
00418c2d  e8 2e bb 12 00                     call       0x544760
00418c32  83 c4 0c                           add        esp, 0xc
00418c35  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00418c38  c6 81 89 20 00 00 00               mov        byte ptr [ecx + 0x2089], 0
00418c3f  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00418c42  c6 82 8a 20 00 00 00               mov        byte ptr [edx + 0x208a], 0
00418c49  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00418c4c  c6 80 8b 20 00 00 00               mov        byte ptr [eax + 0x208b], 0
00418c53  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00418c56  c7 81 8c 20 00 00 02 00 00 00      mov        dword ptr [ecx + 0x208c], 2
00418c60  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00418c63  81 c1 90 20 00 00                  add        ecx, 0x2090
00418c69  e8 62 01 00 00                     call       0x418dd0
00418c6e  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00418c71  c7 82 94 20 00 00 00 00 00 00      mov        dword ptr [edx + 0x2094], 0
00418c7b  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00418c7e  0f 57 c0                           xorps      xmm0, xmm0
00418c81  f2 0f 11 80 98 20 00 00            movsd      qword ptr [eax + 0x2098], xmm0
00418c89  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00418c8c  c7 81 a0 20 00 00 00 00 00 00      mov        dword ptr [ecx + 0x20a0], 0
00418c96  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00418c99  c7 82 a4 20 00 00 00 00 00 00      mov        dword ptr [edx + 0x20a4], 0
00418ca3  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00418ca6  c7 80 a8 20 00 00 00 00 00 00      mov        dword ptr [eax + 0x20a8], 0
00418cb0  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00418cb3  c7 81 ac 20 00 00 00 00 00 00      mov        dword ptr [ecx + 0x20ac], 0
00418cbd  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00418cc0  c7 82 b0 20 00 00 00 00 00 00      mov        dword ptr [edx + 0x20b0], 0
00418cca  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00418ccd  c7 80 b4 20 00 00 00 00 00 00      mov        dword ptr [eax + 0x20b4], 0
00418cd7  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00418cda  c7 81 b8 20 00 00 00 00 00 00      mov        dword ptr [ecx + 0x20b8], 0
00418ce4  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00418ce7  c7 82 bc 20 00 00 00 00 00 00      mov        dword ptr [edx + 0x20bc], 0
00418cf1  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00418cf4  0f 57 c0                           xorps      xmm0, xmm0
00418cf7  f3 0f 11 80 c0 20 00 00            movss      dword ptr [eax + 0x20c0], xmm0
00418cff  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00418d02  0f 57 c0                           xorps      xmm0, xmm0
00418d05  f2 0f 11 81 c8 20 00 00            movsd      qword ptr [ecx + 0x20c8], xmm0
00418d0d  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00418d10  0f 57 c0                           xorps      xmm0, xmm0
00418d13  f2 0f 11 82 d0 20 00 00            movsd      qword ptr [edx + 0x20d0], xmm0
00418d1b  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00418d1e  0f 57 c0                           xorps      xmm0, xmm0
00418d21  f2 0f 11 80 d8 20 00 00            movsd      qword ptr [eax + 0x20d8], xmm0
00418d29  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00418d2c  0f 57 c0                           xorps      xmm0, xmm0
00418d2f  f2 0f 11 81 e0 20 00 00            movsd      qword ptr [ecx + 0x20e0], xmm0
00418d37  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00418d3a  0f 57 c0                           xorps      xmm0, xmm0
00418d3d  f2 0f 11 82 e8 20 00 00            movsd      qword ptr [edx + 0x20e8], xmm0
00418d45  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00418d48  0f 57 c0                           xorps      xmm0, xmm0
00418d4b  f2 0f 11 80 f0 20 00 00            movsd      qword ptr [eax + 0x20f0], xmm0
00418d53  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00418d56  c7 81 f8 20 00 00 00 00 00 00      mov        dword ptr [ecx + 0x20f8], 0
00418d60  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00418d63  c7 82 fc 20 00 00 00 00 00 00      mov        dword ptr [edx + 0x20fc], 0
00418d6d  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00418d70  c7 80 00 21 00 00 00 00 00 00      mov        dword ptr [eax + 0x2100], 0
00418d7a  68 90 40 41 00                     push       0x414090
00418d7f  6a 04                              push       4
00418d81  6a 0c                              push       0xc
00418d83  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00418d86  81 c1 04 21 00 00                  add        ecx, 0x2104
00418d8c  51                                 push       ecx
00418d8d  e8 8e 2e ff ff                     call       0x40bc20
00418d92  90                                 nop        
00418d93  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00418d96  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
00418d99  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
00418da0  59                                 pop        ecx
00418da1  8b e5                              mov        esp, ebp
00418da3  5d                                 pop        ebp
00418da4  c3                                 ret        
