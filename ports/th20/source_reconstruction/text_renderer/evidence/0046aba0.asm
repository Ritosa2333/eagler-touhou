0046aba0  55                                 push       ebp
0046aba1  8b ec                              mov        ebp, esp
0046aba3  51                                 push       ecx
0046aba4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0046aba7  68 00 01 00 00                     push       0x100
0046abac  6a 00                              push       0
0046abae  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046abb1  50                                 push       eax
0046abb2  e8 a9 9b 0d 00                     call       0x544760
0046abb7  83 c4 0c                           add        esp, 0xc
0046abba  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046abbd  81 c1 00 01 00 00                  add        ecx, 0x100
0046abc3  e8 48 82 fb ff                     call       0x422e10
0046abc8  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046abcb  c7 81 0c 01 00 00 00 00 00 00      mov        dword ptr [ecx + 0x10c], 0
0046abd5  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046abd8  c7 82 10 01 00 00 00 00 00 00      mov        dword ptr [edx + 0x110], 0
0046abe2  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046abe5  0f 57 c0                           xorps      xmm0, xmm0
0046abe8  f3 0f 11 80 14 01 00 00            movss      dword ptr [eax + 0x114], xmm0
0046abf0  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046abf3  0f 57 c0                           xorps      xmm0, xmm0
0046abf6  f3 0f 11 81 18 01 00 00            movss      dword ptr [ecx + 0x118], xmm0
0046abfe  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046ac01  0f 57 c0                           xorps      xmm0, xmm0
0046ac04  f3 0f 11 82 1c 01 00 00            movss      dword ptr [edx + 0x11c], xmm0
0046ac0c  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046ac0f  c7 80 20 01 00 00 00 00 00 00      mov        dword ptr [eax + 0x120], 0
0046ac19  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046ac1c  c7 81 24 01 00 00 00 00 00 00      mov        dword ptr [ecx + 0x124], 0
0046ac26  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046ac29  c7 82 28 01 00 00 00 00 00 00      mov        dword ptr [edx + 0x128], 0
0046ac33  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046ac36  c7 80 2c 01 00 00 00 00 00 00      mov        dword ptr [eax + 0x12c], 0
0046ac40  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046ac43  c7 81 30 01 00 00 00 00 00 00      mov        dword ptr [ecx + 0x130], 0
0046ac4d  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046ac50  c7 82 34 01 00 00 00 00 00 00      mov        dword ptr [edx + 0x134], 0
0046ac5a  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046ac5d  c7 80 38 01 00 00 01 00 00 00      mov        dword ptr [eax + 0x138], 1
0046ac67  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046ac6a  c7 81 3c 01 00 00 01 00 00 00      mov        dword ptr [ecx + 0x13c], 1
0046ac74  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046ac77  8b e5                              mov        esp, ebp
0046ac79  5d                                 pop        ebp
0046ac7a  c3                                 ret        
