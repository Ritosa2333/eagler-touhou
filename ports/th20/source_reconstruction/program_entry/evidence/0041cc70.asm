; display mode getter
; source SHA-256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
0041cc70  55                                 push       ebp
0041cc71  8b ec                              mov        ebp, esp
0041cc73  51                                 push       ecx
0041cc74  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041cc77  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041cc7a  8b 80 8c 20 00 00                  mov        eax, dword ptr [eax + 0x208c]
0041cc80  8b e5                              mov        esp, ebp
0041cc82  5d                                 pop        ebp
0041cc83  c3                                 ret
