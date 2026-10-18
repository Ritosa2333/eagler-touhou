; reset flag setter
; source SHA-256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
0041de00  55                                 push       ebp
0041de01  8b ec                              mov        ebp, esp
0041de03  51                                 push       ecx
0041de04  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041de07  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0041de0a  83 e0 01                           and        eax, 1
0041de0d  d1 e0                              shl        eax, 1
0041de0f  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041de12  8b 91 90 20 00 00                  mov        edx, dword ptr [ecx + 0x2090]
0041de18  83 e2 fd                           and        edx, 0xfffffffd
0041de1b  0b d0                              or         edx, eax
0041de1d  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041de20  89 90 90 20 00 00                  mov        dword ptr [eax + 0x2090], edx
0041de26  8b e5                              mov        esp, ebp
0041de28  5d                                 pop        ebp
0041de29  c2 04 00                           ret        4
