; reset flag getter
; source SHA-256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
0041d080  55                                 push       ebp
0041d081  8b ec                              mov        ebp, esp
0041d083  51                                 push       ecx
0041d084  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041d087  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041d08a  8b 80 90 20 00 00                  mov        eax, dword ptr [eax + 0x2090]
0041d090  d1 e8                              shr        eax, 1
0041d092  83 e0 01                           and        eax, 1
0041d095  8b e5                              mov        esp, ebp
0041d097  5d                                 pop        ebp
0041d098  c3                                 ret
