00470ed0  55                                 push       ebp
00470ed1  8b ec                              mov        ebp, esp
00470ed3  83 ec 08                           sub        esp, 8
00470ed6  56                                 push       esi
00470ed7  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00470eda  8b 45 08                           mov        eax, dword ptr [ebp + 8]
00470edd  50                                 push       eax
00470ede  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00470ee1  e8 ba 6d fc ff                     call       0x437ca0
00470ee6  89 45 f8                           mov        dword ptr [ebp - 8], eax
00470ee9  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00470eec  8b 55 08                           mov        edx, dword ptr [ebp + 8]
00470eef  89 51 20                           mov        dword ptr [ecx + 0x20], edx
00470ef2  b8 08 00 00 00                     mov        eax, 8
00470ef7  d1 e0                              shl        eax, 1
00470ef9  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00470efc  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
00470eff  8b 52 38                           mov        edx, dword ptr [edx + 0x38]
00470f02  89 94 01 78 03 00 00               mov        dword ptr [ecx + eax + 0x378], edx
00470f09  b8 08 00 00 00                     mov        eax, 8
00470f0e  d1 e0                              shl        eax, 1
00470f10  b9 08 00 00 00                     mov        ecx, 8
00470f15  6b d1 00                           imul       edx, ecx, 0
00470f18  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00470f1b  8b 75 fc                           mov        esi, dword ptr [ebp - 4]
00470f1e  8b 84 06 78 03 00 00               mov        eax, dword ptr [esi + eax + 0x378]
00470f25  89 84 11 78 03 00 00               mov        dword ptr [ecx + edx + 0x378], eax
00470f2c  b9 08 00 00 00                     mov        ecx, 8
00470f31  6b d1 03                           imul       edx, ecx, 3
00470f34  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00470f37  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
00470f3a  8b 49 40                           mov        ecx, dword ptr [ecx + 0x40]
00470f3d  89 8c 10 78 03 00 00               mov        dword ptr [eax + edx + 0x378], ecx
00470f44  ba 08 00 00 00                     mov        edx, 8
00470f49  6b c2 03                           imul       eax, edx, 3
00470f4c  b9 08 00 00 00                     mov        ecx, 8
00470f51  c1 e1 00                           shl        ecx, 0
00470f54  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00470f57  8b 75 fc                           mov        esi, dword ptr [ebp - 4]
00470f5a  8b 84 06 78 03 00 00               mov        eax, dword ptr [esi + eax + 0x378]
00470f61  89 84 0a 78 03 00 00               mov        dword ptr [edx + ecx + 0x378], eax
00470f68  b9 08 00 00 00                     mov        ecx, 8
00470f6d  c1 e1 00                           shl        ecx, 0
00470f70  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00470f73  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
00470f76  8b 40 3c                           mov        eax, dword ptr [eax + 0x3c]
00470f79  89 84 0a 7c 03 00 00               mov        dword ptr [edx + ecx + 0x37c], eax
00470f80  b9 08 00 00 00                     mov        ecx, 8
00470f85  c1 e1 00                           shl        ecx, 0
00470f88  ba 08 00 00 00                     mov        edx, 8
00470f8d  6b c2 00                           imul       eax, edx, 0
00470f90  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00470f93  8b 75 fc                           mov        esi, dword ptr [ebp - 4]
00470f96  8b 8c 0e 7c 03 00 00               mov        ecx, dword ptr [esi + ecx + 0x37c]
00470f9d  89 8c 02 7c 03 00 00               mov        dword ptr [edx + eax + 0x37c], ecx
00470fa4  ba 08 00 00 00                     mov        edx, 8
00470fa9  6b c2 03                           imul       eax, edx, 3
00470fac  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00470faf  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
00470fb2  8b 52 44                           mov        edx, dword ptr [edx + 0x44]
00470fb5  89 94 01 7c 03 00 00               mov        dword ptr [ecx + eax + 0x37c], edx
00470fbc  b8 08 00 00 00                     mov        eax, 8
00470fc1  6b c8 03                           imul       ecx, eax, 3
00470fc4  ba 08 00 00 00                     mov        edx, 8
00470fc9  d1 e2                              shl        edx, 1
00470fcb  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00470fce  8b 75 fc                           mov        esi, dword ptr [ebp - 4]
00470fd1  8b 8c 0e 7c 03 00 00               mov        ecx, dword ptr [esi + ecx + 0x37c]
00470fd8  89 8c 10 7c 03 00 00               mov        dword ptr [eax + edx + 0x37c], ecx
00470fdf  ba 08 00 00 00                     mov        edx, 8
00470fe4  c1 e2 00                           shl        edx, 0
00470fe7  b8 08 00 00 00                     mov        eax, 8
00470fec  6b c8 00                           imul       ecx, eax, 0
00470fef  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00470ff2  8b 75 fc                           mov        esi, dword ptr [ebp - 4]
00470ff5  f3 0f 10 84 10 78 03 00 00         movss      xmm0, dword ptr [eax + edx + 0x378]
00470ffe  f3 0f 5c 84 0e 78 03 00 00         subss      xmm0, dword ptr [esi + ecx + 0x378]
00471007  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0047100a  f3 0f 11 81 98 03 00 00            movss      dword ptr [ecx + 0x398], xmm0
00471012  ba 08 00 00 00                     mov        edx, 8
00471017  d1 e2                              shl        edx, 1
00471019  b8 08 00 00 00                     mov        eax, 8
0047101e  6b c8 00                           imul       ecx, eax, 0
00471021  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00471024  8b 75 fc                           mov        esi, dword ptr [ebp - 4]
00471027  f3 0f 10 84 10 7c 03 00 00         movss      xmm0, dword ptr [eax + edx + 0x37c]
00471030  f3 0f 5c 84 0e 7c 03 00 00         subss      xmm0, dword ptr [esi + ecx + 0x37c]
00471039  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0047103c  f3 0f 11 81 9c 03 00 00            movss      dword ptr [ecx + 0x39c], xmm0
00471044  5e                                 pop        esi
00471045  8b e5                              mov        esp, ebp
00471047  5d                                 pop        ebp
00471048  c2 04 00                           ret        4
