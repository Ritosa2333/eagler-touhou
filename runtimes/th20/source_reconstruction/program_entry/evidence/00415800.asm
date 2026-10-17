; D3DPRESENT_PARAMETERS.Windowed getter
; source SHA-256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
00415800  55                                 push       ebp
00415801  8b ec                              mov        ebp, esp
00415803  51                                 push       ecx
00415804  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00415807  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041580a  8b 80 04 01 00 00                  mov        eax, dword ptr [eax + 0x104]
00415810  8b e5                              mov        esp, ebp
00415812  5d                                 pop        ebp
00415813  c3                                 ret
