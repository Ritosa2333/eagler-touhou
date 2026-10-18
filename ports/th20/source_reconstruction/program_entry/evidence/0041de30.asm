; reset delay setter
; source SHA-256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
0041de30  55                                 push       ebp
0041de31  8b ec                              mov        ebp, esp
0041de33  51                                 push       ecx
0041de34  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041de37  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041de3a  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0041de3d  89 88 94 20 00 00                  mov        dword ptr [eax + 0x2094], ecx
0041de43  8b e5                              mov        esp, ebp
0041de45  5d                                 pop        ebp
0041de46  c2 04 00                           ret        4
