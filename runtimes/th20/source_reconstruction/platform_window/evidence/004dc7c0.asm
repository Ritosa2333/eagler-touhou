004dc7c0  55                                 push       ebp
004dc7c1  8b ec                              mov        ebp, esp
004dc7c3  51                                 push       ecx
004dc7c4  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004dc7ca  e8 f1 8d f6 ff                     call       0x4455c0
004dc7cf  90                                 nop        
004dc7d0  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
004dc7d7  eb 09                              jmp        0x4dc7e2
004dc7d9  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dc7dc  83 c0 01                           add        eax, 1
004dc7df  89 45 fc                           mov        dword ptr [ebp - 4], eax
004dc7e2  83 7d fc 03                        cmp        dword ptr [ebp - 4], 3
004dc7e6  0f 8d bb 00 00 00                  jge        0x4dc8a7
004dc7ec  b9 6c 01 00 00                     mov        ecx, 0x16c
004dc7f1  6b d1 03                           imul       edx, ecx, 3
004dc7f4  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dc7f7  0f 57 c0                           xorps      xmm0, xmm0
004dc7fa  f3 0f 11 84 c2 e4 50 5c 00         movss      dword ptr [edx + eax*8 + 0x5c50e4], xmm0
004dc803  b9 6c 01 00 00                     mov        ecx, 0x16c
004dc808  6b d1 03                           imul       edx, ecx, 3
004dc80b  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dc80e  0f 57 c0                           xorps      xmm0, xmm0
004dc811  f3 0f 11 84 c2 e8 50 5c 00         movss      dword ptr [edx + eax*8 + 0x5c50e8], xmm0
004dc81a  b9 6c 01 00 00                     mov        ecx, 0x16c
004dc81f  d1 e1                              shl        ecx, 1
004dc821  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dc824  0f 57 c0                           xorps      xmm0, xmm0
004dc827  f3 0f 11 84 d1 e4 50 5c 00         movss      dword ptr [ecx + edx*8 + 0x5c50e4], xmm0
004dc830  b8 6c 01 00 00                     mov        eax, 0x16c
004dc835  d1 e0                              shl        eax, 1
004dc837  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dc83a  0f 57 c0                           xorps      xmm0, xmm0
004dc83d  f3 0f 11 84 c8 e8 50 5c 00         movss      dword ptr [eax + ecx*8 + 0x5c50e8], xmm0
004dc846  ba 6c 01 00 00                     mov        edx, 0x16c
004dc84b  6b c2 00                           imul       eax, edx, 0
004dc84e  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dc851  0f 57 c0                           xorps      xmm0, xmm0
004dc854  f3 0f 11 84 c8 e4 50 5c 00         movss      dword ptr [eax + ecx*8 + 0x5c50e4], xmm0
004dc85d  ba 6c 01 00 00                     mov        edx, 0x16c
004dc862  6b c2 00                           imul       eax, edx, 0
004dc865  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dc868  0f 57 c0                           xorps      xmm0, xmm0
004dc86b  f3 0f 11 84 c8 e8 50 5c 00         movss      dword ptr [eax + ecx*8 + 0x5c50e8], xmm0
004dc874  ba 6c 01 00 00                     mov        edx, 0x16c
004dc879  c1 e2 00                           shl        edx, 0
004dc87c  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dc87f  0f 57 c0                           xorps      xmm0, xmm0
004dc882  f3 0f 11 84 c2 e4 50 5c 00         movss      dword ptr [edx + eax*8 + 0x5c50e4], xmm0
004dc88b  b9 6c 01 00 00                     mov        ecx, 0x16c
004dc890  c1 e1 00                           shl        ecx, 0
004dc893  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dc896  0f 57 c0                           xorps      xmm0, xmm0
004dc899  f3 0f 11 84 d1 e8 50 5c 00         movss      dword ptr [ecx + edx*8 + 0x5c50e8], xmm0
004dc8a2  e9 32 ff ff ff                     jmp        0x4dc7d9
004dc8a7  b8 01 00 00 00                     mov        eax, 1
004dc8ac  8b e5                              mov        esp, ebp
004dc8ae  5d                                 pop        ebp
004dc8af  c3                                 ret        
