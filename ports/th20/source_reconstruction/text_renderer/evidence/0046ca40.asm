0046ca40  55                                 push       ebp
0046ca41  8b ec                              mov        ebp, esp
0046ca43  83 ec 10                           sub        esp, 0x10
0046ca46  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0046ca49  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046ca4c  81 b8 bc a1 01 00 40 01 00 00      cmp        dword ptr [eax + 0x1a1bc], 0x140
0046ca56  7c 05                              jl         0x46ca5d
0046ca58  e9 b0 01 00 00                     jmp        0x46cc0d
0046ca5d  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046ca60  8b 91 e0 a1 01 00                  mov        edx, dword ptr [ecx + 0x1a1e0]
0046ca66  89 55 f4                           mov        dword ptr [ebp - 0xc], edx
0046ca69  8b 45 f4                           mov        eax, dword ptr [ebp - 0xc]
0046ca6c  83 e8 06                           sub        eax, 6
0046ca6f  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
0046ca72  83 7d f4 06                        cmp        dword ptr [ebp - 0xc], 6
0046ca76  77 4c                              ja         0x46cac4
0046ca78  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0046ca7b  ff 24 8d 14 cc 46 00               jmp        dword ptr [ecx*4 + 0x46cc14]
0046ca82  6a 0b                              push       0xb
0046ca84  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046ca87  52                                 push       edx
0046ca88  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046ca8b  e8 00 06 00 00                     call       0x46d090
0046ca90  90                                 nop        
0046ca91  eb 31                              jmp        0x46cac4
0046ca93  6a 08                              push       8
0046ca95  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046ca98  50                                 push       eax
0046ca99  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046ca9c  e8 ef 05 00 00                     call       0x46d090
0046caa1  90                                 nop        
0046caa2  eb 20                              jmp        0x46cac4
0046caa4  6a 09                              push       9
0046caa6  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046caa9  51                                 push       ecx
0046caaa  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046caad  e8 de 05 00 00                     call       0x46d090
0046cab2  90                                 nop        
0046cab3  eb 0f                              jmp        0x46cac4
0046cab5  6a 0d                              push       0xd
0046cab7  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046caba  52                                 push       edx
0046cabb  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046cabe  e8 cd 05 00 00                     call       0x46d090
0046cac3  90                                 nop        
0046cac4  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046cac7  05 bc 11 00 00                     add        eax, 0x11bc
0046cacc  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0046cacf  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046cad2  8b 91 bc a1 01 00                  mov        edx, dword ptr [ecx + 0x1a1bc]
0046cad8  52                                 push       edx
0046cad9  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046cadc  e8 2f e7 ff ff                     call       0x46b210
0046cae1  89 45 f8                           mov        dword ptr [ebp - 8], eax
0046cae4  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046cae7  8b 88 bc a1 01 00                  mov        ecx, dword ptr [eax + 0x1a1bc]
0046caed  83 c1 01                           add        ecx, 1
0046caf0  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046caf3  89 8a bc a1 01 00                  mov        dword ptr [edx + 0x1a1bc], ecx
0046caf9  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
0046cafc  05 00 01 00 00                     add        eax, 0x100
0046cb01  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046cb04  8b 11                              mov        edx, dword ptr [ecx]
0046cb06  89 10                              mov        dword ptr [eax], edx
0046cb08  8b 51 04                           mov        edx, dword ptr [ecx + 4]
0046cb0b  89 50 04                           mov        dword ptr [eax + 4], edx
0046cb0e  8b 49 08                           mov        ecx, dword ptr [ecx + 8]
0046cb11  89 48 08                           mov        dword ptr [eax + 8], ecx
0046cb14  51                                 push       ecx
0046cb15  f3 0f 10 05 18 88 5b 00            movss      xmm0, dword ptr [0x5b8818]
0046cb1d  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
0046cb22  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0046cb25  81 c1 00 01 00 00                  add        ecx, 0x100
0046cb2b  e8 60 cb fb ff                     call       0x429690
0046cb30  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
0046cb33  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046cb36  8b 88 ec a1 01 00                  mov        ecx, dword ptr [eax + 0x1a1ec]
0046cb3c  89 8a 30 01 00 00                  mov        dword ptr [edx + 0x130], ecx
0046cb42  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
0046cb45  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046cb48  8b 88 c0 a1 01 00                  mov        ecx, dword ptr [eax + 0x1a1c0]
0046cb4e  89 8a 0c 01 00 00                  mov        dword ptr [edx + 0x10c], ecx
0046cb54  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
0046cb57  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046cb5a  8b 88 cc a1 01 00                  mov        ecx, dword ptr [eax + 0x1a1cc]
0046cb60  89 8a 14 01 00 00                  mov        dword ptr [edx + 0x114], ecx
0046cb66  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
0046cb69  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046cb6c  8b 88 d0 a1 01 00                  mov        ecx, dword ptr [eax + 0x1a1d0]
0046cb72  89 8a 18 01 00 00                  mov        dword ptr [edx + 0x118], ecx
0046cb78  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
0046cb7b  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046cb7e  8b 88 d4 a1 01 00                  mov        ecx, dword ptr [eax + 0x1a1d4]
0046cb84  89 8a 24 01 00 00                  mov        dword ptr [edx + 0x124], ecx
0046cb8a  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
0046cb8d  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046cb90  8b 88 e0 a1 01 00                  mov        ecx, dword ptr [eax + 0x1a1e0]
0046cb96  89 8a 28 01 00 00                  mov        dword ptr [edx + 0x128], ecx
0046cb9c  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
0046cb9f  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046cba2  8b 88 dc a1 01 00                  mov        ecx, dword ptr [eax + 0x1a1dc]
0046cba8  89 8a 2c 01 00 00                  mov        dword ptr [edx + 0x12c], ecx
0046cbae  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
0046cbb1  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046cbb4  8b 88 f0 a1 01 00                  mov        ecx, dword ptr [eax + 0x1a1f0]
0046cbba  89 8a 34 01 00 00                  mov        dword ptr [edx + 0x134], ecx
0046cbc0  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
0046cbc3  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046cbc6  8b 88 fc a1 01 00                  mov        ecx, dword ptr [eax + 0x1a1fc]
0046cbcc  89 8a 10 01 00 00                  mov        dword ptr [edx + 0x110], ecx
0046cbd2  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
0046cbd5  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046cbd8  8b 88 f4 a1 01 00                  mov        ecx, dword ptr [eax + 0x1a1f4]
0046cbde  89 8a 38 01 00 00                  mov        dword ptr [edx + 0x138], ecx
0046cbe4  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
0046cbe7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046cbea  8b 88 f8 a1 01 00                  mov        ecx, dword ptr [eax + 0x1a1f8]
0046cbf0  89 8a 3c 01 00 00                  mov        dword ptr [edx + 0x13c], ecx
0046cbf6  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046cbf9  81 c1 08 a2 01 00                  add        ecx, 0x1a208
0046cbff  e8 dc c6 fb ff                     call       0x4292e0 ; candidate_rate_access
0046cc04  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
0046cc07  d9 9a 1c 01 00 00                  fstp       dword ptr [edx + 0x11c]
0046cc0d  8b e5                              mov        esp, ebp
0046cc0f  5d                                 pop        ebp
0046cc10  c2 04 00                           ret        4
